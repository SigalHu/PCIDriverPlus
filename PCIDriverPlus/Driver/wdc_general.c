/* Jungo Confidential. Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */

/****************************************************************
* File: wdc_general.c - Implementation of general WDC API -     *
*       init/uninit driver/WDC lib; open/close device           *
*****************************************************************/

#include "utils.h"
#include "wdc_lib.h"
#include "wdc_defs.h"
#include "wdc_err.h"

/*************************************************************
  General definitions
 *************************************************************/
#if defined(WINCE)
    #define strdup _strdup
#endif

/* Demo WinDriver license registration string */
#define WDC_DEMO_LICENSE_STR "12345abcde1234.license"

static HANDLE ghWD = INVALID_HANDLE_VALUE; 

static DWORD PciScanDevices(DWORD dwVendorId, DWORD dwDeviceId,
    WDC_PCI_SCAN_RESULT *pPciScanResult, DWORD dwOptions);
#if !defined (__KERNEL__)
static DWORD PciSlotToId(const WD_PCI_SLOT *pSlot, WD_PCI_ID *pId);
static DWORD PcmciaSlotToId(const WD_PCMCIA_SLOT *pSlot, WD_PCMCIA_ID *pId);
static PWDC_DEVICE WDC_DeviceCreate(const PVOID pDeviceInfo,
    const PVOID pDevCtx, WD_BUS_TYPE bus);
static void WDC_DeviceDestroy(PWDC_DEVICE pDev);
static DWORD WDC_DeviceOpen(WDC_DEVICE_HANDLE *phDev, const PVOID pDeviceInfo,
    const PVOID pDevCtx, const CHAR *pcKPDriverName, PVOID pKPOpenData,
    WD_BUS_TYPE bus);
static DWORD WDC_DeviceClose(WDC_DEVICE_HANDLE hDev);

static DWORD KernelPlugInOpen(WDC_DEVICE_HANDLE hDev, 
    const CHAR *pcKPDriverName, PVOID pKPOpenData);
static DWORD SetDeviceInfo(PWDC_DEVICE pDev);
#endif

/* Get handle to WinDriver (required for WD_XXX functions) */
HANDLE DLLCALLCONV WDC_GetWDHandle(void)
{
    return ghWD;
}

PVOID DLLCALLCONV WDC_GetDevContext(WDC_DEVICE_HANDLE hDev)
{
    return hDev ? ((PWDC_DEVICE)hDev)->pCtx : NULL;
}

/* Get device's bus type */
WD_BUS_TYPE DLLCALLCONV WDC_GetBusType(WDC_DEVICE_HANDLE hDev)
{
    if (!WdcIsValidDevHandle(hDev))
    {
        WDC_Err("WDC_GetBusType: %s", WdcGetLastErrStr());
        return WD_BUS_UNKNOWN;
    }

    return ((PWDC_DEVICE)hDev)->cardReg.Card.Item[0].I.Bus.dwBusType;
}

/* Sleep (default option - WDC_SLEEP_BUSY) */
DWORD DLLCALLCONV WDC_Sleep(DWORD dwMicroSecs, WDC_SLEEP_OPTIONS options)
{
    WD_SLEEP slp;
  
    BZERO(slp);
    slp.dwMicroSeconds = dwMicroSecs;
    slp.dwOptions = options;

    return WD_Sleep(ghWD, &slp);
}

/* Get WinDriver's kernel module version */
DWORD DLLCALLCONV WDC_Version(CHAR *sVersion, DWORD *pdwVersion)
{
    WD_VERSION ver;
    DWORD dwStatus;
    HANDLE hWD;

    BZERO(ver);

    if (!WdcIsValidPtr(sVersion, "NULL pointer to version results") || 
        !WdcIsValidPtr(pdwVersion, "NULL pointer to version results"))
    {
        return WD_INVALID_PARAMETER;
    }

    hWD = WD_Open();
    if (hWD == INVALID_HANDLE_VALUE)
        return WD_INVALID_HANDLE;  

    dwStatus = WD_Version(hWD, &ver);

    /* Assume there is enough room to copy */
    strcpy(sVersion, ver.cVer);
    *pdwVersion = ver.dwVer;

    WD_Close(hWD);

    return dwStatus;
}

