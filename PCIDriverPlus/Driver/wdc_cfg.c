/* Jungo Confidential. Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */

/*
 *  File: wdc_cfg.c
 *
 *  Implementation of WDC functions for reading/writing
 *  PCI configuration space and PCMCIA attribute space
 */

#include "wdc_lib.h"
#include "wdc_defs.h"
#include "wdc_err.h"

#if defined(DEBUG)
static inline BOOL RWParamsValidate(WDC_DEVICE_HANDLE hDev, PVOID pData)
{
    if (!WdcIsValidDevHandle(hDev) || !WdcIsValidPtr(pData, "NULL data buffer"))
        return FALSE;
    return TRUE;
}
#endif

/*
 * Read/Write PCI configuration registers
 */

/* Read/Write by slot */
static DWORD WDC_PciReadWriteCfgBySlot(WD_PCI_SLOT *pPciSlot, DWORD dwOffset,
    PVOID pData, DWORD dwBytes, WDC_DIRECTION direction)
{
    DWORD dwStatus;
    WD_PCI_CONFIG_DUMP pciCnf; 
    
    BZERO(pciCnf);
    pciCnf.pciSlot = *pPciSlot;
    pciCnf.pBuffer = pData;
    pciCnf.dwOffset = dwOffset;
    pciCnf.dwBytes = dwBytes;
    pciCnf.fIsRead = (WDC_READ == direction);
    dwStatus = WD_PciConfigDump(WDC_GetWDHandle(), &pciCnf);

    if (WD_STATUS_SUCCESS != dwStatus)
    {
        WdcSetLastErrStr("Error 0x%lx - %s\n", dwStatus, Stat2Str(dwStatus));
        goto Error;
    }

    return WD_STATUS_SUCCESS;

Error:
    WDC_Err("WDC_PciReadWriteCfgBySlot: Failed %s %ld bytes %s offset"
        "0x%lx.\n%s", (WDC_READ == direction) ? "reading" : "writing",
        dwBytes, (WDC_READ == direction) ? "from" : "to", dwOffset,
        WdcGetLastErrStr());
    
    return dwStatus;
}

/* Read/Write by device handle */
static DWORD WDC_PciReadWriteCfg(WDC_DEVICE_HANDLE hDev, DWORD dwOffset,
    PVOID pData, DWORD dwBytes, WDC_DIRECTION direction)
{
    PWDC_DEVICE pDev = (PWDC_DEVICE)hDev;
    return WDC_PciReadWriteCfgBySlot(&(pDev->slot.pciSlot), dwOffset, pData,
        dwBytes, direction); 
}

DWORD DLLCALLCONV WDC_PciReadCfgBySlot(WD_PCI_SLOT *pPciSlot, DWORD dwOffset,
    PVOID pData, DWORD dwBytes)
{
    return WDC_PciReadWriteCfgBySlot(pPciSlot, dwOffset, pData, dwBytes,
        WDC_READ);
}

DWORD DLLCALLCONV WDC_PciWriteCfgBySlot(WD_PCI_SLOT *pPciSlot, DWORD dwOffset,
    PVOID pData, DWORD dwBytes)
{
    return WDC_PciReadWriteCfgBySlot(pPciSlot, dwOffset, pData, dwBytes,
        WDC_WRITE);
}

DWORD DLLCALLCONV WDC_PciReadCfg(WDC_DEVICE_HANDLE hDev, DWORD dwOffset,
    PVOID pData, DWORD dwBytes)
{
    return WDC_PciReadWriteCfg(hDev, dwOffset, pData, dwBytes, WDC_READ);
}

DWORD DLLCALLCONV WDC_PciWriteCfg(WDC_DEVICE_HANDLE hDev, DWORD dwOffset,
    PVOID pData, DWORD dwBytes)
{
    return WDC_PciReadWriteCfg(hDev, dwOffset, pData, dwBytes, WDC_WRITE);
}

#define DECLARE_PCI_READ_CFG_BY_SLOT(bits,type) \
DWORD DLLCALLCONV WDC_PciReadCfgBySlot##bits(WD_PCI_SLOT *pPciSlot, DWORD dwOffset, type *val) { \
    return WDC_PciReadWriteCfgBySlot(pPciSlot, dwOffset, val, bits/8, WDC_READ); }

#define DECLARE_PCI_WRITE_CFG_BY_SLOT(bits,type) \
DWORD DLLCALLCONV WDC_PciWriteCfgBySlot##bits(WD_PCI_SLOT *pPciSlot, DWORD dwOffset, type val) { \
    return WDC_PciReadWriteCfgBySlot(pPciSlot, dwOffset, &val, bits/8, WDC_WRITE); }
    
#define DECLARE_PCI_READ_CFG(bits,type) \
DWORD DLLCALLCONV WDC_PciReadCfg##bits(WDC_DEVICE_HANDLE hDev, DWORD dwOffset, type *val) { \
    return WDC_PciReadWriteCfg(hDev, dwOffset, val, bits/8, WDC_READ); }

#define DECLARE_PCI_WRITE_CFG(bits,type) \
DWORD DLLCALLCONV WDC_PciWriteCfg##bits(WDC_DEVICE_HANDLE hDev, DWORD dwOffset, type val) { \
    return WDC_PciReadWriteCfg(hDev, dwOffset, &val, bits/8, WDC_WRITE); }

