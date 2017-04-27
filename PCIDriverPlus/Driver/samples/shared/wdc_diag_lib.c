/* Jungo Confidential. Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */

/******************************************************************************
*  File: wdc_diag_lib.c - Implementation of shared WDC PCI, PCMCIA and ISA    *
*        devices' user-mode diagnostics API.                                  *
*******************************************************************************/

#if !defined(__KERNEL__)

#include <stdio.h>
#include "status_strings.h"
#include "pci_regs.h"
#include "pccard_cis.h"
#include "diag_lib.h"
#include "wdc_diag_lib.h"
#include "wdc_defs.h"

/*************************************************************
  General definitions
 *************************************************************/
/* Error messages display */
#define WDC_DIAG_ERR printf

/*************************************************************
  Global variables
 *************************************************************/
/* User's input command */
static CHAR gsInput[256];

/* -----------------------------------------------
    All buses (PCI/PCMCIA/ISA)
   ----------------------------------------------- */

/* Print device's resources information to file.
   For a registered device (hCard != 0), print also kernel and user-mode
   mappings of memory address items */
static void WDC_DIAG_DeviceResourcesPrint(const WD_CARD *pCard, DWORD hCard,
    FILE *fp)
{
    int resources;
    DWORD i;
    const WD_ITEMS *pItem;

    if (!pCard)
    {
        WDC_DIAG_ERR("WDC_DIAG_DeviceResourcesPrint: Error - NULL card "
            "pointer\n");
        return;
    }

    for (i = 0, resources = 0; i < pCard->dwItems; i++)
    {
        pItem = &pCard->Item[i];
        switch (pItem->item)
        {
        case ITEM_MEMORY:
            resources ++;
            fprintf(fp,"    Memory range [BAR %ld]: base 0x%lX, size 0x%lX\n",
                pItem->I.Mem.dwBar, pItem->I.Mem.dwPhysicalAddr, 
                pItem->I.Mem.dwBytes);
            if (hCard) /* Registered device */
            {
                fprintf(fp,"        Kernel-mode address mapping: 0x%"KPRI"X\n",
                    pItem->I.Mem.dwTransAddr);
                fprintf(fp,"        User-mode address mapping: 0x%lX\n",
                    pItem->I.Mem.dwUserDirectAddr);
            }
            break;
        case ITEM_IO:
            resources ++;
            fprintf(fp, "    I/O range [BAR %ld]: base 0x%"KPRI"X, size "
                "0x%lX\n", pItem->I.IO.dwBar, pItem->I.IO.dwAddr,
                pItem->I.IO.dwBytes);
            break;
        case ITEM_INTERRUPT:
            resources ++;
            fprintf(fp, "    Interrupt: IRQ %ld\n", pItem->I.Int.dwInterrupt);
            fprintf(fp, "    Interrupt Options (supported interrupts):\n");
            if (WDC_INT_IS_MSI(pItem->I.Int.dwOptions))
            {
                fprintf(fp, "        %s\n",
                    WDC_DIAG_IntTypeDescriptionGet(pItem->I.Int.dwOptions));
            }
            /* According to the MSI specification, it is recommended that
             * a PCI device will support both MSI/MSI-X and level-sensitive
             * interrupts, and allow the operating system to choose which
             * type of interrupt to use. */
            if (pItem->I.Int.dwOptions & INTERRUPT_LEVEL_SENSITIVE)
            {
                fprintf(fp, "        %s\n",
                    WDC_DIAG_IntTypeDescriptionGet(INTERRUPT_LEVEL_SENSITIVE));
            }
            else if (!WDC_INT_IS_MSI(pItem->I.Int.dwOptions))
            /* MSI/MSI-X interrupts are always edge-triggered, so there is no
             * no need to display a specific edge-triggered indication for
             * such interrupts. */
            {
                fprintf(fp, "        %s\n",
                    WDC_DIAG_IntTypeDescriptionGet(INTERRUPT_LATCHED));
            }
            break;      
        case ITEM_BUS:
            break;
        default:
            fprintf(fp, "    Invalid item type (0x%lx)\n", pItem->item);
            break;
        }
    }

    if (!resources)
        fprintf(fp, "    Device has no resources\n");
}

