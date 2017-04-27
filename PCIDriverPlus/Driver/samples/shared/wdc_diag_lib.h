/* Jungo Confidential. Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */

#ifndef _WDC_DIAG_LIB_H_
#define _WDC_DIAG_LIB_H_

/******************************************************************************
*  File: wdc_diag_lib.h - Shared WDC PCI, PCMCIA and ISA devices' user-mode   *
*        diagnostics API header.                                              *
*******************************************************************************/

#if !defined(__KERNEL__)

#ifdef __cplusplus
extern "C" {
#endif

#include "windrvr.h"
#include "wdc_lib.h"

/* -----------------------------------------------
    All buses (PCI/PCMCIA/ISA)
   ----------------------------------------------- */

/* Print run-time registers and PCI configuration registers information */
#define WDC_DIAG_REG_PRINT_NAME       0x1
#define WDC_DIAG_REG_PRINT_DESC       0x2
#define WDC_DIAG_REG_PRINT_ADDR_SPACE 0x4
#define WDC_DIAG_REG_PRINT_OFFSET     0x8
#define WDC_DIAG_REG_PRINT_SIZE       0x10
#define WDC_DIAG_REG_PRINT_DIRECTION  0x12
#define WDC_DIAG_REG_PRINT_ALL \
    (WDC_DIAG_REG_PRINT_NAME | WDC_DIAG_REG_PRINT_DESC | \
    WDC_DIAG_REG_PRINT_ADDR_SPACE | WDC_DIAG_REG_PRINT_OFFSET | \
    WDC_DIAG_REG_PRINT_SIZE | WDC_DIAG_REG_PRINT_DIRECTION)
#define WDC_DIAG_REG_PRINT_DEFAULT \
        (WDC_DIAG_REG_PRINT_NAME | WDC_DIAG_REG_PRINT_DIRECTION | WDC_DIAG_REG_PRINT_DESC)
typedef DWORD WDC_DIAG_REG_PRINT_OPTIONS;

/* Device configuration space identifier (PCI configuration space / PCMCIA attribute space) */
#define WDC_AD_CFG_SPACE 0xFF

/* Register information struct */
typedef struct {
    DWORD dwAddrSpace;       /* Number of address space in which the register resides */
                             /* For PCI configuration registers, use WDC_AD_CFG_SPACE */
    DWORD dwOffset;          /* Offset of the register in the dwAddrSpace address space */
    DWORD dwSize;            /* Register's size (in bytes) */
    WDC_DIRECTION direction; /* Read/write access mode - see WDC_DIRECTION options */
    CHAR  sName[MAX_NAME];   /* Register's name */
    CHAR  sDesc[MAX_DESC];   /* Register's description */
} WDC_REG;

void WDC_DIAG_RegsInfoPrint(const WDC_REG *pRegs, DWORD dwNumRegs,
    WDC_DIAG_REG_PRINT_OPTIONS options);

/* Set address access mode */
BOOL WDC_DIAG_SetMode(WDC_ADDR_MODE *pMode);

/* Get data for address write operation from user */
/* Data size (dwSize) should be WDC_SIZE_8, WDC_SIZE_16, WDC_SIZE_32 or WDC_SIZE_64 */
BOOL WDC_DIAG_InputWriteData(PVOID pData, WDC_ADDR_SIZE dwSize);

/* Read/write a memory or I/O address */
void WDC_DIAG_ReadWriteAddr(WDC_DEVICE_HANDLE hDev, WDC_DIRECTION direction,
    DWORD dwAddrSpace, WDC_ADDR_MODE mode);

/* Read/write a memory or I/O address OR an offset in the PCI configuration space /
   PCMCIA attribute space (dwAddrSpace == WDC_AD_CFG_SPACE) */
void WDC_DIAG_ReadWriteBlock(WDC_DEVICE_HANDLE hDev, WDC_DIRECTION direction,
    DWORD dwAddrSpace);

/* Read all pre-defined run-time or PCI configuration registers and display results */
void WDC_DIAG_ReadRegsAll(WDC_DEVICE_HANDLE hDev, const WDC_REG *pRegs,
    DWORD dwNumRegs, BOOL fPciCfg);

/* Display a list of pre-defined run-time or PCI configuration registers
   and let user select to read/write from/to a specific register */
void WDC_DIAG_ReadWriteReg(WDC_DEVICE_HANDLE hDev, const WDC_REG *pRegs,
    DWORD dwNumRegs, WDC_DIRECTION direction, BOOL fPciCfg);

/* Get Interrupt Type description */
char *WDC_DIAG_IntTypeDescriptionGet(DWORD dwIntType);

/* -----------------------------------------------
    PCI
   ----------------------------------------------- */
/* Print PCI device location information */
void WDC_DIAG_PciSlotPrint(WD_PCI_SLOT *pPciSlot);
/* Print PCI device location information to file*/
void WDC_DIAG_PciSlotPrintFile(WD_PCI_SLOT *pPciSlot, FILE *fp);
/* Print PCI device location and resources information */
void WDC_DIAG_PciDeviceInfoPrint(WD_PCI_SLOT *pPciSlot, BOOL dump_cfg);
/* Print PCI device location and resources information to file */
void WDC_DIAG_PciDeviceInfoPrintFile(WD_PCI_SLOT *pPciSlot, FILE *fp, BOOL dump_cfg);
/* Print location and resources information for all connected PCI devices */
void WDC_DIAG_PciDevicesInfoPrintAll(BOOL dump_cfg);
/* Print location and resources information for all connected PCI devices to
 * file */
void WDC_DIAG_PciDevicesInfoPrintAllFile(FILE *fp, BOOL dump_cfg);

/* -----------------------------------------------
    PCMCIA
   ----------------------------------------------- */
/* Print PCMCIA device location information */
void WDC_DIAG_PcmciaSlotPrint(const WD_PCMCIA_SLOT *pPcmciaSlot);
/* Print PCMCIA device location and resources information */
void WDC_DIAG_PcmciaDeviceInfoPrint(const WD_PCMCIA_SLOT *pPcmciaSlot);
/* Print location and resources information for all connected PCMCIA devices */
void WDC_DIAG_PcmciaDevicesInfoPrintAll(void);
/* Print tuple information for all tuples found in the PCMCIA attribute space */
void WDC_DIAG_PcmciaTuplesPrintAll(WDC_DEVICE_HANDLE hDev, BOOL fDumpData);

#ifdef __cplusplus
}
#endif

#endif

#endif
