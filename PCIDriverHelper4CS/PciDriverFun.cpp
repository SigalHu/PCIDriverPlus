/*
 * 驱动相关函数
 *
 * @author 胡续俊
 * @date 2016/04/19
 */

#include "stdafx.h"
#include <tlhelp32.h> 
#include <Psapi.h>
#include "PciDriverFunID.h"
#include "PciDriverDataDef.h"
#include "PciDriverFun.h"

// hu 全局变量 /////////////////////////////////////////////
static HWND hDriverWnd;
static ShareMem shareMem0 = {NULL,L"ShareMemForPciDriver",NULL,1024};
static DSPDriver driver[DEVNUM_MAX];
static wchar_t *driverClassName = L"pcidriver";
wchar_t lastError[100] = L"无错误！";
wchar_t driverStatusStr[100];
////////////////////////////////////////////////////////////

// hu 内部函数 /////////////////////////////////////////////
// hu 获得驱动状态(内部使用)
// hu 返回初始化驱动数目
int GetDriverStatus(DSPDriver *dspDriver,int driverNum=DEVNUM_MAX);

// hu 重新申请共享内存(内部使用)
// hu 0--成功 1--打开共享内存失败 2--获取共享内存首地址失败 3--内存泄漏
DWORD ApplyForShareMem(ShareMem *shareMem);
////////////////////////////////////////////////////////////

// hu 获得最新的错误信息
DRIVER_DECLSPEC wchar_t * __stdcall GetLastDriverError()
{
	return lastError;
}

// hu 将驱动状态转化为字符串
DRIVER_DECLSPEC wchar_t * __stdcall DriverStatus2Str(int driverStatus)
{
	switch(driverStatus)
	{
	case 0: wcscpy(driverStatusStr,L"初始化PCI设备成功！");break;
	case 1: wcscpy(driverStatusStr,L"未扫描到PCI设备！");break;
	case 2: wcscpy(driverStatusStr,L"获取设备驱动操作句柄失败！");break;
	case 3: wcscpy(driverStatusStr,L"初始化设备驱动库失败！");break;
	default: wcscpy(driverStatusStr,L"不是驱动初始化函数的默认返回值！");break;
	}
	return driverStatusStr;
}

// hu 将驱动所申请的物理空间状态转化为字符串
DRIVER_DECLSPEC wchar_t * __stdcall DriverMemStatus2Str(int driverMemStatus,UINT driverMemSize)
{
	switch(driverMemStatus)
	{
	case 0: swprintf_s(driverStatusStr,L"申请%dByte连续物理空间成功！",driverMemSize);break;
	case 1: wcscpy(driverStatusStr,L"所申请的物理空间数量超过上限10！");break;
	case 2: wcscpy(driverStatusStr,L"申请物理空间失败！");break;
	case 3: wcscpy(driverStatusStr,L"CPU缓存与物理空间同步失败！");break;
	default: wcscpy(driverStatusStr,L"不是申请物理空间函数的默认返回值！");break;
	}
	return driverStatusStr;
}

// hu 将动态加载FPGA的返回值转化为字符串
DRIVER_DECLSPEC wchar_t * __stdcall IniFpgaFlag2Str(UINT iniFpgaFlag)
{
	switch(iniFpgaFlag)
	{
	case 0: wcscpy(driverStatusStr,L"动态加载成功！");break;
	case 1: wcscpy(driverStatusStr,L"FPGA初始化失败！");break;
	case 2: wcscpy(driverStatusStr,L"未收到FPGA的INTB信号，动态加载失败！");break;
	case 3: wcscpy(driverStatusStr,L"未收到FPGA的DONE信号，动态加载失败！");break;
	default: wcscpy(driverStatusStr,L"未收到约定返回值，动态加载失败！");break;
	}
	return driverStatusStr;
}