/* Print run-time registers and PCI configuration registers information */
void WDC_DIAG_RegsInfoPrint(const WDC_REG *pRegs, DWORD dwNumRegs,
    WDC_DIAG_REG_PRINT_OPTIONS options)
{
    const WDC_REG *pReg;
    BOOL fName, fDesc, fAddrSpace, fOffset, fSize, fDir;
    DWORD i;

    if (!dwNumRegs)
    {
        printf("There are currently no pre-defined registers to display\n");
        return;
    }
    
    if (!pRegs)
    {
        WDC_DIAG_ERR("WDC_DIAG_RegsInfoPrint: Error - NULL registers "
            "information pointer\n");
        return;
    }

    if (!options)
        options = WDC_DIAG_REG_PRINT_DEFAULT;

    fName = options & WDC_DIAG_REG_PRINT_NAME,
    fDesc = options & WDC_DIAG_REG_PRINT_DESC,
    fAddrSpace = options & WDC_DIAG_REG_PRINT_ADDR_SPACE,
    fOffset = options & WDC_DIAG_REG_PRINT_OFFSET,
    fSize = options & WDC_DIAG_REG_PRINT_SIZE;
    fDir = options & WDC_DIAG_REG_PRINT_DIRECTION;

    printf("%3s %-*s %-*s %-*s %-*s %-*s %s\n",
        "",
        MAX_NAME_DISPLAY, fName ? "Name" : "",
        4, fAddrSpace ? "BAR" : "",
        (int)WDC_SIZE_32 * 2 + 2, fOffset ? "Offset" : "",
        5, fSize ? "Size" : "",
        4, fDir ? "R/W" : "",
        fDesc ? "Description" : "");
        
    printf("%3s %-*s %-*s %-*s %-*s %-*s %s\n",
        "",
        MAX_NAME_DISPLAY, fName ? "----" : "",
        4, fAddrSpace ? "---" : "",
        (int)WDC_SIZE_32 * 2 + 2, fOffset ? "------" : "",
        5, fSize ? "----" : "",
        4, fDir ? "---" : "",
        fDesc ? "-----------" : "");
    
    for (i = 1, pReg = pRegs; i <= dwNumRegs; i++, pReg++)
    {
        printf("%2ld. ", i);
        
        if (fName)
            printf("%-*.*s ", MAX_NAME_DISPLAY, MAX_NAME_DISPLAY, pReg->sName);
        else
            printf("%*s ", MAX_NAME_DISPLAY, "");
        if (fAddrSpace && (WDC_AD_CFG_SPACE != pReg->dwAddrSpace))
            printf("%2ld %*s", pReg->dwAddrSpace, 2, "");
        else
            printf("%4s ", "");
        if (fOffset)
            printf("0x%-*lX ", (int)WDC_SIZE_32 * 2, pReg->dwOffset);
        else
            printf("%*s ", (int)WDC_SIZE_32 * 2 + 2, "");
        if (fSize)
            printf("%*ld %*s", 2, pReg->dwSize, 3, "");
        else
            printf("%*s ", 5, "");
        if (fDir)
        {
            printf("%-*s ", 4,
                (WDC_READ_WRITE == pReg->direction) ? "RW" :
                (WDC_READ == pReg->direction) ? "R" : "W");
        }
        else
            printf("%*s ", 4, "");
        if (fDesc)
            printf("%.*s", 36, pReg->sDesc);
        
        printf("\n");
    }
}

/* Set address access mode */
BOOL WDC_DIAG_SetMode(WDC_ADDR_MODE *pMode)
{
    int option = 0;
    
    if (!pMode)
    {
        WDC_DIAG_ERR("WDC_DIAG_SetMode: Error - NULL mode pointer\n");
        return FALSE;
    }
    
    printf("\n");
    printf("Select read/write mode:\n");
    printf("-----------------------\n");
    printf("1. 8 bits (%ld bytes)\n", WDC_SIZE_8);
    printf("2. 16 bits (%ld bytes)\n", WDC_SIZE_16);
    printf("3. 32 bits (%ld bytes)\n", WDC_SIZE_32);
    printf("4. 64 bits (%ld bytes)\n", WDC_SIZE_64);
    printf("\n");
    
    printf("Enter option or 0 to cancel: ");
    fgets(gsInput, sizeof(gsInput), stdin);
    if (sscanf(gsInput, "%d", &option) < 1)
    {
        printf("Invalid input\n");
        return FALSE;
    }

    if (!option)
        return FALSE;

    switch (option)
    {
    case 1:
        *pMode = WDC_MODE_8;
        break;
    case 2:
        *pMode = WDC_MODE_16;
        break;
    case 3:
        *pMode = WDC_MODE_32;
        break;
    case 4:
        *pMode = WDC_MODE_64;
        break;
    default:
        printf("Invalid selection\n");
        return FALSE;
    }

    return TRUE;
}

/* Get data for address write operation from user */
/* Data size (dwSize) should be WDC_SIZE_8, WDC_SIZE_16, WDC_SIZE_32 or WDC_SIZE_64 */
BOOL WDC_DIAG_InputWriteData(PVOID pData, WDC_ADDR_SIZE dwSize)
{
    UINT64 u64Data, u64MaxVal;

    if (!pData)
    {
        WDC_DIAG_ERR("WDC_DIAG_InputWriteData: Error - NULL data pointer\n");
        return FALSE;
    }
    
    u64MaxVal = (dwSize >= WDC_SIZE_64) ? ~((UINT64)0) :
        ((UINT64)1 << (dwSize * 8)) - 1;

    printf("Enter data to write (max value: 0x%"PRI64"X) or '%c' to cancel: 0x",
        u64MaxVal, DIAG_CANCEL);
    fgets(gsInput, sizeof(gsInput), stdin);
    if (DIAG_CANCEL == tolower(gsInput[0]))
        return FALSE;
    BZERO(u64Data);
    if (sscanf(gsInput, "%"PRI64"x", &u64Data) < 1)
    {
        printf("Invalid input\n");
        return FALSE;
    }

    if (u64Data > u64MaxVal)
    {
        printf("Error: Value is too big (max legal value is 0x%"PRI64"X)\n",
            u64MaxVal);
        return FALSE;
    }

    switch (dwSize)
    {
    case WDC_SIZE_8:
        *((BYTE*)pData) = (BYTE)u64Data;
        return TRUE;
    case WDC_SIZE_16:
        *((WORD*)pData) = (WORD)u64Data;
        return TRUE;
    case WDC_SIZE_32:
        *((UINT32*)pData) = (UINT32)u64Data;
        return TRUE;
    case WDC_SIZE_64:
        *((UINT64*)pData) = (UINT64)u64Data;
        return TRUE;
    default:
        WDC_DIAG_ERR("WDC_DIAG_InputWriteData: Error - Invalid size (%ld bytes)\n", dwSize);
    }

    return FALSE;
}

