// This is the main DLL file.

#include "stdafx.h"

#include "DSPDriver.h"

namespace PCIDriverHelper4CS {

	PciDriverFun::DSPDriver4CS::DSPDriver4CS()
	{
		memStatus = gcnew array<int>(10);
		memSize = gcnew array<unsigned int>(10);
	}

	void PciDriverFun::DSPDriver4CS::CopyToCS(const DSPDriver dspDriver)
	{
		status = dspDriver.status;
		memNum = dspDriver.memNum;
		for (int ii=0;ii<10;ii++)
		{
			memStatus[ii] = dspDriver.memStatus[ii];
			memSize[ii] = dspDriver.memSize[ii];
		}
	}

	PciDriverFun::WriteToDev4CS::WriteToDev4CS(int deviceX,int barX,unsigned int offset,unsigned int write32,unsigned int repeatTimes)
	{
		DeviceX = deviceX;
		BarX = barX;
		dwOffset = offset;
		Write32 = write32;
		RepeatTimes = repeatTimes;
	}

	PciDriverFun::WriteToDev4CS::WriteToDev4CS(int deviceX,int barX,unsigned int offset,unsigned int write32)
	{
		DeviceX = deviceX;
		BarX = barX;
		dwOffset = offset;
		Write32 = write32;
		RepeatTimes = 1;
	}

	void PciDriverFun::WriteToDev4CS::CopyFromCS(WriteToDev *writeToDev)
	{
		writeToDev->DeviceX = DeviceX;
		writeToDev->BarX = BarX;
		writeToDev->dwOffset = dwOffset;
		writeToDev->Write32 = Write32;
		writeToDev->RepeatTimes = RepeatTimes;
	}

	PciDriverFun::ReadFromDev4CS::ReadFromDev4CS(int deviceX,int barX,unsigned int offset,unsigned int read32)
	{
		DeviceX = deviceX;
		BarX = barX;
		dwOffset = offset;
		Read32 = read32;
	}

	void PciDriverFun::ReadFromDev4CS::CopyFromCS(ReadFromDev *readFromDev)
	{
		readFromDev->DeviceX = DeviceX;
		readFromDev->BarX = BarX;
		readFromDev->dwOffset = dwOffset;
		readFromDev->Read32 = Read32;
	}

	// hu 获得最新的错误信息
	String^ PciDriverFun::GetLastDriverError4CS()
	{
		return String(GetLastDriverError()).ToString();
	}

	// hu 将驱动状态转化为字符串
	String^ PciDriverFun::DriverStatus2Str4CS(int driverStatus)
	{
		return String(DriverStatus2Str(driverStatus)).ToString();
	}

	// hu 将驱动所申请的物理空间状态转化为字符串
	String^ PciDriverFun::DriverMemStatus2Str4CS(int driverMemStatus,unsigned int driverMemSize)
	{
		return String(DriverMemStatus2Str(driverMemStatus,driverMemSize)).ToString();
	}

	// hu 将动态加载FPGA的返回值转化为字符串
	String^ PciDriverFun::IniFpgaFlag2Str4CS(unsigned int iniFpgaFlag)
	{
		return String(IniFpgaFlag2Str(iniFpgaFlag)).ToString();
	}

	// hu 初始化驱动程序
	// hu 0--成功 1--驱动程序未运行 2--获取应用程序路径失败 3--获取驱动程序路径失败
	//    4--驱动程序与该程序未运行在同一路径 5--打开共享内存失败 6--获取共享内存首地址失败
	unsigned int PciDriverFun::InitAllDSPDriver4CS(array<DSPDriver4CS^>^ dspDriver4CS,bool isStrongCorrelation)
	{
		unsigned int status;
		DSPDriver dspDriver[devNum_Max];
		unsigned int devNum = devNum_Max;

		if (devNum > dspDriver4CS->Length) devNum = dspDriver4CS->Length;

		status = InitAllDSPDriver(dspDriver,devNum,isStrongCorrelation);

		for (int ii=0;ii<devNum;ii++)
		{
			dspDriver4CS[ii]->CopyToCS(dspDriver[ii]);
		}
		return status;
	}
	unsigned int PciDriverFun::InitAllDSPDriver4CS(array<DSPDriver4CS^>^ dspDriver4CS)
	{
		unsigned int status;
		DSPDriver dspDriver[devNum_Max];
		unsigned int devNum = devNum_Max;

		if (devNum > dspDriver4CS->Length) devNum = dspDriver4CS->Length;

		status = InitAllDSPDriver(dspDriver,devNum);

		for (int ii=0;ii<devNum;ii++)
		{
			dspDriver4CS[ii]->CopyToCS(dspDriver[ii]);
		}
		return status;
	}