// hu 初始化驱动程序
// hu 0--成功 1--驱动程序未运行 2--获取应用程序路径失败 3--获取驱动程序路径失败
//    4--驱动程序与该程序未运行在同一路径 5--打开共享内存失败 6--获取共享内存首地址失败
DRIVER_DECLSPEC DWORD __stdcall InitAllDSPDriver(DSPDriver *dspDriver,int driverNum,bool isStrongCorrelation)
{
	wchar_t temp[20];
	int ii;

	// hu 获得驱动程序窗口句柄
	hDriverWnd = ::FindWindow(driverClassName,NULL);
	if (NULL == hDriverWnd) 
	{
		wcscpy(lastError, L"驱动程序未运行！");
		return 1;
	}

	if (isStrongCorrelation)
	{
		wchar_t progPath[MAX_PATH];    // hu 程序工作目录
		wchar_t driverPath[MAX_PATH];  // hu 驱动路径

		// hu 获得自身程序路径
		GetModuleFileNameW(NULL,progPath,MAX_PATH);
		wchar_t *progNametmp = wcsrchr(progPath,L'\\');
		if (NULL == progNametmp) 
		{
			wcscpy(lastError, L"获取应用程序路径失败！");
			return 2;
		}
		progNametmp++;
		*progNametmp = L'\0';

		// hu 获得驱动程序路径
		DWORD processID;
		MODULEENTRY32 pes;
		pes.dwSize = sizeof(MODULEENTRY32);

		::GetWindowThreadProcessId(hDriverWnd,&processID);
		HANDLE hfilename = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE , processID );
		Module32First(hfilename , &pes);
		::CloseHandle(hfilename);

		wcscpy(driverPath,pes.szExePath);
		wchar_t *driverNametmp = wcsrchr(driverPath,L'\\');
		if (NULL == driverNametmp) 
		{
			wcscpy(lastError, L"获取驱动程序路径失败！");
			return 3;
		}
		driverNametmp++;
		*driverNametmp = L'\0';

		// hu 判断驱动程序与该程序是否运行在同一路径
		for (ii = 0;ii<wcslen(driverPath);ii++)
		{
			if (driverPath[ii]>='A'&&driverPath[ii]<='Z')
			{
				driverPath[ii] += 32;  // hu 将盘符转化为小写
			}
			if (progPath[ii]>='A'&&progPath[ii]<='Z')
			{
				progPath[ii] += 32;  // hu 将盘符转化为小写
			}
		}
		if (wcscmp(progPath,driverPath))
		{
			wcscpy(lastError, L"驱动程序与该程序未运行在同一路径！");
			return 4;
		}
	}

	// 打开共享内存
	shareMem0.hMap = ::OpenFileMapping(FILE_MAP_ALL_ACCESS,0,shareMem0.name);
	if (NULL == shareMem0.hMap) 
	{
		wcscpy(lastError, L"打开共享内存失败！");
		return 5;
	}

	shareMem0.headAddr = ::MapViewOfFile(shareMem0.hMap,FILE_MAP_ALL_ACCESS,0,0,0);
	if (NULL == shareMem0.headAddr)
	{
		wcscpy(lastError, L"获取共享内存首地址失败！");
		return 6;
	}

	GetDriverStatus(driver,DEVNUM_MAX);
	for (ii=0;ii<driverNum;ii++)
	{
		GetDSPDevStatus(dspDriver,ii);
	}
	return 0;
}

// hu 获得驱动状态(内部使用)
// hu 返回初始化驱动数目
int GetDriverStatus(DSPDriver *dspDriver,int driverNum)
{
	COPYDATASTRUCT cds;
	cds.dwData = 0;
	cds.lpData = NULL;
	cds.cbData = 0;
	int ii;
	SendMessage(hDriverWnd,WM_COPYDATA,ID_SYS_GetDevStatus,(LPARAM)&cds);

	if (NULL != dspDriver)
	{
		memcpy(dspDriver,(DSPDriver*)shareMem0.headAddr,driverNum*sizeof(dspDriver[0]));
		memcpy(driver,dspDriver,driverNum*sizeof(dspDriver[0]));
	}

	for (ii=0;ii<DEVNUM_MAX;ii++)
	{
		if (-1 == dspDriver[ii].status)  return ii;
	}
	return DEVNUM_MAX;
}

