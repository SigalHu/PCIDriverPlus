/* Jungo Confidential. Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */

////////////////////////////////////////////////////////////////
// File - USB_DIAG_LIB.C
//
// Utility functions for communication with USB devices 
// using WinDriver's API.
// 
////////////////////////////////////////////////////////////////

#include <stdio.h>
#include "windrvr.h"
#include "wdu_lib.h"
#include "status_strings.h"
#include "utils.h"
#ifdef _USE_SPECIFIC_KERNEL_DRIVER_
    #undef WD_Open
    #define WD_Open WD_OpenKernelHandle
    #if defined(UNIX)
        #undef WD_FUNCTION
        #define WD_FUNCTION(wFuncNum,h,pParam,dwSize,fWait) ((ULONG) ioctl((int)(h), wFuncNum, pParam))
    #endif
#endif
#include "usb_diag_lib.h"
#include <ctype.h>
#include <string.h>

#if defined(UNIX)
    #include <sys/time.h>
    typedef struct timeval TIME_TYPE;
#else
    typedef LARGE_INTEGER TIME_TYPE;
#endif

#if !defined(ERR)
#define ERR printf
#endif

#define TRANSFER_TIMEOUT 30000 // in msecs
#define PERF_STREAM_BUFFER_SIZE 5120000 // bytes
#define PERF_DEVICE_TRANSFER_SIZE 10240 // bytes
#define PERF_PERFORMANCE_SAMPLE_TIME 10000      // mSec
#define PERF_TRANSFER_ITERATIONS 15000

typedef struct  
{
    HANDLE Handle;
    WDU_PIPE_INFO *pPipe;
    PVOID pContext;
    BOOL fStopped;
    HANDLE hThread;
    DWORD dwError;
    BOOL fStreamMode;
    DWORD dwBytesToTransfer;
    DWORD dwOptions;
} USB_LISTEN_PIPE;

// Function: CloseListening()
//   Stop listening to USB device pipe
void CloseListening(USB_LISTEN_PIPE *pListenPipe);

// Function: ListenToPipe()
//   Start listening to a USB device pipe
void ListenToPipe(USB_LISTEN_PIPE *pListenPipe);

// Function: pipeType2Str()
//   Returns a string identifying the pipe type
// Parameters:
//   pipeType [in] pipe type
// Return Value:
//   String containing the description of the pipe
char *pipeType2Str(ULONG pipeType)
{
    char *res = "unknown";
    switch (pipeType)
    {
        case PIPE_TYPE_CONTROL: 
            res = "Control";
            break;
        case PIPE_TYPE_ISOCHRONOUS:
            res = "Isochronous";
            break;          
        case PIPE_TYPE_BULK:
            res = "Bulk";
            break;
        case PIPE_TYPE_INTERRUPT:
            res = "Interrupt";
            break;
    }
    return res;
}

// input of command from user
static char line[256];

// Function: CloseListening()
//   Stop listening to USB device pipe
// Parameters:
//   pListenPipe [in] pointer to USB device pipe
// Return Value:
//   None
void CloseListening(USB_LISTEN_PIPE* pListenPipe)
{
    if (!pListenPipe->hThread)
        return;

    printf("Stop listening to pipe\n");
    pListenPipe->fStopped = TRUE;

    if (pListenPipe->fStreamMode)
        WDU_StreamClose(pListenPipe->Handle);
    else
        WDU_HaltTransfer(pListenPipe->Handle, pListenPipe->pPipe->dwNumber);

    ThreadWait(pListenPipe->hThread);
    pListenPipe->hThread = NULL;
}