/* -----------------------------------------------
    Open/close driver 
   ----------------------------------------------- */
DWORD DLLCALLCONV WDC_DriverOpen(WDC_DRV_OPEN_OPTIONS openOptions,
    const CHAR *sLicense)
{
#if !defined(__KERNEL__)
    DWORD dwStatus;
#endif

    if (ghWD != INVALID_HANDLE_VALUE)
        return WD_OPERATION_ALREADY_DONE;

    /* Open a handle to WinDriver */
    ghWD = WD_Open();
    if (ghWD == INVALID_HANDLE_VALUE)
        return WD_INVALID_HANDLE;  

#if defined(__KERNEL__)
    /* the two parameters below are not referenced */
    openOptions = openOptions;
    sLicense = sLicense;
    return WD_STATUS_SUCCESS;
#else
    /* Compare WinDriver files versions with running WinDriver kernel module
     * version */
    WDC_Trace("WDC_DriverOpen: user mode version - %s \n", WD_VER_STR);
    if (openOptions & WDC_DRV_OPEN_CHECK_VER)
    {
        WD_VERSION ver;
        BZERO(ver);
        dwStatus = WD_Version(ghWD, &ver);
        if ((WD_STATUS_SUCCESS != dwStatus) || (ver.dwVer < WD_VER))
        {
            WDC_Err("WDC_DriverOpen: Version check failed. Error 0x%lx - %s\n",
                dwStatus, Stat2Str(dwStatus));
            goto Error;
        }
    }
    
    /* Register WinDriver license registration string */
    if (openOptions & WDC_DRV_OPEN_REG_LIC)
    {
        WD_LICENSE lic;
        BZERO(lic);
        
        if (sLicense && strcmp(sLicense, ""))
            strcpy(lic.cLicense, sLicense);
        else
        {
            WDC_Trace("WDC_DriverOpen: No license to register -> attempting to "
                "register demo WinDriver license\n");
            strcpy(lic.cLicense, WDC_DEMO_LICENSE_STR);
        }

        dwStatus = WD_License(ghWD, &lic);
        if (WD_STATUS_SUCCESS != dwStatus)
        {
            WDC_Err("WDC_DriverOpen: Failed registering WinDriver license. "
                "Error 0x%lx - %s\n",   dwStatus, Stat2Str(dwStatus));
            goto Error;
        }
    }

    return WD_STATUS_SUCCESS;
    
Error:
    WDC_DriverClose();
    return dwStatus;
#endif
}

DWORD DLLCALLCONV WDC_DriverClose(void)
{
    WDC_SetDebugOptions(WDC_DBG_NONE, NULL);

    if (INVALID_HANDLE_VALUE != ghWD)
    {
        WD_Close(ghWD);
        ghWD = INVALID_HANDLE_VALUE;
    }

    return WD_STATUS_SUCCESS;
}

/* -----------------------------------------------
    Scan bus (PCI/PCMCIA)
   ----------------------------------------------- */
DWORD DLLCALLCONV WDC_PciScanDevices(DWORD dwVendorId, DWORD dwDeviceId,
    WDC_PCI_SCAN_RESULT *pPciScanResult)
{    
    WDC_Trace("WDC_PciScanDevices entered\n");

    return PciScanDevices(dwVendorId, dwDeviceId, pPciScanResult, 
        WD_PCI_SCAN_DEFAULT);
}

DWORD DLLCALLCONV WDC_PciScanDevicesByTopology(DWORD dwVendorId, 
    DWORD dwDeviceId, WDC_PCI_SCAN_RESULT *pPciScanResult)
{    
    WDC_Trace("WDC_PciScanDevicesByTopology entered\n");

    return PciScanDevices(dwVendorId, dwDeviceId, pPciScanResult, 
        WD_PCI_SCAN_BY_TOPOLOGY);
}