// hu 获得指定驱动状态
DRIVER_DECLSPEC void __stdcall GetDSPDevStatus(DSPDriver *dspDriver,int index)
{
	dspDriver[index] = driver[index];
}

// hu 刷新驱动状态
// hu 返回初始化驱动数目
DRIVER_DECLSPEC int __stdcall RefreshDSPDevStatus(DSPDriver *dspDriver,int driverNum)
{
	return GetDriverStatus(dspDriver,driverNum);
}

// hu 注册消息传递句柄
DRIVER_DECLSPEC void __stdcall RegistHWNDToDev(HWND hUserWnd)
{
	COPYDATASTRUCT cds;
	cds.dwData = 0;
	cds.lpData = &hUserWnd;
	cds.cbData = sizeof(hUserWnd);
	SendMessage(hDriverWnd,WM_COPYDATA,ID_SYS_RegistHWNDToDev,(LPARAM)&cds);
}

// hu 设置开机自启动
DRIVER_DECLSPEC void __stdcall SetDSPAutoRun(BOOL Enable)
{
	COPYDATASTRUCT cds;
	cds.dwData = 0;
	cds.lpData = &Enable;
	cds.cbData = sizeof(Enable);
	SendMessage(hDriverWnd,WM_COPYDATA,ID_SYS_SetAutoRun,(LPARAM)&cds);
}

// hu 向指定DSP地址写32bit数据
// hu 0 -- 成功 1 -- PCI设备写入失败
DRIVER_DECLSPEC DWORD __stdcall WriteToDSP(int deviceX,int barX, unsigned long dwoffset, unsigned int write32,unsigned int repeatTimes)
{
	COPYDATASTRUCT cds;
	unsigned int status;
	WriteToDev write2Dev = {deviceX,barX,dwoffset,write32,repeatTimes};

	cds.dwData = 0;
	cds.lpData = &write2Dev;
	cds.cbData = sizeof(write2Dev);

	SendMessage(hDriverWnd,WM_COPYDATA,ID_FUN_WriteToDSP,(LPARAM)&cds);
	memcpy(&status,(unsigned int*)shareMem0.headAddr,sizeof(status));
	if (1 == status)
	{
		wcscpy(lastError, L"PCI设备写入失败！");
	}
	return status;
}

// hu 向指定DSP地址读32bit数据
// hu 0 -- 成功 1 -- PCI设备读取失败
DRIVER_DECLSPEC DWORD __stdcall ReadFromDSP(int deviceX,int barX, unsigned long dwoffset, unsigned int *read32)
{
	COPYDATASTRUCT cds;
	unsigned int status;
	ReadFromDev readFromDev = {deviceX,barX,dwoffset,0};

	cds.dwData = 0;
	cds.lpData = &readFromDev;
	cds.cbData = sizeof(readFromDev);

	SendMessage(hDriverWnd,WM_COPYDATA,ID_FUN_ReadFromDSP,(LPARAM)&cds);
	memcpy(&status,(unsigned int*)shareMem0.headAddr,sizeof(status));
	memcpy(read32,(unsigned int*)shareMem0.headAddr+1,sizeof(*read32));
	if (1 == status)
	{
		wcscpy(lastError, L"PCI设备读取失败！");
	}
	return status;
}

