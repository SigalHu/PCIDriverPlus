/* Jungo Confidential. Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */

/*
 * File - bits.h
 */

#ifndef _BITS_H_
#define _BITS_H_

enum 
{
    BIT0  = 0x00000001,
    BIT1  = 0x00000002,
    BIT2  = 0x00000004,
    BIT3  = 0x00000008,
    BIT4  = 0x00000010,
    BIT5  = 0x00000020,
    BIT6  = 0x00000040,
    BIT7  = 0x00000080,
    BIT8  = 0x00000100,
    BIT9  = 0x00000200,
    BIT10 = 0x00000400,
    BIT11 = 0x00000800,
    BIT12 = 0x00001000,
    BIT13 = 0x00002000,
    BIT14 = 0x00004000,
    BIT15 = 0x00008000,
    BIT16 = 0x00010000,
    BIT17 = 0x00020000,
    BIT18 = 0x00040000,
    BIT19 = 0x00080000,
    BIT20 = 0x00100000,
    BIT21 = 0x00200000,
    BIT22 = 0x00400000,
    BIT23 = 0x00800000,
    BIT24 = 0x01000000,
    BIT25 = 0x02000000,
    BIT26 = 0x04000000,
    BIT27 = 0x08000000,
    BIT28 = 0x10000000,
    BIT29 = 0x20000000,
    BIT30 = 0x40000000,
    BIT31 = (int)0x80000000
};

#define SWAP_ENDIAN_16(data) \
    ((((data) & (WORD)0x00FF) << 8) | \
    (((data) & (WORD)0xFF00) >> 8))

#define SWAP_ENDIAN_32(data) \
    ((((data) & 0x000000FF) << 24) | \
    (((data) & 0x0000FF00) << 8) | \
    (((data) & 0x00FF0000) >> 8) | \
    (((data) & 0xFF000000) >> 24))

#define SWAP_ENDIAN_64(data) \
    ((((data) & 0x00000000000000FFULL) << 56) | \
    (((data) & 0x000000000000FF00ULL) << 40) | \
    (((data) & 0x0000000000FF0000ULL) << 24) | \
    (((data) & 0x00000000FF000000ULL) << 8) | \
    (((data) & 0x000000FF00000000ULL) >> 8) | \
    (((data) & 0x0000FF0000000000ULL) >> 24) | \
    (((data) & 0x00FF000000000000ULL) >> 40) | \
    (((data) & 0xFF00000000000000ULL) >> 56))

// Endiannes conversion for PCI Master access (access is initiated by device)
#if defined(POWERPC) || defined(SPARC) || defined (PPC64)
    #define htod16(data) SWAP_ENDIAN_16(data)
    #define htod32(data) SWAP_ENDIAN_32(data)
    #define htod64(data) SWAP_ENDIAN_64(data)
#else
    #define htod16(data) (data)
    #define htod32(data) (data)
    #define htod64(data) (data)
#endif

// Endiannes conversion for PCI Target access (access is initiated by host)
#if defined(POWERPC) || defined (PPC64)
    #define dtoh16(data) SWAP_ENDIAN_16(data)
    #define dtoh32(data) SWAP_ENDIAN_32(data)
    #define dtoh64(data) SWAP_ENDIAN_64(data)
#else
    #define dtoh16(data) (data)
    #define dtoh32(data) (data)
    #define dtoh64(data) (data)
#endif

#endif