DWORD DLLCALLCONV WDC_PcmciaScanDevices(WORD wManufacturerId, WORD wDeviceId,
    WDC_PCMCIA_SCAN_RESULT *pPcmciaScanResult)
{
    DWORD dwStatus, i;
    WD_PCMCIA_SCAN_CARDS scanDevices;
    
    WDC_Trace("WDC_PcmciaScanDevices entered\n");

    if (!WdcIsValidPtr(pPcmciaScanResult,
        "NULL pointer to device scan results struct"))
    {
        return WD_INVALID_PARAMETER;
    }
    
    BZERO(scanDevices);
    scanDevices.searchId.wManufacturerId = wManufacturerId;
    scanDevices.searchId.wCardId = wDeviceId;
 
    dwStatus = WD_PcmciaScanCards(ghWD, &scanDevices);
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        WDC_Err("WDC_PcmciaScanDevices: Failed scanning PCMCIA bus. "
            "Error 0x%lx - %s\n",
            dwStatus, Stat2Str(dwStatus));
        return dwStatus;
    }

    BZERO(*pPcmciaScanResult);
    
    pPcmciaScanResult->dwNumDevices = scanDevices.dwCards;
    
    for (i = 0; i < scanDevices.dwCards; i++)
    {
        pPcmciaScanResult->deviceId[i] = scanDevices.cardId[i];
        pPcmciaScanResult->deviceSlot[i] = scanDevices.cardSlot[i];
    }

    WDC_Trace("WDC_PcmciaScanDevices: PCMCIA bus scanned successfully.\n"
        "Found %ld matching cards (manufacturer ID 0x%lx, device ID 0x%lx)\n",
        pPcmciaScanResult->dwNumDevices, wManufacturerId, wDeviceId);

    return WD_STATUS_SUCCESS;
}

/* -----------------------------------------------
    Get device's resources information (PCI/PCMCIA)
   ----------------------------------------------- */
DWORD DLLCALLCONV WDC_PciGetDeviceInfo(WD_PCI_CARD_INFO *pDeviceInfo)
{
    if (!WdcIsValidPtr(pDeviceInfo, "NULL device information pointer"))
    {
        WDC_Err("WDC_PciGetDeviceInfo: %s", WdcGetLastErrStr());
        return WD_INVALID_PARAMETER;
    }
    
    return WD_PciGetCardInfo(ghWD, pDeviceInfo);
}
        
DWORD DLLCALLCONV WDC_PcmciaGetDeviceInfo(WD_PCMCIA_CARD_INFO *pDeviceInfo)
{
    if (!WdcIsValidPtr(pDeviceInfo, "NULL device information pointer"))
    {
        WDC_Err("WDC_PcmciaDeviceInfo: %s", WdcGetLastErrStr());
        return WD_INVALID_PARAMETER;
    }
    
    return WD_PcmciaGetCardInfo(ghWD, pDeviceInfo);
}

/* -----------------------------------------------
    Scan PCI devices according to options
   ----------------------------------------------- */
static DWORD PciScanDevices(DWORD dwVendorId, DWORD dwDeviceId,
    WDC_PCI_SCAN_RESULT *pPciScanResult, DWORD dwOptions)
{
    DWORD dwStatus, i;
    WD_PCI_SCAN_CARDS scanDevices;
    
    if (!WdcIsValidPtr(pPciScanResult,
        "NULL pointer to device scan results struct"))
    {
        return WD_INVALID_PARAMETER;
    }
    
    BZERO(scanDevices);
    scanDevices.searchId.dwVendorId = dwVendorId;
    scanDevices.searchId.dwDeviceId = dwDeviceId;
    scanDevices.dwOptions = dwOptions;

    if (dwOptions != WD_PCI_SCAN_DEFAULT && 
        dwOptions != WD_PCI_SCAN_BY_TOPOLOGY)
    {
        return WD_INVALID_PARAMETER;
    }
 
    dwStatus = WD_PciScanCards(ghWD, &scanDevices);
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        WDC_Err("PciScanDevices: Failed scanning PCI bus. Error 0x%lx - %s\n",
            dwStatus, Stat2Str(dwStatus));
        return dwStatus;
    }

    BZERO(*pPciScanResult);
    pPciScanResult->dwNumDevices = scanDevices.dwCards;
    
    for (i = 0; i < scanDevices.dwCards; i++)
    {
        pPciScanResult->deviceId[i] = scanDevices.cardId[i];
        pPciScanResult->deviceSlot[i] = scanDevices.cardSlot[i];
    }

    WDC_Trace("PciScanDevices: PCI bus scanned successfully.\n"
        "Found %ld matching cards (vendor ID 0x%lx, device ID 0x%lx)\n",
        pPciScanResult->dwNumDevices, dwVendorId, dwDeviceId);

    return WD_STATUS_SUCCESS;
}