	// hu 获得指定驱动状态
	void PciDriverFun::GetDSPDevStatus4CS(array<DSPDriver4CS^>^ dspDriver4CS,int index)
	{
		DSPDriver dspDriver[devNum_Max];
		unsigned int devIdx = devNum_Max-1;

		if (devIdx > dspDriver4CS->Length-1) devIdx = dspDriver4CS->Length-1;
		if (devIdx > index) devIdx = index;

		GetDSPDevStatus(dspDriver,devIdx);
		dspDriver4CS[devIdx]->CopyToCS(dspDriver[devIdx]);
	}

	// hu 刷新驱动状态
	// hu 返回初始化驱动数目
	int PciDriverFun::RefreshDSPDevStatus4CS(array<DSPDriver4CS^>^ dspDriver4CS)
	{
		int status;
		DSPDriver dspDriver[devNum_Max];
		unsigned int devNum = devNum_Max;

		if (devNum > dspDriver4CS->Length) devNum = dspDriver4CS->Length;

		status = RefreshDSPDevStatus(dspDriver,devNum);

		for (int ii=0;ii<devNum;ii++)
		{
			dspDriver4CS[ii]->CopyToCS(dspDriver[ii]);
		}
		return status;
	}

	// hu 注册消息传递句柄
	void PciDriverFun::RegistHWNDToDev4CS(IntPtr hUserWnd)
	{
		RegistHWNDToDev((HWND)hUserWnd.ToPointer());
	}

	// hu 设置开机自启动
	void PciDriverFun::SetDSPAutoRun4CS(bool Enable)
	{
		SetDSPAutoRun(Enable);
	}

	// hu 向指定DSP地址写32bit数据
	// hu 0 -- 成功 1 -- PCI设备写入失败
	unsigned int PciDriverFun::WriteToDSP4CS(int deviceX,int barX, unsigned int dwoffset, unsigned int write32,unsigned int repeatTimes)
	{
		return WriteToDSP(deviceX,barX,dwoffset,write32,repeatTimes);
	}
	unsigned int PciDriverFun::WriteToDSP4CS(int deviceX,int barX, unsigned int dwoffset, unsigned int write32)
	{
		return WriteToDSP(deviceX,barX,dwoffset,write32,1);
	}

	// hu 向指定DSP地址读32bit数据
	// hu 0 -- 成功 1 -- PCI设备读取失败
	unsigned int PciDriverFun::ReadFromDSP4CS(int deviceX,int barX, unsigned int dwoffset, unsigned int %read324CS)
	{
		unsigned int read32,status;
		status = ReadFromDSP(deviceX,barX,dwoffset,&read32);
		read324CS = read32;
		return status;
	}

