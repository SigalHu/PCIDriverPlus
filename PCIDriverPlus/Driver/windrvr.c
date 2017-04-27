/* Jungo Confidential. Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */

/*
 * File - windrvr.c
 *
 * WinDriver driver name
 */

#if defined (__KERNEL__)
    #include "kpstdlib.h"
#else
    #include <stdio.h>
#endif
#include "windrvr.h"//hu<windrvr.h>

#if defined(WIN32) || defined(WINCE)
    #define snprintf _snprintf
#endif

const char * DLLCALLCONV WD_DriverName(const char *sName)
{
    static const char *sDriverName = WD_DEFAULT_DRIVER_NAME;
#if !defined(WINCE)
    static char sTmpName[WD_MAX_DRIVER_NAME_LENGTH];
    
    if (!sName)
        return sDriverName;

    BZERO(sTmpName);
    snprintf(sTmpName, sizeof(sTmpName), "%s%s", WD_DRIVER_NAME_PREFIX,
        sName);

    /* Driver name can only be set once */
    if (strcmp(sDriverName, WD_DEFAULT_DRIVER_NAME) &&
        strcmp(sDriverName, sTmpName))
    {
        return NULL;
    }

    sDriverName = sTmpName;
#else
    /* Driver Name change is not supported on Windows CE */
#endif
    return sDriverName;
}

#if !defined(__KERNEL__)

OS_TYPE DLLCALLCONV get_os_type(void)
{
#if defined(WIN32) || defined(WINCE)    
    static OS_TYPE os_type = OS_NOT_SET;
    OSVERSIONINFO lVerInfo;

    if (os_type)
        return os_type;
    lVerInfo.dwOSVersionInfoSize = sizeof (lVerInfo);
    if (!GetVersionEx (&lVerInfo)) 
        os_type = OS_CAN_NOT_DETECT;
    else if (lVerInfo.dwPlatformId==VER_PLATFORM_WIN32_NT)
    {
        if (lVerInfo.dwMajorVersion<=4)
            os_type = OS_WIN_NT_4;
        else if (lVerInfo.dwMajorVersion == 5)
            os_type = OS_WIN_NT_5;
        else
            os_type = OS_WIN_NT_6;
    }
#if defined(WINCE)
    else if (lVerInfo.dwPlatformId==VER_PLATFORM_WIN32_CE)
    {
        if (lVerInfo.dwMajorVersion > 5)
            os_type = OS_WINCE_6;
        else
            os_type = OS_WINCE_4;
    }
#endif/*WINCE*/
    else
        os_type = OS_CAN_NOT_DETECT;
    return os_type;
#elif defined(LINUX)
    return OS_LINUX;
#elif defined(SOLARIS)
    return OS_SOLARIS;
#else
    return OS_CAN_NOT_DETECT;
#endif
}

#if defined(WINCE)
DWORD WINAPI SetProcPermissions(DWORD dwPermissions);

void DLLCALLCONV OsSetMaxProcPermissions(void)
{
    if (get_os_type() == OS_WINCE_4)
        SetProcPermissions(0xFFFF);
}
#endif

#endif
