/*
 * 定义驱动相关全局变量(此文件中的class都为POD,可用memcpy等函数) 
 *
 * @author SigalHu
 * @date 2016/01/11
 */

#pragma once

//#define DEBUG_HU

class ShareMem
{
public:
	HANDLE hMap;
	wchar_t name[25];
	LPVOID headAddr;
	UINT   memSize;   // hu byte
};

class driver
{
public:
	// hu 0 -- 成功 1 -- 未扫描到PCI设备 2 -- 获取设备驱动操作句柄失败 3 -- 初始化设备驱动库失败
	int status;                                       // hu 驱动状态
	UINT memNum;                                      // hu 所要申请的物理空间个数
	// hu 0 -- 成功 1 -- 所申请的物理空间数量超过上限10 2 -- 申请物理空间失败 3 -- CPU缓存与物理空间同步失败
	int memStatus[10];                                // hu 物理空间状态
	UINT memSize[10];                                 // hu 所要申请的物理空间大小
	void * EDMAWinDriver_Device[10];                  // hu 指向底层驱动申请的内存空间首地址(操作系统可用的虚拟地址)
	unsigned long EDMAPhysicalAddress_Device[10];     // hu 物理地址
	unsigned long EDMAHandle_Device[10];              // hu 物理空间句柄
};

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

class DmaToDevFifo
{
public:
	DmaToDev dma2Dev;
	DmaToDevFifo* pNext;

	DmaToDevFifo(DmaToDev *dma2dev)
	{
		memcpy(&dma2Dev,dma2dev,sizeof(DmaToDev));
		pNext = NULL;
	}
};

class DmaSource
{
public:
	int DeviceX;
	int dmaFlag;                   // hu 0--dma未启动 1--通过文件 2--通过内存
	HWND hUserWndforEnd;           // hu dma结束消息句柄(若为NULL，则发送到注册消息)
	FILE *dmaFp;                   // hu 文件指针
	UINT *pShareAddr;              // hu 内存地址
	UINT *pBufAddr;                // hu 对应于物理地址的虚拟地址
	UINT memSize;                  // hu 物理空间(共享空间)大小
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

class DmaFromDevFifo
{
public:
	DmaFromDev dma2Pc;
	DmaFromDevFifo* pNext;

	DmaFromDevFifo(DmaFromDev *dma2pc)
	{
		memcpy(&dma2Pc,dma2pc,sizeof(DmaFromDev));
		pNext = NULL;
	}
};

class DmaDest
{
public:
	int DeviceX;
	int dmaFlag;                   // hu 0--dma未启动 1--通过文件 2--通过内存
	HWND hUserWndforEnd;           // hu dma结束消息句柄(若为NULL，则发送到注册消息)
	FILE *dmaFp;                   // hu 文件指针
	UINT *pShareAddr;              // hu 内存地址
	UINT *pBufAddr;                // hu 对应于物理地址的虚拟地址
	UINT memSize;                  // hu 物理空间(共享空间)大小
	UINT dmaLen;                   // hu DMA传输的数据长度
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

class TrayShow
{
public:
	int status;           // hu 托盘状态  0--不显示 1--显示
	HMENU hmenu;          // hu 菜单句柄
	NOTIFYICONDATA nid;
	UINT WM_TASKBARCREATED;
};