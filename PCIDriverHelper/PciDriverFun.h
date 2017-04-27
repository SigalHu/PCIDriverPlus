/*
 * 定义驱动相关函数
 *
 * @author SigalHu
 * @date 2016/01/11
 */
#pragma once

#include "DSPDriverDataDef.h"

//#define DEBUG_HU
//#define XP_HU
#define CSharp_HU
#define WM_CALLBACKMESSAGE_0 WM_USER+470
#define DEVNUM_MAX 5

#ifdef XP_HU
const int addrOffset4FPGA = 8;
#else
const int addrOffset4FPGA = 4;
#endif

#ifdef _EXPORTING
#define DRIVER_DECLSPEC    __declspec(dllexport)
#else
#define DRIVER_DECLSPEC    __declspec(dllimport)
#endif

#ifdef CSharp_HU
extern "C" {
#endif

// hu 获得最新的错误信息
DRIVER_DECLSPEC wchar_t * __stdcall GetLastDriverError();

// hu 将驱动状态转化为字符串
DRIVER_DECLSPEC wchar_t * __stdcall DriverStatus2Str(int driverStatus);

// hu 将驱动所申请的物理空间状态转化为字符串
DRIVER_DECLSPEC wchar_t * __stdcall DriverMemStatus2Str(int driverMemStatus,UINT driverMemSize);

// hu 将动态加载FPGA的返回值转化为字符串
DRIVER_DECLSPEC wchar_t * __stdcall IniFpgaFlag2Str(UINT iniFpgaFlag);

// hu 初始化驱动程序
// hu 0--成功 1--驱动程序未运行 2--获取应用程序路径失败 3--获取驱动程序路径失败
//    4--驱动程序与该程序未运行在同一路径 5--打开共享内存失败 6--获取共享内存首地址失败
DRIVER_DECLSPEC DWORD __stdcall InitAllDSPDriver(DSPDriver *dspDriver,int driverNum=DEVNUM_MAX,bool isStrongCorrelation=true);

// hu 获得指定驱动状态
DRIVER_DECLSPEC void  __stdcall GetDSPDevStatus(DSPDriver *dspDriver,int index=0);

// hu 刷新驱动状态
// hu 返回初始化驱动数目
DRIVER_DECLSPEC int   __stdcall RefreshDSPDevStatus(DSPDriver *dspDriver,int driverNum=DEVNUM_MAX);

// hu 注册消息传递句柄
DRIVER_DECLSPEC void  __stdcall RegistHWNDToDev(HWND hUserWnd);

// hu 设置开机自启动
DRIVER_DECLSPEC void  __stdcall SetDSPAutoRun(BOOL Enable);

// hu 向指定DSP地址写32bit数据
// hu 0 -- 成功 1 -- PCI设备写入失败
DRIVER_DECLSPEC DWORD __stdcall WriteToDSP(int deviceX,int barX, unsigned long dwoffset, unsigned int write32,unsigned int repeatTimes=1);

// hu 向指定DSP地址读32bit数据
// hu 0 -- 成功 1 -- PCI设备读取失败
DRIVER_DECLSPEC DWORD __stdcall ReadFromDSP(int deviceX,int barX, unsigned long dwoffset, unsigned int *read32);

// hu 通过文件向DSP进行DMA传输(推荐使用)
// hu memIdx为使用的物理空间索引(一般为0)        | hEndWnd若为NULL,则向注册窗口发送中断号
// hu pPhyAddr.Write32,dataLen.Write32由底层写入 | endFlag若为NULL,在传输结束时不写入结束标志
// hu 1--物理空间索引不能大于等于10 2--设备号(deviceX)超过范围 3--物理空间索引超出范围 
//    4--索引物理空间状态出错 5--dma传输文件无法打开 6--结束标识不能为0
DRIVER_DECLSPEC DWORD __stdcall DmaFileToDSP(int deviceX,UINT memIdx,wchar_t *binPath,WriteToDev pPhyAddr,WriteToDev dataLen,WriteToDev *endFlag=NULL,HWND hEndWnd=NULL);

// hu 从DSP通过DMA传输到指定路径文件(推荐使用)
// hu memIdx为使用的物理空间索引(一般为0) | hEndWnd若为NULL,则向注册窗口发送中断号 | pPhyAddr.Write32由底层写入
// hu endFlag若为NULL,在传输时不读取结束标志,DMA传输的数据量为dataLen.Write32时结束DMA传输
// hu endFlag若不为NULL，则在DMA传输时只要读取到endFlag.Read32立即结束DMA传输(每次DMA传输完读取)
// hu 1--物理空间索引不能大于等于10 2--设备号(deviceX)超过范围 3--物理空间索引超出范围 
//    4--索引物理空间状态出错 5--dma传输文件无法打开 6--DMA传输的数据量(Byte)不能为0
DRIVER_DECLSPEC DWORD __stdcall DmaDSPToFile(int deviceX,UINT memIdx,wchar_t *binPath,WriteToDev pPhyAddr,WriteToDev dataLen,ReadFromDev *endFlag=NULL,HWND hEndWnd=NULL);

// hu 获得内存虚拟地址(与DmaMemToDSP,DmaDSPToMem配套使用)
// hu 0--成功 1--物理空间索引不能大于等于10 2--设备号(deviceX)只能为0或1 3--物理空间索引超出范围
//    4--索引物理空间状态出错 5--打开共享内存失败 6--获取共享内存首地址失败 7--内存泄漏
DRIVER_DECLSPEC DWORD __stdcall GetMemAddr(int deviceX,UINT memIdx,UINT **pBufAddr);

// hu 通过内存向DSP进行DMA传输(不推荐使用)
// hu deviceX为设备号(与GetMemAddr中对应) | memIdx为使用的物理空间索引(与GetMemAddr中对应)
// hu pBufAddr为虚拟首地址(与GetMemAddr中对应) | hEndWnd若为NULL,则向注册窗口发送中断号
// hu dataLen.Write32由用户写入(Byte) | pPhyAddr.Write32由底层写入 | 若传输未结束,将endFlag置为NULL
// hu 1--物理空间索引不能大于等于10 2--设备号(deviceX)超过范围 3--物理空间索引超出范围 
//    4--索引物理空间状态出错 5--内存空间大小与物理空间不匹配 6--数据长度(Byte)超出范围
DRIVER_DECLSPEC DWORD __stdcall DmaMemToDSP(int deviceX,UINT memIdx,UINT *pBufAddr,WriteToDev pPhyAddr,WriteToDev dataLen,WriteToDev *endFlag=NULL,HWND hEndWnd=NULL);

// hu 从DSP通过DMA传输到指定内存地址(不推荐使用)
// hu memIdx为使用的物理空间索引(一般为0) | hEndWnd若为NULL,则向注册窗口发送中断号 | pPhyAddr.Write32由底层写入
// hu 1--物理空间索引不能大于等于10 2--设备号(deviceX)超过范围 3--物理空间索引超出范围 
//    4--索引物理空间状态出错 5--内存空间大小与物理空间不匹配 6--数据长度(Byte)超出范围
DRIVER_DECLSPEC DWORD __stdcall DmaDSPToMem(int deviceX,UINT memIdx,UINT *pBufAddr,WriteToDev pPhyAddr,WriteToDev dataLen,HWND hEndWnd=NULL);

// hu 关闭所有驱动与驱动库并退出驱动程序
DRIVER_DECLSPEC void  __stdcall CloseAllDSPDriver();

// hu 初始化驱动与驱动库(非特定情况，不需要使用)
// hu 0 -- 成功 1 -- 未扫描到PCI设备 2 -- 获取设备驱动操作句柄失败 3 -- 初始化设备驱动库失败
DRIVER_DECLSPEC DWORD __stdcall InitDSPDriver(int deviceX);

// hu 关闭驱动与驱动库(非特定情况，不需要使用)
// hu 0 -- 成功 1 -- 关闭PCI驱动失败 2 -- 卸载设备驱动库失败 3 -- 关闭PCI驱动与卸载设备驱动库失败
DRIVER_DECLSPEC DWORD __stdcall CloseDSPDriver(int deviceX);

// hu 申请物理地址连续的内存空间(非特定情况，不需要使用)
// hu 0--成功 1--设备号(deviceX)超过范围 2--已申请的物理空间数目已达上限 3--申请空间大小不能为0 
//    4--所申请的物理空间数量超过上限10 5--申请物理空间失败 6--CPU缓存与物理空间同步失败
DRIVER_DECLSPEC DWORD __stdcall ApplyForPA(int deviceX,DWORD dwDMABufSize);

// hu 释放所申请的空间(非特定情况，不需要使用)
// hu 0 -- 成功 1--设备号(deviceX)超过范围 2--物理空间索引超出范围 3--索引物理空间状态出错
//    4--I/O缓存与物理空间同步失败 5--释放物理空间失败
DRIVER_DECLSPEC DWORD __stdcall FreePA(int deviceX,UINT memIdx);

#ifdef CSharp_HU
}
#endif