/* Jungo Confidential. Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */

////////////////////////////////////////////////////////////////
// File - PCMCIA_DIAG_LIB.C
//
// Utility functions for printing card information,
// detecting PCMCIA cards, and accessing PCMCIA attribute memory
// 
////////////////////////////////////////////////////////////////

#include "windrvr.h"
#ifdef _USE_SPECIFIC_KERNEL_DRIVER_
    #undef WD_Open
    #define WD_Open WD_OpenKernelHandle
    #if defined(UNIX)
        #undef WD_FUNCTION
        #define WD_FUNCTION(wFuncNum,h,pParam,dwSize,fWait) ((ULONG) ioctl((int)(h), wFuncNum, pParam))
    #endif
#endif
#include "print_struct.h"
#include "status_strings.h"
#include <stdio.h>
#include "pcmcia_diag_lib.h"

// Function: PCMCIA_Get_WD_handle()
//   Get handle to WinDriver
// Parameters:
//   phWD [out] pointer to handle to be received
// Return Value:
//   Success/fail

BOOL PCMCIA_Get_WD_handle(HANDLE *phWD)
{
    WD_VERSION ver;

    *phWD = WD_Open();
    if (*phWD==INVALID_HANDLE_VALUE) 
    {
        printf("Failed opening " WD_PROD_NAME " device\n");
        return FALSE;
    }

    BZERO(ver);
    WD_Version(*phWD,&ver);
    if (ver.dwVer<WD_VER) 
    {
        printf("Incorrect " WD_PROD_NAME " version\n");
        WD_Close (*phWD);
        *phWD = INVALID_HANDLE_VALUE;
        return FALSE;
    }

    return TRUE;
}

// Function: PCMCIA_Print_card_info()
//   Print information about PCMCIA device attached
// Parameters:
//   pcmciaSlot [in] physical location of PCMCIA device
// Return Value:
//   None

void PCMCIA_Print_card_info(WD_PCMCIA_SLOT pcmciaSlot)
{
    HANDLE hWD;
    DWORD dwStatus;
    WD_PCMCIA_CARD_INFO pcmciaCardInfo;

    if (!PCMCIA_Get_WD_handle(&hWD))
        return;

    BZERO(pcmciaCardInfo);
    pcmciaCardInfo.pcmciaSlot = pcmciaSlot;
    dwStatus = WD_PcmciaGetCardInfo(hWD, &pcmciaCardInfo);
    if (dwStatus)
    {
        printf("WD_PcmciaGetCardInfo() failed (0x%lx) - %s\n", dwStatus, Stat2Str(dwStatus));
        goto Exit;
    }

    WD_CARD_print(&pcmciaCardInfo.Card, "   ");
Exit:
    WD_Close(hWD);
}

// Function: PCMCIA_Print_all_cards_info()
//   Print all PCMCIA devices attached
// Parameters:
//   None
// Return Value:
//   None

void PCMCIA_Print_all_cards_info()
{
    HANDLE hWD;
    WD_PCMCIA_SCAN_CARDS scanCards;
    WD_PCMCIA_CARD_INFO cardInfo;
    DWORD dwStatus, i;

    if (!PCMCIA_Get_WD_handle (&hWD)) return;

    printf ("PCMCIA bus scan:\n\n");
    BZERO(scanCards);
    dwStatus = WD_PcmciaScanCards (hWD, &scanCards);
    if (dwStatus)
    {
        printf("WD_PcmciaScanCards() failed (0x%lx) - %s\n",
            dwStatus, Stat2Str(dwStatus));
        goto Exit;
    }
    
    for (i=0; i<scanCards.dwCards; i++)
    {
        printf ("Card %ld: Manufacturer ID: %04hx, Card ID: %04hx\n",
            i, scanCards.cardId[i].wManufacturerId, scanCards.cardId[i].wCardId);
        printf ("Bus %d, socket %d, function %d\n",
            (UINT32)scanCards.cardSlot[i].uBus,
            (UINT32)scanCards.cardSlot[i].uSocket,
            (UINT32)scanCards.cardSlot[i].uFunction);

        BZERO(cardInfo);
        cardInfo.pcmciaSlot = scanCards.cardSlot[i];
        WD_PcmciaGetCardInfo(hWD, &cardInfo);
        WD_CARD_print(&cardInfo.Card, "   ");
        printf ("\n");
    }
Exit:
    WD_Close (hWD);
}

