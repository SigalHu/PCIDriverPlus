/* Jungo Confidential. Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */

#ifdef WDLOG
#undef WDLOG
#endif

#include "windrvr.h"
#include <stdio.h>
#if defined(LINUX) || defined(SOLARIS) 
#include <stdarg.h>
#endif 
FILE *fpWdLog;

#define STR(s) ((s) ? (s) : "(null)")

static int print_ioctl_data(DWORD dwIoctl, PVOID src, DWORD src_bytes);

DWORD DLLCALLCONV WD_LogStart(const char *sFileName, const char *sMode)
{
    if (!sMode)
        sMode = "w";
    fpWdLog = fopen(sFileName, sMode);
    if (!fpWdLog)
        return WD_SYSTEM_INTERNAL_ERROR;
    return WD_STATUS_SUCCESS;
}

VOID DLLCALLCONV WD_LogStop(void)
{
    if (fpWdLog)
        fclose(fpWdLog);
    fpWdLog = NULL;
}

DWORD DLLCALLCONV WdFunctionLog(DWORD dwIoctl, HANDLE h, PVOID pParam,
    DWORD dwSize, BOOL fWait)
{
    DWORD rc;
    /* Don't log debug messages - too messy */
    DWORD skip = (dwIoctl == IOCTL_WD_DEBUG_ADD); 
    
    if (fpWdLog && !skip)
    {
        fprintf(fpWdLog, "\nLogging ioctl %x (%x), handle %p, size %lx\n",
            (UINT32)dwIoctl, WD_CTL_DECODE_FUNC((UINT32)dwIoctl), h, dwSize);
        print_ioctl_data(dwIoctl, pParam, dwSize);
    }
    
    rc = WD_FUNCTION_LOCAL(dwIoctl, h, pParam, dwSize, fWait);

    if (fpWdLog && !skip)
    {
        fprintf(fpWdLog, "ioctl %x (%x) returned status %lx\n",
            (UINT32)dwIoctl, WD_CTL_DECODE_FUNC((UINT32)dwIoctl), rc);
        print_ioctl_data(dwIoctl, pParam, dwSize);
    }
    return rc;
}

HANDLE DLLCALLCONV WD_OpenLog(void)
{
    HANDLE hWD = WD_Open();
    if (fpWdLog)
        fprintf(fpWdLog, "WD_Open() returned %p\n", hWD);
    return hWD;
}

void DLLCALLCONV WD_CloseLog(HANDLE hWD)
{
    WD_Close(hWD);
    if (fpWdLog)
        fprintf(fpWdLog, "WD_Close() called for handle %p\n", hWD);
}

VOID DLLCALLCONV WD_LogAdd(const char *sFormat, ...)
{
    va_list ap;
    
    if (!fpWdLog)
        return;

    va_start(ap, sFormat);
    vfprintf(fpWdLog, sFormat, ap);
    va_end(ap);
}

static VOID WD_LogAddIdented(int ident, const char *sFormat, ...)
{
    va_list ap;

    if (!fpWdLog)
        return;

    va_start(ap, sFormat);
    for (; ident>0; ident--)
    {
        fprintf(fpWdLog, "  ");
    }
    vfprintf(fpWdLog, sFormat, ap);
    va_end(ap);
    fflush(fpWdLog);
}


#define LOG WD_LogAddIdented

static void log_hexbuf(PVOID src, DWORD src_bytes, int ident)
{
    DWORD i;

    LOG(ident, "0x ");
    for(i=0; i<src_bytes; i++)
        LOG(0, "%02x ", ((BYTE *)src)[i]);
    LOG(0, "\n");
}

static void log_WD_PCI_ID(WD_PCI_ID *p, int ident)
{
    LOG(ident, "WD_PCI_ID:\n");
    LOG(ident+1, "dwVendorId=%lx, dwDeviceId=%lx\n", p->dwVendorId,
        p->dwDeviceId);
}

