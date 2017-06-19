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

	// hu ������µĴ�����Ϣ
	String^ PciDriverFun::GetLastDriverError4CS()
	{
		return String(GetLastDriverError()).ToString();
	}

	// hu ������״̬ת��Ϊ�ַ���
	String^ PciDriverFun::DriverStatus2Str4CS(int driverStatus)
	{
		return String(DriverStatus2Str(driverStatus)).ToString();
	}

	// hu �����������������ռ�״̬ת��Ϊ�ַ���
	String^ PciDriverFun::DriverMemStatus2Str4CS(int driverMemStatus,unsigned int driverMemSize)
	{
		return String(DriverMemStatus2Str(driverMemStatus,driverMemSize)).ToString();
	}

	// hu ����̬����FPGA�ķ���ֵת��Ϊ�ַ���
	String^ PciDriverFun::IniFpgaFlag2Str4CS(unsigned int iniFpgaFlag)
	{
		return String(IniFpgaFlag2Str(iniFpgaFlag)).ToString();
	}

	// hu ��ʼ����������
	// hu 0--�ɹ� 1--��������δ���� 2--��ȡӦ�ó���·��ʧ�� 3--��ȡ��������·��ʧ��
	//    4--����������ó���δ������ͬһ·�� 5--�򿪹����ڴ�ʧ�� 6--��ȡ�����ڴ��׵�ַʧ��
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

	// hu ���ָ������״̬
	void PciDriverFun::GetDSPDevStatus4CS(array<DSPDriver4CS^>^ dspDriver4CS,int index)
	{
		DSPDriver dspDriver[devNum_Max];
		unsigned int devIdx = devNum_Max-1;

		if (devIdx > dspDriver4CS->Length-1) devIdx = dspDriver4CS->Length-1;
		if (devIdx > index) devIdx = index;

		GetDSPDevStatus(dspDriver,devIdx);
		dspDriver4CS[devIdx]->CopyToCS(dspDriver[devIdx]);
	}

	// hu ˢ������״̬
	// hu ���س�ʼ��������Ŀ
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

	// hu ע����Ϣ���ݾ��
	void PciDriverFun::RegistHWNDToDev4CS(IntPtr hUserWnd)
	{
		RegistHWNDToDev((HWND)hUserWnd.ToPointer());
	}

	// hu ���ÿ���������
	void PciDriverFun::SetDSPAutoRun4CS(bool Enable)
	{
		SetDSPAutoRun(Enable);
	}

	// hu ��ָ��DSP��ַд32bit����
	// hu 0 -- �ɹ� 1 -- PCI�豸д��ʧ��
	unsigned int PciDriverFun::WriteToDSP4CS(int deviceX,int barX, unsigned int dwoffset, unsigned int write32,unsigned int repeatTimes)
	{
		return WriteToDSP(deviceX,barX,dwoffset,write32,repeatTimes);
	}
	unsigned int PciDriverFun::WriteToDSP4CS(int deviceX,int barX, unsigned int dwoffset, unsigned int write32)
	{
		return WriteToDSP(deviceX,barX,dwoffset,write32,1);
	}

	// hu ��ָ��DSP��ַ��32bit����
	// hu 0 -- �ɹ� 1 -- PCI�豸��ȡʧ��
	unsigned int PciDriverFun::ReadFromDSP4CS(int deviceX,int barX, unsigned int dwoffset, unsigned int %read324CS)
	{
		unsigned int read32,status;
		status = ReadFromDSP(deviceX,barX,dwoffset,&read32);
		read324CS = read32;
		return status;
	}

	// hu ͨ���ļ���DSP����DMA����(�Ƽ�ʹ��)
	// hu memIdxΪʹ�õ�����ռ�����(һ��Ϊ0)        | hEndWnd��ΪNULL,����ע�ᴰ�ڷ����жϺ�
	// hu pPhyAddr.Write32,dataLen.Write32�ɵײ�д�� | endFlag��ΪNULL,�ڴ������ʱ��д�������־
	// hu 1--����ռ��������ܴ��ڵ���10 2--�豸��(deviceX)������Χ 3--����ռ�����������Χ 
	//    4--��������ռ�״̬���� 5--dma�����ļ��޷��� 6--������ʶ����Ϊ0
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

	// hu ��DSPͨ��DMA���䵽ָ��·���ļ�(�Ƽ�ʹ��)
	// hu memIdxΪʹ�õ�����ռ�����(һ��Ϊ0) | hEndWnd��ΪNULL,����ע�ᴰ�ڷ����жϺ� | pPhyAddr.Write32�ɵײ�д��
	// hu endFlag��ΪNULL,�ڴ���ʱ����ȡ������־,DMA�����������ΪdataLen.Write32ʱ����DMA����
	// hu endFlag����ΪNULL������DMA����ʱֻҪ��ȡ��endFlag.Read32��������DMA����(ÿ��DMA�������ȡ)
	// hu 1--����ռ��������ܴ��ڵ���10 2--�豸��(deviceX)������Χ 3--����ռ�����������Χ 
	//    4--��������ռ�״̬���� 5--dma�����ļ��޷��� 6--DMA�����������(Byte)����Ϊ0
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

	// hu ����ڴ������ַ(��DmaMemToDSP,DmaDSPToMem����ʹ��)
	// hu 0--�ɹ� 1--����ռ��������ܴ��ڵ���10 2--�豸��(deviceX)ֻ��Ϊ0��1 3--����ռ�����������Χ
	//    4--��������ռ�״̬���� 5--�򿪹����ڴ�ʧ�� 6--��ȡ�����ڴ��׵�ַʧ�� 7--�ڴ�й©
	unsigned int PciDriverFun::GetMemAddr4CS(int deviceX,unsigned int memIdx,IntPtr %pBufAddr4CS)
	{
		unsigned int status;
		unsigned int *pBufAddr;

		status = GetMemAddr(deviceX,memIdx,&pBufAddr);
		pBufAddr4CS = (IntPtr)pBufAddr;
		return status;
	}

	// hu ͨ���ڴ���DSP����DMA����(���Ƽ�ʹ��)
	// hu deviceXΪ�豸��(��GetMemAddr�ж�Ӧ) | memIdxΪʹ�õ�����ռ�����(��GetMemAddr�ж�Ӧ)
	// hu pBufAddrΪ�����׵�ַ(��GetMemAddr�ж�Ӧ) | hEndWnd��ΪNULL,����ע�ᴰ�ڷ����жϺ�
	// hu dataLen.Write32���û�д��(Byte) | pPhyAddr.Write32�ɵײ�д�� | ������δ����,��endFlag��ΪNULL
	// hu 1--����ռ��������ܴ��ڵ���10 2--�豸��(deviceX)������Χ 3--����ռ�����������Χ 
	//    4--��������ռ�״̬���� 5--�ڴ�ռ��С������ռ䲻ƥ�� 6--���ݳ���(Byte)������Χ
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

	// hu ��DSPͨ��DMA���䵽ָ���ڴ��ַ(���Ƽ�ʹ��)
	// hu memIdxΪʹ�õ�����ռ�����(һ��Ϊ0) | hEndWnd��ΪNULL,����ע�ᴰ�ڷ����жϺ� | pPhyAddr.Write32�ɵײ�д��
	// hu 1--����ռ��������ܴ��ڵ���10 2--�豸��(deviceX)������Χ 3--����ռ�����������Χ 
	//    4--��������ռ�״̬���� 5--�ڴ�ռ��С������ռ䲻ƥ�� 6--���ݳ���(Byte)������Χ
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

	// hu �ر����������������Ⲣ�˳���������
	void PciDriverFun::CloseAllDSPDriver4CS()
	{
		CloseAllDSPDriver();
	}

	// hu ��ʼ��������������(���ض����������Ҫʹ��)
	// hu 0 -- �ɹ� 1 -- δɨ�赽PCI�豸 2 -- ��ȡ�豸�����������ʧ�� 3 -- ��ʼ���豸������ʧ��
	unsigned int PciDriverFun::InitDSPDriver4CS(int deviceX)
	{
		return InitDSPDriver(deviceX);
	}

	// hu �ر�������������(���ض����������Ҫʹ��)
	// hu 0 -- �ɹ� 1 -- �ر�PCI����ʧ�� 2 -- ж���豸������ʧ�� 3 -- �ر�PCI������ж���豸������ʧ��
	unsigned int PciDriverFun::CloseDSPDriver4CS(int deviceX)
	{
		return CloseDSPDriver(deviceX);
	}

	// hu ���������ַ�������ڴ�ռ�(���ض����������Ҫʹ��)
	// hu 0--�ɹ� 1--�豸��(deviceX)������Χ 2--�����������ռ���Ŀ�Ѵ����� 3--����ռ��С����Ϊ0 
	//    4--�����������ռ�������������10 5--��������ռ�ʧ�� 6--CPU����������ռ�ͬ��ʧ��
	unsigned int PciDriverFun::ApplyForPA4CS(int deviceX,unsigned int dwDMABufSize)
	{
		return ApplyForPA(deviceX,dwDMABufSize);
	}

	// hu �ͷ�������Ŀռ�(���ض����������Ҫʹ��)
	// hu 0 -- �ɹ� 1--�豸��(deviceX)������Χ 2--����ռ�����������Χ 3--��������ռ�״̬����
	//    4--I/O����������ռ�ͬ��ʧ�� 5--�ͷ�����ռ�ʧ��
	unsigned int PciDriverFun::FreePA4CS(int deviceX,unsigned int memIdx)
	{
		return FreePA(deviceX,memIdx);
	}
}