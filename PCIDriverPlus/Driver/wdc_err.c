/* Jungo Confidential. Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */

/*****************************************************************
*  File: wdc_err.c - WD card (WDC) library error handling.       *
*        Internal file, available only from wdc_* library files  *
******************************************************************/

#include "utils.h"
#include "wdc_err.h"

static CHAR gsLastErr[256];

/* WDC debug messages display options */
/* [Initialized to enable error prints in case of debug options initialization failure] */
static WDC_DBG_OPTIONS gDbgOptions = WDC_DBG_DEFAULT;

#if !defined (__KERNEL__)
#define DEFAULT_DBG_OUT_FILE stderr /* Default debug output file */
static FILE *gfpDbgFile = NULL; /* Handle to debug output file */
#endif

/*************************************************************
  Static functions prototypes
 *************************************************************/
static void DbgLog(DEBUG_LEVEL dbgLevel, const CHAR *format, va_list argp);
static void DbgInit(void);

/*************************************************************
  Functions implementation
 *************************************************************/
void WdcSetLastErrStr(const CHAR *format, ...)
{
    va_list argp;
    va_start(argp, format);

    vsnprintf(gsLastErr, sizeof(gsLastErr), format, argp);
    gsLastErr[sizeof(gsLastErr) - 1] = '\0';
    
    va_end(argp);
}

const CHAR *WdcGetLastErrStr()
{
    return gsLastErr;
}

void DLLCALLCONV WDC_Err(const CHAR *format, ...)
{
    if (gDbgOptions & WDC_DBG_LEVEL_ERR)
    {
        va_list argp;
        va_start(argp, format);
        DbgLog(D_ERROR, format, argp);
        va_end(argp);
    }
}

void DLLCALLCONV WDC_Trace(const CHAR *format, ...)
{
    if (gDbgOptions & WDC_DBG_LEVEL_TRACE)
    {
        va_list argp;
        va_start(argp, format);
        DbgLog(D_TRACE, format, argp);
        va_end(argp);
    }
}

DWORD DLLCALLCONV WDC_SetDebugOptions(WDC_DBG_OPTIONS dbgOptions, const CHAR *sDbgFile)
{
    /* Perform any necessary cleanup for previous debug options and set the default debug options */
    DbgInit();
    
    /* If no debug option is set (dbgOptions == 0) - use default options */
    if (!dbgOptions)
        dbgOptions = WDC_DBG_DEFAULT;
    
    if (dbgOptions & WDC_DBG_NONE)
    {
        gDbgOptions = WDC_DBG_NONE;
        return WD_STATUS_SUCCESS;
    }
    
    if (!(dbgOptions & WDC_DBG_LEVEL_ERR) && !(dbgOptions & WDC_DBG_LEVEL_TRACE))
    {
        WDC_Err("WDC_SetDebugOptions: Error - You must set either the "
            "WDC_DBG_LEVEL_TRACE, WDC_DBG_LEVEL_ERR, WDC_DBG_DEFAULT or "
            "WDC_DBG_NONE debug option.\n");
        return WD_INVALID_PARAMETER;
    }

    if (dbgOptions & WDC_DBG_LEVEL_TRACE)
        dbgOptions |= WDC_DBG_LEVEL_ERR;
    
    if (dbgOptions & WDC_DBG_OUT_FILE)
    {
    #if defined(__KERNEL__)
        sDbgFile = sDbgFile;
        WDC_Err("WDC_SetDebugOptions: Error - WDC_DBG_OUT_FILE (0x%lx) debug option is "
            "only allowed in user-mode.\n");
        return WD_INVALID_PARAMETER;
    #else
        if (sDbgFile && strcmp(sDbgFile, ""))
        {
            gfpDbgFile = fopen(sDbgFile, "w");
            
            if (!gfpDbgFile)
            {
                WDC_Err("WDC_SetDebugOptions: Debug init: Failed opening debug file "
                    "(%s) for writing.\n"
                    "Debug messages will be sent to the Debug Monitor and not "
                    "to %s\n", sDbgFile, sDbgFile);
                dbgOptions &= ~WDC_DBG_OUT_FILE;
                dbgOptions |= WDC_DBG_OUT_DBM;
            }
        }
        else /* If no file was selected, default debug output file will be used */
        {
            gfpDbgFile = DEFAULT_DBG_OUT_FILE;
            
            if (!gfpDbgFile)
            {
                WDC_Trace("WDC_SetDebugOptions: Debug init: Cannot access default debug "
                    "output file.\n"
                    "Debug messages will be sent to the Debug Monitor\n");
                dbgOptions &= ~WDC_DBG_OUT_FILE;
                dbgOptions |= WDC_DBG_OUT_DBM;
            }
        }
    #endif
    }
    else if (!(dbgOptions & WDC_DBG_OUT_DBM))
    {
        WDC_Err("WDC_SetDebugOptions: Error - You must set either the WDC_DBG_OUT_DBM "
            "or WDC_DBG_OUT_FILE debug flag.\n");
        return WD_INVALID_PARAMETER;
    }
    
    gDbgOptions = dbgOptions;

    WDC_Trace("WDC_SetDebugOptions: Debug options set to 0x%lx\n", gDbgOptions);

    return WD_STATUS_SUCCESS;
}

static void DbgInit(void)
{
#if !defined (__KERNEL__)
    if (gfpDbgFile)
    {
        if (gfpDbgFile == DEFAULT_DBG_OUT_FILE)
            fflush(gfpDbgFile);
        else
        {
            fclose(gfpDbgFile);
            gfpDbgFile = NULL;
        }
    }
#endif

    gDbgOptions = WDC_DBG_DEFAULT;
}

static void DbgLog(DEBUG_LEVEL dbgLevel, const CHAR *format, va_list argp)
{
#if !defined (__KERNEL__)
    va_list argp1;
    va_copy(argp1, argp);
#endif

    if (gDbgOptions & WDC_DBG_OUT_DBM)
    {
        WD_DEBUG_ADD add;
        BZERO(add);
        add.dwLevel = dbgLevel;
        add.dwSection = (DWORD)S_ALL;
        vsnprintf(add.pcBuffer, sizeof(add.pcBuffer) - 1, format, argp);
        WD_DebugAdd(WDC_GetWDHandle(), &add);
    }

#if !defined (__KERNEL__)
    if (gDbgOptions & WDC_DBG_OUT_FILE)
        vfprintf(gfpDbgFile, format, argp1);
    va_end(argp1);
#endif
}