static void log_WD_PCI_SLOT(WD_PCI_SLOT *p, int ident)
{
    LOG(ident, "WD_PCI_SLOT:\n");
    LOG(ident+1, "dwBus=%lx, dwSlot=%lx, dwFunction=%lx\n", p->dwBus, p->dwSlot,
        p->dwFunction);
}

static void log_WD_USB_ID(WD_USB_ID *p, int ident)
{
    LOG(ident, "WD_USB_ID:\n");
    LOG(ident+1, "dwVendorId=%lx, dwProductId=%lx\n", p->dwVendorId,
        p->dwProductId);
}

static void log_WD_PCMCIA_ID(WD_PCMCIA_ID *p, int ident)
{
    LOG(ident, "WD_PCMCIA_ID:\n");
    LOG(ident+1, "wManufacturerId=%04hx, wCardId=%04hx\n", p->wManufacturerId,
        p->wCardId);
}

static void log_WD_PCMCIA_SLOT(WD_PCMCIA_SLOT *p, int ident)
{
    LOG(ident, "WD_PCMCIA_SLOT:\n");
    LOG(ident+1, "uBus=%x, uSocket=%x, uFunction=%x\n", (UINT32)p->uBus,
        (UINT32)p->uSocket, (UINT32)p->uFunction);
}

static void log_WDU_MATCH_TABLE(WDU_MATCH_TABLE *p, int ident)
{
    LOG(ident, "WDU_MATCH_TABLE:\n");
    LOG(ident+1, "wVendorId=%hx, wProductId=%hx, bDeviceClass=%x, "
        "bDeviceSubClass=%x\n",
        p->wVendorId, p->wProductId, (UINT32)p->bDeviceClass,
        (UINT32)p->bDeviceSubClass);
    LOG(ident+1, "bInterfaceClass=%x, bInterfaceSubClass=%x, "
        "bInterfaceProtocol=%x\n",
        (UINT32)p->bInterfaceClass, (UINT32)p->bInterfaceSubClass,
        (UINT32)p->bInterfaceProtocol);
}

static void log_WD_ITEMS(WD_ITEMS *p, int ident)
{
    LOG(ident, "WD_ITEMS:\n");
    LOG(ident+1, "item=%lx, fNotSharable=%lx, dwOptions=%lx\n", 
        p->item, p->fNotSharable, p->dwOptions);

    switch(p->item)
    {
    case ITEM_INTERRUPT:
        LOG(ident+1, "union I <ITEM_INTERRUPT>:\n");
        LOG(ident+2, "dwInterrupt=%lx, dwOptions=%lx, hInterrupt=%lx\n",
            p->I.Int.dwInterrupt, p->I.Int.dwOptions, p->I.Int.hInterrupt);
        break;

    case ITEM_MEMORY:
        LOG(ident+1, "union I <ITEM_MEMORY>:\n");
#if defined(KERNEL_64BIT)
        LOG(ident+2, "dwPhysicalAddr=%lx, dwBytes=%lx, dwTransAddr=%x:%x, "
            "dwUserDirectAddr=%lx, dwBar=%lx\n",
            p->I.Mem.dwPhysicalAddr, p->I.Mem.dwBytes, 
            (UINT32)(p->I.Mem.dwTransAddr>>32), (UINT32)p->I.Mem.dwTransAddr, 
            p->I.Mem.dwUserDirectAddr, p->I.Mem.dwBar);
#else
        LOG(ident+2, "dwPhysicalAddr=%lx, dwBytes=%lx, dwTransAddr=%lx, "
        "dwUserDirectAddr=%lx, dwBar=%lx\n",
            p->I.Mem.dwPhysicalAddr, p->I.Mem.dwBytes, p->I.Mem.dwTransAddr,  
            p->I.Mem.dwUserDirectAddr, p->I.Mem.dwBar);
#endif
        break;

    case ITEM_IO:
        LOG(ident+1, "union I <ITEM_IO>:\n");
#if defined(KERNEL_64BIT)
        LOG(ident+2, "dwAddr=%x:%x, dwBytes=%lx, dwBar=%lx\n", 
            (UINT32)(p->I.IO.dwAddr>>32), (UINT32)(p->I.IO.dwAddr), 
            p->I.IO.dwBytes, p->I.IO.dwBar);
#else
        LOG(ident+2, "dwAddr=%lx, dwBytes=%lx, dwBar=%lx\n", p->I.IO.dwAddr,
            p->I.IO.dwBytes, p->I.IO.dwBar);
#endif
        break;
        
    case ITEM_BUS:
        LOG(ident+1, "union I <ITEM_BUS>:\n");
        LOG(ident+2, "dwBusType=%lx, dwBusNum=%lx, dwSlotFunc=%lx\n",
            p->I.Bus.dwBusType, p->I.Bus.dwBusNum, p->I.Bus.dwSlotFunc);
        break;

    case ITEM_NONE:
    default:
        LOG(ident+1, "union I <ITEM_NONE>:\n");
        log_hexbuf(&p->I, sizeof(p->I), ident+2);
        break;
    }
}

