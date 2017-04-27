/* Jungo Confidential. Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */

#include "utils.h"
#include "windrvr.h"
#if defined(WIN32) && !defined(WINCE)
    #include <process.h>
#endif
#if defined(UNIX) && !defined(VXWORKS)
    #include <pthread.h>
    #include <sys/time.h>
    #include <unistd.h>
    #include <errno.h>
#endif

#if !defined (__KERNEL__)
    #include <stdarg.h>
    #include <stdio.h>
    #if !defined(VXWORKS)
        #include <malloc.h>
    #endif
#else
    #if defined(LINUX)
        #define sprintf LINUX_sprintf
    #endif
#endif
#if defined(BCPP)
    #include <conio.h>
#endif

#if defined(LINUX)
typedef struct {
    pthread_cond_t cond;
    pthread_mutex_t mutex;
    BOOL signaled;
} wd_linux_event_t;
#endif

#if defined(__KERNEL__) && defined(WIN32)
    #pragma warning( disable :4013 4100)
#endif

#if !defined(WIN32) || defined(WINCE) || defined(_MT) 
/* threads functions */
typedef struct
{
    HANDLER_FUNC func;
    void *data;
} thread_struct_t;

#if defined(WIN32) && !defined (WINCE)
    static unsigned int DLLCALLCONV thread_handler(void *data)
#elif defined(WINCE)
    static unsigned long DLLCALLCONV thread_handler(void *data)
#else
    static void *thread_handler(void *data)
#endif
{
    thread_struct_t *t = (thread_struct_t *)data;
#if defined(WINCE)
    OsSetMaxProcPermissions();
#endif
    t->func(t->data);
    free(t);
    return 0;
}

DWORD DLLCALLCONV ThreadStart(HANDLE *phThread, HANDLER_FUNC pFunc, void *pData)
{
    #if defined(WIN32) || defined(WINCE)
    DWORD dwTmp;
    #endif
    thread_struct_t *t = (thread_struct_t *)malloc(sizeof(thread_struct_t));
    void *ret = NULL;

    t->func = pFunc;
    t->data = pData;
    #if defined(WIN32) && !defined(WINCE)
        ret = (void *)_beginthreadex(NULL, 0x1000, thread_handler,
            (void *)t, 0, (unsigned int *)&dwTmp);
    #elif defined(WINCE)
        ret = (void *)CreateThread(NULL, 0x1000, thread_handler,
            (void *)t, 0, (unsigned long *)&dwTmp);
    #elif defined(VXWORKS)
    {
        int priority, std, task_id = ERROR;
        if (taskPriorityGet(taskIdSelf(), &priority)!=ERROR)
        {
            task_id = taskSpawn(NULL, priority, 0, 0x4000,
                (FUNCPTR)thread_handler, (int)t, 0, 0, 0, 0, 0,
                0, 0, 0, 0);
        }
        if (task_id!=ERROR)
        {
            std = ioTaskStdGet(0, 1);
            ioTaskStdSet(task_id, 0, std);
            ioTaskStdSet(task_id, 1, std);
            ioTaskStdSet(task_id, 2, std);
            ret = (void *)task_id;
        }
    }
    #elif defined(UNIX)
    {
        int err = 0;
        ret = malloc(sizeof(pthread_t));
        if (ret)
            err = pthread_create((pthread_t *)ret, NULL, thread_handler, (PVOID)t);
        if (err)
        {
            free(ret);
            ret = NULL;
        }
    }
    #endif
    *phThread = ret;
    if (!ret)
    {
        free(t);
        return WD_INSUFFICIENT_RESOURCES;
    }
    return WD_STATUS_SUCCESS;
}


#if defined(VXWORKS)
    #define WAIT_FOR_EVER 0
    static void vxTask_wait(int taskId, int waitTime)
    {
        SEM_ID waitSem;

        if (waitTime==WAIT_FOR_EVER)
        {
            /* Loop while task is still alive */
            while (taskIdVerify(taskId)==OK)
                taskDelay(3);
        }
        else
        {
            /* create a dummy semaphore and try to take it for the specified
             * time. */
            waitSem = semBCreate(SEM_Q_PRIORITY, SEM_EMPTY);
            semTake(waitSem, waitTime);
            semDelete(waitSem);
        }
    }
#endif

