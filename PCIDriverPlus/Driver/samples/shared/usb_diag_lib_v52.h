/* Jungo Confidential. Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */

#ifndef _USB_DIAG_LIB_H_
#define _USB_DIAG_LIB_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "windrvr.h"

enum {MAX_BUFFER_SIZE = 4096};

typedef DWORD (*READ_PIPE_FUNC)(HANDLE hDevice, PVOID pBuffer, DWORD dwSize);
typedef VOID (*PROCESS_DATA_FUNC)(PVOID pBuffer, DWORD dwSize, PVOID pContext);
typedef VOID (*STOP_PIPE_FUNC)(HANDLE hDevice);

typedef struct  
{
    READ_PIPE_FUNC read_pipe_func;
    STOP_PIPE_FUNC stop_pipe_func;
    HANDLE hDevice;
    DWORD dwPacketSize;
    PROCESS_DATA_FUNC process_data_func; // if NULL call PrintHexBuffer
    PVOID pContext;
    BOOL   fStopped;
    HANDLE hThread;
} USB_LISTEN_PIPE;

BOOL USB_Get_WD_handle(HANDLE *phWD);
void USB_Print_device_info(DWORD dwVendorId, DWORD dwProductId);
void USB_Print_all_devices_info();
void USB_Print_device_Configurations();

void PrintHexBuffer(PVOID pBuffer, DWORD dwBytes);
DWORD GetHexBuffer(PVOID pBuffer, DWORD dwBytes);
void CloseListening(USB_LISTEN_PIPE *usbReadPipe);
void ListenToPipe(USB_LISTEN_PIPE *usbReadPipe);

#ifdef __cplusplus
}
#endif

#endif