static void log_WD_CARD(WD_CARD *p, int ident)
{
    DWORD i;
    LOG(ident, "WD_CARD:\n");
    LOG(ident+1, "dwItems=%lx\n", p->dwItems);
    for (i=0; i<p->dwItems; i++)
    {
        LOG(ident+1, "[%lx]", i);
        log_WD_ITEMS(&p->Item[i], ident+1);
    }
}

static void log_WD_TRANSFER(WD_TRANSFER *p, int ident)
{
    LOG(ident, "WD_TRANSFER:\n");
#if defined(KERNEL_64BIT)
    LOG(ident+1, "dwPort=%x:%x, cmdTrans=%lx, dwBytes=%lx, fAutoinc=%lx, "
        "dwOptions=%lx\n", 
        (UINT32)(p->dwPort>>32), (UINT32)(p->dwPort), p->cmdTrans, p->dwBytes,
        p->fAutoinc, p->dwOptions);
#else
    LOG(ident+1, "dwPort=%lx, cmdTrans=%lx, dwBytes=%lx, fAutoinc=%lx, "
        "dwOptions=%lx\n", 
        p->dwPort, p->cmdTrans, p->dwBytes, p->fAutoinc, p->dwOptions);
#endif
    LOG(ident+1, "Data=");
    log_hexbuf(&p->Data, sizeof(p->Data), 0);
}

static void log_WD_CLEANUP_SETUP(WD_CARD_CLEANUP *p, int ident)
{
    DWORD i;
    LOG(ident, "WD_CLEANUP_SETUP:\n");
    LOG(ident+1, "hCard=%lx, dwOptions=%lx, dwCmds=%lx\n",
        p->hCard, p->dwOptions, p->dwCmds);
    for (i=0; i<p->dwCmds; i++)
    {
        LOG(ident+1, "[%lx]", i);
        log_WD_TRANSFER(&p->Cmd[i], ident+1);
    }
}

static void log_WD_DMA_PAGE(WD_DMA_PAGE *p, int ident)
{
    LOG(ident, "WD_DMA_PAGE:\n");
#if defined(KERNEL_64BIT)
    LOG(ident+1, "pPhysicalAddr=%x:%x, dwBytes=%lx\n", 
        (UINT32)(p->pPhysicalAddr>>32), (UINT32)(p->pPhysicalAddr), p->dwBytes);
#else
    LOG(ident+1, "pPhysicalAddr=%lx, dwBytes=%lx\n", p->pPhysicalAddr,
        p->dwBytes);
#endif
}

static void log_WD_KERNEL_PLUGIN_CALL(WD_KERNEL_PLUGIN_CALL *p, int ident)
{
    LOG(ident, "WD_KERNEL_PLUGIN_CALL:\n");
    LOG(ident+1, "hKernelPlugIn=%lx, dwMessage=%lx, pData=%p, dwResult=%lx\n", 
        p->hKernelPlugIn, p->dwMessage, p->pData, p->dwResult);
}

static void log_WD_ISAPNP_CARD_ID(WD_ISAPNP_CARD_ID *p, int ident)
{
    LOG(ident, "WD_ISAPNP_CARD_ID:\n");
    LOG(ident+1, "cVendor=%s, dwSerial=%lx\n", STR(p->cVendor), p->dwSerial);
}

