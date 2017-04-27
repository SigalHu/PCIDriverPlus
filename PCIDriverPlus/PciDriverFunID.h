/*
 * 定义驱动相关函数ID 
 *
 * @author SigalHu
 * @date 2016/01/11
 */

#pragma once
#define WM_TRAYMSG WM_USER+400
#define WM_DMAFIFO WM_USER+401

#define ID_FUN_InitDSPDriver         1
#define ID_FUN_CloseDSPDriver        2
#define ID_FUN_WriteToDSP            3
#define ID_FUN_ReadFromDSP           4
#define ID_FUN_ApplyForPA            5
#define ID_FUN_FreePA                6

#define ID_SYS_RegistHWNDToDev       7
#define ID_SYS_GetDevStatus          8
#define ID_SYS_DMAToDev              9
#define ID_SYS_ApplyForShareMem      10
#define ID_SYS_DMAFromDev            11
#define ID_SYS_SetAutoRun            12
#define ID_SYS_CreatTray             13

#define ID_FIFO_DMAToDev             14
#define ID_FIFO_DMAFromDev           15