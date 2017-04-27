/* Jungo Confidential. Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */

#ifndef _PCCARD_CIS_H_
#define _PCCARD_CIS_H_

/*
 * File - pccard_cis.h - PCCARD card information structure tuples definitions
 */

#include "windrvr.h"

/* -----------------------------------------------
    PCCARD card information structure tuple codes
   ----------------------------------------------- */
#define CISTPL_NULL        0x00
#define CISTPL_DEVICE      0x01
#define CISTPL_CHECKSUM    0x10
#define CISTPL_LONGLINK_A  0x11
#define CISTPL_LONGLINK_C  0x12
#define CISTPL_LINKTARGET  0x13
#define CISTPL_NO_LINK     0x14
#define CISTPL_VERS_1      0x15
#define CISTPL_ALTSTR      0x16
#define CISTPL_DEVICE_A    0x17
#define CISTPL_JEDEC_C     0x18
#define CISTPL_JEDEC_A     0x19
#define CISTPL_MANFID      0x20
#define CISTPL_FUNCID      0x21
#define CISTPL_FUNCE       0x22
#define CISTPL_VERS_2      0x40
#define CISTPL_FORMAT      0x41
#define CISTPL_GEOMETRY    0x42
#define CISTPL_BYTEORDER   0x43
#define CISTPL_DATE        0x44
#define CISTPL_BATTERY     0x45
#define CISTPL_ORG         0x46
#define CISTPL_END         0xFF
typedef BYTE PCCARD_CISTPL_CODE;

#endif