// Function: PipeListenHandler()
//   Callback function, which listens to a pipe continuously when there is data 
//   available in the pipe
// Parameters:
//   pParam [in] contains the relevant pipe information
// Return Value:
//   None
void DLLCALLCONV PipeListenHandler(void * pParam)
{
    DWORD dwError;
    USB_LISTEN_PIPE *pListenPipe = (USB_LISTEN_PIPE*) pParam;
    DWORD dwBufsize = pListenPipe->dwBytesToTransfer;
    PVOID buf;
    DWORD dwOptions = pListenPipe->dwOptions;

    buf = malloc(dwBufsize);

    for (;;)
    {
        DWORD dwBytesTransferred;

        if (pListenPipe->fStreamMode)
        {
            dwError = WDU_StreamRead(pListenPipe->Handle, buf, dwBufsize, 
                &dwBytesTransferred);
        }
        else
        {
            dwError = WDU_Transfer(pListenPipe->Handle,
                pListenPipe->pPipe->dwNumber, TRUE, dwOptions, buf, dwBufsize,
                &dwBytesTransferred, NULL, TRANSFER_TIMEOUT);
        }

        if (pListenPipe->fStopped)
            break;
        if (dwError)
        {
            pListenPipe->dwError = dwError;
            printf("Listen ended due to an error, press <Enter> to stop.\n");
            break;
        }
        DIAG_PrintHexBuffer(buf, dwBytesTransferred, TRUE);
    }
    free(buf);
}

// Function: ListenToPipe()
//   Start listening to a USB device pipe
// Parameters:
//   pListenPipe [in] pipe to listen to
// Return Value:
//   None
void ListenToPipe(USB_LISTEN_PIPE *pListenPipe)
{
    // start the running thread
    pListenPipe->fStopped = FALSE;
    printf("Start listening to pipe\n");

    if (pListenPipe->fStreamMode)
    {
        pListenPipe->dwError = WDU_StreamStart(pListenPipe->Handle); 
        if (pListenPipe->dwError)
        {
            ERR("ListenToPipe: WDU_StreamStart() failed: error 0x%lx (\"%s\")\n",
                pListenPipe->dwError, Stat2Str(pListenPipe->dwError));
            return;
        }
    }

    pListenPipe->dwError = ThreadStart(&pListenPipe->hThread, 
        PipeListenHandler, (PVOID) pListenPipe);
}

// Function: PrintPipe()
//   Prints the pipe information (helper function)
// Parameters:
//   pPipe [in] pointer to the pipe information
// Return Value:
//   None
static void PrintPipe(const WDU_PIPE_INFO *pPipe)
{
    printf("  pipe num. 0x%lx: packet size %ld, type %s, dir %s, interval %ld (ms)\n",
        pPipe->dwNumber,
        pPipe->dwMaximumPacketSize,
        pipeType2Str(pPipe->type),
        pPipe->direction==WDU_DIR_IN ? "In" : pPipe->direction==WDU_DIR_OUT ? "Out" : "In & Out",
        pPipe->dwInterval);
}

// Function: PrintPipe0Info()
//   Prints the pipe0 (control pipe) information
void PrintPipe0Info(WDU_DEVICE *pDevice)
{
    printf("Control pipe:\n");
    PrintPipe(&pDevice->Pipe0);
}

// Function: PrintPipesInfo()
//   Prints the pipes information for the specified alternate setting
// Parameters:
//   pAltSet [in] pointer to the alternate setting information
// Return Value:
//   None
void PrintPipesInfo(WDU_ALTERNATE_SETTING *pAltSet)
{
    BYTE p;
    WDU_PIPE_INFO *pPipe;

    if (!pAltSet->Descriptor.bNumEndpoints)
    {
        printf("  no pipes are defined for this device other than the default pipe (number 0).\n");
        return;
    }

    printf("Alternate Setting: %d\n", pAltSet->Descriptor.bAlternateSetting);
    for (p=0, pPipe = pAltSet->pPipes; p<pAltSet->Descriptor.bNumEndpoints; p++, pPipe++)
        PrintPipe(pPipe);
}