// hu 通过文件向DSP进行DMA传输(推荐使用)
// hu memIdx为使用的物理空间索引(一般为0)        | hEndWnd若为NULL,则向注册窗口发送中断号
// hu pPhyAddr.Write32,dataLen.Write32由底层写入 | endFlag若为NULL,在传输结束时不写入结束标志
// hu 1--物理空间索引不能大于等于10 2--设备号(deviceX)超过范围 3--物理空间索引超出范围 
//    4--索引物理空间状态出错 5--dma传输文件无法打开 6--结束标识不能为0
DRIVER_DECLSPEC DWORD __stdcall DmaFileToDSP(int deviceX,UINT memIdx,wchar_t *binPath,WriteToDev pPhyAddr,WriteToDev dataLen,WriteToDev *endFlag,HWND hEndWnd)
{
	COPYDATASTRUCT cds;
	DmaToDev dma2Dev;
	FILE *fp;

	dma2Dev.DeviceX = deviceX;
	dma2Dev.dmaFlag = 1;     // hu 1--通过文件 2--通过内存
	if (memIdx >= 10)  
	{
		wcscpy(lastError, L"物理空间索引不能大于等于10！");
		return 1;
	}
	dma2Dev.memIdx = memIdx;

	if (deviceX>=DEVNUM_MAX || deviceX<0)  
	{
		wcscpy(lastError, L"设备号(deviceX)超过范围！");
		return 2;
	}

	if(dma2Dev.memIdx >= driver[deviceX].memNum)
	{
		wcscpy(lastError, L"物理空间索引超出范围！");
		return 3;
	}

	if((0 != driver[deviceX].memStatus[dma2Dev.memIdx]) || (0 == driver[deviceX].memSize[dma2Dev.memIdx])) 
	{
		wcscpy(lastError, L"索引物理空间状态出错！");
		return 4;
	}

	wcscpy(dma2Dev.binPath,binPath);
	fp = _wfopen(dma2Dev.binPath,L"rb");
	if (NULL == fp)   
	{
		wcscpy(lastError, L"dma传输文件无法打开！");
		return 5;
	}
	fclose(fp);

	dma2Dev.hUserWndforEnd = hEndWnd;
	dma2Dev.pPhyAddr = pPhyAddr;
	dma2Dev.pPhyAddr.Write32 = 0;
	dma2Dev.dataLen = dataLen;
	dma2Dev.dataLen.Write32 = 0;
	if (NULL == endFlag)  dma2Dev.endFlag.DeviceX = -99;
	else
	{
		dma2Dev.endFlag = *endFlag;
		if (0 == dma2Dev.endFlag.Write32) 
		{
			wcscpy(lastError, L"结束标识不能为0！");
			return 6;
		}
	}

	cds.dwData = 0;
	cds.lpData = &dma2Dev;
	cds.cbData = sizeof(dma2Dev);
	SendMessage(hDriverWnd,WM_COPYDATA,ID_SYS_DMAToDev,(LPARAM)&cds);
	return 0;
}

// hu 从DSP通过DMA传输到指定路径文件(推荐使用)
// hu memIdx为使用的物理空间索引(一般为0) | hEndWnd若为NULL,则向注册窗口发送中断号 | pPhyAddr.Write32由底层写入
// hu endFlag若为NULL,在传输时不读取结束标志,DMA传输的数据量为dataLen.Write32时结束DMA传输
// hu endFlag若不为NULL，则在DMA传输时只要读取到endFlag.Read32立即结束DMA传输(每次DMA传输完读取)
// hu 1--物理空间索引不能大于等于10 2--设备号(deviceX)超过范围 3--物理空间索引超出范围 
//    4--索引物理空间状态出错 5--dma传输文件无法打开 6--DMA传输的数据量(Byte)不能为0
DRIVER_DECLSPEC DWORD __stdcall DmaDSPToFile(int deviceX,UINT memIdx,wchar_t *binPath,WriteToDev pPhyAddr,WriteToDev dataLen,ReadFromDev *endFlag,HWND hEndWnd)
{
	COPYDATASTRUCT cds;
	DmaFromDev dma2Pc;
	FILE *fp;

	dma2Pc.DeviceX = deviceX;
	dma2Pc.dmaFlag = 1;     // hu 1--通过文件 2--通过内存
	if (memIdx >= 10)  
	{
		wcscpy(lastError, L"物理空间索引不能大于等于10！");
		return 1;
	}
	dma2Pc.memIdx = memIdx;

	if (deviceX>=DEVNUM_MAX || deviceX<0)  
	{
		wcscpy(lastError, L"设备号(deviceX)超过范围！");
		return 2;
	}

	if(dma2Pc.memIdx >= driver[deviceX].memNum) 
	{
		wcscpy(lastError, L"物理空间索引超出范围！");
		return 3;
	}

	if((0 != driver[deviceX].memStatus[dma2Pc.memIdx]) || (0 == driver[deviceX].memSize[dma2Pc.memIdx])) 
	{
		wcscpy(lastError, L"索引物理空间状态出错！");
		return 4;
	}

	wcscpy(dma2Pc.binPath,binPath);
	fp = _wfopen(dma2Pc.binPath,L"wb+");
	if (NULL == fp)   
	{
		wcscpy(lastError, L"dma传输文件无法打开！");
		return 5;
	}
	fclose(fp);

	dma2Pc.hUserWndforEnd = hEndWnd;
	dma2Pc.pPhyAddr = pPhyAddr;
	dma2Pc.pPhyAddr.Write32 = 0;
	dma2Pc.dataLen = dataLen;
	if (0 == dma2Pc.dataLen.Write32) 
	{
		wcscpy(lastError, L"DMA传输的数据量(Byte)不能为0！");
		return 6;
	}

	if (NULL == endFlag)  dma2Pc.endFlag.DeviceX = -99;
	else                  dma2Pc.endFlag = *endFlag;

	cds.dwData = 0;
	cds.lpData = &dma2Pc;
	cds.cbData = sizeof(dma2Pc);
	SendMessage(hDriverWnd,WM_COPYDATA,ID_SYS_DMAFromDev,(LPARAM)&cds);
	return 0;
}