/* -----------------------------------------------
    Open/close device
   ----------------------------------------------- */
#if !defined (__KERNEL__)
DWORD DLLCALLCONV WDC_PciDeviceOpen(WDC_DEVICE_HANDLE *phDev,
    const WD_PCI_CARD_INFO *pDeviceInfo, const PVOID pDevCtx,
    PVOID reserved, const CHAR *pcKPDriverName, PVOID pKPOpenData)
{
    DWORD dwStatus;
    
    dwStatus = WDC_DeviceOpen(phDev, (const PVOID)pDeviceInfo, pDevCtx,
        pcKPDriverName, pKPOpenData, WD_BUS_PCI);
    if (WD_STATUS_SUCCESS != dwStatus)
        WDC_Err("WDC_PciDeviceOpen failed: %s", WdcGetLastErrStr());

    return dwStatus;
}

DWORD DLLCALLCONV WDC_PcmciaDeviceOpen(WDC_DEVICE_HANDLE *phDev,
    const WD_PCMCIA_CARD_INFO *pDeviceInfo, const PVOID pDevCtx,
    PVOID reserved, const CHAR *pcKPDriverName, PVOID pKPOpenData)
{
    DWORD dwStatus;
    
    dwStatus = WDC_DeviceOpen(phDev, (const PVOID)pDeviceInfo, pDevCtx,
        pcKPDriverName, pKPOpenData, WD_BUS_PCMCIA);
    if (WD_STATUS_SUCCESS != dwStatus)
        WDC_Err("WDC_PcmciaDeviceOpen failed: %s", WdcGetLastErrStr());

    return dwStatus;
}

DWORD DLLCALLCONV WDC_IsaDeviceOpen(WDC_DEVICE_HANDLE *phDev,
    const WD_CARD *pDeviceInfo, const PVOID pDevCtx,
    PVOID reserved, const CHAR *pcKPDriverName, PVOID pKPOpenData)
{
    DWORD dwStatus;
    
    dwStatus = WDC_DeviceOpen(phDev, (const PVOID)pDeviceInfo, pDevCtx,
        pcKPDriverName, pKPOpenData, WD_BUS_ISA);
    if (WD_STATUS_SUCCESS != dwStatus)
        WDC_Err("WDC_IsaDeviceOpen failed: %s", WdcGetLastErrStr());

    return dwStatus;
}

DWORD DLLCALLCONV WDC_PciDeviceClose(WDC_DEVICE_HANDLE hDev)
{
    DWORD dwStatus;

    dwStatus = WDC_DeviceClose(hDev);
    if (WD_STATUS_SUCCESS != dwStatus)
        WDC_Err("WDC_PciDeviceClose failed: %s", WdcGetLastErrStr());

    return dwStatus;
}

DWORD DLLCALLCONV WDC_PcmciaDeviceClose(WDC_DEVICE_HANDLE hDev)
{
    DWORD dwStatus;

    dwStatus = WDC_DeviceClose(hDev);
    if (WD_STATUS_SUCCESS != dwStatus)
        WDC_Err("WDC_PcmciaDeviceClose failed: %s", WdcGetLastErrStr());

    return dwStatus;
}