/* Read/write a memory or I/O address */
void WDC_DIAG_ReadWriteAddr(WDC_DEVICE_HANDLE hDev, WDC_DIRECTION direction,
    DWORD dwAddrSpace, WDC_ADDR_MODE mode)
{
    DWORD dwStatus;
    DWORD dwOffset;
    BYTE bData = 0;
    WORD wData = 0;
    UINT32 u32Data = 0;
    UINT64 u64Data = 0;

    if (!hDev)
    {
        WDC_DIAG_ERR("WDC_DIAG_ReadWriteAddr: Error - NULL WDC device handle\n");
        return;
    }
    
    if (!DIAG_InputDWORD(&dwOffset, (WDC_READ == direction) ?
        "Enter offset to read from" : "Enter offset to write to", TRUE, 0, 0))
        return;

    if ((WDC_WRITE == direction) &&
        !WDC_DIAG_InputWriteData((WDC_MODE_8 == mode) ? (PVOID)&bData :
        (WDC_MODE_16 == mode) ? (PVOID)&wData :
        (WDC_MODE_32 == mode) ? (PVOID)&u32Data : (PVOID)&u64Data,
        WDC_ADDR_MODE_TO_SIZE(mode)))
    {
            return;
    }

    switch (mode)
    {
    case WDC_MODE_8:
        dwStatus = (WDC_READ == direction) ?
            WDC_ReadAddr8(hDev, dwAddrSpace, dwOffset, &bData) :
            WDC_WriteAddr8(hDev, dwAddrSpace, dwOffset, bData);
        if (WD_STATUS_SUCCESS == dwStatus)
        {
            printf("%s 0x%X %s offset 0x%lX in BAR %ld\n",
                (WDC_READ == direction) ? "Read" : "Wrote", (UINT32)bData,
                (WDC_READ == direction) ? "from" : "to", dwOffset, dwAddrSpace);
        }
        break;
    case WDC_MODE_16:
        dwStatus = (WDC_READ == direction) ?
            WDC_ReadAddr16(hDev, dwAddrSpace, dwOffset, &wData) :
            WDC_WriteAddr16(hDev, dwAddrSpace, dwOffset, wData);
        if (WD_STATUS_SUCCESS == dwStatus)
        {
            printf("%s 0x%hX %s offset 0x%lX in BAR %ld\n",
                (WDC_READ == direction) ? "Read" : "Wrote", wData,
                (WDC_READ == direction) ? "from" : "to", dwOffset, dwAddrSpace);
        }
        break;
    case WDC_MODE_32:
        dwStatus = (WDC_READ == direction) ?
            WDC_ReadAddr32(hDev, dwAddrSpace, dwOffset, &u32Data) :
            WDC_WriteAddr32(hDev, dwAddrSpace, dwOffset, u32Data);
        if (WD_STATUS_SUCCESS == dwStatus)
        {
            printf("%s 0x%X %s offset 0x%lX in BAR %ld\n",
                (WDC_READ == direction) ? "Read" : "Wrote", u32Data,
                (WDC_READ == direction) ? "from" : "to", dwOffset, dwAddrSpace);
        }
        break;
    case WDC_MODE_64:
        dwStatus = (WDC_READ == direction) ?
            WDC_ReadAddr64(hDev, dwAddrSpace, dwOffset, &u64Data) :
            WDC_WriteAddr64(hDev, dwAddrSpace, dwOffset, u64Data);
        if (WD_STATUS_SUCCESS == dwStatus)
        {
            printf("%s 0x%"PRI64"X %s offset 0x%lX in BAR %ld\n",
                (WDC_READ == direction) ? "Read" : "Wrote", u64Data,
                (WDC_READ == direction) ? "from" : "to", dwOffset, dwAddrSpace);
        }
        break;
    default:
        WDC_DIAG_ERR("WDC_DIAG_ReadWriteAddr: Error - Invalid mode (%d)\n", mode);
        return;
    }

    if (WD_STATUS_SUCCESS != dwStatus)
    {
        printf("Failed to %s offset 0x%lX in BAR %ld. Error 0x%lx - %s\n",
            (WDC_READ == direction) ? "read from" : "write to", dwOffset,
            dwAddrSpace, dwStatus, Stat2Str(dwStatus));
    }
}

/* Read/write a memory or I/O address OR an offset in the PCI configuration space /
   PCMCIA attribute space (dwAddrSpace == WDC_AD_CFG_SPACE) */