void DLLCALLCONV ThreadWait(HANDLE hThread)
{
    #if defined(WIN32)
        WaitForSingleObject(hThread, INFINITE);
        CloseHandle(hThread);
    #elif defined(VXWORKS)
        vxTask_wait((int)hThread, WAIT_FOR_EVER);
    #elif defined(UNIX)
        pthread_join(*((pthread_t *)hThread), NULL);
        free(hThread);
    #endif
}

/* For backward compatability */
void DLLCALLCONV ThreadStop(HANDLE hThread)
{
    ThreadWait(hThread);
}
/* end of threads functions */

#endif /* defined(_MT) */

/* Synchronization objects */

/* Auto-reset events */
DWORD DLLCALLCONV OsEventCreate(HANDLE *phOsEvent)
{
#if defined(__KERNEL__)
    return WD_NOT_IMPLEMENTED;
#else
#if defined(WIN32)
    *phOsEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    return *phOsEvent ? WD_STATUS_SUCCESS : WD_INSUFFICIENT_RESOURCES;
#elif defined(LINUX)
    wd_linux_event_t *linux_event = (wd_linux_event_t *)malloc(
        sizeof(wd_linux_event_t));
    if (!linux_event)
        return WD_INSUFFICIENT_RESOURCES;
    memset(linux_event, 0, sizeof(wd_linux_event_t));
    pthread_cond_init(&linux_event->cond, NULL); 
    pthread_mutex_init(&linux_event->mutex, NULL);
    *phOsEvent = linux_event;
    return WD_STATUS_SUCCESS;
#else
    return WD_NOT_IMPLEMENTED;
#endif
#endif
}

DWORD DLLCALLCONV OsEventReset(HANDLE hOsEvent)
{
    DWORD dwStatus = OsEventWait(hOsEvent, 0);
    return (dwStatus==WD_TIME_OUT_EXPIRED) ? WD_STATUS_SUCCESS : dwStatus;
}

void DLLCALLCONV OsEventClose(HANDLE hOsEvent)
{
#if defined(__KERNEL__)
#else
#if defined(WIN32)
    if (hOsEvent)
        CloseHandle(hOsEvent);
#elif defined(LINUX)
    wd_linux_event_t *linux_event = (wd_linux_event_t *)hOsEvent;
    pthread_cond_destroy(&linux_event->cond); 
    pthread_mutex_destroy(&linux_event->mutex);
    free(linux_event);
#endif
#endif
}

DWORD DLLCALLCONV OsEventWait(HANDLE hOsEvent, DWORD dwSecTimeout)
{
#if defined(__KERNEL__)
    return WD_STATUS_SUCCESS;
#else
    DWORD rc = WD_STATUS_SUCCESS;
#if defined(WIN32)
    rc = WaitForSingleObject(hOsEvent, 
        (dwSecTimeout == INFINITE) ? INFINITE : dwSecTimeout * 1000);
    switch (rc) 
    {
        case WAIT_OBJECT_0:
            rc = WD_STATUS_SUCCESS;
            break;
        case WAIT_TIMEOUT:
            rc = WD_TIME_OUT_EXPIRED;
            break;
        default:
            rc = WD_SYSTEM_INTERNAL_ERROR;
            break;
    }
#elif defined(LINUX)
    struct timeval now;
    struct timespec timeout;
    wd_linux_event_t *linux_event = (wd_linux_event_t *)hOsEvent;

    pthread_mutex_lock(&linux_event->mutex);
    if (!linux_event->signaled)
    {
        if (dwSecTimeout == INFINITE)
            rc = pthread_cond_wait(&linux_event->cond, &linux_event->mutex);
        else
        {
            gettimeofday(&now, NULL);
            timeout.tv_sec = now.tv_sec + dwSecTimeout;
            timeout.tv_nsec = now.tv_usec * 1000;

            rc = pthread_cond_timedwait(&linux_event->cond, &linux_event->mutex, &timeout);
        }
    }
    linux_event->signaled = FALSE;
    pthread_mutex_unlock(&linux_event->mutex);
    rc = (rc==ETIMEDOUT ?  WD_TIME_OUT_EXPIRED : WD_STATUS_SUCCESS);
#endif
    return rc;
#endif
}

DWORD DLLCALLCONV OsEventSignal(HANDLE hOsEvent)
{
#if defined(__KERNEL__)
    return WD_STATUS_SUCCESS;
#else
#if defined(WIN32)
    if (!SetEvent(hOsEvent))
        return WD_SYSTEM_INTERNAL_ERROR;
#elif defined(LINUX)
    wd_linux_event_t *linux_event = (wd_linux_event_t *)hOsEvent;
    pthread_mutex_lock(&linux_event->mutex);
    linux_event->signaled = TRUE;
    pthread_cond_signal(&linux_event->cond);
    pthread_mutex_unlock(&linux_event->mutex);
#endif
    return WD_STATUS_SUCCESS;
#endif
}