// hu 获得内存虚拟地址(与DmaMemToDSP,DmaDSPToMem配套使用)
// hu 0--成功 1--物理空间索引不能大于等于10 2--设备号(deviceX)只能为0或1 3--物理空间索引超出范围
//    4--索引物理空间状态出错 5--打开共享内存失败 6--获取共享内存首地址失败 7--内存泄漏
DRIVER_DECLSPEC DWORD __stdcall GetMemAddr(int deviceX,UINT memIdx,UINT **pBufAddr)
{
	unsigned int status;

	if (memIdx >= 10)  
	{
		wcscpy(lastError, L"物理空间索引不能大于等于10！");
		return 1;
	}

	if (deviceX>=DEVNUM_MAX || deviceX<0)  
	{
		wcscpy(lastError, L"设备号(deviceX)只能为0或1！");
		return 2;
	}

	if(memIdx >= driver[deviceX].memNum) 
	{
		wcscpy(lastError, L"物理空间索引超出范围！");
		return 3;
	}
	if((0 != driver[deviceX].memStatus[memIdx]) || (0 == driver[deviceX].memSize[memIdx])) 
	{
		wcscpy(lastError, L"索引物理空间状态出错！");
		return 4;
	}

	shareMem0.memSize = driver[deviceX].memSize[memIdx];
	shareMem0.headAddr = NULL;
	shareMem0.hMap = 0;
	status = ApplyForShareMem(&shareMem0);
	if (status)
	{
		switch (status)
		{
		case 1:wcscpy(lastError, L"打开共享内存失败！");break;
		case 2:wcscpy(lastError, L"获取共享内存首地址失败！");break;
		case 3:wcscpy(lastError, L"内存泄漏！");break;
		}
		return status+4;
	}
	*pBufAddr = (UINT*)shareMem0.headAddr;
	return 0;
}

