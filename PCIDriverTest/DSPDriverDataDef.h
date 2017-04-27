/*
 * 定义全局变量
 *
 * @author 胡续俊
 * @date 2016/01/12
 */

#pragma once

class DSPDriver
{
public:
	// hu 0 -- 成功 1 -- 未扫描到PCI设备 2 -- 获取设备驱动操作句柄失败 3 -- 初始化设备驱动库失败
	int status;                                       // hu 驱动状态
	UINT memNum;                                      // hu 所申请的物理空间个数
	// hu 0 -- 成功 1 -- 所申请的物理空间数量超过上限10 2 -- 申请物理空间失败 3 -- CPU缓存与物理空间同步失败
	int memStatus[10];                                // hu 物理空间状态
	UINT memSize[10];                                 // hu 所要申请的物理空间大小
};

class WriteToDev
{
public:
	int DeviceX;
	int BarX;
	unsigned long dwOffset;
	unsigned int Write32;
	unsigned int RepeatTimes;
};

class ReadFromDev
{
public:
	int DeviceX;
	int BarX;
	unsigned long dwOffset;
	unsigned int Read32;
};

class dmaDeliver
{
public:
	UINT  Deliver_Time;
	CFile Deliver_FP;
};