// Function: PrintEndpoints()
//   Prints the endpoints (pipes) information for the specified alternate setting
//   (helper function for PrintDeviceConfigurations)
// Parameters:
//   pAltSet [in] pointer to the alternate setting information
// Return Value:
//   None
static void PrintEndpoints(const WDU_ALTERNATE_SETTING *pAltSet)
{
    BYTE endp;
    WDU_ENDPOINT_DESCRIPTOR *pEndp;

    for (endp=0; endp<pAltSet->Descriptor.bNumEndpoints; endp++)
    {
        pEndp = &pAltSet->pEndpointDescriptors[endp];
        printf("    end-point address: 0x%02x, attributes: 0x%x, max packet %d, Interval: %d\n",
            pEndp->bEndpointAddress,
            pEndp->bmAttributes,
            pEndp->wMaxPacketSize,
            pEndp->bInterval);
    }
}

// Function: PrintDeviceConfigurations()
//   Prints the device's configurations information
// Parameters:
//   hDevice [in] handle to the USB device
// Return Value:
//   None
void PrintDeviceConfigurations(HANDLE hDevice)
{
    DWORD dwError;
    WDU_DEVICE *pDevice = NULL;
    DWORD ifc, alt;
    UINT32 iConf;

    WDU_CONFIGURATION *pConf;
    WDU_INTERFACE *pInterface;
    WDU_ALTERNATE_SETTING *pAltSet;

    dwError = WDU_GetDeviceInfo(hDevice, &pDevice);
    if (dwError)
    {
        ERR("PrintDeviceConfigurations: WDU_GetDeviceInfo failed: error 0x%lx (\"%s\")\n",
            dwError, Stat2Str(dwError));
        goto Exit;
    }

    printf("This device has %d configurations:\n", pDevice->Descriptor.bNumConfigurations);
    for (iConf=0; iConf<pDevice->Descriptor.bNumConfigurations; iConf++)
    {
        printf("  %d. configuration value %d (has %ld interfaces)\n", 
            iConf, pDevice->pConfigs[iConf].Descriptor.bConfigurationValue,
            pDevice->pConfigs[iConf].dwNumInterfaces);
    }
    iConf = 0;
    if (pDevice->Descriptor.bNumConfigurations>1)
    {
        printf("Please enter the configuration index to display (dec - zero based): ");
        fgets(line, sizeof(line), stdin);
        sscanf(line, "%d", &iConf);
        if (iConf >= pDevice->Descriptor.bNumConfigurations)
        {
            printf("ERROR: invalid configuration index, valid values are 0-%d\n",
                pDevice->Descriptor.bNumConfigurations);
            goto Exit;
        }
    }
    pConf = &pDevice->pConfigs[iConf];

    printf("The configuration indexed %d has %ld interface(s):\n",
        iConf, pConf->dwNumInterfaces);

    for (ifc=0; ifc<pConf->dwNumInterfaces; ifc++)
    {
        pInterface = &pConf->pInterfaces[ifc];
        printf("interface no. %d has %ld alternate settings:\n", 
            pInterface->pAlternateSettings[0].Descriptor.bInterfaceNumber,
            pInterface->dwNumAltSettings);
        for (alt=0; alt<pInterface->dwNumAltSettings; alt++)
        {
            pAltSet = &pInterface->pAlternateSettings[alt];

            printf("  alternate: %d, endpoints: %d, class: 0x%x, subclass: 0x%x, protocol: 0x%x\n",
                pAltSet->Descriptor.bAlternateSetting,
                pAltSet->Descriptor.bNumEndpoints,
                pAltSet->Descriptor.bInterfaceClass,
                pAltSet->Descriptor.bInterfaceSubClass,
                pAltSet->Descriptor.bInterfaceProtocol);

            PrintEndpoints(pAltSet);
        }
        printf("\n");
    }
    printf("\n");

Exit:
    if (pDevice)
        WDU_PutDeviceInfo(pDevice);
}

