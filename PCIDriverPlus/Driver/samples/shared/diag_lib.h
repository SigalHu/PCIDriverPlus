/* Jungo Confidential. Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */

#ifndef _DIAG_LIB_H_
#define _DIAG_LIB_H_

/******************************************************************************
*  File: diag_lib.h - Shared WD user-mode diagnostics API header.             *
*******************************************************************************/

#if !defined(__KERNEL__)

#ifdef __cplusplus
extern "C" {
#endif

#include "windrvr.h"

/*************************************************************
  General definitions
 *************************************************************/
/* Cancel selection */
#define DIAG_CANCEL 'x'

/* Exit menu */
#define DIAG_EXIT_MENU 99

/****************************************************************
* Function: DIAG_PrintHexBuffer()                               *
*   Print a buffer in hexadecimal format                        *
* Parameters:                                                   *
*   pBuf [in] - Pointer to buffer                               *
*   dwBytes [in] - Number of bytes to print                     *
*   fAscii [in] - If TRUE, print the buffer also as an ASCII    *
*                 string                                        *
* Return Value:                                                 *
*   None                                                        *
*****************************************************************/
void DIAG_PrintHexBuffer(PVOID pBuf, DWORD dwBytes, BOOL fAscii);

/****************************************************************
* Function: DIAG_GetHexChar                                             *
*   Get a hexadecimal character from user                       *
* Parameters:                                                   *
*   None                                                        *
* Return Value:                                                 *
*   Character received                                          *
*****************************************************************/
int DIAG_GetHexChar(void);

/****************************************************************
* Function: DIAG_GetHexBuffer()                                 *
*   Get a hexadecimal buffer from user                          *
* Parameters:                                                   *
*   pBuffer [in/out] Pointer to buffer to be filled with data   *
*   dwBytes [in] Length of buffer                               *
* Return Value:                                                 *
*   Size of buffer received                                     *
*****************************************************************/
DWORD DIAG_GetHexBuffer(PVOID pBuffer, DWORD dwBytes);
    
typedef enum {
    DIAG_INPUT_CANCEL = -1,
    DIAG_INPUT_FAIL = 0,
    DIAG_INPUT_SUCCESS = 1
} DIAG_INPUT_RESULT;

/* Get menu option from user */
DIAG_INPUT_RESULT DIAG_GetMenuOption(DWORD *pdwOption, DWORD dwMax);

/* Get numeric value from user
   To avoid range check, set min == max (e.g. set both min and max to 0) */
DIAG_INPUT_RESULT DIAG_InputNum(PVOID pInput, const CHAR *sInputText,
    BOOL fHex, DWORD dwSize, UINT64 min, UINT64 max);

#define DIAG_InputDWORD(pdwInput, sInputText, fHex, min, max) \
    DIAG_InputNum((PVOID)(pdwInput), sInputText, fHex, sizeof(DWORD), min, max)

#define DIAG_InputWORD(pwInput, sInputText, fHex, min, max) \
    DIAG_InputNum((PVOID)(pwInput), sInputText, fHex, sizeof(WORD), min, max)

#define DIAG_InputBYTE(pbInput, sInputText, fHex, min, max) \
    DIAG_InputNum((PVOID)(pwInput), sInputText, fHex, sizeof(BYTE), min, max)

#define DIAG_InputUINT32(pu32Input, sInputText, fHex, min, max) \
    DIAG_InputNum((PVOID)(pu32Input), sInputText, fHex, sizeof(UINT32), min, \
        max)

#define DIAG_InputUINT64(pu64Input, sInputText, fHex, min, max) \
    DIAG_InputNum((PVOID)(pu64Input), sInputText, fHex, sizeof(UINT64), min, \
        max)
    
#ifdef __cplusplus
}
#endif

#endif

#endif