void WDC_DIAG_ReadWriteBlock(WDC_DEVICE_HANDLE hDev, WDC_DIRECTION direction,
    DWORD dwAddrSpace)
{
    DWORD dwStatus;
    DWORD dwOffset, dwBytes;
    const CHAR *sDir = (WDC_READ == direction) ? "read" : "write";
    PVOID pBuf = NULL;
 
    if (!hDev)
    {
        WDC_DIAG_ERR("WDC_DIAG_ReadWriteBlock: Error - NULL WDC device handle\n");
        return;
    }

    if (!DIAG_InputDWORD(&dwOffset, "offset", TRUE, 0, 0))
        return;

    if (!DIAG_InputDWORD(&dwBytes, "bytes", TRUE, 0, 0))
        return;

    if (!dwBytes)
        return;

    pBuf = malloc(dwBytes);
    if (!pBuf)
    {
        WDC_DIAG_ERR("WDC_DIAG_ReadWriteBlock: Failed allocating %s data buffer\n", sDir);
        goto Exit;
    }
    memset(pBuf, 0, dwBytes);

    if (WDC_WRITE == direction)
    {
        printf("data to write (hex format): 0x"); 
        if (!DIAG_GetHexBuffer(pBuf, dwBytes))
            goto Exit;
    }

    if (WDC_AD_CFG_SPACE == dwAddrSpace) /* Read/write a configuration/attribute space address */
    {
        WD_BUS_TYPE busType = WDC_GetBusType(hDev);

        if (WD_BUS_PCI == busType) /* Read/write PCI configuration space offset */
        {
            if (direction == WDC_READ)
                dwStatus = WDC_PciReadCfg(hDev, dwOffset, pBuf, dwBytes);
            else
                dwStatus = WDC_PciWriteCfg(hDev, dwOffset, pBuf, dwBytes);
        }
        else if (WD_BUS_PCMCIA == busType) /* Read/write PCMCIA attribute space offset */
        {
            if (direction == WDC_READ)
                dwStatus = WDC_PcmciaReadAttribSpace(hDev, dwOffset, pBuf, dwBytes);
            else
                dwStatus = WDC_PcmciaWriteAttribSpace(hDev, dwOffset, pBuf, dwBytes);
        }
        else
        {
            printf("Error - Cannot read/write configuration space address space "
                "for bus type 0x%lx\n", busType);
            goto Exit;
        }
    }
    else /* Read/write a memory or I/O address */
    {
        WDC_ADDR_MODE mode;
        WDC_ADDR_RW_OPTIONS options;
        BOOL fAutoInc;

        if (!WDC_DIAG_SetMode(&mode))
            goto Exit;
            
        printf("Do you wish to increment the address after each %s block "
            "(0x%lX bytes) (0 - No, Otherwise - Yes)? ",
            sDir, WDC_ADDR_MODE_TO_SIZE(mode));
        fgets(gsInput, sizeof(gsInput), stdin);
        if (sscanf(gsInput, "%ld", &fAutoInc) < 1)
        {
            printf("Invalid input\n");
            goto Exit;
        }

        options = (fAutoInc ? WDC_ADDR_RW_DEFAULT : WDC_ADDR_RW_NO_AUTOINC);

        dwStatus = direction == WDC_READ ?
            WDC_ReadAddrBlock(hDev, dwAddrSpace, dwOffset, dwBytes, pBuf, mode, options) :
            WDC_WriteAddrBlock(hDev, dwAddrSpace, dwOffset, dwBytes, pBuf, mode, options);
    }
    
    if (WD_STATUS_SUCCESS == dwStatus)
    {
        if (WDC_READ == direction)
            DIAG_PrintHexBuffer(pBuf, dwBytes, FALSE);
        else
            printf("Wrote 0x%lX bytes to offset 0x%lX\n", dwBytes, dwOffset);
    }
    else
    {
        printf("Failed to %s 0x%lX bytes %s offset 0x%lX. Error 0x%lx - %s\n",
            sDir, dwBytes, (WDC_READ == direction) ? "from" : "to", dwOffset,
            dwStatus, Stat2Str(dwStatus));
    }

Exit:
    if (pBuf)
        free(pBuf);

    printf("\n");
    printf("Press ENTER to continue\n");
    fgets(gsInput, sizeof(gsInput), stdin);
}