	// hu 通过文件向DSP进行DMA传输(推荐使用)
	// hu memIdx为使用的物理空间索引(一般为0)        | hEndWnd若为NULL,则向注册窗口发送中断号
	// hu pPhyAddr.Write32,dataLen.Write32由底层写入 | endFlag若为NULL,在传输结束时不写入结束标志
	// hu 1--物理空间索引不能大于等于10 2--设备号(deviceX)超过范围 3--物理空间索引超出范围 
	//    4--索引物理空间状态出错 5--dma传输文件无法打开 6--结束标识不能为0
	unsigned int PciDriverFun::DmaFileToDSP4CS(int deviceX,unsigned int memIdx,String^ binPath4CS,WriteToDev4CS^ pPhyAddr4CS,WriteToDev4CS^ dataLen4CS,WriteToDev4CS^ endFlag4CS,IntPtr hEndWnd)
	{
		WriteToDev pPhyAddr,dataLen,endFlag;
		pin_ptr<const WCHAR> binPath;

		pPhyAddr4CS->CopyFromCS(&pPhyAddr);
		dataLen4CS->CopyFromCS(&dataLen);
		endFlag4CS->CopyFromCS(&endFlag);
		binPath = PtrToStringChars(binPath4CS);

		return DmaFileToDSP(deviceX,memIdx,(wchar_t*)binPath,pPhyAddr,dataLen,&endFlag,(HWND)hEndWnd.ToPointer());
	}
	unsigned int PciDriverFun::DmaFileToDSP4CS(int deviceX,unsigned int memIdx,String^ binPath4CS,WriteToDev4CS^ pPhyAddr4CS,WriteToDev4CS^ dataLen4CS,WriteToDev4CS^ endFlag4CS)
	{
		WriteToDev pPhyAddr,dataLen,endFlag;
		pin_ptr<const WCHAR> binPath;

		pPhyAddr4CS->CopyFromCS(&pPhyAddr);
		dataLen4CS->CopyFromCS(&dataLen);
		endFlag4CS->CopyFromCS(&endFlag);
		binPath = PtrToStringChars(binPath4CS);

		return DmaFileToDSP(deviceX,memIdx,(wchar_t*)binPath,pPhyAddr,dataLen,&endFlag);
	}
	unsigned int PciDriverFun::DmaFileToDSP4CS(int deviceX,unsigned int memIdx,String^ binPath4CS,WriteToDev4CS^ pPhyAddr4CS,WriteToDev4CS^ dataLen4CS)
	{
		WriteToDev pPhyAddr,dataLen;
		pin_ptr<const WCHAR> binPath;

		pPhyAddr4CS->CopyFromCS(&pPhyAddr);
		dataLen4CS->CopyFromCS(&dataLen);
		binPath = PtrToStringChars(binPath4CS);

		return DmaFileToDSP(deviceX,memIdx,(wchar_t*)binPath,pPhyAddr,dataLen);
	}

	// hu 从DSP通过DMA传输到指定路径文件(推荐使用)
	// hu memIdx为使用的物理空间索引(一般为0) | hEndWnd若为NULL,则向注册窗口发送中断号 | pPhyAddr.Write32由底层写入
	// hu endFlag若为NULL,在传输时不读取结束标志,DMA传输的数据量为dataLen.Write32时结束DMA传输
	// hu endFlag若不为NULL，则在DMA传输时只要读取到endFlag.Read32立即结束DMA传输(每次DMA传输完读取)
	// hu 1--物理空间索引不能大于等于10 2--设备号(deviceX)超过范围 3--物理空间索引超出范围 
	//    4--索引物理空间状态出错 5--dma传输文件无法打开 6--DMA传输的数据量(Byte)不能为0
	unsigned int PciDriverFun::DmaDSPToFile4CS(int deviceX,unsigned int memIdx,String^ binPath4CS,WriteToDev4CS^ pPhyAddr4CS,WriteToDev4CS^ dataLen4CS,ReadFromDev4CS^ endFlag4CS,IntPtr hEndWnd)
	{
		WriteToDev pPhyAddr,dataLen;
		ReadFromDev endFlag;
		pin_ptr<const WCHAR> binPath;

		pPhyAddr4CS->CopyFromCS(&pPhyAddr);
		dataLen4CS->CopyFromCS(&dataLen);
		endFlag4CS->CopyFromCS(&endFlag);
		binPath = PtrToStringChars(binPath4CS);

		return DmaDSPToFile(deviceX,memIdx,(wchar_t*)binPath,pPhyAddr,dataLen,&endFlag,(HWND)hEndWnd.ToPointer());
	}
	unsigned int PciDriverFun::DmaDSPToFile4CS(int deviceX,unsigned int memIdx,String^ binPath4CS,WriteToDev4CS^ pPhyAddr4CS,WriteToDev4CS^ dataLen4CS,ReadFromDev4CS^ endFlag4CS)
	{
		WriteToDev pPhyAddr,dataLen;
		ReadFromDev endFlag;
		pin_ptr<const WCHAR> binPath;

		pPhyAddr4CS->CopyFromCS(&pPhyAddr);
		dataLen4CS->CopyFromCS(&dataLen);
		endFlag4CS->CopyFromCS(&endFlag);
		binPath = PtrToStringChars(binPath4CS);

		return DmaDSPToFile(deviceX,memIdx,(wchar_t*)binPath,pPhyAddr,dataLen,&endFlag);
	}
	unsigned int PciDriverFun::DmaDSPToFile4CS(int deviceX,unsigned int memIdx,String^ binPath4CS,WriteToDev4CS^ pPhyAddr4CS,WriteToDev4CS^ dataLen4CS)
	{
		WriteToDev pPhyAddr,dataLen;
		pin_ptr<const WCHAR> binPath;

		pPhyAddr4CS->CopyFromCS(&pPhyAddr);
		dataLen4CS->CopyFromCS(&dataLen);
		binPath = PtrToStringChars(binPath4CS);

		return DmaDSPToFile(deviceX,memIdx,(wchar_t*)binPath,pPhyAddr,dataLen);
	}

