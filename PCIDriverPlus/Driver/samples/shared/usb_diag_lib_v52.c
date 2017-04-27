/* Jungo Confidential. Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */

////////////////////////////////////////////////////////////////
// File - USB_DIAG_LIB.C
//
// Utility functions for printing device information,
// detecting USB devices
// 
////////////////////////////////////////////////////////////////

#include "windrvr.h"
#ifdef _USE_SPECIFIC_KERNEL_DRIVER_
    #undef WD_Open
    #define WD_Open WD_OpenKernelHandle
    #if defined(UNIX)
        #undef WD_FUNCTION
        #define WD_FUNCTION(wFuncNum,h,pParam,dwSize,fWait) ((ULONG) ioctl((int)(h), wFuncNum, pParam))
    #endif
#endif
#include "usb_diag_lib_v52.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>

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

BOOL USB_Get_WD_handle(HANDLE *phWD)
{
    WD_VERSION ver;

    *phWD = WD_Open();
    if (*phWD==INVALID_HANDLE_VALUE) 
    {
        printf("Cannot open " WD_PROD_NAME " device\n");
        return FALSE;
    }

    BZERO(ver);
    WD_Version(*phWD, &ver);
    if (ver.dwVer<WD_VER) 
    {
        printf("Error - incorrect " WD_PROD_NAME " version\n");
        WD_Close (*phWD);
        *phWD = INVALID_HANDLE_VALUE;
        return FALSE;
    }

    return TRUE;
}

void USB_Print_device_info(DWORD dwVendorId, DWORD dwProductId)
{
    DWORD i;
    HANDLE hWD;
    WD_USB_SCAN_DEVICES usbScan;
    CHAR tmp[100];

    if (!USB_Get_WD_handle (&hWD)) return;

    BZERO(usbScan);
    usbScan.searchId.dwVendorId  = dwVendorId;
    usbScan.searchId.dwProductId = dwProductId;
    WD_UsbScanDevice(hWD,&usbScan);
    for (i=0; i<usbScan.dwDevices; i++)
    {
        WD_USB_DEVICE_GENERAL_INFO *genInfo = &usbScan.deviceGeneralInfo[i];
        printf("USB device - Vendor ID: %04x, Product ID: %04x, unique ID: 0x%x\n", 
            genInfo->deviceId.dwVendorId,
            genInfo->deviceId.dwProductId,
            usbScan.uniqueId[i]);
        if (genInfo->dwInterfaceNum==WD_SINGLE_INTERFACE)
            printf("Single interface");
        else
            printf("Interface number %d", genInfo->dwInterfaceNum);
        printf(", Device has %d configuration%s.\n", 
            genInfo->dwConfigurationsNum,
            genInfo->dwConfigurationsNum==1 ? "" : "s");
        printf("\n");
        if (i < usbScan.dwDevices - 1)
        {
           printf("Press Enter to continue to the next device\n");
           fgets(tmp, sizeof(tmp), stdin);
        }
    }
    if (!usbScan.dwDevices)
    {
        printf("\nNo USB device is registered to WinDriver.\n"
            "Please use the DriverWizard to generate and load an INF\n"
            "file for all USB devices you would like WinDriver\n"
            "to handle.\n");
    }
    WD_Close (hWD);
}

void USB_Print_all_devices_info() 
{
    USB_Print_device_info(0, 0);
}

void USB_Print_device_Configurations()
{
    WD_USB_CONFIGURATION config;
    DWORD i, j;
    HANDLE hWD;
    CHAR tmp[100];

    if (!USB_Get_WD_handle (&hWD)) return;

    BZERO (config);

    printf("Please enter the unique ID of the device:  ");
    fgets(line, sizeof(line), stdin);
    sscanf(line, "%x", &config.uniqueId);
    printf("Please enter the configuration index to display (zero based): ");
    fgets(line, sizeof(line), stdin);
    sscanf(line, "%d", &config.dwConfigurationIndex);

    WD_UsbGetConfiguration(hWD, &config);

    if (config.dwStatus!=WD_USBD_STATUS_SUCCESS)
    {
        printf("\n"
            "Get configuration descriptor failed (Error status:0x%x)\n"
            "For Configuration status codes - see WD_USB_ERROR_CODES enum\n"
            "definition, in \\WinDriver\\Include\\windrvr.h file\n",
            config.dwStatus);
        return;
    }

    printf("Configuration no. %d has %d interface(s)\n",
        config.configuration.dwValue,
        config.configuration.dwNumInterfaces);
    printf("configuration attributes: 0x%x,  max power: %d mA\n",
        config.configuration.dwAttributes,
        config.configuration.MaxPower*2);
    printf("\n");
    for (i=0; i<config.dwInterfaceAlternatives; i++)
    {
        WD_USB_INTERFACE_DESC *pInterface = &config.Interface[i].Interface;
        printf("interface no. %d,  alternate setting: %d,  index: %d\n",
            pInterface->dwNumber,
            pInterface->dwAlternateSetting,
            pInterface->dwIndex);
        printf("end-points: %d,  class: 0x%x,  sub-class: 0x%x,  protocol: 0x%x\n",
            pInterface->dwNumEndpoints,
            pInterface->dwClass,
            pInterface->dwSubClass,
            pInterface->dwProtocol);
        for (j=0; j<pInterface->dwNumEndpoints; j++)
        {
            WD_USB_ENDPOINT_DESC *pEndPoint = &config.Interface[i].Endpoints[j];
            printf("  end-point address: 0x%x, attributes: 0x%x, max packet "
                "size: %d, Interval: %d\n",
                pEndPoint->dwEndpointAddress,
                pEndPoint->dwAttributes,
                pEndPoint->dwMaxPacketSize,
                pEndPoint->dwInterval);
        }
        printf("\n");
        if (i < config.dwInterfaceAlternatives-1)
        {
           printf("Press Enter to continue to the next configuration\n");
           fgets(tmp, sizeof(tmp), stdin);
        }

    }
    WD_Close (hWD);
}