/* Read all pre-defined run-time or PCI configuration registers and display results */
void WDC_DIAG_ReadRegsAll(WDC_DEVICE_HANDLE hDev, const WDC_REG *pRegs,
    DWORD dwNumRegs, BOOL fPciCfg)
{
    DWORD i, dwStatus;
    const WDC_REG *pReg;
    BYTE bData = 0;
    WORD wData = 0;
    UINT32 u32Data = 0;
    UINT64 u64Data = 0;
 
    if (!hDev)
    {
        WDC_DIAG_ERR("WDC_DIAG_ReadRegsAll: Error - NULL WDC device handle\n");
        return;
    }

    if (!dwNumRegs || !pRegs)
    {
        WDC_DIAG_ERR("WDC_DIAG_ReadRegsAll: %s\n",
            !dwNumRegs ? "No registers (dwNumRegs == 0)" :
            "Error - NULL registers pointer");
        
        return;
    }

    printf("\n");
    printf("%s registers data:\n", fPciCfg ? "PCI configuration" : "run-time");
    printf("---------------------------------\n\n");
    printf("%3s %-*s %-*s  %s\n",
        "", MAX_NAME_DISPLAY, "Name", 8 * 2 + 2, "Data", "Description");
    printf("%3s %-*s %-*s  %s\n",
        "", MAX_NAME_DISPLAY, "----", 8 * 2 + 2, "----", "-----------");

    for (i = 1, pReg = pRegs; i <= dwNumRegs; i++, pReg++)
    {
        printf("%2ld. %-*.*s ", i, MAX_NAME_DISPLAY, MAX_NAME_DISPLAY, pReg->sName);

        if (WDC_WRITE == pReg->direction)
        {
            printf("Write-only register\n");
            continue;
        }

        switch (pReg->dwSize)
        {
        case WDC_SIZE_8:
            dwStatus = fPciCfg ? WDC_PciReadCfg8(hDev, pReg->dwOffset, &bData) :
                WDC_ReadAddr8(hDev, pReg->dwAddrSpace, pReg->dwOffset, &bData);
            if (WD_STATUS_SUCCESS == dwStatus)
                printf("0x%-*X  ", (int)WDC_SIZE_64 * 2, (UINT32)bData);
            break;
        case WDC_SIZE_16:
            dwStatus = fPciCfg ?  WDC_PciReadCfg16(hDev, pReg->dwOffset, &wData) :
                WDC_ReadAddr16(hDev, pReg->dwAddrSpace, pReg->dwOffset, &wData);
            if (WD_STATUS_SUCCESS == dwStatus)
                printf("0x%-*hX  ", (int)WDC_SIZE_64 * 2, wData);
            break;
        case WDC_SIZE_32:
            dwStatus = fPciCfg ?  WDC_PciReadCfg32(hDev, pReg->dwOffset, &u32Data) :
                WDC_ReadAddr32(hDev, pReg->dwAddrSpace, pReg->dwOffset, &u32Data);
            if (WD_STATUS_SUCCESS == dwStatus)
                printf("0x%-*X  ", (int)WDC_SIZE_64 * 2, u32Data);
            break;
        case WDC_SIZE_64:
            dwStatus = fPciCfg ?  WDC_PciReadCfg64(hDev, pReg->dwOffset, &u64Data) :
                WDC_ReadAddr64(hDev, pReg->dwAddrSpace, pReg->dwOffset, &u64Data);
            if (WD_STATUS_SUCCESS == dwStatus)
                printf("0x%-*"PRI64"X  ", (int)WDC_SIZE_64 * 2, u64Data);
            break;
        default:
            printf("Invalid register size (%ld)\n", pReg->dwSize);
            return;
        }

        if (WD_STATUS_SUCCESS != dwStatus)
            printf("Error: 0x%-*lx  ", (int)WDC_SIZE_64 * 2 - 7, dwStatus);
        
        printf("%.*s\n", 43, pReg->sDesc);
    }

    printf("\n");
    printf("Press ENTER to continue\n");
    fgets(gsInput, sizeof(gsInput), stdin);
}

/* Display a list of pre-defined run-time or PCI configuration registers
   and let user select to read/write from/to a specific register */