DWORD DLLCALLCONV WDC_IsaDeviceClose(WDC_DEVICE_HANDLE hDev)
{    
    DWORD dwStatus;

    dwStatus = WDC_DeviceClose(hDev);
    if (WD_STATUS_SUCCESS != dwStatus)
        WDC_Err("WDC_IsaDeviceClose failed: %s", WdcGetLastErrStr());

    return dwStatus;
}

/* -----------------------------------------------
    Open Kernel PlugIn Driver
   ----------------------------------------------- */
DWORD DLLCALLCONV WDC_KernelPlugInOpen(WDC_DEVICE_HANDLE hDev, 
    const CHAR *pcKPDriverName, PVOID pKPOpenData)
{
    if (!WdcIsValidDevHandle(hDev))
    {
        WDC_Err("WDC_KernelPlugInOpen: %s", WdcGetLastErrStr());
        return WD_INVALID_HANDLE;
    }
        
    if (!pcKPDriverName || !strcmp(pcKPDriverName, ""))
    {
        WdcSetLastErrStr("Error - empty kernel-plugin driver name\n");
        return WD_INVALID_PARAMETER;
    }

    return KernelPlugInOpen(hDev, pcKPDriverName, pKPOpenData);
}

static DWORD PciSlotToId(const WD_PCI_SLOT *pSlot, WD_PCI_ID *pId)
{
    DWORD dwStatus, i;
    WD_PCI_SCAN_CARDS scanDevices;
    
    BZERO(scanDevices); /* scanCards.searchId.dwVendorId/dwDeviceId = 0 - all cards */
    
    dwStatus = WD_PciScanCards(ghWD, &scanDevices);
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        WdcSetLastErrStr("Failed scanning PCI bus. Error 0x%lx - %s\n",
            dwStatus, Stat2Str(dwStatus));      
        return dwStatus;
    }

    for (i = 0; i < scanDevices.dwCards; i++)
    {
        if ((scanDevices.cardSlot[i].dwBus == pSlot->dwBus) &&
            (scanDevices.cardSlot[i].dwSlot == pSlot->dwSlot) &&
            (scanDevices.cardSlot[i].dwFunction == pSlot->dwFunction))
        {
            pId->dwVendorId = scanDevices.cardId[i].dwVendorId;
            pId->dwDeviceId = scanDevices.cardId[i].dwDeviceId;
            return WD_STATUS_SUCCESS;
        }
    }

    WdcSetLastErrStr("Failed to locate the device "
        "(bus: 0x%lx, slot: 0x%lx, function: 0x%lx)\n",
        pSlot->dwBus, pSlot->dwSlot, pSlot->dwFunction);
    
    return WD_DEVICE_NOT_FOUND;
}

static DWORD PcmciaSlotToId(const WD_PCMCIA_SLOT *pSlot, WD_PCMCIA_ID *pId)
{
    DWORD dwStatus, i;
    WD_PCMCIA_SCAN_CARDS scanDevices;
    
    BZERO(scanDevices); /* scanCards.searchId.wManufacturerId/wCardId = 0 - all cards */
    
    dwStatus = WD_PcmciaScanCards(ghWD, &scanDevices);
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        WdcSetLastErrStr("Failed scanning PCMCIA bus. ",
            "Error 0x%lx - %s\n", dwStatus, Stat2Str(dwStatus));
        return dwStatus;
    }

    for (i = 0; i < scanDevices.dwCards; i++)
    {
        if ((scanDevices.cardSlot[i].uBus == pSlot->uBus) &&
            (scanDevices.cardSlot[i].uSocket == pSlot->uSocket) &&
            (scanDevices.cardSlot[i].uFunction == pSlot->uFunction))
        {
            pId->wManufacturerId = scanDevices.cardId[i].wManufacturerId;
            pId->wCardId = scanDevices.cardId[i].wCardId;
            return WD_STATUS_SUCCESS;
        }
    }

    WdcSetLastErrStr("Failed to locate the device (socket: 0x%x, "
        "function: 0x%x)\n", pSlot->uSocket, pSlot->uFunction);
    
    return WD_DEVICE_NOT_FOUND;
}