DECLARE_PCI_READ_CFG_BY_SLOT(8, BYTE)
DECLARE_PCI_READ_CFG_BY_SLOT(16, WORD)
DECLARE_PCI_READ_CFG_BY_SLOT(32, UINT32)
DECLARE_PCI_READ_CFG_BY_SLOT(64, UINT64)
DECLARE_PCI_WRITE_CFG_BY_SLOT(8, BYTE)
DECLARE_PCI_WRITE_CFG_BY_SLOT(16, WORD)
DECLARE_PCI_WRITE_CFG_BY_SLOT(32, UINT32)
DECLARE_PCI_WRITE_CFG_BY_SLOT(64, UINT64)

DECLARE_PCI_READ_CFG(8, BYTE)
DECLARE_PCI_READ_CFG(16, WORD)
DECLARE_PCI_READ_CFG(32, UINT32)
DECLARE_PCI_READ_CFG(64, UINT64)
DECLARE_PCI_WRITE_CFG(8, BYTE)
DECLARE_PCI_WRITE_CFG(16, WORD)
DECLARE_PCI_WRITE_CFG(32, UINT32)
DECLARE_PCI_WRITE_CFG(64, UINT64)

/*
 * Read/Write PCMCIA attribute space tuples
 */
static DWORD WDC_PcmciaReadWriteAttribSpace(WDC_DEVICE_HANDLE hDev,
    DWORD dwOffset, PVOID pData, DWORD dwBytes, WDC_DIRECTION direction)
{
    DWORD dwStatus;
    PWDC_DEVICE pDev = (PWDC_DEVICE)hDev;
    WD_PCMCIA_CONFIG_DUMP pcmciaCnf;

#if defined(DEBUG)
    if (!RWParamsValidate(hDev, pData))
    {
        dwStatus = WD_INVALID_PARAMETER;
        goto Error;
    }
#endif
    
    BZERO(pcmciaCnf);
    pcmciaCnf.pcmciaSlot = pDev->slot.pcmciaSlot;
    pcmciaCnf.pBuffer = pData;
    pcmciaCnf.dwOffset = dwOffset;
    pcmciaCnf.dwBytes = dwBytes;
    pcmciaCnf.fIsRead = (WDC_READ == direction);
    dwStatus = WD_PcmciaConfigDump(WDC_GetWDHandle(), &pcmciaCnf);

    if (WD_STATUS_SUCCESS != dwStatus)
    {
        WdcSetLastErrStr("Error 0x%lx - %s\n", dwStatus, Stat2Str(dwStatus));
        goto Error;
    }

    return WD_STATUS_SUCCESS;

Error:
    WDC_Err("WDC_PcmciaReadWriteAttribSpace: Failed %s %ld bytes %s offset"
        "0x%lx of the PCMCIA attribute space.\n%s",
        (WDC_READ == direction) ? "reading" : "writing", dwBytes,
        (WDC_READ == direction) ? "from" : "to", dwOffset, WdcGetLastErrStr());

    return dwStatus;
}

DWORD DLLCALLCONV WDC_PcmciaReadAttribSpace(WDC_DEVICE_HANDLE hDev,
    DWORD dwOffset, PVOID pData, DWORD dwBytes)
{
    return WDC_PcmciaReadWriteAttribSpace(hDev, dwOffset, pData, dwBytes,
        WDC_READ);
}

DWORD DLLCALLCONV WDC_PcmciaWriteAttribSpace(WDC_DEVICE_HANDLE hDev,
    DWORD dwOffset, PVOID pData, DWORD dwBytes)
{
    return WDC_PcmciaReadWriteAttribSpace(hDev, dwOffset, pData, dwBytes,
        WDC_WRITE);
}

static DWORD WDC_PcmciaControl(WDC_DEVICE_HANDLE hDev,
    WD_PCMCIA_ACC_SPEED speed, WD_PCMCIA_ACC_WIDTH width, WD_PCMCIA_VPP vpp,
    DWORD dwCardBase)
{
    PWDC_DEVICE pDev = (PWDC_DEVICE)hDev;
    WD_PCMCIA_CONTROL control;

#if defined(DEBUG)
    if (!WdcIsValidDevHandle(hDev))
        return WD_INVALID_PARAMETER;
#endif
    BZERO(control);
    control.pcmciaSlot = pDev->slot.pcmciaSlot;
    control.uAccessSpeed = (BYTE)speed;
    control.uBusWidth = (BYTE)width;
    control.uVppLevel = (BYTE)vpp;
    control.dwCardBase = dwCardBase;
    return WD_PcmciaControl(WDC_GetWDHandle(), &control);       
}    

DWORD DLLCALLCONV WDC_PcmciaSetWindow(WDC_DEVICE_HANDLE hDev,
    WD_PCMCIA_ACC_SPEED speed, WD_PCMCIA_ACC_WIDTH width, DWORD dwCardBase)
{
    return WDC_PcmciaControl(hDev, speed, width, WD_PCMCIA_VPP_DEFAULT,
        dwCardBase);
}

DWORD DLLCALLCONV WDC_PcmciaSetVpp(WDC_DEVICE_HANDLE hDev, WD_PCMCIA_VPP vpp)
{
    return WDC_PcmciaControl(hDev, WD_PCMCIA_ACC_SPEED_DEFAULT,
        WD_PCMCIA_ACC_WIDTH_DEFAULT, vpp, 0);
}