void WDC_DIAG_ReadWriteReg(WDC_DEVICE_HANDLE hDev, const WDC_REG *pRegs,
    DWORD dwNumRegs, WDC_DIRECTION direction, BOOL fPciCfg)
{
    DWORD dwStatus;
    DWORD dwReg;
    const WDC_REG *pReg; 
    BYTE bData = 0;
    WORD wData = 0;
    UINT32 u32Data = 0;
    UINT64 u64Data = 0;
    
    if (!hDev)
    {
        WDC_DIAG_ERR("WDC_DIAG_ReadWriteReg: Error - NULL WDC device handle\n");
        return;
    }

    if (!dwNumRegs || !pRegs)
    {
        WDC_DIAG_ERR("WDC_DIAG_ReadWriteReg: %s\n",
            !dwNumRegs ? "No registers to read/write (dwNumRegs == 0)" :
            "Error - NULL registers pointer");
        
        return;
    }

    /* Display pre-defined registers' information */
    printf("\n");
    printf("PCI %s registers:\n", fPciCfg ? "configuration" : "run-time");
    printf("----------------------------\n");   
    WDC_DIAG_RegsInfoPrint(pRegs, dwNumRegs, WDC_DIAG_REG_PRINT_ALL);

    /* Read/write register */
    printf("\n");
    printf("Select a register from the list above to %s or 0 to cancel: ",
        (WDC_READ == direction) ? "read from" : "write to");
    fgets(gsInput, sizeof(gsInput), stdin);
    if (sscanf(gsInput, "%ld", &dwReg) < 1)
    {
        printf("Invalid selection\n");
        goto Exit;
    }
    
    if (!dwReg)
        return;

    if (dwReg > dwNumRegs)
    {
        printf("Selection (%ld) is out of range (1 - %ld)\n",
            dwReg, dwNumRegs);
        goto Exit;
    }

    pReg = &pRegs[dwReg - 1];


    if ( ((WDC_READ == direction) && (WDC_WRITE == pReg->direction)) ||
        ((WDC_WRITE == direction) && (WDC_READ == pReg->direction)))
    {
        printf("Error - you have selected to %s a %s-only register\n",
            (WDC_READ == direction) ? "read from" : "write to",
            (WDC_WRITE == pReg->direction) ? "write" : "read");
        goto Exit;
    }

    if ((WDC_WRITE == direction) &&
        !WDC_DIAG_InputWriteData((WDC_SIZE_8 == pReg->dwSize) ? (PVOID)&bData :
        (WDC_SIZE_16 == pReg->dwSize) ? (PVOID)&wData :
        (WDC_SIZE_32 == pReg->dwSize) ? (PVOID)&u32Data : (PVOID)&u64Data,
        pReg->dwSize))
    {
        goto Exit;
    }

    switch (pReg->dwSize)
    {
    case WDC_SIZE_8:
        if (WDC_READ == direction)
            dwStatus = fPciCfg ? WDC_PciReadCfg8(hDev, pReg->dwOffset, &bData) :
                WDC_ReadAddr8(hDev, pReg->dwAddrSpace, pReg->dwOffset, &bData);
        else
            dwStatus = fPciCfg ? WDC_PciWriteCfg8(hDev, pReg->dwOffset, bData) :
                WDC_WriteAddr8(hDev, pReg->dwAddrSpace, pReg->dwOffset, bData);
        if (WD_STATUS_SUCCESS == dwStatus)
        {
            printf("%s 0x%X %s register %s\n", (WDC_READ == direction) ? "Read" : "Wrote",
                (UINT32)bData, (WDC_READ == direction) ? "from" : "to", 
                pReg->sName);
        }
        break;
    case WDC_SIZE_16:
        if (WDC_READ == direction)
            dwStatus = fPciCfg ? WDC_PciReadCfg16(hDev, pReg->dwOffset, &wData) :
                WDC_ReadAddr16(hDev, pReg->dwAddrSpace, pReg->dwOffset, &wData);
        else
            dwStatus = fPciCfg ? WDC_PciWriteCfg16(hDev, pReg->dwOffset, wData) :
                WDC_WriteAddr16(hDev, pReg->dwAddrSpace, pReg->dwOffset, wData);
        if (WD_STATUS_SUCCESS == dwStatus)
        {
            printf("%s 0x%hX %s register %s\n", (WDC_READ == direction) ? "Read" : "Wrote",
                wData, (WDC_READ == direction) ? "from" : "to", pReg->sName);
        }
        break;
    case WDC_SIZE_32:
        if (WDC_READ == direction)
            dwStatus = fPciCfg ? WDC_PciReadCfg32(hDev, pReg->dwOffset, &u32Data) :
                WDC_ReadAddr32(hDev, pReg->dwAddrSpace, pReg->dwOffset, &u32Data);
        else
            dwStatus = fPciCfg ? WDC_PciWriteCfg32(hDev, pReg->dwOffset, u32Data) :
                WDC_WriteAddr32(hDev, pReg->dwAddrSpace, pReg->dwOffset, u32Data);
        if (WD_STATUS_SUCCESS == dwStatus)
        {
            printf("%s 0x%X %s register %s\n", (WDC_READ == direction) ? "Read" : "Wrote",
                u32Data, (WDC_READ == direction) ? "from" : "to", pReg->sName);
        }
        break;
    case WDC_SIZE_64:
        if (WDC_READ == direction)
            dwStatus = fPciCfg ? WDC_PciReadCfg64(hDev, pReg->dwOffset, &u64Data) :
                WDC_ReadAddr64(hDev, pReg->dwAddrSpace, pReg->dwOffset, &u64Data);
        else
            dwStatus = fPciCfg ? WDC_PciWriteCfg64(hDev, pReg->dwOffset, u64Data) :
                WDC_WriteAddr64(hDev, pReg->dwAddrSpace, pReg->dwOffset, u64Data);
        if (WD_STATUS_SUCCESS == dwStatus)
        {
            printf("%s 0x%"PRI64"X %s register %s\n", (WDC_READ == direction) ? "Read" : "Wrote",
                u64Data, (WDC_READ == direction) ? "from" : "to", pReg->sName);
        }
        break;
    default:
        printf("Invalid register size (%ld)\n", pReg->dwSize);
        return;
    }

    if (WD_STATUS_SUCCESS != dwStatus)
    {
        printf("Failed %s %s. Error 0x%lx - %s\n",
            (WDC_READ == direction) ? "reading data from" : "writing data to",
            pReg->sName, dwStatus, Stat2Str(dwStatus));
    }

Exit:
    printf("\n");
    printf("Press ENTER to continue\n");
    fgets(gsInput, sizeof(gsInput), stdin);
}

char *WDC_DIAG_IntTypeDescriptionGet(DWORD dwIntType)
{
    if (dwIntType & INTERRUPT_MESSAGE_X)
        return "Extended Message-Signaled Interrupt (MSI-X)";
    else if (dwIntType & INTERRUPT_MESSAGE)
        return "Message-Signaled Interrupt (MSI)";
    else if (dwIntType & INTERRUPT_LEVEL_SENSITIVE)
        return "Level-Sensitive Interrupt";
    return "Edge-Triggered Interrupt";
}