	// hu 获得内存虚拟地址(与DmaMemToDSP,DmaDSPToMem配套使用)
	// hu 0--成功 1--物理空间索引不能大于等于10 2--设备号(deviceX)只能为0或1 3--物理空间索引超出范围
	//    4--索引物理空间状态出错 5--打开共享内存失败 6--获取共享内存首地址失败 7--内存泄漏
	unsigned int PciDriverFun::GetMemAddr4CS(int deviceX,unsigned int memIdx,IntPtr %pBufAddr4CS)
	{
		unsigned int status;
		unsigned int *pBufAddr;

		status = GetMemAddr(deviceX,memIdx,&pBufAddr);
		pBufAddr4CS = (IntPtr)pBufAddr;
		return status;
	}

	// hu 通过内存向DSP进行DMA传输(不推荐使用)
	// hu deviceX为设备号(与GetMemAddr中对应) | memIdx为使用的物理空间索引(与GetMemAddr中对应)
	// hu pBufAddr为虚拟首地址(与GetMemAddr中对应) | hEndWnd若为NULL,则向注册窗口发送中断号
	// hu dataLen.Write32由用户写入(Byte) | pPhyAddr.Write32由底层写入 | 若传输未结束,将endFlag置为NULL
	// hu 1--物理空间索引不能大于等于10 2--设备号(deviceX)超过范围 3--物理空间索引超出范围 
	//    4--索引物理空间状态出错 5--内存空间大小与物理空间不匹配 6--数据长度(Byte)超出范围
	unsigned int PciDriverFun::DmaMemToDSP4CS(int deviceX,unsigned int memIdx,IntPtr pBufAddr,WriteToDev4CS^ pPhyAddr4CS,WriteToDev4CS^ dataLen4CS,WriteToDev4CS^ endFlag4CS,IntPtr hEndWnd)
	{
		WriteToDev pPhyAddr,dataLen,endFlag;

		pPhyAddr4CS->CopyFromCS(&pPhyAddr);
		dataLen4CS->CopyFromCS(&dataLen);
		endFlag4CS->CopyFromCS(&endFlag);

		return DmaMemToDSP(deviceX,memIdx,(UINT *)pBufAddr.ToPointer(),pPhyAddr,dataLen,&endFlag,(HWND)hEndWnd.ToPointer());
	}
	unsigned int PciDriverFun::DmaMemToDSP4CS(int deviceX,unsigned int memIdx,IntPtr pBufAddr,WriteToDev4CS^ pPhyAddr4CS,WriteToDev4CS^ dataLen4CS,WriteToDev4CS^ endFlag4CS)
	{
		WriteToDev pPhyAddr,dataLen,endFlag;

		pPhyAddr4CS->CopyFromCS(&pPhyAddr);
		dataLen4CS->CopyFromCS(&dataLen);
		endFlag4CS->CopyFromCS(&endFlag);

		return DmaMemToDSP(deviceX,memIdx,(UINT *)pBufAddr.ToPointer(),pPhyAddr,dataLen,&endFlag);
	}
	unsigned int PciDriverFun::DmaMemToDSP4CS(int deviceX,unsigned int memIdx,IntPtr pBufAddr,WriteToDev4CS^ pPhyAddr4CS,WriteToDev4CS^ dataLen4CS)
	{
		WriteToDev pPhyAddr,dataLen;

		pPhyAddr4CS->CopyFromCS(&pPhyAddr);
		dataLen4CS->CopyFromCS(&dataLen);

		return DmaMemToDSP(deviceX,memIdx,(UINT *)pBufAddr.ToPointer(),pPhyAddr,dataLen);
	}

