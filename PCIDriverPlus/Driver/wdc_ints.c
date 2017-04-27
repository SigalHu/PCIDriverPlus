/* Jungo Confidential. Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */

/********************************************************************
*  File: wdc_ints.c - Implementation of WDC interrupt handling API  *
*********************************************************************/

#include "wdc_lib.h"
#include "wdc_defs.h"
#include "wdc_err.h"

#if defined(WINCE) && defined(WD_CE_ENHANCED_INTR)
static WD_ITEMS *GetInterruptItem(WDC_DEVICE *pDev)
{
    DWORD i;
    for (i=0; i < pDev->cardReg.Card.dwItems; i++)
    {
        if (pDev->cardReg.Card.Item[i].item == ITEM_INTERRUPT && 
            pDev->cardReg.Card.Item[i].I.Int.hInterrupt == pDev->Int.hInterrupt)
        {
            return &pDev->cardReg.Card.Item[i];
        }
    }
    return NULL;
}
#endif

/*************************************************************
  Functions implementations
 *************************************************************/
DWORD DLLCALLCONV WDC_IntEnable(WDC_DEVICE_HANDLE hDev, WD_TRANSFER *pTransCmds,
    DWORD dwNumCmds, DWORD dwOptions, INT_HANDLER funcIntHandler, PVOID pData,
    BOOL fUseKP)
{
    DWORD dwStatus;
    PWDC_DEVICE pDev = (PWDC_DEVICE)hDev;

    WDC_Trace("WDC_IntEnable entered\n");

    if (!funcIntHandler || !WdcIsValidDevHandle(hDev))
    {
        WDC_Err("WDC_IntEnable: %s",
            !funcIntHandler ? "Error - NULL event handler callback function\n" : WdcGetLastErrStr());
        return WD_INVALID_PARAMETER;
    }

    if (pDev->hIntThread)
    {
        WDC_Trace("WDC_IntEnable: Interrupt is already enabled\n");
        return WD_OPERATION_ALREADY_DONE;
    }
    
    if (!pDev->Int.hInterrupt)
    {
        WDC_Trace("WDC_IntEnable: Error - No interrupt handle\n");
        return WD_INVALID_PARAMETER;
    }
    
    if (fUseKP)
    {
        if (!WDC_IS_KP(pDev))
        {
            WDC_Err("WDC_IntEnable: Error - No Kernel PlugIn handle\n");
            return WD_INVALID_PARAMETER;
        }

        pDev->Int.kpCall.hKernelPlugIn = WDC_GET_KP_HANDLE(pDev);
    }
    
    if (dwNumCmds && !pTransCmds)
    {
        WDC_Err("WDC_IntEnable: Error - No interrupt transfer commands "
            "(expecting %ld commands)\n", dwNumCmds);
        return WD_INVALID_PARAMETER;
    }

    pDev->Int.dwOptions |= dwOptions;
    pDev->Int.Cmd = pTransCmds;
    pDev->Int.dwCmds = dwNumCmds;

    dwStatus = InterruptEnable(&pDev->hIntThread, WDC_GetWDHandle(),
        &pDev->Int, funcIntHandler, pData);

    if (WD_STATUS_SUCCESS != dwStatus)
    {
        WDC_Err("WDC_IntEnable: Failed enabling interrupt.\n"
            "Error 0x%lx - %s\n", dwStatus, Stat2Str(dwStatus));

        pDev->hIntThread = NULL;
        return dwStatus;
    }

    WDC_Trace("WDC_IntEnable: Interrupt enabled successfully\n");
    
    return WD_STATUS_SUCCESS;
}

DWORD DLLCALLCONV WDC_IntDisable(WDC_DEVICE_HANDLE hDev)
{
    DWORD dwStatus = WD_STATUS_SUCCESS;
    PWDC_DEVICE pDev = (PWDC_DEVICE)hDev;

    WDC_Trace("WDC_IntDisable entered\n");
    
    if (!WdcIsValidDevHandle(hDev))
    {
        WDC_Err("WDC_IntDisable: %s\n", WdcGetLastErrStr());
        return WD_INVALID_PARAMETER;
    }
        
    if (!pDev->hIntThread)
    {
        WDC_Trace("WDC_IntDisable: Interrupt is already disabled\n");
        return WD_OPERATION_ALREADY_DONE;
    }
    
    dwStatus = InterruptDisable(pDev->hIntThread);

    if (WD_STATUS_SUCCESS == dwStatus)
        WDC_Trace("WDC_IntDisable: Interrupt disabled successfully\n");
    else
    {
        WDC_Err("WDC_IntDisable: Failed disabling interrupt.\n"
            "Error 0x%lx - %s\n", dwStatus, Stat2Str(dwStatus));
    }
    
    pDev->hIntThread = NULL;

    return dwStatus;
}

BOOL DLLCALLCONV WDC_IntIsEnabled(WDC_DEVICE_HANDLE hDev)
{
#if defined(DEBUG)
    if (!WdcIsValidDevHandle(hDev))
    {
        WDC_Err("WDC_IntIsEnabled: %s", WdcGetLastErrStr());
        return FALSE;
    }
#endif
    
    return ((PWDC_DEVICE)hDev)->hIntThread ? TRUE : FALSE;
}
