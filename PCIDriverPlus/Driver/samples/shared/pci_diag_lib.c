/* Jungo Confidential. Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */

////////////////////////////////////////////////////////////////
// File - PCI_DIAG_LIB.C
//
// Utility functions for printing card information,
// detecting PCI cards, and accessing PCI configuration
// registers.
// 
////////////////////////////////////////////////////////////////
 
#include "windrvr.h"
#ifdef _USE_SPECIFIC_KERNEL_DRIVER_
    #undef WD_Open
    #define WD_Open WD_OpenKernelHandle
        #if defined(UNIX)
                #undef WD_FUNCTION
                #define WD_FUNCTION(wFuncNum,h,pParam,dwSize,fWait) \
                        ((ULONG) ioctl((int)(h), wFuncNum, pParam))
        #endif
#endif
#include "pci_diag_lib.h"
#include "pci_regs.h"
#include "print_struct.h"
#include "status_strings.h"
#include <stdio.h>

// input of command from user
static char line[256];

// Function: PCI_Get_WD_handle()
//   Get handle to WinDriver
// Parameters:
//   phWD [out] pointer to handle to be received
// Return Value:
//   Success/fail

BOOL PCI_Get_WD_handle(HANDLE *phWD)
{
    WD_VERSION ver;
    DWORD dwStatus;

    *phWD = WD_Open();
    if (*phWD==INVALID_HANDLE_VALUE) 
    {
        printf("Failed opening " WD_PROD_NAME " device\n");
        return FALSE;
    }

    BZERO(ver);
    dwStatus = WD_Version(*phWD, &ver);
    if (dwStatus)
    {
        printf("WD_Version() failed (0x%lx) - %s\n", dwStatus, Stat2Str(dwStatus));
        goto Error;
    }
    if (ver.dwVer<WD_VER) 
    {
        printf("Incorrect " WD_PROD_NAME " version. Expected %d.%02d, got %ld.%02ld\n",
            WD_VER/100, WD_VER%100, ver.dwVer/100, ver.dwVer%100);
        goto Error;
    }
    return TRUE;
Error:
    WD_Close(*phWD);
    *phWD = INVALID_HANDLE_VALUE;
    return FALSE;
}

// Function: PCI_Print_card_info()
//   Print information about PCI device attached
// Parameters:
//   pciSlot [in] physical location of PCI device
// Return Value:
//   None

void PCI_Print_card_info(WD_PCI_SLOT pciSlot)
{
    HANDLE hWD;
    DWORD dwStatus;
    WD_PCI_CARD_INFO pciCardInfo;

    if (!PCI_Get_WD_handle(&hWD))
        return;

    BZERO(pciCardInfo);
    pciCardInfo.pciSlot = pciSlot;
    dwStatus = WD_PciGetCardInfo(hWD, &pciCardInfo);
    if (dwStatus)
    {
        printf("WD_PciGetCardInfo() failed (0x%lx) - %s\n", dwStatus, Stat2Str(dwStatus));
        goto Exit;
    }

    WD_CARD_print(&pciCardInfo.Card, "   ");
Exit:
    WD_Close(hWD);
}

// Function: PCI_Print_all_cards_info(void)
//   Print all the PCI devices in the system
// Parameters:
//   None
// Return Value:
//   None

void PCI_Print_all_cards_info(void)
{
    HANDLE hWD;
    DWORD i;
    DWORD dwStatus;
    WD_PCI_SCAN_CARDS pciScan;
    WD_PCI_SLOT pciSlot;
    WD_PCI_ID   pciId;

    if (!PCI_Get_WD_handle(&hWD)) 
        return;

    BZERO(pciScan);
    pciScan.searchId.dwVendorId = 0;
    pciScan.searchId.dwDeviceId = 0;

    printf("Pci bus scan:\n\n");
    dwStatus = WD_PciScanCards(hWD, &pciScan);
    if (dwStatus)
    {
        printf("WD_PciScanCards() failed (0x%lx) - %s\n",
            dwStatus, Stat2Str(dwStatus));
        goto Exit;
    }

    for (i=0; i<pciScan.dwCards; i++)
    {
        CHAR tmp[100];
        pciId = pciScan.cardId[i];
        pciSlot = pciScan.cardSlot[i];
        printf("Bus %ld Slot %ld Function %ld, VendorID %04lx DeviceID %04lx\n",
            pciSlot.dwBus, pciSlot.dwSlot, pciSlot.dwFunction,
            pciId.dwVendorId, pciId.dwDeviceId);
        PCI_Print_card_info(pciSlot);
        printf("Press Enter to continue to next slot\n");
        fgets(tmp, sizeof(tmp), stdin);
    }
Exit:
    WD_Close (hWD);
}

