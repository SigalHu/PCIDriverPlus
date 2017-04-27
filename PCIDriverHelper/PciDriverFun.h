/*
 * ����������غ���
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

// hu ������µĴ�����Ϣ
DRIVER_DECLSPEC wchar_t * __stdcall GetLastDriverError();

// hu ������״̬ת��Ϊ�ַ���
DRIVER_DECLSPEC wchar_t * __stdcall DriverStatus2Str(int driverStatus);

// hu �����������������ռ�״̬ת��Ϊ�ַ���
DRIVER_DECLSPEC wchar_t * __stdcall DriverMemStatus2Str(int driverMemStatus,UINT driverMemSize);

// hu ����̬����FPGA�ķ���ֵת��Ϊ�ַ���
DRIVER_DECLSPEC wchar_t * __stdcall IniFpgaFlag2Str(UINT iniFpgaFlag);

// hu ��ʼ����������
// hu 0--�ɹ� 1--��������δ���� 2--��ȡӦ�ó���·��ʧ�� 3--��ȡ��������·��ʧ��
//    4--����������ó���δ������ͬһ·�� 5--�򿪹����ڴ�ʧ�� 6--��ȡ�����ڴ��׵�ַʧ��
DRIVER_DECLSPEC DWORD __stdcall InitAllDSPDriver(DSPDriver *dspDriver,int driverNum=DEVNUM_MAX,bool isStrongCorrelation=true);

// hu ���ָ������״̬
DRIVER_DECLSPEC void  __stdcall GetDSPDevStatus(DSPDriver *dspDriver,int index=0);

// hu ˢ������״̬
// hu ���س�ʼ��������Ŀ
DRIVER_DECLSPEC int   __stdcall RefreshDSPDevStatus(DSPDriver *dspDriver,int driverNum=DEVNUM_MAX);

// hu ע����Ϣ���ݾ��
DRIVER_DECLSPEC void  __stdcall RegistHWNDToDev(HWND hUserWnd);

// hu ���ÿ���������
DRIVER_DECLSPEC void  __stdcall SetDSPAutoRun(BOOL Enable);

// hu ��ָ��DSP��ַд32bit����
// hu 0 -- �ɹ� 1 -- PCI�豸д��ʧ��
DRIVER_DECLSPEC DWORD __stdcall WriteToDSP(int deviceX,int barX, unsigned long dwoffset, unsigned int write32,unsigned int repeatTimes=1);

// hu ��ָ��DSP��ַ��32bit����
// hu 0 -- �ɹ� 1 -- PCI�豸��ȡʧ��
DRIVER_DECLSPEC DWORD __stdcall ReadFromDSP(int deviceX,int barX, unsigned long dwoffset, unsigned int *read32);

// hu ͨ���ļ���DSP����DMA����(�Ƽ�ʹ��)
// hu memIdxΪʹ�õ�����ռ�����(һ��Ϊ0)        | hEndWnd��ΪNULL,����ע�ᴰ�ڷ����жϺ�
// hu pPhyAddr.Write32,dataLen.Write32�ɵײ�д�� | endFlag��ΪNULL,�ڴ������ʱ��д�������־
// hu 1--����ռ��������ܴ��ڵ���10 2--�豸��(deviceX)������Χ 3--����ռ�����������Χ 
//    4--��������ռ�״̬���� 5--dma�����ļ��޷��� 6--������ʶ����Ϊ0
DRIVER_DECLSPEC DWORD __stdcall DmaFileToDSP(int deviceX,UINT memIdx,wchar_t *binPath,WriteToDev pPhyAddr,WriteToDev dataLen,WriteToDev *endFlag=NULL,HWND hEndWnd=NULL);

// hu ��DSPͨ��DMA���䵽ָ��·���ļ�(�Ƽ�ʹ��)
// hu memIdxΪʹ�õ�����ռ�����(һ��Ϊ0) | hEndWnd��ΪNULL,����ע�ᴰ�ڷ����жϺ� | pPhyAddr.Write32�ɵײ�д��
// hu endFlag��ΪNULL,�ڴ���ʱ����ȡ������־,DMA�����������ΪdataLen.Write32ʱ����DMA����
// hu endFlag����ΪNULL������DMA����ʱֻҪ��ȡ��endFlag.Read32��������DMA����(ÿ��DMA�������ȡ)
// hu 1--����ռ��������ܴ��ڵ���10 2--�豸��(deviceX)������Χ 3--����ռ�����������Χ 
//    4--��������ռ�״̬���� 5--dma�����ļ��޷��� 6--DMA�����������(Byte)����Ϊ0
DRIVER_DECLSPEC DWORD __stdcall DmaDSPToFile(int deviceX,UINT memIdx,wchar_t *binPath,WriteToDev pPhyAddr,WriteToDev dataLen,ReadFromDev *endFlag=NULL,HWND hEndWnd=NULL);

// hu ����ڴ������ַ(��DmaMemToDSP,DmaDSPToMem����ʹ��)
// hu 0--�ɹ� 1--����ռ��������ܴ��ڵ���10 2--�豸��(deviceX)ֻ��Ϊ0��1 3--����ռ�����������Χ
//    4--��������ռ�״̬���� 5--�򿪹����ڴ�ʧ�� 6--��ȡ�����ڴ��׵�ַʧ�� 7--�ڴ�й©
DRIVER_DECLSPEC DWORD __stdcall GetMemAddr(int deviceX,UINT memIdx,UINT **pBufAddr);

// hu ͨ���ڴ���DSP����DMA����(���Ƽ�ʹ��)
// hu deviceXΪ�豸��(��GetMemAddr�ж�Ӧ) | memIdxΪʹ�õ�����ռ�����(��GetMemAddr�ж�Ӧ)
// hu pBufAddrΪ�����׵�ַ(��GetMemAddr�ж�Ӧ) | hEndWnd��ΪNULL,����ע�ᴰ�ڷ����жϺ�
// hu dataLen.Write32���û�д��(Byte) | pPhyAddr.Write32�ɵײ�д�� | ������δ����,��endFlag��ΪNULL
// hu 1--����ռ��������ܴ��ڵ���10 2--�豸��(deviceX)������Χ 3--����ռ�����������Χ 
//    4--��������ռ�״̬���� 5--�ڴ�ռ��С������ռ䲻ƥ�� 6--���ݳ���(Byte)������Χ
DRIVER_DECLSPEC DWORD __stdcall DmaMemToDSP(int deviceX,UINT memIdx,UINT *pBufAddr,WriteToDev pPhyAddr,WriteToDev dataLen,WriteToDev *endFlag=NULL,HWND hEndWnd=NULL);

// hu ��DSPͨ��DMA���䵽ָ���ڴ��ַ(���Ƽ�ʹ��)
// hu memIdxΪʹ�õ�����ռ�����(һ��Ϊ0) | hEndWnd��ΪNULL,����ע�ᴰ�ڷ����жϺ� | pPhyAddr.Write32�ɵײ�д��
// hu 1--����ռ��������ܴ��ڵ���10 2--�豸��(deviceX)������Χ 3--����ռ�����������Χ 
//    4--��������ռ�״̬���� 5--�ڴ�ռ��С������ռ䲻ƥ�� 6--���ݳ���(Byte)������Χ
DRIVER_DECLSPEC DWORD __stdcall DmaDSPToMem(int deviceX,UINT memIdx,UINT *pBufAddr,WriteToDev pPhyAddr,WriteToDev dataLen,HWND hEndWnd=NULL);

// hu �ر����������������Ⲣ�˳���������
DRIVER_DECLSPEC void  __stdcall CloseAllDSPDriver();

// hu ��ʼ��������������(���ض����������Ҫʹ��)
// hu 0 -- �ɹ� 1 -- δɨ�赽PCI�豸 2 -- ��ȡ�豸�����������ʧ�� 3 -- ��ʼ���豸������ʧ��
DRIVER_DECLSPEC DWORD __stdcall InitDSPDriver(int deviceX);

// hu �ر�������������(���ض����������Ҫʹ��)
// hu 0 -- �ɹ� 1 -- �ر�PCI����ʧ�� 2 -- ж���豸������ʧ�� 3 -- �ر�PCI������ж���豸������ʧ��
DRIVER_DECLSPEC DWORD __stdcall CloseDSPDriver(int deviceX);

// hu ���������ַ�������ڴ�ռ�(���ض����������Ҫʹ��)
// hu 0--�ɹ� 1--�豸��(deviceX)������Χ 2--�����������ռ���Ŀ�Ѵ����� 3--����ռ��С����Ϊ0 
//    4--�����������ռ�������������10 5--��������ռ�ʧ�� 6--CPU����������ռ�ͬ��ʧ��
DRIVER_DECLSPEC DWORD __stdcall ApplyForPA(int deviceX,DWORD dwDMABufSize);

// hu �ͷ�������Ŀռ�(���ض����������Ҫʹ��)
// hu 0 -- �ɹ� 1--�豸��(deviceX)������Χ 2--����ռ�����������Χ 3--��������ռ�״̬����
//    4--I/O����������ռ�ͬ��ʧ�� 5--�ͷ�����ռ�ʧ��
DRIVER_DECLSPEC DWORD __stdcall FreePA(int deviceX,UINT memIdx);

#ifdef CSharp_HU
}
#endif