/* -----------------------------------------------
    PCI
   ----------------------------------------------- */
/* Print PCI device location information */
void WDC_DIAG_PciSlotPrint(WD_PCI_SLOT *pPciSlot)
{
    WDC_DIAG_PciSlotPrintFile(pPciSlot, stdout);
}

/* Print PCI device location information to file */
void WDC_DIAG_PciSlotPrintFile(WD_PCI_SLOT *pPciSlot, FILE *fp)
{
    if (!pPciSlot)
    {
        WDC_DIAG_ERR("WDC_DIAG_PciSlotPrint: Error - NULL PCI slot pointer\n");
        return;
    }

    fprintf(fp, "Bus 0x%lx, Slot 0x%lx, Function 0x%lx\n",
        pPciSlot->dwBus, pPciSlot->dwSlot, pPciSlot->dwFunction);
}

/* Print PCI device location and resources information */
void WDC_DIAG_PciDeviceInfoPrint(WD_PCI_SLOT *pPciSlot, BOOL dump_cfg)
{
    WDC_DIAG_PciDeviceInfoPrintFile(pPciSlot, stdout, dump_cfg);
}

/* Print PCI device location and resources information to file */
void WDC_DIAG_PciDeviceInfoPrintFile(WD_PCI_SLOT *pPciSlot, FILE *fp,
    BOOL dump_cfg) 
{
    DWORD dwStatus;
    WD_PCI_CARD_INFO deviceInfo;
    
    if (!pPciSlot)
    {
        WDC_DIAG_ERR("WDC_DIAG_PciDeviceInfoPrint: Error - NULL PCI slot "
            "pointer\n");
        return;
    }
        
    fprintf(fp, "    Location: ");
    WDC_DIAG_PciSlotPrintFile(pPciSlot, fp);

    if (dump_cfg)
    {   
        UINT32 config;       
        DWORD dwOffset=0;
        
        for (dwOffset=0; dwOffset<256; dwOffset+=sizeof(UINT32))
        {
            dwStatus = WDC_PciReadCfgBySlot32(pPciSlot, dwOffset, &config);
            if (dwStatus)
            {
                WDC_DIAG_ERR("    Failed reading PCI configuration space.\n"  
                "    Error 0x%lx - %s\n", dwStatus, Stat2Str(dwStatus));
                return;
            }
           
            if ((dwOffset/4) % 8 == 0)
                fprintf(fp, "%02lx ", dwOffset);
            fprintf(fp, "%08x ", config);
            if ((dwOffset/4) % 8 == 7)
                fprintf(fp, "\n");
        }
    }

    BZERO(deviceInfo);
    deviceInfo.pciSlot = *pPciSlot;
    dwStatus = WDC_PciGetDeviceInfo(&deviceInfo);
    
    if ((WD_NO_RESOURCES_ON_DEVICE != dwStatus) &&
        (WD_STATUS_SUCCESS != dwStatus))
    {
        WDC_DIAG_ERR("    Failed retrieving PCI resources information.\n"
            "    Error 0x%lx - %s\n", dwStatus, Stat2Str(dwStatus));
        return;
    }

    WDC_DIAG_DeviceResourcesPrint(&deviceInfo.Card, 0, fp);
}

/* Print location and resources information for all connected PCI devices */
void WDC_DIAG_PciDevicesInfoPrintAll(BOOL dump_cfg)
{
    WDC_DIAG_PciDevicesInfoPrintAllFile(stdout, dump_cfg);
}

/* Print location and resources information for all connected PCI devices to
 * file */
void WDC_DIAG_PciDevicesInfoPrintAllFile(FILE *fp, BOOL dump_cfg)
{
    DWORD dwStatus;
    DWORD i, dwNumDevices;
    WDC_PCI_SCAN_RESULT scanResult;

    BZERO(scanResult);

    dwStatus = WDC_PciScanDevices(0, 0, &scanResult);
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        fprintf(fp,"Failed scanning PCI bus. Error 0x%lx - %s\n",
            dwStatus, Stat2Str(dwStatus));
        return;
    }

    dwNumDevices = scanResult.dwNumDevices;

    if (!dwNumDevices)
    {
        fprintf(fp,"No devices were found on the PCI bus\n");
        return;
    }

    fprintf(fp,"\n");
    fprintf(fp,"Found %ld devices on the PCI bus:\n", dwNumDevices);
    fprintf(fp,"---------------------------------\n");
    
    for (i = 0; i < dwNumDevices; i++)
    {
        fprintf(fp,"%2ld. Vendor ID: 0x%lX, Device ID: 0x%lX\n",
            i + 1,
            scanResult.deviceId[i].dwVendorId,
            scanResult.deviceId[i].dwDeviceId);
        WDC_DIAG_PciDeviceInfoPrintFile(&scanResult.deviceSlot[i], fp,
            dump_cfg);

        if (fp == stdout)
        {
            printf("Press ENTER to proceed to next device");
            fgets(gsInput, sizeof(gsInput), stdin);
        }
        fprintf(fp, "\n");
    }
}

/* -----------------------------------------------
    PCMCIA
   ----------------------------------------------- */