// Function: get_cur_time()
//   Get current time in OS units
// Parameters:
//   time [out] OS time value
// Return Value:
//   TRUE if successfull, FALSE otherwise
static BOOL get_cur_time(TIME_TYPE *time)
{
#if defined(UNIX)
    return !gettimeofday(time, NULL);
#else
    return QueryPerformanceCounter(time);
#endif
}

// Function: get_cur_time()
//   Calculate time difference
// Parameters:
//   end [in]   OS time value
//   start [in] OS time value
// Return Value:
//   The time difference in milliseconds.
static DWORD time_diff(const TIME_TYPE *end, const TIME_TYPE *start)
{
#if defined(UNIX)
    return (end->tv_usec - start->tv_usec) / 1000 + 
        (end->tv_sec - start->tv_sec) * 1000;
#else
    TIME_TYPE ctr_freq;

    if (!QueryPerformanceFrequency(&ctr_freq))
    {
        ERR("Error reading timer frequency\n");
        return (DWORD)-1;
    }

    return (DWORD)((end->QuadPart - start->QuadPart) * 1000 /
        ctr_freq.QuadPart);
#endif
}

enum {
    MENU_RW_READ_PIPE = 1,
    MENU_RW_WRITE_PIPE,
    MENU_RW_LISTEN_PIPE,
    MENU_RW_STREAM_READ_SPEED,
    MENU_RW_STREAM_WRITE_SPEED,
    MENU_RW_CHANGE_MODE,
    MENU_RW_EXIT = 99,
};

