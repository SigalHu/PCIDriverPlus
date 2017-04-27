/* Jungo Confidential. Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */

/*****************************************************************************
*  File: wdc_mem_io.c - Implementation of WDC functions for reading/writing  *
*       a device's memory and I/O addresses                                  *
******************************************************************************/

#include "wdc_lib.h"
#include "wdc_defs.h"
#include "wdc_err.h"

/*************************************************************
  General definitions
 *************************************************************/
/* Data types definitions (based on WDC address access mode definitions) */
typedef BYTE U8;
typedef WORD U16;
typedef UINT32 U32;
typedef UINT64 U64;

/* Get data type from address access mode */
#define MODE_TO_DATA_TYPE(mode) mode##_DT

/* WD_TRANSFER data types defintions (based on WDC address access mode defintions) */
#define WDC_MODE_8_TRANS_DT Byte
#define WDC_MODE_16_TRANS_DT Word
#define WDC_MODE_32_TRANS_DT Dword
#define WDC_MODE_64_TRANS_DT Qword

/* Get WD_TRANSFER data type from address access mode */
#define MODE_TO_WD_TRANS_DATA_TYPE(mode) mode##_TRANS_DT

/* Transfer commands definitions (based on address access mode definitions) */
/* I/O commands */
#define WDC_MODE_8_TC_RP  RP_BYTE
#define WDC_MODE_16_TC_RP RP_WORD
#define WDC_MODE_32_TC_RP RP_DWORD
#define WDC_MODE_64_TC_RP RP_QWORD
#define WDC_MODE_8_TC_WP  WP_BYTE
#define WDC_MODE_16_TC_WP WP_WORD
#define WDC_MODE_32_TC_WP WP_DWORD
#define WDC_MODE_64_TC_WP WP_QWORD

#define WDC_MODE_8_TC_RPS  RP_SBYTE
#define WDC_MODE_16_TC_RPS RP_SWORD
#define WDC_MODE_32_TC_RPS RP_SDWORD
#define WDC_MODE_64_TC_RPS RP_SQWORD
#define WDC_MODE_8_TC_WPS  WP_SBYTE
#define WDC_MODE_16_TC_WPS WP_SWORD
#define WDC_MODE_32_TC_WPS WP_SDWORD
#define WDC_MODE_64_TC_WPS WP_SQWORD

/* Memory commands */
#define WDC_MODE_8_TC_RM  RM_BYTE
#define WDC_MODE_16_TC_RM RM_WORD
#define WDC_MODE_32_TC_RM RM_DWORD
#define WDC_MODE_64_TC_RM RM_QWORD
#define WDC_MODE_8_TC_WM  WM_BYTE
#define WDC_MODE_16_TC_WM WM_WORD
#define WDC_MODE_32_TC_WM WM_DWORD
#define WDC_MODE_64_TC_WM WM_QWORD

#define WDC_MODE_8_TC_RMS  RM_SBYTE
#define WDC_MODE_16_TC_RMS RM_SWORD
#define WDC_MODE_32_TC_RMS RM_SDWORD
#define WDC_MODE_64_TC_RMS RM_SQWORD
#define WDC_MODE_8_TC_WMS  WM_SBYTE
#define WDC_MODE_16_TC_WMS WM_SWORD
#define WDC_MODE_32_TC_WMS WM_SDWORD
#define WDC_MODE_64_TC_WMS WM_SQWORD