// hu 通过内存向DSP进行DMA传输(不推荐使用)
// hu deviceX为设备号(与GetMemAddr中对应) | memIdx为使用的物理空间索引(与GetMemAddr中对应)
// hu pBufAddr为虚拟首地址(与GetMemAddr中对应) | hEndWnd若为NULL,则向注册窗口发送中断号
// hu dataLen.Write32由用户写入(Byte) | pPhyAddr.Write32由底层写入 | 若传输未结束,将endFlag置为NULL
// hu 1--物理空间索引不能大于等于10 2--设备号(deviceX)超过范围 3--物理空间索引超出范围 
//    4--索引物理空间状态出错 5--内存空间大小与物理空间不匹配 6--数据长度(Byte)超出范围
DRIVER_DECLSPEC DWORD __stdcall DmaMemToDSP(int deviceX,UINT memIdx,UINT *pBufAddr,WriteToDev pPhyAddr,WriteToDev dataLen,WriteToDev *endFlag,HWND hEndWnd)
{
	COPYDATASTRUCT cds;
	DmaToDev dma2Dev;

	dma2Dev.DeviceX = deviceX;
	dma2Dev.dmaFlag = 2;   // hu 1--通过文件 2--通过内存
	if (memIdx >= 10)  
	{
		wcscpy(lastError, L"物理空间索引不能大于等于10！");
		return 1;
	}
	dma2Dev.memIdx = memIdx;

	if (dma2Dev.DeviceX>=DEVNUM_MAX || dma2Dev.DeviceX<0) 
	{
		wcscpy(lastError, L"设备号(deviceX)超过范围！");
		return 2;
	}
	if(dma2Dev.memIdx >= driver[dma2Dev.DeviceX].memNum) 
	{
		wcscpy(lastError, L"物理空间索引超出范围！");
		return 3;
	}
	if((0 != driver[dma2Dev.DeviceX].memStatus[dma2Dev.memIdx]) || (0 == driver[dma2Dev.DeviceX].memSize[dma2Dev.memIdx])) 
	{
		wcscpy(lastError, L"索引物理空间状态出错！");
		return 4;
	}
	if (driver[dma2Dev.DeviceX].memSize[dma2Dev.memIdx] != shareMem0.memSize)
	{
		wcscpy(lastError, L"内存空间大小与物理空间不匹配！");
		return 5;
	}

	dma2Dev.pBufAddr = pBufAddr;
	dma2Dev.hUserWndforEnd = hEndWnd;
	dma2Dev.pPhyAddr = pPhyAddr;
	dma2Dev.pPhyAddr.Write32 = 0;
	dma2Dev.dataLen = dataLen;
	if (0==dma2Dev.dataLen.Write32 || dma2Dev.dataLen.Write32>shareMem0.memSize)
	{
		wcscpy(lastError, L"数据长度(Byte)超出范围！");
		return 6;
	}
	if (NULL == endFlag) dma2Dev.endFlag.DeviceX = -99;
	else                 dma2Dev.endFlag = *endFlag;

	cds.dwData = 0;
	cds.lpData = &dma2Dev;
	cds.cbData = sizeof(dma2Dev);
	SendMessage(hDriverWnd,WM_COPYDATA,ID_SYS_DMAToDev,(LPARAM)&cds);
	return 0;
}

// hu 从DSP通过DMA传输到指定内存地址(不推荐使用)
// hu memIdx为使用的物理空间索引(一般为0) | hEndWnd若为NULL,则向注册窗口发送中断号 | pPhyAddr.Write32由底层写入
// hu 1--物理空间索引不能大于等于10 2--设备号(deviceX)超过范围 3--物理空间索引超出范围 
//    4--索引物理空间状态出错 5--内存空间大小与物理空间不匹配 6--数据长度(Byte)超出范围
DRIVER_DECLSPEC DWORD __stdcall DmaDSPToMem(int deviceX,UINT memIdx,UINT *pBufAddr,WriteToDev pPhyAddr,WriteToDev dataLen,HWND hEndWnd)
{
	COPYDATASTRUCT cds;
	DmaFromDev dma2Pc;

	dma2Pc.DeviceX = deviceX;
	dma2Pc.dmaFlag = 2;     // hu 1--通过文件 2--通过内存
	if (memIdx >= 10)  
	{
		wcscpy(lastError, L"物理空间索引不能大于等于10！");
		return 1;
	}
	dma2Pc.memIdx = memIdx;

	if (dma2Pc.DeviceX>=DEVNUM_MAX || dma2Pc.DeviceX<0) 
	{
		wcscpy(lastError, L"设备号(deviceX)超过范围！");
		return 2;
	}
	if(dma2Pc.memIdx >= driver[deviceX].memNum) 
	{
		wcscpy(lastError, L"物理空间索引超出范围！");
		return 3;
	}
	if((0 != driver[deviceX].memStatus[dma2Pc.memIdx]) || (0 == driver[deviceX].memSize[dma2Pc.memIdx])) 
	{
		wcscpy(lastError, L"索引物理空间状态出错！");
		return 4;
	}
	if (driver[deviceX].memSize[dma2Pc.memIdx] != shareMem0.memSize)
	{
		wcscpy(lastError, L"内存空间大小与物理空间不匹配！");
		return 5;
	}

	dma2Pc.pBufAddr = pBufAddr;
	dma2Pc.hUserWndforEnd = hEndWnd;
	dma2Pc.pPhyAddr = pPhyAddr;
	dma2Pc.pPhyAddr.Write32 = 0;
	dma2Pc.dataLen = dataLen;
	if (0==dma2Pc.dataLen.Write32 || dma2Pc.dataLen.Write32>shareMem0.memSize) 
	{
		wcscpy(lastError, L"数据长度(Byte)超出范围！");
		return 6;
	}

	cds.dwData = 0;
	cds.lpData = &dma2Pc;
	cds.cbData = sizeof(dma2Pc);
	SendMessage(hDriverWnd,WM_COPYDATA,ID_SYS_DMAFromDev,(LPARAM)&cds);
	return 0;
}