static PWDC_DEVICE WDC_DeviceCreate(const PVOID pDeviceInfo,
    const PVOID pDevCtx, WD_BUS_TYPE bus)
{
    PWDC_DEVICE pDev;

    pDev = (PWDC_DEVICE)malloc(sizeof(WDC_DEVICE));
    if (!pDev)
    {
        WdcSetLastErrStr("WDC_DeviceCreate: Failed memory allocation\n");
        return NULL;
    }
    
    BZERO(*pDev);

    switch (bus)
    {
    case WD_BUS_PCI:
    {
        WD_PCI_CARD_INFO *pInfo = (WD_PCI_CARD_INFO*)pDeviceInfo;
        pDev->slot.pciSlot = pInfo->pciSlot;
        pDev->cardReg.Card = pInfo->Card;

        if (WD_STATUS_SUCCESS != PciSlotToId(&pDev->slot.pciSlot, &pDev->id.pciId))
            goto Error;

        break;
    }
    case WD_BUS_PCMCIA:
    {
        WD_PCMCIA_CARD_INFO *pInfo = (WD_PCMCIA_CARD_INFO*)pDeviceInfo;
        pDev->slot.pcmciaSlot = pInfo->pcmciaSlot;
        pDev->cardReg.Card = pInfo->Card;

        if (WD_STATUS_SUCCESS != PcmciaSlotToId(&pDev->slot.pcmciaSlot, &pDev->id.pcmciaId))
            goto Error;

        break;
    }
    case WD_BUS_ISA:
    {
        pDev->cardReg.Card = *(WD_CARD*)pDeviceInfo;
        break;
    }
    default:
        WdcSetLastErrStr("Error - Invalid bus type (0x%lx)\n", bus);
        goto Error;
        break;
    }

    pDev->pCtx = pDevCtx;

    return pDev;

Error:
    WDC_DeviceDestroy(pDev);

    return NULL;
}

static DWORD WDC_DeviceOpen(WDC_DEVICE_HANDLE *phDev, const PVOID pDeviceInfo,
    const PVOID pDevCtx, const CHAR *pcKPDriverName, PVOID pKPOpenData,
    WD_BUS_TYPE bus)
{
    DWORD dwStatus;
    PWDC_DEVICE pDev;

    if (!WdcIsValidPtr(phDev, "NULL device handle pointer") ||
        !WdcIsValidPtr(pDeviceInfo, "NULL device information pointer"))
    {
        return WD_INVALID_PARAMETER;
    }

    *phDev = NULL;
    
    pDev = WDC_DeviceCreate(pDeviceInfo, pDevCtx, bus);
    if (!pDev)
        return WD_INSUFFICIENT_RESOURCES;

    dwStatus = WD_CardRegister(ghWD, &pDev->cardReg);
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        WdcSetLastErrStr("Failed registering the device. Error 0x%lx - %s\n",
            dwStatus, Stat2Str(dwStatus));
        goto Error;
    }
    
    dwStatus = SetDeviceInfo(pDev);
    if (WD_STATUS_SUCCESS != dwStatus)
        goto Error;
    
    /* Set device handle before KP open to enable passing &hDev as pOpenData */
    *phDev = (WDC_DEVICE_HANDLE)pDev;
    
    if (pcKPDriverName && strcmp(pcKPDriverName, ""))
    {
        dwStatus = KernelPlugInOpen(pDev, pcKPDriverName, pKPOpenData);
        if(dwStatus != WD_STATUS_SUCCESS)
            goto Error; 
    }
    
    return WD_STATUS_SUCCESS;
    
Error:
    WDC_DeviceClose((WDC_DEVICE_HANDLE)pDev);

    *phDev = NULL;
    
    return dwStatus;
}