	// hu 从DSP通过DMA传输到指定内存地址(不推荐使用)
	// hu memIdx为使用的物理空间索引(一般为0) | hEndWnd若为NULL,则向注册窗口发送中断号 | pPhyAddr.Write32由底层写入
	// hu 1--物理空间索引不能大于等于10 2--设备号(deviceX)超过范围 3--物理空间索引超出范围 
	//    4--索引物理空间状态出错 5--内存空间大小与物理空间不匹配 6--数据长度(Byte)超出范围
	unsigned int PciDriverFun::DmaDSPToMem4CS(int deviceX,unsigned int memIdx,IntPtr pBufAddr,WriteToDev4CS^ pPhyAddr4CS,WriteToDev4CS^ dataLen4CS,IntPtr hEndWnd)
	{
		WriteToDev pPhyAddr,dataLen;

		pPhyAddr4CS->CopyFromCS(&pPhyAddr);
		dataLen4CS->CopyFromCS(&dataLen);

		return DmaDSPToMem(deviceX,memIdx,(UINT *)pBufAddr.ToPointer(),pPhyAddr,dataLen,(HWND)hEndWnd.ToPointer());
	}
	unsigned int PciDriverFun::DmaDSPToMem4CS(int deviceX,unsigned int memIdx,IntPtr pBufAddr,WriteToDev4CS^ pPhyAddr4CS,WriteToDev4CS^ dataLen4CS)
	{
		WriteToDev pPhyAddr,dataLen;

		pPhyAddr4CS->CopyFromCS(&pPhyAddr);
		dataLen4CS->CopyFromCS(&dataLen);

		return DmaDSPToMem(deviceX,memIdx,(UINT *)pBufAddr.ToPointer(),pPhyAddr,dataLen);
	}

	// hu 关闭所有驱动与驱动库并退出驱动程序
	void PciDriverFun::CloseAllDSPDriver4CS()
	{
		CloseAllDSPDriver();
	}

	// hu 初始化驱动与驱动库(非特定情况，不需要使用)
	// hu 0 -- 成功 1 -- 未扫描到PCI设备 2 -- 获取设备驱动操作句柄失败 3 -- 初始化设备驱动库失败
	unsigned int PciDriverFun::InitDSPDriver4CS(int deviceX)
	{
		return InitDSPDriver(deviceX);
	}

	// hu 关闭驱动与驱动库(非特定情况，不需要使用)
	// hu 0 -- 成功 1 -- 关闭PCI驱动失败 2 -- 卸载设备驱动库失败 3 -- 关闭PCI驱动与卸载设备驱动库失败
	unsigned int PciDriverFun::CloseDSPDriver4CS(int deviceX)
	{
		return CloseDSPDriver(deviceX);
	}

	// hu 申请物理地址连续的内存空间(非特定情况，不需要使用)
	// hu 0--成功 1--设备号(deviceX)超过范围 2--已申请的物理空间数目已达上限 3--申请空间大小不能为0 
	//    4--所申请的物理空间数量超过上限10 5--申请物理空间失败 6--CPU缓存与物理空间同步失败
	unsigned int PciDriverFun::ApplyForPA4CS(int deviceX,unsigned int dwDMABufSize)
	{
		return ApplyForPA(deviceX,dwDMABufSize);
	}

	// hu 释放所申请的空间(非特定情况，不需要使用)
	// hu 0 -- 成功 1--设备号(deviceX)超过范围 2--物理空间索引超出范围 3--索引物理空间状态出错
	//    4--I/O缓存与物理空间同步失败 5--释放物理空间失败
	unsigned int PciDriverFun::FreePA4CS(int deviceX,unsigned int memIdx)
	{
		return FreePA(deviceX,memIdx);
	}
}