// hu 关闭所有驱动与驱动库并退出驱动程序
DRIVER_DECLSPEC void __stdcall CloseAllDSPDriver()
{
	SendMessage(hDriverWnd,WM_DESTROY,0,0);
}

// hu 重新申请共享内存(内部使用)
// hu 0--成功 1--打开共享内存失败 2--获取共享内存首地址失败 3--内存泄漏
DWORD ApplyForShareMem(ShareMem *shareMem)
{
	COPYDATASTRUCT cds;
	unsigned int status;

	cds.dwData = 0;
	cds.lpData = shareMem;
	cds.cbData = sizeof(*shareMem);

	::UnmapViewOfFile(shareMem0.headAddr);
	::CloseHandle(shareMem0.hMap);

	SendMessage(hDriverWnd,WM_COPYDATA,ID_SYS_ApplyForShareMem,(LPARAM)&cds);

	shareMem->hMap = ::OpenFileMapping(FILE_MAP_ALL_ACCESS,0,shareMem->name);
	if (NULL == shareMem->hMap) return 1;

	shareMem->headAddr = ::MapViewOfFile(shareMem->hMap,FILE_MAP_ALL_ACCESS,0,0,0);
	if (NULL == shareMem->hMap) return 2;

	memcpy(&status,(unsigned int*)shareMem->headAddr,sizeof(status));
	if (1 == status)     return 3;
	return 0;
}

// hu 初始化驱动与驱动库(非特定情况，不需要使用)
// hu 0 -- 成功 1 -- 未扫描到PCI设备 2 -- 获取设备驱动操作句柄失败 3 -- 初始化设备驱动库失败
DRIVER_DECLSPEC DWORD __stdcall InitDSPDriver(int deviceX)
{
	COPYDATASTRUCT cds;
	unsigned int status;

	cds.dwData = 0;
	cds.lpData = &deviceX;
	cds.cbData = sizeof(deviceX);

	SendMessage(hDriverWnd,WM_COPYDATA,ID_FUN_InitDSPDriver,(LPARAM)&cds);
	memcpy(&status,(unsigned int*)shareMem0.headAddr,sizeof(status));
	switch (status)
	{
	case 1:wcscpy(lastError, L"未扫描到PCI设备！");break;
	case 2:wcscpy(lastError, L"获取设备驱动操作句柄失败！");break;
	case 3:wcscpy(lastError, L"初始化设备驱动库失败！");break;
	}
	return status;
}

// hu 关闭驱动与驱动库(非特定情况，不需要使用)
// hu 0 -- 成功 1 -- 关闭PCI驱动失败 2 -- 卸载设备驱动库失败 3 -- 关闭PCI驱动与卸载设备驱动库失败
DRIVER_DECLSPEC DWORD __stdcall CloseDSPDriver(int deviceX)
{
	COPYDATASTRUCT cds;
	unsigned int status;

	cds.dwData = 0;
	cds.lpData = &deviceX;
	cds.cbData = sizeof(deviceX);

	SendMessage(hDriverWnd,WM_COPYDATA,ID_FUN_CloseDSPDriver,(LPARAM)&cds);
	memcpy(&status,(unsigned int*)shareMem0.headAddr,sizeof(status));
	switch (status)
	{
	case 1:wcscpy(lastError, L"关闭PCI驱动失败！");break;
	case 2:wcscpy(lastError, L"卸载设备驱动库失败！");break;
	case 3:wcscpy(lastError, L"关闭PCI驱动与卸载设备驱动库失败！");break;
	}
	return status;
}