static void WDC_DeviceDestroy(PWDC_DEVICE pDev)
{ 
    if (!pDev)
        return;
    
    if (pDev->pAddrDesc)
        free(pDev->pAddrDesc);

    free(pDev);
}

static DWORD WDC_DeviceClose(WDC_DEVICE_HANDLE hDev)
{
    DWORD dwStatus = WD_STATUS_SUCCESS;
    PWDC_DEVICE pDev = (PWDC_DEVICE)hDev;
    
    if (!WdcIsValidDevHandle(hDev))
        return WD_INVALID_PARAMETER;

    if (pDev->hIntThread)
    {
        dwStatus = WDC_IntDisable(hDev);
        if (WD_STATUS_SUCCESS != dwStatus)
        {
            WdcSetLastErrStr("Failed disabling interrupt. Error 0x%lx - %s\n",
                dwStatus, Stat2Str(dwStatus));
        }
    }

    if (pDev->hEvent)
    {
        dwStatus = WDC_EventUnregister(hDev);
        if (WD_STATUS_SUCCESS != dwStatus)
        {
            WdcSetLastErrStr("Failed unregistering events. Error 0x%lx - %s\n",
                dwStatus, Stat2Str(dwStatus));
        }
    }
    
    if (WDC_IS_KP(pDev))
    {
        dwStatus = WD_KernelPlugInClose(ghWD, &pDev->kerPlug);
        if (WD_STATUS_SUCCESS != dwStatus)
        {
            WdcSetLastErrStr("Failed closing Kernel PlugIn handle %p.\n"
                "Error 0x%lx - %s\n", WDC_GET_KP_HANDLE(pDev), dwStatus,
                Stat2Str(dwStatus));
        }
    }

    if (pDev->kerPlug.pcDriverName)
        free(pDev->kerPlug.pcDriverName);

    if (WDC_GET_CARD_HANDLE(pDev))
    {
        dwStatus = WD_CardUnregister(ghWD, &pDev->cardReg);
        if (WD_STATUS_SUCCESS != dwStatus)
        {
            WdcSetLastErrStr("Failed unregistering the device (handle %p). "
                "Error 0x%lx - %s\n",
                WDC_GET_CARD_HANDLE(pDev), dwStatus, Stat2Str(dwStatus));
        }
    }

    WDC_DeviceDestroy(pDev);
    
    return dwStatus;
}

static DWORD KernelPlugInOpen(WDC_DEVICE_HANDLE hDev, 
    const CHAR *pcKPDriverName, PVOID pKPOpenData)
{
    DWORD dwStatus;
    PWDC_DEVICE pDev = (PWDC_DEVICE)hDev;

    pDev->kerPlug.pcDriverName = strdup(pcKPDriverName);
    pDev->kerPlug.pOpenData = pKPOpenData;
        
    dwStatus = WD_KernelPlugInOpen(ghWD, &pDev->kerPlug);

    if (WD_STATUS_SUCCESS != dwStatus)
    {
        free(pDev->kerPlug.pcDriverName);
        pDev->kerPlug.pcDriverName = NULL;
        WdcSetLastErrStr("Failed opening a Kernel PlugIn handle. "
            "Error 0x%lx - %s\n", dwStatus, Stat2Str(dwStatus));
    }

    return dwStatus;    
}

