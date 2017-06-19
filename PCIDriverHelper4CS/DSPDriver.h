// DSPDriver.h

#pragma once
#include "PciDriverFun.h"
#include "Vcclr.h"

using namespace System;
using namespace System::Runtime::InteropServices;

namespace PCIDriverHelper4CS {

	public ref class PciDriverFun
	{
	public:
		const static unsigned int wm_CallbackMessage_0 = WM_CALLBACKMESSAGE_0;
		const static unsigned int devNum_Max = DEVNUM_MAX;

		ref class DSPDriver4CS
		{
		public:
			// hu 0 -- �ɹ� 1 -- δɨ�赽PCI�豸 2 -- ��ȡ�豸�����������ʧ�� 3 -- ��ʼ���豸������ʧ��
			int status;                                       // hu ����״̬
			unsigned int memNum;                                      // hu �����������ռ����
			// hu 0 -- �ɹ� 1 -- �����������ռ�������������10 2 -- ��������ռ�ʧ�� 3 -- CPU����������ռ�ͬ��ʧ��
			array<int>^ memStatus;                                // hu ����ռ�״̬
			array<unsigned int>^ memSize;                                 // hu ��Ҫ���������ռ��С

			DSPDriver4CS();
			void CopyToCS(const DSPDriver dspDriver);
		};

		ref class WriteToDev4CS
		{
		public:
			int DeviceX;
			int BarX;
			unsigned int dwOffset;
			unsigned int Write32;
			unsigned int RepeatTimes;

			WriteToDev4CS(int deviceX,int barX,unsigned int offset,unsigned int write32,unsigned int repeatTimes);
			WriteToDev4CS(int deviceX,int barX,unsigned int offset,unsigned int write32);
			void CopyFromCS(WriteToDev *writeToDev);
		};

		ref class ReadFromDev4CS
		{
		public:
			int DeviceX;
			int BarX;
			unsigned int dwOffset;
			unsigned int Read32;

			ReadFromDev4CS(int deviceX,int barX,unsigned int offset,unsigned int read32);
			void CopyFromCS(ReadFromDev *readFromDev);
		};

		// hu ������µĴ�����Ϣ
		static String^ GetLastDriverError4CS();

		// hu ������״̬ת��Ϊ�ַ���
		static String^ DriverStatus2Str4CS(int driverStatus);

		// hu �����������������ռ�״̬ת��Ϊ�ַ���
		static String^ DriverMemStatus2Str4CS(int driverMemStatus,unsigned int driverMemSize);

		// hu ����̬����FPGA�ķ���ֵת��Ϊ�ַ���
		static String^ IniFpgaFlag2Str4CS(unsigned int iniFpgaFlag);

		// hu ��ʼ����������
		// hu 0--�ɹ� 1--��������δ���� 2--��ȡӦ�ó���·��ʧ�� 3--��ȡ��������·��ʧ��
		//    4--����������ó���δ������ͬһ·�� 5--�򿪹����ڴ�ʧ�� 6--��ȡ�����ڴ��׵�ַʧ��
		static unsigned int InitAllDSPDriver4CS(array<DSPDriver4CS^>^ dspDriver4CS,bool isStrongCorrelation);
		static unsigned int InitAllDSPDriver4CS(array<DSPDriver4CS^>^ dspDriver4CS);

		// hu ���ָ������״̬
		static void GetDSPDevStatus4CS(array<DSPDriver4CS^>^ dspDriver4CS,int index);

		// hu ˢ������״̬
		// hu ���س�ʼ��������Ŀ
		static int RefreshDSPDevStatus4CS(array<DSPDriver4CS^>^ dspDriver4CS);

		// hu ע����Ϣ���ݾ��
		static void RegistHWNDToDev4CS(IntPtr hUserWnd);

		// hu ���ÿ���������
		static void SetDSPAutoRun4CS(bool Enable);

		// hu ��ָ��DSP��ַд32bit����
		// hu 0 -- �ɹ� 1 -- PCI�豸д��ʧ��
		static unsigned int WriteToDSP4CS(int deviceX,int barX, unsigned int dwoffset, unsigned int write32,unsigned int repeatTimes);
		static unsigned int WriteToDSP4CS(int deviceX,int barX, unsigned int dwoffset, unsigned int write32);

		// hu ��ָ��DSP��ַ��32bit����
		// hu 0 -- �ɹ� 1 -- PCI�豸��ȡʧ��
		static unsigned int ReadFromDSP4CS(int deviceX,int barX, unsigned int dwoffset, unsigned int %read324CS);

		// hu ͨ���ļ���DSP����DMA����(�Ƽ�ʹ��)
		// hu memIdxΪʹ�õ�����ռ�����(һ��Ϊ0)        | hEndWnd��ΪNULL,����ע�ᴰ�ڷ����жϺ�
		// hu pPhyAddr.Write32,dataLen.Write32�ɵײ�д�� | endFlag��ΪNULL,�ڴ������ʱ��д�������־
		// hu 1--����ռ��������ܴ��ڵ���10 2--�豸��(deviceX)������Χ 3--����ռ�����������Χ 
		//    4--��������ռ�״̬���� 5--dma�����ļ��޷��� 6--������ʶ����Ϊ0
		static unsigned int DmaFileToDSP4CS(int deviceX,unsigned int memIdx,String^ binPath4CS,WriteToDev4CS^ pPhyAddr4CS,WriteToDev4CS^ dataLen4CS,WriteToDev4CS^ endFlag4CS,IntPtr hEndWnd);
		static unsigned int DmaFileToDSP4CS(int deviceX,unsigned int memIdx,String^ binPath4CS,WriteToDev4CS^ pPhyAddr4CS,WriteToDev4CS^ dataLen4CS,WriteToDev4CS^ endFlag4CS);
		static unsigned int DmaFileToDSP4CS(int deviceX,unsigned int memIdx,String^ binPath4CS,WriteToDev4CS^ pPhyAddr4CS,WriteToDev4CS^ dataLen4CS);

		// hu ��DSPͨ��DMA���䵽ָ��·���ļ�(�Ƽ�ʹ��)
		// hu memIdxΪʹ�õ�����ռ�����(һ��Ϊ0) | hEndWnd��ΪNULL,����ע�ᴰ�ڷ����жϺ� | pPhyAddr.Write32�ɵײ�д��
		// hu endFlag��ΪNULL,�ڴ���ʱ����ȡ������־,DMA�����������ΪdataLen.Write32ʱ����DMA����
		// hu endFlag����ΪNULL������DMA����ʱֻҪ��ȡ��endFlag.Read32��������DMA����(ÿ��DMA�������ȡ)
		// hu 1--����ռ��������ܴ��ڵ���10 2--�豸��(deviceX)������Χ 3--����ռ�����������Χ 
		//    4--��������ռ�״̬���� 5--dma�����ļ��޷��� 6--DMA�����������(Byte)����Ϊ0
		static unsigned int DmaDSPToFile4CS(int deviceX,unsigned int memIdx,String^ binPath4CS,WriteToDev4CS^ pPhyAddr4CS,WriteToDev4CS^ dataLen4CS,ReadFromDev4CS^ endFlag4CS,IntPtr hEndWnd);
		static unsigned int DmaDSPToFile4CS(int deviceX,unsigned int memIdx,String^ binPath4CS,WriteToDev4CS^ pPhyAddr4CS,WriteToDev4CS^ dataLen4CS,ReadFromDev4CS^ endFlag4CS);
		static unsigned int DmaDSPToFile4CS(int deviceX,unsigned int memIdx,String^ binPath4CS,WriteToDev4CS^ pPhyAddr4CS,WriteToDev4CS^ dataLen4CS);

		// hu ����ڴ������ַ(��DmaMemToDSP,DmaDSPToMem����ʹ��)
		// hu 0--�ɹ� 1--����ռ��������ܴ��ڵ���10 2--�豸��(deviceX)ֻ��Ϊ0��1 3--����ռ�����������Χ
		//    4--��������ռ�״̬���� 5--�򿪹����ڴ�ʧ�� 6--��ȡ�����ڴ��׵�ַʧ�� 7--�ڴ�й©
		static unsigned int GetMemAddr4CS(int deviceX,unsigned int memIdx,IntPtr %pBufAddr4CS);

		// hu ͨ���ڴ���DSP����DMA����(���Ƽ�ʹ��)
		// hu deviceXΪ�豸��(��GetMemAddr�ж�Ӧ) | memIdxΪʹ�õ�����ռ�����(��GetMemAddr�ж�Ӧ)
		// hu pBufAddrΪ�����׵�ַ(��GetMemAddr�ж�Ӧ) | hEndWnd��ΪNULL,����ע�ᴰ�ڷ����жϺ�
		// hu dataLen.Write32���û�д��(Byte) | pPhyAddr.Write32�ɵײ�д�� | ������δ����,��endFlag��ΪNULL
		// hu 1--����ռ��������ܴ��ڵ���10 2--�豸��(deviceX)������Χ 3--����ռ�����������Χ 
		//    4--��������ռ�״̬���� 5--�ڴ�ռ��С������ռ䲻ƥ�� 6--���ݳ���(Byte)������Χ
		static unsigned int DmaMemToDSP4CS(int deviceX,unsigned int memIdx,IntPtr pBufAddr,WriteToDev4CS^ pPhyAddr4CS,WriteToDev4CS^ dataLen4CS,WriteToDev4CS^ endFlag4CS,IntPtr hEndWnd);
		static unsigned int DmaMemToDSP4CS(int deviceX,unsigned int memIdx,IntPtr pBufAddr,WriteToDev4CS^ pPhyAddr4CS,WriteToDev4CS^ dataLen4CS,WriteToDev4CS^ endFlag4CS);
		static unsigned int DmaMemToDSP4CS(int deviceX,unsigned int memIdx,IntPtr pBufAddr,WriteToDev4CS^ pPhyAddr4CS,WriteToDev4CS^ dataLen4CS);

		// hu ��DSPͨ��DMA���䵽ָ���ڴ��ַ(���Ƽ�ʹ��)
		// hu memIdxΪʹ�õ�����ռ�����(һ��Ϊ0) | hEndWnd��ΪNULL,����ע�ᴰ�ڷ����жϺ� | pPhyAddr.Write32�ɵײ�д��
		// hu 1--����ռ��������ܴ��ڵ���10 2--�豸��(deviceX)������Χ 3--����ռ�����������Χ 
		//    4--��������ռ�״̬���� 5--�ڴ�ռ��С������ռ䲻ƥ�� 6--���ݳ���(Byte)������Χ
		static unsigned int DmaDSPToMem4CS(int deviceX,unsigned int memIdx,IntPtr pBufAddr,WriteToDev4CS^ pPhyAddr4CS,WriteToDev4CS^ dataLen4CS,IntPtr hEndWnd);
		static unsigned int DmaDSPToMem4CS(int deviceX,unsigned int memIdx,IntPtr pBufAddr,WriteToDev4CS^ pPhyAddr4CS,WriteToDev4CS^ dataLen4CS);

		// hu �ر����������������Ⲣ�˳���������
		static void CloseAllDSPDriver4CS();

		// hu ��ʼ��������������(���ض����������Ҫʹ��)
		// hu 0 -- �ɹ� 1 -- δɨ�赽PCI�豸 2 -- ��ȡ�豸�����������ʧ�� 3 -- ��ʼ���豸������ʧ��
		static unsigned int InitDSPDriver4CS(int deviceX);

		// hu �ر�������������(���ض����������Ҫʹ��)
		// hu 0 -- �ɹ� 1 -- �ر�PCI����ʧ�� 2 -- ж���豸������ʧ�� 3 -- �ر�PCI������ж���豸������ʧ��
		static unsigned int CloseDSPDriver4CS(int deviceX);

		// hu ���������ַ�������ڴ�ռ�(���ض����������Ҫʹ��)
		// hu 0--�ɹ� 1--�豸��(deviceX)������Χ 2--�����������ռ���Ŀ�Ѵ����� 3--����ռ��С����Ϊ0 
		//    4--�����������ռ�������������10 5--��������ռ�ʧ�� 6--CPU����������ռ�ͬ��ʧ��
		static unsigned int ApplyForPA4CS(int deviceX,unsigned int dwDMABufSize);

		// hu �ͷ�������Ŀռ�(���ض����������Ҫʹ��)
		// hu 0 -- �ɹ� 1--�豸��(deviceX)������Χ 2--����ռ�����������Χ 3--��������ռ�״̬����
		//    4--I/O����������ռ�ͬ��ʧ�� 5--�ͷ�����ռ�ʧ��
		static unsigned int FreePA4CS(int deviceX,unsigned int memIdx);
	};
}
