/* Jungo Confidential. Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */

#ifndef _WDC_ERR_H_
#define _WDC_ERR_H_

/****************************************************************
*  File: wdc_err.h - WD card (WDC) internal error handling API  *
*        (for use only by the WDC library)                      *
*****************************************************************/

#if !defined (__KERNEL__)
    #include <stdio.h>
    #include <stdarg.h>
#else
    #include "kpstdlib.h"
#endif
#include "utils.h"
#include "wdc_lib.h"

/*************************************************************
  Static inline functions implementations
 **************************************************************/
/* -----------------------------------------------
    Debugging and error handling
   ----------------------------------------------- */
void WdcSetLastErrStr(const CHAR *format, ...);
const CHAR *WdcGetLastErrStr(void);

/* [Additional error handling function prototypes are declared in wdc_lib.h] */

/* -----------------------------------------------
    Variables validation
   ----------------------------------------------- */
static inline BOOL WdcIsValidPtr(PVOID ptr, const CHAR *sErrMssg)
{
    if (!ptr)
    {
        WdcSetLastErrStr("Error - %s\n", sErrMssg);
        return FALSE;
    }

    return TRUE;
}

static inline BOOL WdcIsValidDevHandle(WDC_DEVICE_HANDLE hDev)
{
    return WdcIsValidPtr(hDev, "Invalid device handle");
}

#endif