// hu 申请物理地址连续的内存空间(非特定情况，不需要使用)
// hu 0--成功 1--设备号(deviceX)超过范围 2--已申请的物理空间数目已达上限 3--申请空间大小不能为0 
//    4--所申请的物理空间数量超过上限10 5--申请物理空间失败 6--CPU缓存与物理空间同步失败
DRIVER_DECLSPEC DWORD __stdcall ApplyForPA(int deviceX,DWORD dwDMABufSize)
{
	COPYDATASTRUCT cds;
	unsigned int status;
	ApplyForPa applyPA = {deviceX,dwDMABufSize};

	if (applyPA.DeviceX>=DEVNUM_MAX || applyPA.DeviceX<0)  
	{
		wcscpy(lastError, L"设备号(deviceX)超过范围！");
		return 1;  
	}
	if (10 == driver[applyPA.DeviceX].memNum)  
	{
		wcscpy(lastError, L"已申请的物理空间数目已达上限！");
		return 2;
	}
	if (0 == applyPA.dwDMABufSize) 
	{
		wcscpy(lastError, L"申请空间大小不能为0！");
		return 3;
	}

	cds.dwData = 0;
	cds.lpData = &applyPA;
	cds.cbData = sizeof(applyPA);

	SendMessage(hDriverWnd,WM_COPYDATA,ID_FUN_ApplyForPA,(LPARAM)&cds);
	memcpy(&status,(unsigned int*)shareMem0.headAddr,sizeof(status));
	if (0 != status) 
	{
		switch (status)
		{
		case 1:wcscpy(lastError, L"所申请的物理空间数量超过上限10！");break;
		case 2:wcscpy(lastError, L"申请物理空间失败！");break;
		case 3:wcscpy(lastError, L"CPU缓存与物理空间同步失败！");break;
		}
		return status+3;
	}

	GetDriverStatus(driver);
	return 0;
}

// hu 释放所申请的空间(非特定情况，不需要使用)
// hu 0 -- 成功 1--设备号(deviceX)超过范围 2--物理空间索引超出范围 3--索引物理空间状态出错
//    4--I/O缓存与物理空间同步失败 5--释放物理空间失败
DRIVER_DECLSPEC DWORD __stdcall FreePA(int deviceX,UINT memIdx)
{
	COPYDATASTRUCT cds;
	unsigned int status;
	FreePa freePA = {deviceX,memIdx};

	if (freePA.DeviceX>=DEVNUM_MAX || freePA.DeviceX<0)  
	{
		wcscpy(lastError, L"设备号(deviceX)超过范围！");
		return 1;
	}
	if (freePA.memIdx >= driver[freePA.DeviceX].memNum) 
	{
		wcscpy(lastError, L"物理空间索引超出范围！");
		return 2;
	}
	if((0 != driver[freePA.DeviceX].memStatus[freePA.memIdx]) || (0 == driver[freePA.DeviceX].memSize[freePA.memIdx])) 
	{
		wcscpy(lastError, L"索引物理空间状态出错！");
		return 3;
	}

	cds.dwData = 0;
	cds.lpData = &freePA;
	cds.cbData = sizeof(freePA);

	SendMessage(hDriverWnd,WM_COPYDATA,ID_FUN_FreePA,(LPARAM)&cds);
	memcpy(&status,(unsigned int*)shareMem0.headAddr,sizeof(status));
	if (0 != status) 
	{
		switch (status)
		{
		case 1:wcscpy(lastError, L"I/O缓存与物理空间同步失败！");break;
		case 2:wcscpy(lastError, L"释放物理空间失败！");break;
		}
		return status+3;
	}

	GetDriverStatus(driver); 
	return 0;
}