// Function: ReadWritePipesMenu()
//   Displays menu to read/write from the device's pipes
// Parameters:
//   hDevice [in] handle to the USB device
// Return Value:
//   None
void ReadWritePipesMenu(HANDLE hDevice)
{
    DWORD dwError;
    WDU_DEVICE *pDevice;
    WDU_ALTERNATE_SETTING *pAltSet;
    WDU_PIPE_INFO *pPipes;
    BYTE  SetupPacket[8];
    DWORD cmd, dwPipeNum, dwSize, dwBytesTransferred;
    BYTE i=0;
    VOID *pBuffer = NULL;
    USB_LISTEN_PIPE listenPipe;
    int c;
    WDU_STREAM_HANDLE stream;
    BOOL fStreamMode = TRUE;
    DWORD dwBufferSize = 0x20000;
    TIME_TYPE streaming_time_start, streaming_time_end;
    DWORD perf_time_total;
#if defined(WINNT)
    BOOL fStreamAble = TRUE;
#else
    BOOL fStreamAble = FALSE;
#endif

    dwError = WDU_GetDeviceInfo(hDevice, &pDevice);
    if (dwError)
    {
        ERR("ReadWritePipesMenu: WDU_GetDeviceInfo() failed: error 0x%lx (\"%s\")\n",
            dwError, Stat2Str(dwError));
        return;
    }

    pAltSet = pDevice->pActiveInterface[0]->pActiveAltSetting;
    pPipes = pAltSet->pPipes;

    PrintPipe0Info(pDevice);
    PrintPipesInfo(pAltSet);

    if (!fStreamAble)
        fStreamMode = FALSE;

    do {
        printf("\n");
        printf("Read/Write from/to device's pipes using %s\n", 
            fStreamMode ? "Streaming Data Transfers" : 
            "Single Blocking Transfers");
        printf("---------------------\n");
        printf("%d.  Read from pipe\n", MENU_RW_READ_PIPE);
        printf("%d.  Write to pipe\n", MENU_RW_WRITE_PIPE);
        printf("%d.  Listen to pipe (continuous read)\n", MENU_RW_LISTEN_PIPE);
        if (fStreamMode)
        {
            printf("%d.  Check streaming READ speed\n",
                MENU_RW_STREAM_READ_SPEED);
            printf("%d.  Check streaming WRITE speed\n",
                MENU_RW_STREAM_WRITE_SPEED);
        }
        /* Toggle menu from regular to streaming */
        if (fStreamAble)
        {
            printf("%d.  Switch to %s mode\n", MENU_RW_CHANGE_MODE,
                !fStreamMode ? "Streaming Data Transfers" : "Single Blocking "
                "Transfers");
        }
        printf("%d. Main menu\n", MENU_RW_EXIT);
        printf("Enter option: ");
        cmd = 0;
        fgets(line, sizeof(line), stdin);
        sscanf(line, "%ld", &cmd);

        if (cmd == MENU_RW_EXIT)
            break;
        if (cmd == MENU_RW_CHANGE_MODE)
        {
            if (fStreamAble)
                fStreamMode = !fStreamMode;
            continue;
        }

        /* Make sure the option entered is available for the current mode */
        if (cmd<MENU_RW_READ_PIPE || (!fStreamMode && cmd>MENU_RW_LISTEN_PIPE)
            || cmd>MENU_RW_STREAM_WRITE_SPEED)
            continue;

        printf("Please enter the pipe number (hex): 0x");
        fgets(line, sizeof(line), stdin);
        dwPipeNum=0;
        sscanf(line, "%lx", &dwPipeNum);

        // search for the pipe
        if (dwPipeNum)
        {
            for (i=0; i<pAltSet->Descriptor.bNumEndpoints; i++)
                if (pPipes[i].dwNumber == dwPipeNum)
                    break;
            if (i >= pAltSet->Descriptor.bNumEndpoints)
            {
                printf("The pipe number 0x%lx does not exist, please try again.\n", dwPipeNum);
                continue;
            }
        }

        switch (cmd)
        {
        case MENU_RW_READ_PIPE:
        case MENU_RW_WRITE_PIPE:
            if (!dwPipeNum || pPipes[i].type == PIPE_TYPE_CONTROL)
            {
                printf("Please enter setup packet (hex - 8 bytes): ");
                DIAG_GetHexBuffer((PVOID) SetupPacket, 8);
            }

            printf("Please enter the size of the buffer (dec):  ");
            fgets(line, sizeof(line), stdin);
            sscanf(line, "%ld", &dwSize);
            if (dwSize)
            {
                pBuffer = malloc(dwSize);
                if (!pBuffer)
                {
                    ERR("Cannot alloc memory\n");
                    break;
                }
                if (cmd == MENU_RW_WRITE_PIPE)
                {
                    printf("Please enter the input buffer (hex): ");
                    DIAG_GetHexBuffer(pBuffer, dwSize);
                }
            }
            if (fStreamMode)
            {
                dwError = WDU_StreamOpen(hDevice, dwPipeNum, dwBufferSize, 
                    dwSize, TRUE, 0, TRANSFER_TIMEOUT, &stream); 
                if (dwError)
                {    
                    ERR("ReadWritePipesMenu: WDU_StreamOpen() failed: error 0x%lx (\"%s\")\n",
                        dwError, Stat2Str(dwError));
                    goto End_transfer;
                }
                dwError = WDU_StreamStart(stream); 
                if (dwError)
                {
                    ERR("ReadWritePipesMenu: WDU_StreamStart() failed: error "
                        "0x%lx (\"%s\")\n", dwError, Stat2Str(dwError));
                    goto End_transfer;
                }

                if (cmd == MENU_RW_READ_PIPE)
                {
                    dwError = WDU_StreamRead(stream, pBuffer, dwSize,
                        &dwBytesTransferred);
                }
                else 
                {
                    dwError = WDU_StreamWrite(stream, pBuffer, dwSize,
                        &dwBytesTransferred);  
                }
                if (dwError)
                {
                    BOOL fIsRunning;
                    DWORD dwLastError;
                    DWORD dwBytesInBuffer;

                    dwError = WDU_StreamGetStatus(stream, &fIsRunning, 
                        &dwLastError, &dwBytesInBuffer);
                    if (!dwError)
                        dwError = dwLastError;
                }
            }
            else
            {
                dwError = WDU_Transfer(hDevice,
                    dwPipeNum ? pPipes[i].dwNumber : 0,
                    cmd==MENU_RW_READ_PIPE, 0, pBuffer, dwSize,
                    &dwBytesTransferred, SetupPacket, TRANSFER_TIMEOUT);
            }

            if (dwError)
            {
                ERR("ReadWritePipesMenu: Transfer failed: error 0x%lx "
                    "(\"%s\")\n", dwError, Stat2Str(dwError));
            }
            else
            {
                printf("Transferred %ld bytes\n", dwBytesTransferred);
                if (cmd == MENU_RW_READ_PIPE && pBuffer)
                    DIAG_PrintHexBuffer(pBuffer, dwBytesTransferred, TRUE);
            }
End_transfer:
            if (pBuffer)
            {
                free(pBuffer);
                pBuffer = NULL;
            }
            if (fStreamMode && stream != NULL)
            {
                dwError = WDU_StreamClose(stream);
                if (dwError)
                {
                    ERR("ReadWritePipesMenu: WDU_StreamClose() failed: error "
                        "0x%lx (\"%s\")\n", dwError, Stat2Str(dwError));
                    break;
                }
            }
            break;

        case MENU_RW_LISTEN_PIPE:
            if (!dwPipeNum || pPipes[i].type==PIPE_TYPE_CONTROL)
            {
                printf("Cannot listen to control pipes.\n");
                break;
            }
            BZERO(listenPipe);

            listenPipe.pPipe = &pPipes[i];
            listenPipe.fStreamMode = fStreamMode;
            
            if (pPipes[i].type == PIPE_TYPE_ISOCHRONOUS)  
            {
                listenPipe.dwBytesToTransfer = pPipes[i].dwMaximumPacketSize * 8; // suit 8 minimum packets for high speed transfers 
                listenPipe.dwOptions |= USB_ISOCH_FULL_PACKETS_ONLY; 
            }
            else
                listenPipe.dwBytesToTransfer = pPipes[i].dwMaximumPacketSize;

            if (fStreamMode)
            {      
                dwError = WDU_StreamOpen(hDevice, pPipes[i].dwNumber, dwBufferSize, 
                    listenPipe.dwBytesToTransfer, TRUE, listenPipe.dwOptions, TRANSFER_TIMEOUT, &listenPipe.Handle); 
                if (dwError)
                {    
                    ERR("ReadWritePipesMenu: WDU_StreamOpen() failed: error "
                        "0x%lx (\"%s\")\n", dwError, Stat2Str(dwError));
                    break;
                }
            }
            else
                listenPipe.Handle = hDevice;

            printf("Press <Enter> to start listening. While listening, press <Enter> to stop\n\n");
            getchar();
            ListenToPipe(&listenPipe);
            if (listenPipe.dwError)
            {
                ERR("ReadWritePipesMenu: error listening to pipe 0x%lx: error "
                    "0x%lx (\"%s\")\n", dwPipeNum, listenPipe.dwError,
                    Stat2Str(listenPipe.dwError));
                break;
            }

            while ((c=getchar())!=10) {}   // ESC code
            CloseListening(&listenPipe);
            if (listenPipe.dwError)
            {
                ERR("ReadWritePipesMenu: Transfer failed: error 0x%lx "
                    "(\"%s\")\n", listenPipe.dwError,
                    Stat2Str(listenPipe.dwError));
            }
            break;

       case MENU_RW_STREAM_READ_SPEED:
       case MENU_RW_STREAM_WRITE_SPEED:

            if (!fStreamMode)
                break; // We shouldn't reach this line

            if (!dwPipeNum || pPipes[i].type == PIPE_TYPE_CONTROL)
            {
                printf("Please enter setup packet (hex - 8 bytes): ");
                DIAG_GetHexBuffer((PVOID)SetupPacket, 8);
            }
            
            printf("The size of the buffer to transfer(dec): %d\n",
                PERF_DEVICE_TRANSFER_SIZE);
            printf("The size of the internal Rx/Tx stream buffer (dec): %d"
                "\n", PERF_STREAM_BUFFER_SIZE);
            printf("Making the transfer of %d times the buffer size, please "
                "wait ...\n", PERF_TRANSFER_ITERATIONS);

            if (cmd == MENU_RW_STREAM_WRITE_SPEED)
            {
                // Here you can fill pBuffer with the right data for the board
            }

            dwError = WDU_StreamOpen(hDevice, dwPipeNum,
                PERF_STREAM_BUFFER_SIZE, PERF_DEVICE_TRANSFER_SIZE, TRUE, 0,
                TRANSFER_TIMEOUT, &stream);
            if (dwError)
            {    
                ERR("ReadWritePipesMenu: WDU_StreamOpen() failed: error 0x%lx "
                    "(\"%s\")\n", dwError, Stat2Str(dwError));
                break;
            }
            
            pBuffer = malloc(PERF_DEVICE_TRANSFER_SIZE);
            if (!pBuffer)
            {
                ERR("Cannot alloc memory\n");
                break;
            }

            dwError = WDU_StreamStart(stream);    
            if (dwError)
            {
                ERR("ReadWritePipesMenu: WDU_StreamStart() failed: error "
                    "0x%lx (\"%s\")\n", dwError, Stat2Str(dwError));
                goto End_perf_test;
            }

            dwBytesTransferred = 0;
            get_cur_time(&streaming_time_start);
            do {
                DWORD dwBytesTransferredSingle;

                if (cmd == MENU_RW_STREAM_READ_SPEED)
                {
                    dwError = WDU_StreamRead(stream, pBuffer, PERF_DEVICE_TRANSFER_SIZE,
                        &dwBytesTransferredSingle);
                }
                else 
                {
                    dwError = WDU_StreamWrite(stream, pBuffer, PERF_DEVICE_TRANSFER_SIZE,
                        &dwBytesTransferredSingle);
                }

                if (dwError)
                {
                    ERR("ReadWritePipesMenu: Transfer failed: error 0x%lx "
                        "(\"%s\")\n", dwError, Stat2Str(dwError));
                    goto End_perf_test;
                }

                dwBytesTransferred += dwBytesTransferredSingle;

            } while (dwBytesTransferred < PERF_DEVICE_TRANSFER_SIZE * PERF_TRANSFER_ITERATIONS);

            // If write command wait for all the data to be written
            if (cmd == MENU_RW_STREAM_WRITE_SPEED)
            {
                dwError = WDU_StreamFlush(stream);
                if (dwError)
                {
                    ERR("ReadWritePipesMenu: Transfer failed: error 0x%lx "
                        "(\"%s\")\n", dwError, Stat2Str(dwError));
                    goto End_perf_test;
                }
            }

            get_cur_time(&streaming_time_end);
            perf_time_total = time_diff(&streaming_time_end, &streaming_time_start);
            if (perf_time_total == -1)
                goto End_perf_test;

            printf("Transferred %ld bytes, elapsed time %ld[ms], "
                "rate %ld[MB/sec]\n", dwBytesTransferred, perf_time_total,
                //        bytes         msec                     sec         MB
                (dwBytesTransferred / (perf_time_total + 1)) * 1000 / (1024 * 1024));

End_perf_test:
            if (pBuffer)
            {           
                free(pBuffer);
                pBuffer = NULL;
            }           

            dwError = WDU_StreamClose(stream);
            if (dwError)                
            {                   
                ERR("ReadWritePipesMenu: WDU_StreamClose() failed: error "      
                    "0x%lx (\"%s\")\n", dwError, Stat2Str(dwError));    
            }                                           

            break;                                              
        } 
    } while (1);

    if (pDevice)
        WDU_PutDeviceInfo(pDevice);
}

