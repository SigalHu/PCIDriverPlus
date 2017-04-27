/*
 * pci6455驱动相关函数对外接口
 *
 * @author SigalHu
 * @date 2015/12/21
 */

#ifndef _PCI_DRIVER_LIB_HEAD_H_
#define _PCI_DRIVER_LIB_HEAD_H_

// hu 消除重定义错误/////////////////////////////////
#if defined(_WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(_WIN64)
#include <windows.h>
#endif
/////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

// hu //////////////////////////////////////////////////////
#define WM_CALLBACKMESSAGE_0 WM_USER+470
#define DEVNUM_MAX 5
extern HWND ghWnd[DEVNUM_MAX];
////////////////////////////////////////////////////////////

// hu 初始化驱动与驱动库
// hu 0 -- 成功 1 -- 未扫描到PCI设备 2 -- 获取设备驱动操作句柄失败 3 -- 初始化设备驱动库失败
DWORD Initialize_DSPDriver(int deviceX);

// hu 向指定DSP地址写32bit数据
// hu 0 -- 成功 1 -- PCI设备写入失败
DWORD WriteToDSP(int deviceX,int barX, unsigned long dwoffset, unsigned int write32,unsigned int repeatTimes);

// hu 向指定DSP地址写1KB数据
// hu 0 -- 成功 1 -- PCI设备写入失败
DWORD WriteToDSP_256Words(int deviceX,int barX, DWORD dwoffset, UINT32 *write256Words);

// hu 向指定DSP地址读32bit数据
// hu 0 -- 成功 1 -- PCI设备读取失败
DWORD ReadfromDSP(int deviceX,int barX, unsigned long dwoffset, unsigned int *read32);

// hu 向指定DSP地址读1KB数据
// hu 0 -- 成功 1 -- PCI设备读取失败
DWORD ReadfromDSP_256Words(int deviceX,int barX, DWORD dwoffset, UINT32 *read256Words);

// hu 向驱动程序传递消息句柄
// hu 0 -- 成功 1 -- 传递句柄失败
DWORD DllFun(int deviceX,HWND hWn);

// hu 使能来自DSP的中断
// hu 0 -- 成功 1 -- 使能PCI中断失败
DWORD EnableInterruptFromDSP(int deviceX);

// hu 屏蔽来自DSP的中断
// hu 0 -- 成功 1 -- 屏蔽PCI中断失败
DWORD DisEnableInterruptFromDSP(int deviceX);

// hu 向DSP发送中断
// hu 0 -- 成功
DWORD SendIntToDSP(int deviceX);

// hu 申请物理地址连续的内存空间
// hu 0 -- 成功 1 -- 所申请的物理空间数量超过上限10 2 -- 申请物理空间失败 3 -- CPU缓存与物理空间同步失败
DWORD ApplyForPA(int deviceX, DWORD dwDMABufSize, void * * pBufAddress, DWORD * pBufPhysicalAddress, DWORD * DMAHandle);

// hu 释放所申请的空间
// hu 0 -- 成功 1 -- I/O缓存与物理空间同步失败 2 -- 释放物理空间失败
DWORD FreePA(int deviceX, DWORD DMAHandle);

// hu 关闭驱动与驱动库
// hu 0 -- 成功 1 -- 关闭PCI驱动失败 2 -- 卸载设备驱动库失败 3 -- 关闭PCI驱动与卸载设备驱动库失败
DWORD Close_DSPDriver(int deviceX);
//void RandGroupToDSP_to_Buffer(void * RandGroupToDSP);
//void Buffer_to_EDMA_Receive(void * EDMA_Receive);*/

#ifdef __cplusplus
}
#endif

#endif