// Function: PCI_ReadBytes()
//   Read data from the configuration registers of a specific card
// Parameters:
//   hWD [in] handle to WinDriver
//   pciSlot [in] PCI card to read from 
//   dwOffset [in] Offset from memory to read from 
//   dwBytes [in] Number of bytes to read
// Return Value:
//   Data read

DWORD PCI_ReadBytes(HANDLE hWD, WD_PCI_SLOT pciSlot, DWORD dwOffset, 
    DWORD dwBytes)
{
    WD_PCI_CONFIG_DUMP pciCnf;
    DWORD dwVal = 0;
    
    BZERO(pciCnf);
    pciCnf.pciSlot = pciSlot;
    pciCnf.pBuffer = &dwVal;
#if defined(_BIGENDIAN)
        pciCnf.pBuffer = (char *) pciCnf.pBuffer + sizeof(DWORD)-dwBytes;
#endif
    pciCnf.dwOffset = dwOffset;
    pciCnf.dwBytes = dwBytes;
    pciCnf.fIsRead = TRUE;
    WD_PciConfigDump(hWD,&pciCnf);
    return dwVal;
}

// Function: PCI_WriteBytes()
//   Read data from a specific device
// Parameters:
//   hWD [in] handle to WinDriver
//   pciSlot [in] PCI card to write to
//   dwOffset [in] Offset from memory to write to
//   dwBytes [in] Number of bytes to write
//   dwData [in] Data to write
// Return Value:
//   None

void PCI_WriteBytes(HANDLE hWD, WD_PCI_SLOT pciSlot, DWORD dwOffset, 
        DWORD dwBytes, DWORD dwData)
{
    WD_PCI_CONFIG_DUMP pciCnf;
    
    BZERO(pciCnf);
    pciCnf.pciSlot = pciSlot;
    pciCnf.pBuffer = &dwData;
#if defined(_BIGENDIAN)
        pciCnf.pBuffer = (char *) pciCnf.pBuffer + sizeof(DWORD)-dwBytes;
#endif
    pciCnf.dwOffset = dwOffset;
    pciCnf.dwBytes = dwBytes;
    pciCnf.fIsRead = FALSE;
    WD_PciConfigDump(hWD,&pciCnf);
}

// Function: PCI_EditConfigReg()
//   Edit PCI configuration registers
// Parameters:
//   pciSlot [in] PCI card to edit registers
// Return Value:
//   None