/* Print PCMCIA device location information */
void WDC_DIAG_PcmciaSlotPrint(const WD_PCMCIA_SLOT *pPcmciaSlot)
{
    if (!pPcmciaSlot)
    {
        WDC_DIAG_ERR("WDC_DIAG_PcmciaSlotPrint: Error - NULL PCMCIA slot pointer\n");
        return;
    }

    printf("Bus 0x%x, Slot 0x%x, Function 0x%x\n",      
        (UINT32)pPcmciaSlot->uBus, (UINT32)pPcmciaSlot->uSocket,
        (UINT32)pPcmciaSlot->uFunction);
}

/* Print PCMCIA device location and resources information */
void WDC_DIAG_PcmciaDeviceInfoPrint(const WD_PCMCIA_SLOT *pPcmciaSlot)
{
    DWORD dwStatus;
    WD_PCMCIA_CARD_INFO deviceInfo;

    if (!pPcmciaSlot)
    {
        WDC_DIAG_ERR("WDC_DIAG_PcmciaDeviceInfoPrint: Error - NULL PCMCIA slot pointer\n");
        return;
    }

    printf("    Location: ");
    WDC_DIAG_PcmciaSlotPrint(pPcmciaSlot);

    BZERO(deviceInfo);
    deviceInfo.pcmciaSlot = *pPcmciaSlot;
    dwStatus = WDC_PcmciaGetDeviceInfo(&deviceInfo);
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        WDC_DIAG_ERR("    Failed retrieving PCMCIA resources information.\n"
            "    Error 0x%lx - %s\n", dwStatus, Stat2Str(dwStatus));
        return;
    }

    WDC_DIAG_DeviceResourcesPrint(&deviceInfo.Card, 0, stdout);
}

/* Print location and resources information for all connected PCMCIA devices */
void WDC_DIAG_PcmciaDevicesInfoPrintAll(void)
{
    DWORD dwStatus;
    DWORD i, dwNumDevices;
    WDC_PCMCIA_SCAN_RESULT scanResult;

    BZERO(scanResult);

    dwStatus = WDC_PcmciaScanDevices(0, 0, &scanResult);
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        printf("Failed scanning PCMCIA bus. Error 0x%lx - %s\n",
            dwStatus, Stat2Str(dwStatus));
        return;
    }

    dwNumDevices = scanResult.dwNumDevices;

    if (!dwNumDevices)
    {
        printf("No devices were found on the PCMCIA bus\n");
        return;
    }

    printf("Found %ld devices on the PCMCIA bus:\n", dwNumDevices);
    printf("-------------------------------------\n");
    
    for (i = 0; i < dwNumDevices; i++)
    {
        printf("%2ld. Manufacturer ID: 0x%hX, Device ID: 0x%hX\n",
            i + 1,
            scanResult.deviceId[i].wManufacturerId,
            scanResult.deviceId[i].wCardId);
        
        WDC_DIAG_PcmciaDeviceInfoPrint(&scanResult.deviceSlot[i]);
        printf("Press ENTER to proceed to next device\n");
        fgets(gsInput, sizeof(gsInput), stdin);
    }
}

/* Print tuple information for all tuples found in the PCMCIA attribute space */
void WDC_DIAG_PcmciaTuplesPrintAll(WDC_DEVICE_HANDLE hDev, BOOL fDumpData)
{
    DWORD dwStatus;
    DWORD dwOffset = 0;
    BYTE header[2]; /* header[0] - tuple code; header[1] - tuple's data size */
    
    if (!hDev)
    {
        WDC_DIAG_ERR("WDC_DIAG_PcmciaTuplesPrintAll: Error - NULL WDC device handle\n");
        return;
    }

    while (1)
    {
        dwStatus = WDC_PcmciaReadAttribSpace(hDev, dwOffset, header, sizeof(header));
        if (WD_STATUS_SUCCESS != dwStatus)
        {
            WDC_DIAG_ERR("Failed to read from offset 0x%lX of the PCMCIA atribute space.\n"
                "Error 0x%lx - %s\n", dwOffset, dwStatus, Stat2Str(dwStatus));
            return;
        }

        if (CISTPL_END == header[0])
            return;
        
        if (CISTPL_NULL == header[0])
        {
            dwOffset++;
            continue;
        }

        printf("Tuple code (ID): 0x%02x,  Data size: %2d bytes,  Offset: 0x%02x\n",
            (UINT32)header[0], (UINT32)header[1], (UINT32)dwOffset);

        dwOffset += 2;

        if (fDumpData)
        {
            if (header[1])
            {
                BYTE buf[0xFF];

                BZERO(buf);
                dwStatus = WDC_PcmciaReadAttribSpace(hDev, dwOffset, buf, header[1]);

                if (WD_STATUS_SUCCESS == dwStatus)
                {
                    printf("Data (hex format):\n");
                    DIAG_PrintHexBuffer(buf, header[1], FALSE);
                }
                else
                {
                    printf("Failed to read tuple's data. Error 0x%lx - %s\n",
                        dwStatus, Stat2Str(dwStatus));
                }
            }
            else
                printf("Empty buffer (size 0)\n");
            
            printf("-----------------------------------------------------------\n");
        }

        dwOffset += header[1];
    }
}

#endif