#define MAX_ADDR_SPACE_NUM WD_CARD_ITEMS
static DWORD SetDeviceInfo(PWDC_DEVICE pDev)
{
    DWORD i;
    DWORD dwNumAddrs = 0;
    DWORD dwNumItems = pDev->cardReg.Card.dwItems;
    WD_ITEMS *pItem = pDev->cardReg.Card.Item;
    WDC_ADDR_DESC addrDescs[MAX_ADDR_SPACE_NUM];
    WDC_ADDR_DESC *pAddrDesc;

    BZERO(addrDescs);
    for (i=0; i<dwNumItems; i++, pItem++)
    {
        switch (pItem->item)
        {
        case ITEM_INTERRUPT:
            pDev->Int.hInterrupt = pItem->I.Int.hInterrupt;
            pDev->Int.dwOptions = pItem->I.Int.dwOptions;
            break;
        case ITEM_MEMORY:
        case ITEM_IO:
            if (pItem->item == ITEM_MEMORY)
            {
                pAddrDesc = &addrDescs[pItem->I.Mem.dwBar];
                pAddrDesc->fIsMemory = TRUE;
                pAddrDesc->dwAddrSpace = pItem->I.Mem.dwBar;
                pAddrDesc->kptAddr = pItem->I.Mem.dwTransAddr;
                pAddrDesc->dwUserDirectMemAddr = pItem->I.Mem.dwUserDirectAddr;
                pAddrDesc->dwBytes = pItem->I.Mem.dwBytes;
            }
            else
            {
                pAddrDesc = &addrDescs[pItem->I.IO.dwBar];
                pAddrDesc->fIsMemory = FALSE;
                pAddrDesc->dwAddrSpace = pItem->I.IO.dwBar;
                pAddrDesc->kptAddr = pItem->I.IO.dwAddr;
                pAddrDesc->dwBytes = pItem->I.IO.dwBytes;
            }

            pAddrDesc->dwItemIndex = i;
            dwNumAddrs = MAX(pAddrDesc->dwAddrSpace + 1, dwNumAddrs);
            break;
        default:
            break;
        }
    }

    if (dwNumAddrs)
    {
        pDev->pAddrDesc = (WDC_ADDR_DESC*)malloc(dwNumAddrs * sizeof(addrDescs[0]));
        if (!pDev->pAddrDesc)
        {
            WdcSetLastErrStr("SetDeviceInfo: Failed memory allocation\n");
            return WD_INSUFFICIENT_RESOURCES;
        }
        memcpy(pDev->pAddrDesc, addrDescs, dwNumAddrs * sizeof(addrDescs[0]));
        pDev->dwNumAddrSpaces = dwNumAddrs;
    }

    return WD_STATUS_SUCCESS;
}

#endif

/* -----------------------------------------------
    Set card cleanup commands
   ----------------------------------------------- */
DWORD WDC_CardCleanupSetup(WDC_DEVICE_HANDLE hDev, WD_TRANSFER *Cmd,
    DWORD dwCmds, BOOL bForceCleanup)
{
    PWDC_DEVICE pDev = (PWDC_DEVICE)hDev;
    WD_CARD_CLEANUP cardCleanup;

    if (!Cmd || !dwCmds)
        return WD_INVALID_PARAMETER;
    
    cardCleanup.hCard = WDC_GET_CARD_HANDLE(pDev);
    cardCleanup.Cmd = Cmd;
    cardCleanup.dwCmds = dwCmds;
    cardCleanup.dwOptions = bForceCleanup ? WD_FORCE_CLEANUP : 0;
    
    return WD_CardCleanupSetup(ghWD, &cardCleanup);
}

/* -----------------------------------------------
    Send Kernel PlugIn messages
   ----------------------------------------------- */
DWORD DLLCALLCONV WDC_CallKerPlug(WDC_DEVICE_HANDLE hDev, DWORD dwMsg,
    PVOID pData, PDWORD pdwResult)
{
    DWORD dwStatus;
    PWDC_DEVICE pDev = (PWDC_DEVICE)hDev;
    WD_KERNEL_PLUGIN_CALL kpCall;

    BZERO(kpCall);
    kpCall.hKernelPlugIn = WDC_GET_KP_HANDLE(pDev);
    kpCall.dwMessage = dwMsg;
    kpCall.pData = pData;
    dwStatus = WD_KernelPlugInCall(ghWD, &kpCall);
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        WDC_Err("WDC_CallKerPlug: Failed sending 0x%lx message to the Kernel PlugIn (%s).\n"
            "Error 0x%lx - %s\n",
            dwMsg, pDev->kerPlug.pcDriverName, dwStatus, Stat2Str(dwStatus));
    }
    else if (pdwResult)
        *pdwResult = kpCall.dwResult;

    return dwStatus;
}
