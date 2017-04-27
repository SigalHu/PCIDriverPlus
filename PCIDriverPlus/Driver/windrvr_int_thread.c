/* Jungo Confidential. Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */

/*
 * File - windrvr_int_thread.c
 *
 * Implementation of a thread that waits for WinDriver events.
 */

#if defined (__KERNEL_DRIVER__)
    #include "kd.h"
#elif defined (__KERNEL__)
    #include "kpstdlib.h"
#else
    #include "utils.h"
#endif

#include "windrvr_int_thread.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

// backward compatibility
BOOL InterruptThreadEnable(HANDLE *phThread, HANDLE hWD,
    WD_INTERRUPT *pInt, INT_HANDLER func, PVOID pData)
{
    DWORD rc;
    rc = InterruptEnable(phThread, hWD, pInt, func, pData);
    return rc==WD_STATUS_SUCCESS;
}

void InterruptThreadDisable(HANDLE hThread)
{
    InterruptDisable(hThread);
}

#if !defined(__KERNEL__)
    typedef struct
    {
        HANDLE hWD;
        INT_HANDLER func;
        PVOID pData;
        WD_INTERRUPT *pInt;
        void *thread;
#if defined(WINCE) && defined(WD_CE_ENHANCED_INTR)
        HANDLE hIntEvent;
        DWORD dwSysintr;
#endif
    } INT_THREAD_DATA;

#if defined(WINCE) && defined(WD_CE_ENHANCED_INTR)
    static void DLLCALLCONV interrupt_thread_handler(void *data)
    {
        INT_THREAD_DATA *pThread = (INT_THREAD_DATA *)data;

        CeSetThreadPriority (pThread->thread, 
            WD_CE_PRIORITY(THREAD_PRIORITY_TIME_CRITICAL)); 
        for (;;)
        {
            if (pThread->hIntEvent)
                // wait directly on the WINCE's event
                WaitForSingleObject(pThread->hIntEvent, INFINITE);
            else
                // use old style interrupts - another IST is running in the kernel
                WD_IntWait (pThread->hWD, pThread->pInt);

            if (pThread->pInt->fStopped==INTERRUPT_STOPPED)
            {
                // release interrupt on exit
                WD_InterruptDoneCe(pThread->hWD, pThread->pInt);
                break;
            }

            if (pThread->pInt->fStopped==INTERRUPT_INTERRUPTED)
                continue;
            pThread->func(pThread->pData);

            if (pThread->hIntEvent)
            {
                pThread->pInt->dwCounter++;
                /* Clear interrupt and ackhnowledge the OS */
                WD_InterruptDoneCe(pThread->hWD, pThread->pInt);
            }
            /* else
             *     In the old-style interrupts, acknowledge is done by kernel
             */
        }
    }
   
#else
    static void DLLCALLCONV interrupt_thread_handler(void *data)
    {
        INT_THREAD_DATA *pThread = (INT_THREAD_DATA *)data;
        for (;;)
        {
            WD_IntWait (pThread->hWD, pThread->pInt);
            if (pThread->pInt->fStopped==INTERRUPT_STOPPED)
                break;
            if (pThread->pInt->fStopped==INTERRUPT_INTERRUPTED)
                continue;
            pThread->func(pThread->pData);
        }
    }
#endif

    DWORD DLLCALLCONV InterruptEnable(HANDLE *phThread, HANDLE hWD, WD_INTERRUPT *pInt,
        INT_HANDLER func, PVOID pData)
    {
        DWORD dwStatus;
        INT_THREAD_DATA *pThread;

#if defined(WINCE) && defined(WD_CE_ENHANCED_INTR)
        HANDLE hIntEvent = NULL;
        WCHAR event_name[64] = L"";
#endif
        *phThread = NULL;

        pThread = (INT_THREAD_DATA *)malloc(sizeof(INT_THREAD_DATA));
        if (!pThread)
            return WD_INSUFFICIENT_RESOURCES;

        dwStatus = WD_IntEnable(hWD, pInt);
        if (dwStatus)
            goto Error;

        BZERO(*pThread);
        pThread->func = func;
        pThread->pData = pData;
        pThread->hWD = hWD;
        pThread->pInt = pInt;

#if defined(WINCE) && defined(WD_CE_ENHANCED_INTR)
        /* This event is shared (by its name) between the user mode and the kernel mode */
        swprintf(event_name, L"WDINT_EVENT_%d", pThread->pInt->hInterrupt);
        hIntEvent = CreateEvent(NULL, FALSE, FALSE, event_name);
        if (hIntEvent && GetLastError() == ERROR_ALREADY_EXISTS)
        {
            // use enhanced interrupts
            pThread->hIntEvent = hIntEvent;
        }
        else if (hIntEvent)
        {
            CloseHandle(hIntEvent);
            hIntEvent = NULL;
        }
        else
        {
            dwStatus = WD_INSUFFICIENT_RESOURCES;
            goto Error;
        }
#endif

        dwStatus = ThreadStart(&pThread->thread, interrupt_thread_handler, (void *)pThread);
        if (dwStatus)
        {
            WD_IntDisable(hWD, pInt);
            goto Error;
        }
        *phThread = (HANDLE) pThread;
        return WD_STATUS_SUCCESS;
Error:
        if (pThread)
            free(pThread);
#if defined(WINCE) && defined(WD_CE_ENHANCED_INTR)
        if (hIntEvent)
            CloseHandle(hIntEvent);
#endif
        return dwStatus;
    }

    DWORD DLLCALLCONV InterruptDisable(HANDLE hThread)
    {
        INT_THREAD_DATA *pThread = (INT_THREAD_DATA *)hThread;
        DWORD dwStatus;
        if (!pThread)
            return WD_INVALID_HANDLE;

#if defined(WINCE) && defined(WD_CE_ENHANCED_INTR)
        if (pThread->hIntEvent)
            pThread->pInt->fStopped = INTERRUPT_STOPPED;
#endif

        dwStatus = WD_IntDisable(pThread->hWD, pThread->pInt);

        ThreadWait(pThread->thread);
#if defined(WINCE) && defined(WD_CE_ENHANCED_INTR)
        if (pThread->hIntEvent)
            CloseHandle(pThread->hIntEvent);
#endif
        free(pThread);
        return dwStatus;
    }
#endif

#ifdef __cplusplus
}
#endif  // __cplusplus