static void log_WD_ISAPNP_CARD(WD_ISAPNP_CARD *p, int ident)
{
    LOG(ident, "WD_ISAPNP_CARD:\n");
    log_WD_ISAPNP_CARD_ID(&p->cardId, ident+1); 
    LOG(ident+1, "dwLogicalDevices=%lx, bPnPVersionMajor=%lx, "
        "bPnPVersionMinor=%lx, bVendorVersionMajor=%lx, "
        "bVendorVersionMinor=%lx\n", p->dwLogicalDevices,
        (DWORD)p->bPnPVersionMajor, (UINT32)p->bPnPVersionMinor,
        (DWORD)p->bVendorVersionMajor, (DWORD)p->bVendorVersionMinor);
    LOG(ident+1, "cIdent=%s\n", STR(p->cIdent));
}

#define INIT_STRUCT_LOG(s_type) \
        s_type *p = (s_type *)src; \
        LOG(ident, #s_type ", %lx (%lx)\n", dwIoctl, \
        WD_CTL_DECODE_FUNC(dwIoctl));

static int print_ioctl_data(DWORD dwIoctl, PVOID src, DWORD src_bytes)
{
    DWORD i;
    int ident = 0;

    switch (dwIoctl)
    {
    case IOCTL_WD_CARD_REGISTER:
    case IOCTL_WD_CARD_UNREGISTER:
    {
        INIT_STRUCT_LOG(WD_CARD_REGISTER);
        log_WD_CARD(&p->Card, ident+1);
        LOG(ident+1, "fCheckLockOnly=%lx, hCard=%lx, dwOptions=%lx\n",
            p->fCheckLockOnly, p->hCard, p->dwOptions);
        LOG(ident+1, "cName=%s\n", STR(p->cName));
        LOG(ident+1, "cDescription=%s\n", STR(p->cDescription));
        break;
    }
    case IOCTL_WD_CARD_CLEANUP_SETUP:
    {
        INIT_STRUCT_LOG(WD_CARD_CLEANUP);
        log_WD_CLEANUP_SETUP(p, ident+1);
        break;
    }
    case IOCTL_WD_DEBUG:
    {
        INIT_STRUCT_LOG(WD_DEBUG);
        LOG(ident+1, "dwCmd=%lx, dwLevel=%lx, dwSection=%lx, "
            "dwLevelMessageBox=%lx, dwBufferSize=%lx\n",     
            p->dwCmd, p->dwLevel, p->dwSection, p->dwLevelMessageBox,
            p->dwBufferSize);
        break;
    }
    case IOCTL_WD_DEBUG_ADD:
    {
        INIT_STRUCT_LOG(WD_DEBUG_ADD);
        LOG(ident+1, "pcBuffer=%p, dwLevel=%lx, dwSection=%lx\n", p->pcBuffer,
            p->dwLevel, p->dwSection);
        break;
    }
    case IOCTL_WD_DEBUG_DUMP:
    {
        INIT_STRUCT_LOG(WD_DEBUG_DUMP);
        LOG(ident+1, "pcBuffer=%p, dwSize=%lx\n", p->pcBuffer, p->dwSize);
        break;
    }
    case IOCTL_WD_DMA_LOCK:
    case IOCTL_WD_DMA_UNLOCK:
    {
        INIT_STRUCT_LOG(WD_DMA);
#if defined(KERNEL_64BIT)
        LOG(ident+1, "hDma=%lx, pUserAddr=%p, pKernelAddr%x:%x\n", p->hDma,
            p->pUserAddr, 
            (UINT32)(p->pKernelAddr>>32), (UINT32)(p->pKernelAddr));
#else
        LOG(ident+1, "hDma=%lx, pUserAddr=%p, pKernelAddr=%lx\n", p->hDma,
            p->pUserAddr, p->pKernelAddr);
#endif
        LOG(ident+1, "dwBytes=%lx, dwOptions=%lx, dwPages=%lx, hCard=%lx\n", 
            p->dwBytes, p->dwOptions, p->dwPages, p->hCard);
        if (p->hDma)
        {
            for (i=0; i<p->dwPages; i++)
            {
                LOG(ident+1, "[%lx]", i);
                log_WD_DMA_PAGE(&p->Page[i], ident+1);
            }
        }
        break;
    }
    case IOCTL_WD_EVENT_PULL:
    case IOCTL_WD_EVENT_REGISTER:
    case IOCTL_WD_EVENT_SEND:
    case IOCTL_WD_EVENT_UNREGISTER:
    {
        INIT_STRUCT_LOG(WD_EVENT);
        LOG(ident+1, "handle=%lx, dwAction=%lx, dwStatus=%lx, dwEventId=%lx\n", 
            p->handle, p->dwAction, p->dwStatus, p->dwEventId);
        LOG(ident+1, "dwCardType=%lx, hKernelPlugIn=%lx, dwOptions=%lx\n", 
            p->dwCardType, p->hKernelPlugIn, p->dwOptions);
        if (p->dwCardType == WD_BUS_PCI)
        {
            LOG(ident+1, "union u <WD_BUS_PCI>:\n");
            log_WD_PCI_ID(&p->u.Pci.cardId, ident+2);
            log_WD_PCI_SLOT(&p->u.Pci.pciSlot, ident+2);
        }
        else if (p->dwCardType == WD_BUS_USB)
        {
            LOG(ident+1, "union u <WD_BUS_USB>:\n");
            log_WD_USB_ID(&p->u.Usb.deviceId, ident+2);
            LOG(ident+2, "dwUniqueID=%lx\n", p->u.Usb.dwUniqueID);
        }
        else
            LOG(ident+1, "ERROR: unknown bus type.\n");

        LOG(ident+1, "dwEventVer=%lx, dwNumMatchTables=%lx\n", p->dwEventVer,
            p->dwNumMatchTables);

        log_WDU_MATCH_TABLE(p->matchTables, ident+1); /* Pointer to an array
                                                         of size 1 */
        break;
    }
    case IOCTL_WD_INT_COUNT:
    case IOCTL_WD_INT_DISABLE:
    case IOCTL_WD_INT_ENABLE:
    case IOCTL_WD_INT_WAIT:
    {
        INIT_STRUCT_LOG(WD_INTERRUPT);
        LOG(ident+1, "hInterrupt=%lx, dwOptions=%lx, dwCmds=%lx\n",
            p->hInterrupt, p->dwOptions, p->dwCmds);
        for (i=0; i<p->dwCmds; i++)
        {
            LOG(ident+1, "[%lx]", i);
            log_WD_TRANSFER(&p->Cmd[i], ident+1);
        }
        log_WD_KERNEL_PLUGIN_CALL(&p->kpCall, ident+1);
        LOG(ident+1, "fEnableOk=%lx, dwCounter=%lx, dwLost=%lx, fStopped=%lx\n",
            p->fEnableOk, p->dwCounter, p->dwLost, p->fStopped);
        break;
    }
    case IOCTL_WD_ISAPNP_GET_CARD_INFO:
    {
        INIT_STRUCT_LOG(WD_ISAPNP_CARD_INFO);
        log_WD_ISAPNP_CARD_ID(&p->cardId, ident+1);
        LOG(ident+1, "dwLogicalDevice=%lx, cLogicalDeviceId=%s\n",
            p->dwLogicalDevice, STR(p->cLogicalDeviceId));
        LOG(ident+1, "dwCompatibleDevices=%lx\n", p->dwCompatibleDevices);
        for (i=0; i<p->dwCompatibleDevices; i++)
        {
            LOG(ident+1, "CompatibleDevice[%lx]=%s\n", i,
                STR(p->CompatibleDevice[i]));
        }
        LOG(ident+1, "cIdent=%s\n", STR(p->cIdent));
        log_WD_CARD(&p->Card, ident+1);
        break;
    }
    case IOCTL_WD_ISAPNP_SCAN_CARDS:
    {
        INIT_STRUCT_LOG(WD_ISAPNP_SCAN_CARDS);
        log_WD_ISAPNP_CARD_ID(&p->searchId, ident+1);
        LOG(ident+1, "dwCards=%lx\n", p->dwCards);
        for (i=0; i<p->dwCards; i++)
        {
            LOG(ident+1, "[%lx]", i);
            log_WD_ISAPNP_CARD(&p->Card[i], ident+1);
        }
        break;
    }
    case IOCTL_WD_KERNEL_PLUGIN_CALL:
    {
        log_WD_KERNEL_PLUGIN_CALL((WD_KERNEL_PLUGIN_CALL *)src, ident);
        break;
    }
    case IOCTL_WD_KERNEL_PLUGIN_CLOSE:
    case IOCTL_WD_KERNEL_PLUGIN_OPEN:
    {
        INIT_STRUCT_LOG(WD_KERNEL_PLUGIN);
        LOG(ident+1, "hKernelPlugIn=%lx, pcDriverName=%s, pcDriverPath=%s, "
            "pOpenData=%p\n",
            p->hKernelPlugIn, STR(p->pcDriverName), STR(p->pcDriverPath),
            p->pOpenData);
        break;
    }
    case IOCTL_WD_LICENSE:
    {
        INIT_STRUCT_LOG(WD_LICENSE);
        LOG(ident+1, "cLicense=%s, dwLicense=%lx, dwLicense2=%lx\n",
            STR(p->cLicense), p->dwLicense, p->dwLicense2);
        break;
    }
    case IOCTL_WD_MULTI_TRANSFER:
    {
        WD_TRANSFER *p = (WD_TRANSFER *)src;
        LOG(ident, "WD_MULTI_TRANSFER:\n");
        for (i=0; i<src_bytes / sizeof (WD_TRANSFER); i++)
        {
            LOG(ident+1, "[%lx]", i);
            log_WD_TRANSFER(&p[i], ident+1);
        }
        break;
    }
    case IOCTL_WD_PCI_CONFIG_DUMP:
    {
        INIT_STRUCT_LOG(WD_PCI_CONFIG_DUMP);
        log_WD_PCI_SLOT(&p->pciSlot, ident+1);
        LOG(ident+1, "pBuffer=%p, dwOffset=%lx, dwBytes=%lx, fIsRead=%lx, "
            "dwResult=%lx\n", 
            p->pBuffer, p->dwOffset, p->dwBytes, p->fIsRead, p->dwResult);
        break;
    }
    case IOCTL_WD_PCI_GET_CARD_INFO:
    {
        INIT_STRUCT_LOG(WD_PCI_CARD_INFO);
        log_WD_PCI_SLOT(&p->pciSlot, ident+1);
        log_WD_CARD(&p->Card, ident+1);
        break;
    }
    case IOCTL_WD_PCI_SCAN_CARDS:
    {
        INIT_STRUCT_LOG(WD_PCI_SCAN_CARDS);
        LOG(ident+1, "searchId.dwVendorId=%lx, searchId.dwDeviceId=%lx\n",
            p->searchId.dwVendorId, p->searchId.dwDeviceId);
        LOG(ident+1, "dwCards=%lx\n", p->dwCards);
        for (i=0; i<p->dwCards; i++)
        {
            LOG(ident+1, "[%lx]", i);
            log_WD_PCI_ID(&p->cardId[i], ident+1);
            LOG(ident+1, "[%lx]", i);
            log_WD_PCI_SLOT(&p->cardSlot[i], ident+1);
        }
        break;
    }
    case IOCTL_WD_PCMCIA_CONFIG_DUMP:
    {
        INIT_STRUCT_LOG(WD_PCMCIA_CONFIG_DUMP);
        log_WD_PCMCIA_SLOT(&p->pcmciaSlot, ident+1);
        LOG(ident+1, "pBuffer=%p, dwOffset=%lx, dwBytes=%lx, fIsRead=%lx, "
            "dwResult=%lx, dwOptions=%lx\n",
            p->pBuffer, p->dwOffset, p->dwBytes, p->fIsRead, p->dwResult,
            p->dwOptions);
        break;
    }
    case IOCTL_WD_PCMCIA_GET_CARD_INFO:
    {
        INIT_STRUCT_LOG(WD_PCMCIA_CARD_INFO);
        log_WD_PCMCIA_SLOT(&p->pcmciaSlot, ident+1);
        log_WD_CARD(&p->Card, ident+1);
        LOG(ident+1, "cVersion=%s, cManufacturer=%s, cProductName=%s\n", 
            STR(p->cVersion), STR(p->cManufacturer), STR(p->cProductName));
        LOG(ident+1, "wManufacturerId=%04hx, wCardId=%04hx, wFuncId=%04hx, "
            "dwOptions=%lx\n", 
            p->wManufacturerId, p->wCardId, p->wFuncId, p->dwOptions);
        break;
    }
    case IOCTL_WD_PCMCIA_SCAN_CARDS:
    {
        INIT_STRUCT_LOG(WD_PCMCIA_SCAN_CARDS);
        LOG(ident+1, "searchId.wManufacturerId=%04hx, searchId.wCardId=%04hx\n",
            p->searchId.wManufacturerId, p->searchId.wCardId);
        LOG(ident+1, "dwCards=%lx, dwOptions=%lx\n", p->dwCards, p->dwOptions);
        for (i=0; i<p->dwCards; i++)
        {
            LOG(ident+1, "[%lx]", i);
            log_WD_PCMCIA_ID(&p->cardId[i], ident+1);
            LOG(ident+1, "[%lx]", i);
            log_WD_PCMCIA_SLOT(&p->cardSlot[i], ident+1);
        }
        break;
    }
    case IOCTL_WD_SLEEP:
    {
        INIT_STRUCT_LOG(WD_SLEEP);
        LOG(ident+1, "dwMicroSeconds=%lx, dwOptions=%lx\n", p->dwMicroSeconds,
            p->dwOptions);
        break;
    }
    case IOCTL_WD_TRANSFER:
    {
        INIT_STRUCT_LOG(WD_TRANSFER);
        log_WD_TRANSFER(p, ident+1);
        break;
    }
    case IOCTL_WD_USAGE:
    {
        INIT_STRUCT_LOG(WD_USAGE);
        LOG(ident+1, "applications_num=%lx, devices_num=%lx\n",
            p->applications_num, p->devices_num);
        break;
    }
    case IOCTL_WD_VERSION:
    {
        INIT_STRUCT_LOG(WD_VERSION);
        LOG(ident+1, "dwVer=%lx, cVer=%s\n", p->dwVer, STR(p->cVer));
        break;
    }
    case IOCTL_WD_WATCH_PCI_START:
    case IOCTL_WD_WATCH_PCI_STOP:
    {
        INIT_STRUCT_LOG(WD_HS_WATCH);
        LOG(ident+1, "handle=%lx\n", p->handle);
        break;
    }
    case IOCTL_WDU_GET_DEVICE_DATA:
    {
        INIT_STRUCT_LOG(WDU_GET_DEVICE_DATA);
        LOG(ident+1, "dwUniqueID=%lx, pBuf=%p, dwBytes=%lx, dwOptions=%lx\n", 
            p->dwUniqueID, p->pBuf, p->dwBytes, p->dwOptions);
        break;
    }
    case IOCTL_WDU_SET_INTERFACE:
    {
        INIT_STRUCT_LOG(WDU_SET_INTERFACE);
        LOG(ident+1, "dwUniqueID=%lx, dwInterfaceNum=%lx, "
            "dwAlternateSetting=%lx, dwOptions=%lx\n", p->dwUniqueID,
            p->dwInterfaceNum, p->dwAlternateSetting, p->dwOptions);
        break;
    }
    case IOCTL_WDU_RESET_PIPE:
    {
        INIT_STRUCT_LOG(WDU_RESET_PIPE);
        LOG(ident+1, "dwUniqueID=%lx, dwPipeNum=%lx, dwOptions=%lx\n", 
            p->dwUniqueID, p->dwPipeNum, p->dwOptions);
        break;
    }
    case IOCTL_WDU_TRANSFER:
    {
        INIT_STRUCT_LOG(WDU_TRANSFER);
        LOG(ident+1, "dwUniqueID=%lx, dwPipeNum=%lx, fRead=%lx, "
            "dwOptions=%lx\n", p->dwUniqueID, p->dwPipeNum, p->fRead,
            p->dwOptions);
        LOG(ident+1, "pBuffer=%p, dwBufferSize=%lx, dwBytesTransferred=%lx, "
            "dwTimeout=%lx\n", 
            p->pBuffer, p->dwBufferSize, p->dwBytesTransferred, p->dwTimeout);
        log_hexbuf(p->SetupPacket, sizeof(p->SetupPacket), ident+1);
        break;
    }

    case IOCTL_WDU_HALT_TRANSFER:
    {
        INIT_STRUCT_LOG(WDU_HALT_TRANSFER);
        LOG(ident+1, "dwUniqueID=%lx, dwPipeNum=%lx, dwOptions=%lx\n", 
            p->dwUniqueID, p->dwPipeNum, p->dwOptions);
        break;
    }
    case IOCTL_WDU_WAKEUP:
    {
        INIT_STRUCT_LOG(WDU_WAKEUP);
        LOG(ident+1, "dwUniqueID=%lx, dwOptions=%lx\n", p->dwUniqueID,
            p->dwOptions);
        break;
    }
    case IOCTL_WDU_RESET_DEVICE:
    {
        INIT_STRUCT_LOG(WDU_RESET_DEVICE);
        LOG(ident+1, "dwUniqueID=%lx, dwOptions=%lx\n", p->dwUniqueID,
            p->dwOptions);
        break;
    }
    case IOCTL_WDU_SELECTIVE_SUSPEND:
    {
        INIT_STRUCT_LOG(WDU_SELECTIVE_SUSPEND);
        LOG(ident+1, "dwUniqueID=%lx, dwOptions=%lx\n", p->dwUniqueID,
            p->dwOptions);
        break;
    }
    case IOCTL_WDU_STREAM_OPEN:
    {
        INIT_STRUCT_LOG(WDU_STREAM);
        LOG(ident+1, "dwUniqueID=%lx, dwOptions=%lx\n", p->dwUniqueID,
            p->dwOptions);
        break;
    }
    case IOCTL_WDU_STREAM_CLOSE:
    {
        INIT_STRUCT_LOG(WDU_STREAM);
        LOG(ident+1, "dwUniqueID=%lx, dwOptions=%lx\n", p->dwUniqueID,
            p->dwOptions);
        break;
    }
    case IOCTL_WDU_STREAM_FLUSH:
    {
        INIT_STRUCT_LOG(WDU_STREAM);
        LOG(ident+1, "dwUniqueID=%lx, dwOptions=%lx\n", p->dwUniqueID,
            p->dwOptions);
        break;
    }
    case IOCTL_WDU_STREAM_START:
    {
        INIT_STRUCT_LOG(WDU_STREAM);
        LOG(ident+1, "dwUniqueID=%lx, dwOptions=%lx\n", p->dwUniqueID,
            p->dwOptions);
        break;
    }
    case IOCTL_WDU_STREAM_STOP:
    {
        INIT_STRUCT_LOG(WDU_STREAM);
        LOG(ident+1, "dwUniqueID=%lx, dwOptions=%lx\n", p->dwUniqueID,
            p->dwOptions);
        break;
    }
    case IOCTL_WDU_STREAM_GET_STATUS:
    {
        INIT_STRUCT_LOG(WDU_STREAM_STATUS);
        LOG(ident+1, "dwUniqueID=%lx, dwOptions=%lx\n", p->dwUniqueID,
            p->dwOptions);
        break;
    }

    default:
    {
        return WD_INVALID_PARAMETER;
    }
    }
    return WD_STATUS_SUCCESS;
}

