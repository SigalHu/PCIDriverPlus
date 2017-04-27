/* Jungo Confidential. Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */

////////////////////////////////////////////////////////////////
// File - pci_diag_lib.h
////////////////////////////////////////////////////////////////

#ifndef _PCI_DIAG_LIB_H_
#define _PCI_DIAG_LIB_H_

#ifdef __cplusplus
extern "C" {
#endif

// Function: PCI_Get_WD_handle()
//   Get handle to WinDriver
BOOL PCI_Get_WD_handle(HANDLE *phWD);

// Function: PCI_Print_card_info()
//   Print the PCI devices attached
void PCI_Print_card_info(WD_PCI_SLOT pciSlot);

// Function: PCI_Print_all_cards_info()
//   Print all the PCI devices in the system
void PCI_Print_all_cards_info(void);

// Function: PCI_EditConfigReg()
//   Edit PCI configuration registers
void PCI_EditConfigReg(WD_PCI_SLOT pciSlot);

// Function: PCI_ChooseCard()
//   Choose PCI card to work with
BOOL PCI_ChooseCard(WD_PCI_SLOT *ppciSlot);

// Function: PCI_ReadBytes()
//   Read data from the configuration registers of a specific card
DWORD PCI_ReadBytes(HANDLE hWD, WD_PCI_SLOT pciSlot, DWORD dwOffset, 
    DWORD dwBytes);


#ifdef __cplusplus
}
#endif

#endif