void PCI_EditConfigReg(WD_PCI_SLOT pciSlot)
{
    HANDLE hWD;
    struct {
        CHAR *name;
        UINT32 dwOffset;
        UINT32 dwBytes;
        UINT32 dwVal;
    } fields[30] = {
        { "VID", PCI_VID, 2 },
        { "DID", PCI_DID, 2 },
        { "CMD", PCI_CR, 2 },
        { "STS", PCI_SR, 2 },
        { "RID", PCI_REV, 1 },
        { "CLCD", PCI_CCR, 3 },
        { "CALN", PCI_CLSR, 1 },
        { "LAT", PCI_LTR, 1 },
        { "HDR", PCI_HDR, 1 },
        { "BIST", PCI_BISTR, 1 },
        { "BADDR0", PCI_BAR0, 4 },
        { "BADDR1", PCI_BAR1, 4 },
        { "BADDR2", PCI_BAR2, 4 },
        { "BADDR3", PCI_BAR3, 4 },
        { "BADDR4", PCI_BAR4, 4 },
        { "BADDR5", PCI_BAR5, 4 },
        { "EXROM", PCI_EROM, 4 },
        { "NEW_CAP", PCI_CAP, 1 },
        { "INTLN", PCI_ILR, 1 },
        { "INTPIN", PCI_IPR, 1 },
        { "MINGNT", PCI_MGR, 1 },
        { "MAXLAT", PCI_MLR, 1 },
        { NULL, 0, 0 },
        { NULL, 0, 0 }
    };

    int cmd, i;

    if (!PCI_Get_WD_handle (&hWD)) 
        return;

    do {
        int row;
        int col;

        printf ("\n");
        printf ("Edit PCI configuration registers\n");
        printf ("--------------------------------\n");
        for (row = 0; row<=10; row++)
        {
            for (col = 0; col <=1; col++)
            {
                if (col==0)
                    i = row;
                else
                    i = row + 10;

                if (row==10 && col==0)
                    printf("%26s","");
                else
                {
                    DWORD dwVal;
                    dwVal = PCI_ReadBytes(hWD, pciSlot, fields[i].dwOffset, 
                                                fields[i].dwBytes);
                    fields[i].dwVal = dwVal;
                    printf ("%2d. %6s : %0*x %*s    ",i+1, fields[i].name, 
                        fields[i].dwBytes*2, fields[i].dwVal, 
                        8-fields[i].dwBytes*2, "");
                }
                if (col==1)
                    printf ("\n");
            }
        }

        printf ("99. Back to main menu\n");
        printf ("Choose register to write to, or 99 to exit: ");
        cmd = 0;
        fgets(line, sizeof(line), stdin);
        sscanf(line, "%d",&cmd);
        if (cmd>=1 && cmd <=21)
        {
            i = cmd-1;
            printf("Enter value (hex format) to write to %s register (or 'X' to cancel): ",
                fields[i].name);
            fgets(line, sizeof(line), stdin);
            if (toupper(line[0]) != 'X')
            {
                UINT32 dwVal;
                dwVal = 0;
                sscanf(line,"%x",&dwVal);
                if ((dwVal>0xff && fields[i].dwBytes==1)|| 
                    (dwVal>0xffff && fields[i].dwBytes==2)|| 
                    (dwVal>0xffffff && fields[i].dwBytes==3))
                {
                    printf ("Error: value too big for register\n");
                }
                else
                {
                    PCI_WriteBytes(hWD, pciSlot, fields[i].dwOffset, 
                        fields[i].dwBytes, dwVal);
                }
            }
        }
    } while (cmd!=99);

    WD_Close (hWD);
}

// Function: PCI_ChooseCard()
//   Choose PCI card to work with
// Parameters:
//   pciSlot [out] pointer to PCI card selected
// Return Value:
//   Success/fail

BOOL PCI_ChooseCard(WD_PCI_SLOT *ppciSlot)
{
    BOOL fHasCard;
    WD_PCI_SCAN_CARDS pciScan;
    UINT32 dwVendorID, dwDeviceID;
    HANDLE hWD;
    UINT32 i;
    DWORD dwStatus;

    if (!PCI_Get_WD_handle (&hWD)) 
        return FALSE;

    fHasCard = FALSE;

    for (;!fHasCard;) 
    {
        dwVendorID = 0;
        printf("Enter VendorID: ");
        fgets(line, sizeof(line), stdin);
        sscanf(line, "%x",&dwVendorID);
        if (dwVendorID==0) 
            break;

        printf("Enter DeviceID: ");
        fgets(line, sizeof(line), stdin);
        sscanf(line, "%x",&dwDeviceID);

        BZERO(pciScan);
        pciScan.searchId.dwVendorId = dwVendorID;
        pciScan.searchId.dwDeviceId = dwDeviceID;
        dwStatus = WD_PciScanCards(hWD, &pciScan);
        if (dwStatus)
            printf("WD_PciScanCards() failed (0x%lx) - %s\n", dwStatus, Stat2Str(dwStatus));
        else if (pciScan.dwCards==0) // Find at least one card
            printf("Could not find PCI card\n");
        else if (pciScan.dwCards==1)
        {
            *ppciSlot = pciScan.cardSlot[0];
            fHasCard = TRUE;
        }
        else
        {
            printf("Found %lu matching PCI cards\n", pciScan.dwCards);
            printf("Select card (1-%lu): ", pciScan.dwCards);
            i = 0;
            fgets(line, sizeof(line), stdin);
            sscanf(line, "%u", &i);
            if (i>=1 && i <=pciScan.dwCards)
            {
                *ppciSlot = pciScan.cardSlot[i-1];
                fHasCard = TRUE;
            }
            else printf ("Choice out of range\n");
        }
        if (!fHasCard)
        {
            printf ("Do you want to try a different VendorID/DeviceID? ");
            fgets(line, sizeof(line), stdin);
            if (toupper(line[0])!='Y')
                break;
        }
    }

    WD_Close (hWD);

    return fHasCard;
}

