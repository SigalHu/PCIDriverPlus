/* Jungo Confidential. Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */

#ifndef _PCI_REGS_H_
#define _PCI_REGS_H_

/********************************************************************************
* File - pci_regs.h - PCI configuration space and address spaces definitions    *
*********************************************************************************/

/* -----------------------------------------------
    PCI configuration registers offsets
   ----------------------------------------------- */
enum {
    PCI_VID   = 0x00, /* Vendor ID */
    PCI_DID   = 0x02, /* Device ID */
    PCI_CR    = 0x04, /* Command register */
    PCI_SR    = 0x06, /* Status register */
    PCI_REV   = 0x08, /* Revision ID */
    PCI_CCR   = 0x09, /* Class code */
    PCI_CCSC  = 0x0a, /* Sub class code */
    PCI_CCBC  = 0x0b, /* Base class code */
    PCI_CLSR  = 0x0c, /* Cache line size */
    PCI_LTR   = 0x0d, /* Latency timer */
    PCI_HDR   = 0x0e, /* Header type */
    PCI_BISTR = 0x0f, /* Built-in self test */
    PCI_BAR0  = 0x10, /* Base address register */
    PCI_BAR1  = 0x14, /* Base address register */
    PCI_BAR2  = 0x18, /* Base address register */
    PCI_BAR3  = 0x1c, /* Base address register */
    PCI_BAR4  = 0x20, /* Base address register */
    PCI_BAR5  = 0x24, /* Base address register */
    PCI_CIS   = 0x28, /* CardBus CIS pointer */
    PCI_SVID  = 0x2c, /* Sub-system vendor ID */
    PCI_SDID  = 0x2e, /* Sub-system device ID */
    PCI_EROM  = 0x30, /* Expansion ROM base address */
    PCI_CAP   = 0x34, /* New capability pointer */
    PCI_ILR   = 0x3c, /* Interrupt line */
    PCI_IPR   = 0x3d, /* Interrupt pin */
    PCI_MGR   = 0x3e, /* Minimum required burst period */
    PCI_MLR   = 0x3f  /* Maximum latency - How often device must gain PCI bus access */
};

#define PCI_HDR_NORMAL 0
#define PCI_HDR_BRIDGE 1
#define PCI_HDR_CARDBUS 2

/* Size of PCI configuration space */
#define PCI_CONFIG_SPACE_SIZE 0x100

/* PCI base address spaces (BARs) */
enum {
    AD_PCI_BAR0 = 0,
    AD_PCI_BAR1 = 1,
    AD_PCI_BAR2 = 2,
    AD_PCI_BAR3 = 3,
    AD_PCI_BAR4 = 4,
    AD_PCI_BAR5 = 5,
    AD_PCI_BARS = 6
};

#endif
