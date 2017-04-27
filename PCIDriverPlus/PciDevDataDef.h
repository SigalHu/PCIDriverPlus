/*
 * �����������ȫ�ֱ���(���ļ��е�class��ΪPOD,����memcpy�Ⱥ���) 
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
	// hu 0 -- �ɹ� 1 -- δɨ�赽PCI�豸 2 -- ��ȡ�豸�����������ʧ�� 3 -- ��ʼ���豸������ʧ��
	int status;                                       // hu ����״̬
	UINT memNum;                                      // hu ��Ҫ���������ռ����
	// hu 0 -- �ɹ� 1 -- �����������ռ�������������10 2 -- ��������ռ�ʧ�� 3 -- CPU����������ռ�ͬ��ʧ��
	int memStatus[10];                                // hu ����ռ�״̬
	UINT memSize[10];                                 // hu ��Ҫ���������ռ��С
	void * EDMAWinDriver_Device[10];                  // hu ָ��ײ�����������ڴ�ռ��׵�ַ(����ϵͳ���õ������ַ)
	unsigned long EDMAPhysicalAddress_Device[10];     // hu �����ַ
	unsigned long EDMAHandle_Device[10];              // hu ����ռ���
};

class DSPDriver
{
public:
	// hu 0 -- �ɹ� 1 -- δɨ�赽PCI�豸 2 -- ��ȡ�豸�����������ʧ�� 3 -- ��ʼ���豸������ʧ��
	int status;                                       // hu ����״̬
	UINT memNum;                                      // hu �����������ռ����
	// hu 0 -- �ɹ� 1 -- �����������ռ�������������10 2 -- ��������ռ�ʧ�� 3 -- CPU����������ռ�ͬ��ʧ��
	int memStatus[10];                                // hu ����ռ�״̬
	UINT memSize[10];                                 // hu ��Ҫ���������ռ��С
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
	int dmaFlag;                   // hu 0--dmaδ���� 1--ͨ���ļ� 2--ͨ���ڴ�
	HWND hUserWndforEnd;           // hu dma������Ϣ���(��ΪNULL�����͵�ע����Ϣ)
	UINT memIdx;                   // hu ��ʹ�õ�����ռ�����(��0��ʼ)
	wchar_t binPath[MAX_PATH];     // hu ׼��DMA������ļ�·��
	UINT *pBufAddr;                // hu �ڴ��׵�ַ
	WriteToDev pPhyAddr;           // hu �����׵�ַ(�����ַ�ɵײ㸳ֵ)
	WriteToDev dataLen;            // hu ���ݳ���(���ݳ��� dmaFlag=0,�ɵײ㸳ֵ;dmaFlag=1,���û���ֵ)
	WriteToDev endFlag;            // hu dma���������־λ
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
	int dmaFlag;                   // hu 0--dmaδ���� 1--ͨ���ļ� 2--ͨ���ڴ�
	HWND hUserWndforEnd;           // hu dma������Ϣ���(��ΪNULL�����͵�ע����Ϣ)
	FILE *dmaFp;                   // hu �ļ�ָ��
	UINT *pShareAddr;              // hu �ڴ��ַ
	UINT *pBufAddr;                // hu ��Ӧ�������ַ�������ַ
	UINT memSize;                  // hu ����ռ�(����ռ�)��С
	WriteToDev pPhyAddr;           // hu �����׵�ַ(�����ַ�ɵײ㸳ֵ)
	WriteToDev dataLen;            // hu ���ݳ���(���ݳ��� dmaFlag=0,�ɵײ㸳ֵ;dmaFlag=1,���û���ֵ)
	WriteToDev endFlag;            // hu dma���������־λ
};

class DmaFromDev
{
public:
	int DeviceX;
	int dmaFlag;                   // hu 0--dmaδ���� 1--ͨ���ļ� 2--ͨ���ڴ�
	HWND hUserWndforEnd;           // hu dma������Ϣ���(��ΪNULL�����͵�ע����Ϣ)
	UINT memIdx;                   // hu ��ʹ�õ�����ռ�����(��0��ʼ)
	wchar_t binPath[MAX_PATH];     // hu ׼��DMA������ļ�·��
	UINT *pBufAddr;                // hu �ڴ��׵�ַ
	WriteToDev pPhyAddr;           // hu �����׵�ַ(�����ַ�ɵײ㸳ֵ)
	WriteToDev dataLen;            // hu ���ݳ���(���ݳ��� dmaFlag=0,�ɵײ㸳ֵ;dmaFlag=1,���û���ֵ)
	ReadFromDev endFlag;           // hu dma���������־λ
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
	int dmaFlag;                   // hu 0--dmaδ���� 1--ͨ���ļ� 2--ͨ���ڴ�
	HWND hUserWndforEnd;           // hu dma������Ϣ���(��ΪNULL�����͵�ע����Ϣ)
	FILE *dmaFp;                   // hu �ļ�ָ��
	UINT *pShareAddr;              // hu �ڴ��ַ
	UINT *pBufAddr;                // hu ��Ӧ�������ַ�������ַ
	UINT memSize;                  // hu ����ռ�(����ռ�)��С
	UINT dmaLen;                   // hu DMA��������ݳ���
	WriteToDev pPhyAddr;           // hu �����׵�ַ(�����ַ�ɵײ㸳ֵ)
	WriteToDev dataLen;            // hu ���ݳ���(���ݳ��� dmaFlag=0,�ɵײ㸳ֵ;dmaFlag=1,���û���ֵ)
	ReadFromDev endFlag;           // hu dma���������־λ
};

class ApplyForPa
{
public:
	int DeviceX;
	DWORD dwDMABufSize;            // hu ��Ҫ���������ռ��С
};

class FreePa
{
public:
	int DeviceX;
	UINT memIdx;                   // hu ��Ҫ�ͷŵ�����ռ�����
};

class TrayShow
{
public:
	int status;           // hu ����״̬  0--����ʾ 1--��ʾ
	HMENU hmenu;          // hu �˵����
	NOTIFYICONDATA nid;
	UINT WM_TASKBARCREATED;
};