DWORD DLLCALLCONV OsMutexCreate(HANDLE *phOsMutex)
{
#if defined(__KERNEL__)
    return WD_NOT_IMPLEMENTED;
#else
#if defined(WIN32)
    *phOsMutex = CreateMutex(NULL, FALSE, NULL);
    return *phOsMutex ? WD_STATUS_SUCCESS : WD_INSUFFICIENT_RESOURCES;
#elif defined(LINUX)
    pthread_mutex_t *linux_mutex = (pthread_mutex_t *)malloc(
        sizeof(pthread_mutex_t));
    if (!linux_mutex)
        return WD_INSUFFICIENT_RESOURCES;
    memset(linux_mutex, 0, sizeof(pthread_mutex_t));
    pthread_mutex_init(linux_mutex, NULL);
    *phOsMutex = linux_mutex;
    return WD_STATUS_SUCCESS;
#else
    return WD_NOT_IMPLEMENTED;
#endif
#endif
}

void DLLCALLCONV OsMutexClose(HANDLE hOsMutex)
{
#if defined(__KERNEL__)
#else
#if defined(WIN32)
    if (hOsMutex)
        CloseHandle(hOsMutex);
#elif defined(LINUX)
    pthread_mutex_t *linux_mutex = (pthread_mutex_t *)hOsMutex;
    pthread_mutex_destroy(linux_mutex); 
    free(linux_mutex);
#endif
#endif
}

DWORD DLLCALLCONV OsMutexLock(HANDLE hOsMutex)
{
#if defined(__KERNEL__)
    return WD_NOT_IMPLEMENTED;
#else
#if defined(WIN32)
    WaitForSingleObject(hOsMutex, INFINITE);
#elif defined(LINUX)
    pthread_mutex_t *linux_mutex = (pthread_mutex_t *)hOsMutex;
    pthread_mutex_lock(linux_mutex);
#endif
    return WD_STATUS_SUCCESS;
#endif
}

DWORD DLLCALLCONV OsMutexUnlock(HANDLE hOsMutex)
{
#if defined(__KERNEL__)
    return WD_NOT_IMPLEMENTED;
#else
#if defined(WIN32)
    ReleaseMutex(hOsMutex);
#elif defined(LINUX)
    pthread_mutex_t *linux_mutex = (pthread_mutex_t*)hOsMutex;
    pthread_mutex_unlock(linux_mutex);
#endif
    return WD_STATUS_SUCCESS;
#endif
}

void DLLCALLCONV SleepWrapper(DWORD dwMicroSecs)
{
    HANDLE hWD;
    WD_SLEEP slp;

    hWD = WD_Open();
    if (hWD==INVALID_HANDLE_VALUE)
        return;
    
    BZERO(slp);
    slp.dwMicroSeconds = dwMicroSecs;
    slp.dwOptions = SLEEP_NON_BUSY;
    WD_Sleep(hWD, &slp);
    WD_Close(hWD);
}

#ifdef WIN32
/* For backward compatability, no longer used */
void DLLCALLCONV FreeDllPtr(void **ptr)
{
    free(*ptr);
}
#endif

#if !defined(__KERNEL__)
void DLLCALLCONV vPrintDbgMessage(DWORD dwLevel, DWORD dwSection, 
    const char *format, va_list ap)
{
    WD_DEBUG_ADD add;
    HANDLE hWD;

    hWD = WD_Open();
    if (hWD==INVALID_HANDLE_VALUE) 
        return;
    BZERO(add);
    add.dwLevel = dwLevel;
    add.dwSection = dwSection;
    vsnprintf(add.pcBuffer, 255, format, ap);
    WD_DebugAdd(hWD, &add);
    WD_Close(hWD);
}

void DLLCALLCONV PrintDbgMessage(DWORD dwLevel, DWORD dwSection, 
    const char *format, ...)
{
    va_list ap;    
    va_start(ap, format);
    vPrintDbgMessage(dwLevel,dwSection,format,ap);
    va_end(ap);
}