/* Get transfer command from address access mode */
#define MODE_TO_TRANS_CMD(mode, direction, fBlock, fMem) \
    fBlock ? \
        (WDC_READ == direction ? (fMem ? mode##_TC_RMS : mode##_TC_RPS) : \
            (fMem ? mode##_TC_WMS : mode##_TC_WPS)) : \
        (WDC_READ == direction ? (fMem ? mode##_TC_RM : mode##_TC_RP) : \
            (fMem ? mode##_TC_WM : mode##_TC_WP)) \

static inline DWORD get_trans_cmd(WDC_ADDR_MODE mode, WDC_DIRECTION direction, BOOL fBlock, BOOL fMem)
{
    switch (mode)
    {
    case WDC_MODE_8:
        return MODE_TO_TRANS_CMD(WDC_MODE_8, direction, fBlock, fMem);
    case WDC_MODE_16:
        return MODE_TO_TRANS_CMD(WDC_MODE_16, direction, fBlock, fMem);
    case WDC_MODE_32:
        return MODE_TO_TRANS_CMD(WDC_MODE_32, direction, fBlock, fMem);
    case WDC_MODE_64:
        return MODE_TO_TRANS_CMD(WDC_MODE_64, direction, fBlock, fMem);
    default:
        return 0;
    }
}

/* -----------------------------------------------
    Is memory/IO address space active
   ----------------------------------------------- */
BOOL DLLCALLCONV WDC_AddrSpaceIsActive(WDC_DEVICE_HANDLE hDev, DWORD dwAddrSpace)
{
    WDC_ADDR_DESC *pAddrDesc;

#if defined(DEBUG)
    if (!WdcIsValidDevHandle(hDev))
    {
        WDC_Err("WDC_AddrSpaceIsActive: %s", WdcGetLastErrStr());
        return FALSE;
    }
#endif

    pAddrDesc = WDC_GET_ADDR_DESC(hDev, dwAddrSpace);
    return (BOOL)pAddrDesc->dwBytes;
}
    
/* -----------------------------------------------
    Read/Write memory and I/O addresses
   ----------------------------------------------- */
#define READ_WRITE_ADDR_TRANS(pAddrDesc, dwOffset, pData, bytes, mode, \
    direction, options, dwStatus) \
{ \
    BOOL fMem = WDC_ADDR_IS_MEM(pAddrDesc); \
    WD_TRANSFER trans; \
    \
    BZERO(trans); \
    trans.dwPort = (pAddrDesc)->kptAddr + dwOffset; \
    trans.dwBytes = bytes; \
    trans.cmdTrans = get_trans_cmd(mode, direction, TRUE, fMem); \
    trans.Data.pBuffer = (PVOID)pData; \
    trans.fAutoinc = !(options & WDC_ADDR_RW_NO_AUTOINC); \
    \
    dwStatus = WD_Transfer(WDC_GetWDHandle(), &trans); \
}

#define DECLARE_READ_ADDR(bits) \
DWORD DLLCALLCONV WDC_ReadAddr##bits(WDC_DEVICE_HANDLE hDev, DWORD dwAddrSpace, KPTR dwOffset, U##bits *val) \
{ \
    DWORD dwStatus = WD_STATUS_SUCCESS; \
    WDC_ADDR_DESC *pAddrDesc = WDC_GET_ADDR_DESC(hDev, dwAddrSpace); \
\
    if (WDC_ADDR_IS_MEM(pAddrDesc)) \
        *val = WDC_ReadMem##bits(WDC_MEM_DIRECT_ADDR(pAddrDesc), dwOffset); \
    else \
    { \
        READ_WRITE_ADDR_TRANS(pAddrDesc, dwOffset, val, WDC_MODE_##bits, \
            WDC_MODE_##bits, WDC_READ, WDC_ADDR_RW_DEFAULT, dwStatus); \
    } \
    return dwStatus; \
}

#define DECLARE_WRITE_ADDR(bits) \
DWORD DLLCALLCONV WDC_WriteAddr##bits(WDC_DEVICE_HANDLE hDev, DWORD dwAddrSpace, KPTR dwOffset, U##bits val) \
{ \
    DWORD dwStatus = WD_STATUS_SUCCESS; \
    WDC_ADDR_DESC *pAddrDesc = WDC_GET_ADDR_DESC(hDev, dwAddrSpace); \
\
    if (WDC_ADDR_IS_MEM(pAddrDesc)) \
        WDC_WriteMem##bits(WDC_MEM_DIRECT_ADDR(pAddrDesc), dwOffset, val); \
    else \
    { \
        READ_WRITE_ADDR_TRANS(pAddrDesc, dwOffset, (PVOID)&val, WDC_MODE_##bits, \
            WDC_MODE_##bits, WDC_WRITE, WDC_ADDR_RW_DEFAULT, dwStatus); \
    } \
    return dwStatus; \
}

DECLARE_READ_ADDR(8)
DECLARE_READ_ADDR(16)
DECLARE_READ_ADDR(32)
DECLARE_READ_ADDR(64)

DECLARE_WRITE_ADDR(8)
DECLARE_WRITE_ADDR(16)
DECLARE_WRITE_ADDR(32)
DECLARE_WRITE_ADDR(64)

DWORD DLLCALLCONV WDC_ReadAddrBlock(WDC_DEVICE_HANDLE hDev, DWORD dwAddrSpace,
    KPTR dwOffset, DWORD dwBytes, PVOID pData, WDC_ADDR_MODE mode,
    WDC_ADDR_RW_OPTIONS options)
{
    DWORD dwStatus = WD_STATUS_SUCCESS;
    WDC_ADDR_DESC *pAddrDesc = WDC_GET_ADDR_DESC(hDev, dwAddrSpace);

    READ_WRITE_ADDR_TRANS(pAddrDesc, dwOffset, pData, dwBytes, mode, WDC_READ, options, dwStatus);
    return dwStatus;
}

DWORD DLLCALLCONV WDC_WriteAddrBlock(WDC_DEVICE_HANDLE hDev, DWORD dwAddrSpace,
    KPTR dwOffset, DWORD dwBytes, PVOID pData, WDC_ADDR_MODE mode,
    WDC_ADDR_RW_OPTIONS options)
{
    DWORD dwStatus = WD_STATUS_SUCCESS;
    WDC_ADDR_DESC *pAddrDesc = WDC_GET_ADDR_DESC(hDev, dwAddrSpace);

    READ_WRITE_ADDR_TRANS(pAddrDesc, dwOffset, pData, dwBytes, mode, WDC_WRITE, options, dwStatus);
    return dwStatus;
}

/* Read/Write multiple addresses */
DWORD DLLCALLCONV WDC_MultiTransfer(WD_TRANSFER *pTrans, DWORD dwNumTrans)
{
#if defined (DEBUG)
    if (!pTrans || !dwNumTrans)
    {
        WDC_Err("WDC_MultiTransfer: Error - %s\n",
            !pTrans ? "NULL WD_TRANSFER pointer" : "no transfer commands (dwNumTrans == 0)");
        return WD_INVALID_PARAMETER;
    }
#endif

    return WD_MultiTransfer(WDC_GetWDHandle(), pTrans, dwNumTrans);
}