#define BYTES_IN_LINE 16
#define HEX_CHARS_PER_BYTE 3
#define HEX_STOP_POS BYTES_IN_LINE*HEX_CHARS_PER_BYTE

void PrintHexBuffer(PVOID pBuffer, DWORD dwBytes)
{
    PBYTE pData = (PBYTE) pBuffer;
    CHAR pHex[HEX_STOP_POS+1];
    CHAR pAscii[BYTES_IN_LINE+1];
    DWORD offset;
    DWORD i;
    
    if (!dwBytes)
        return;
    for (offset=0; offset<dwBytes; offset++)
    {
        DWORD line_offset = offset%BYTES_IN_LINE;
        if (offset && !line_offset)
        {
            pAscii[line_offset] = '\0';
            printf("%s | %s\n", pHex, pAscii);
        }
        sprintf(pHex+line_offset*HEX_CHARS_PER_BYTE, "%02X ", (UINT)pData[offset]);
        pAscii[line_offset] = (CHAR)((pData[offset]>=0x20) ? pData[offset] : '.');
    }

    // print the last line. fill with blanks if needed
    if (offset%BYTES_IN_LINE)
    {
        for (i=(offset%BYTES_IN_LINE)*HEX_CHARS_PER_BYTE; i<BYTES_IN_LINE*HEX_CHARS_PER_BYTE; i++)
            pHex[i] = ' ';
        pHex[i] = '\0';
    }
    pAscii[offset%BYTES_IN_LINE]='\0';
    printf("%s | %s\n", pHex, pAscii);
}

void CloseListening(USB_LISTEN_PIPE* pListenPipe)
{
    WD_USB_TRANSFER transfer;
    BZERO(transfer);

    if (!pListenPipe->hThread)
        return;

    printf("Stop listening to pipe\n");
    pListenPipe->fStopped = TRUE;

    pListenPipe->stop_pipe_func(pListenPipe->hDevice);
    WaitForSingleObject(pListenPipe->hThread, INFINITE);
    CloseHandle(pListenPipe->hThread);
    pListenPipe->hThread = NULL;
}

DWORD WINAPI PipeListenHandler(void * pParam)
{
    USB_LISTEN_PIPE *pListenPipe = (USB_LISTEN_PIPE*) pParam;
    PVOID buf = malloc(pListenPipe->dwPacketSize);

    for (;;)
    {
        DWORD dwBytesTransfered = pListenPipe->read_pipe_func(pListenPipe->hDevice, buf, pListenPipe->dwPacketSize);
        if(pListenPipe->fStopped)
            break;
        if(dwBytesTransfered==-1)
        {
            printf("Transfer failed\n");
            break;
        }
        if (pListenPipe->process_data_func)
            pListenPipe->process_data_func(buf, dwBytesTransfered, pListenPipe->pContext);
        else
            PrintHexBuffer(buf, dwBytesTransfered);
    }
    free(buf);
    return 0;
}

void ListenToPipe(USB_LISTEN_PIPE *pListenPipe)
{
    // start the running thread
    DWORD threadId;
    pListenPipe->fStopped = FALSE;
    printf("Start listening to pipe\n");
    pListenPipe->hThread = CreateThread (0, 0x1000, PipeListenHandler, 
        (PVOID) pListenPipe, 0, &threadId);
}

int GetHexChar()
{
    int ch;

    ch = getchar();

    if (!isxdigit(ch))
        return -1;

    if (isdigit(ch))
        return ch - '0';
    else
        return toupper(ch) - 'A' + 10;
}

DWORD GetHexBuffer(PVOID pBuffer, DWORD dwBytes)
{
    DWORD i;
    PBYTE pData = pBuffer;
    int res;
    int ch;

    for (i=0; i<dwBytes;)
    {
        ch = GetHexChar();
        if (ch<0)
            continue;

        res = ch << 4;

        ch = GetHexChar();
        if (ch<0)
            continue;

        res += ch;
        pData[i] = (BYTE)res;
        i++;
    }

    // advance to new line
    while (getchar()!=10){}

    // return the number of bytes that was read
    return i;
}