int DLLCALLCONV GetPageSize(void)
{
#if defined(UNIX)
    return sysconf(_SC_PAGESIZE);
#elif defined(WIN32) 
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    return si.dwPageSize;
#else
    #error not implemented
#endif
}

DWORD DLLCALLCONV GetPageCount(void *pBuf, DWORD dwSize)
{
    DWORD dwPageSize = GetPageSize();
    UPTR dwMask = ~(dwPageSize - 1);
    return (DWORD)((((UPTR)pBuf & ~dwMask) + dwSize + ~dwMask) / dwPageSize);
}

BOOL DLLCALLCONV UtilGetFileSize(const PCHAR sFileName, DWORD *dwFileSize, 
    PCHAR sErrString)
{
    char err_msg[1024];
    PCHAR pc_err = sErrString ? sErrString : &err_msg[0];

#if defined (WIN32) /* Windows and Windows CE */
    HANDLE fh;
    #if defined(WINCE)
        WCHAR swFileName[MAX_PATH];

        swprintf(swFileName, L"%s", sFileName);
        fh = CreateFile(swFileName, GENERIC_READ, FILE_SHARE_READ, NULL, 
            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    #else
        fh = CreateFile(sFileName, GENERIC_READ, FILE_SHARE_READ, NULL, 
            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    #endif
    if (fh == INVALID_HANDLE_VALUE)
    {
        DWORD dwLastError;
        LPTSTR lpMsgBuf;

        dwLastError = GetLastError();
        sprintf(pc_err, "Failed opening %s for reading.\n"
            "Last error: 0x%lx", sFileName, dwLastError);
        
        if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL,
            dwLastError,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), /* Default language */
            (LPTSTR)&lpMsgBuf, 0, NULL))
        {
            /* ('\n' is already included in lpMsgBuf) */
            sprintf(pc_err, "%s - %s", pc_err, lpMsgBuf);
            LocalFree(lpMsgBuf);
        }
        else
            sprintf(pc_err, "%s\n", pc_err);
        
        return FALSE;
    } 

    *dwFileSize = GetFileSize(fh, NULL);
    if (!CloseHandle(fh))
    {
        sprintf(pc_err, "Failed closing file handle to %s (0x%p)\n", sFileName, fh);
        return FALSE;
    }

    return (*dwFileSize != INVALID_FILE_SIZE);
#elif defined (UNIX)
    struct stat file_stat;
    if (stat(sFileName, &file_stat))
    {
        sprintf(pc_err, "Failed retrieving %s file information", sFileName);
        return FALSE;
    }

    *dwFileSize = (DWORD)file_stat.st_size;
    return TRUE;
#else
    sprintf(pc_err, "UtilGetFileSize: not implemented for this OS\n");
    return FALSE;
#endif
}

DWORD DLLCALLCONV UtilGetStringFromUser(PCHAR pcString, DWORD dwSizeStr, 
    const CHAR *pcInputText, const CHAR *pcDefaultString)
{
    DWORD dwStrLen;
    
    if (!pcString || (dwSizeStr <= 1))
        return WD_INVALID_PARAMETER;
        
    pcString[0] = '\0';
    
    printf("%s", pcInputText);
    if (pcDefaultString && strcmp(pcDefaultString, ""))
        printf(" (Default: %s)", pcDefaultString);
    printf(":\n > ");
    fgets(pcString, dwSizeStr, stdin);

    dwStrLen = (DWORD)strlen(pcString);
    if (dwStrLen > 1)
    {
        /* Remove '\n' (may be missing if dwSizeStr < size of input string) */
        if (pcString[dwStrLen - 1] == '\n')
            pcString[dwStrLen - 1] = '\0'; 
    }
    else if (pcDefaultString)
        strcpy(pcString, pcDefaultString);
    else /* User did not provide any input and no default string exists */
        return WD_OPERATION_FAILED;

    return WD_STATUS_SUCCESS;
}

DWORD DLLCALLCONV UtilGetFileName(PCHAR pcFileName, DWORD dwFileNameSize, 
    const CHAR *pcDefaultFileName)
{
    return UtilGetStringFromUser(pcFileName, dwFileNameSize, 
        "Please enter the file name", pcDefaultFileName);
}

void DLLCALLCONV UtilClrScr(void)
{
#if defined(UNIX)
    system("clear");
#elif defined(WIN32) && !defined(WINCE)
    #if defined(BCPP)
        clrscr();
    #else
        system("cls");
    #endif
#else
    /* Not implemented ... */
#endif
}

#endif
