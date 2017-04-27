/*
 * 定义驱动相关全局变量(此文件中的class都为POD,可用memcpy等函数)  
 *
 * @author SigalHu
 * @date 2016/01/11
 */

#pragma once

#include "DSPDriverDataDef.h"

class ShareMem
{
public:
	HANDLE hMap;
	wchar_t name[25];
	LPVOID headAddr;
	UINT   memSize;   // hu byte
};

class DmaToDev
{
public:
	int DeviceX;
	int dmaFlag;                   // hu 0--dma未启动 1--通过文件 2--通过内存
	HWND hUserWndforEnd;           // hu dma结束消息句柄(若为NULL，则发送到注册消息)
	UINT memIdx;                   // hu 所使用的物理空间索引(从0开始)
	wchar_t binPath[MAX_PATH];     // hu 准备DMA传输的文件路径
	UINT *pBufAddr;                // hu 内存首地址
	WriteToDev pPhyAddr;           // hu 物理首地址(物理地址由底层赋值)
	WriteToDev dataLen;            // hu 数据长度(数据长度 dmaFlag=0,由底层赋值;dmaFlag=1,由用户赋值)
	WriteToDev endFlag;            // hu dma传输结束标志位
};

class DmaFromDev
{
public:
	int DeviceX;
	int dmaFlag;                   // hu 0--dma未启动 1--通过文件 2--通过内存
	HWND hUserWndforEnd;           // hu dma结束消息句柄(若为NULL，则发送到注册消息)
	UINT memIdx;                   // hu 所使用的物理空间索引(从0开始)
	wchar_t binPath[MAX_PATH];     // hu 准备DMA传输的文件路径
	UINT *pBufAddr;                // hu 内存首地址
	WriteToDev pPhyAddr;           // hu 物理首地址(物理地址由底层赋值)
	WriteToDev dataLen;            // hu 数据长度(数据长度 dmaFlag=0,由底层赋值;dmaFlag=1,由用户赋值)
	ReadFromDev endFlag;           // hu dma传输结束标志位
};

class ApplyForPa
{
public:
	int DeviceX;
	DWORD dwDMABufSize;            // hu 所要申请的物理空间大小
};

class FreePa
{
public:
	int DeviceX;
	UINT memIdx;                   // hu 所要释放的物理空间索引
}; 