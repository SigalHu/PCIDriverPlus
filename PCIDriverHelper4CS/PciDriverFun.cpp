/*
 * ������غ���
 *
 * @author ������
 * @date 2016/04/19
 */

#include "stdafx.h"
#include <tlhelp32.h> 
#include <Psapi.h>
#include "PciDriverFunID.h"
#include "PciDriverDataDef.h"
#include "PciDriverFun.h"

// hu ȫ�ֱ��� /////////////////////////////////////////////
static HWND hDriverWnd;
static ShareMem shareMem0 = {NULL,L"ShareMemForPciDriver",NULL,1024};
static DSPDriver driver[DEVNUM_MAX];
static wchar_t *driverClassName = L"pcidriver";
wchar_t lastError[100] = L"�޴���";
wchar_t driverStatusStr[100];
////////////////////////////////////////////////////////////

// hu �ڲ����� /////////////////////////////////////////////
// hu �������״̬(�ڲ�ʹ��)
// hu ���س�ʼ��������Ŀ
int GetDriverStatus(DSPDriver *dspDriver,int driverNum=DEVNUM_MAX);

// hu �������빲���ڴ�(�ڲ�ʹ��)
// hu 0--�ɹ� 1--�򿪹����ڴ�ʧ�� 2--��ȡ�����ڴ��׵�ַʧ�� 3--�ڴ�й©
DWORD ApplyForShareMem(ShareMem *shareMem);
////////////////////////////////////////////////////////////

// hu ������µĴ�����Ϣ
DRIVER_DECLSPEC wchar_t * __stdcall GetLastDriverError()
{
	return lastError;
}

// hu ������״̬ת��Ϊ�ַ���
DRIVER_DECLSPEC wchar_t * __stdcall DriverStatus2Str(int driverStatus)
{
	switch(driverStatus)
	{
	case 0: wcscpy(driverStatusStr,L"��ʼ��PCI�豸�ɹ���");break;
	case 1: wcscpy(driverStatusStr,L"δɨ�赽PCI�豸��");break;
	case 2: wcscpy(driverStatusStr,L"��ȡ�豸�����������ʧ�ܣ�");break;
	case 3: wcscpy(driverStatusStr,L"��ʼ���豸������ʧ�ܣ�");break;
	default: wcscpy(driverStatusStr,L"����������ʼ��������Ĭ�Ϸ���ֵ��");break;
	}
	return driverStatusStr;
}

// hu �����������������ռ�״̬ת��Ϊ�ַ���
DRIVER_DECLSPEC wchar_t * __stdcall DriverMemStatus2Str(int driverMemStatus,UINT driverMemSize)
{
	switch(driverMemStatus)
	{
	case 0: swprintf_s(driverStatusStr,L"����%dByte��������ռ�ɹ���",driverMemSize);break;
	case 1: wcscpy(driverStatusStr,L"�����������ռ�������������10��");break;
	case 2: wcscpy(driverStatusStr,L"��������ռ�ʧ�ܣ�");break;
	case 3: wcscpy(driverStatusStr,L"CPU����������ռ�ͬ��ʧ�ܣ�");break;
	default: wcscpy(driverStatusStr,L"������������ռ亯����Ĭ�Ϸ���ֵ��");break;
	}
	return driverStatusStr;
}

// hu ����̬����FPGA�ķ���ֵת��Ϊ�ַ���
DRIVER_DECLSPEC wchar_t * __stdcall IniFpgaFlag2Str(UINT iniFpgaFlag)
{
	switch(iniFpgaFlag)
	{
	case 0: wcscpy(driverStatusStr,L"��̬���سɹ���");break;
	case 1: wcscpy(driverStatusStr,L"FPGA��ʼ��ʧ�ܣ�");break;
	case 2: wcscpy(driverStatusStr,L"δ�յ�FPGA��INTB�źţ���̬����ʧ�ܣ�");break;
	case 3: wcscpy(driverStatusStr,L"δ�յ�FPGA��DONE�źţ���̬����ʧ�ܣ�");break;
	default: wcscpy(driverStatusStr,L"δ�յ�Լ������ֵ����̬����ʧ�ܣ�");break;
	}
	return driverStatusStr;
}

// hu ��ʼ����������
// hu 0--�ɹ� 1--��������δ���� 2--��ȡӦ�ó���·��ʧ�� 3--��ȡ��������·��ʧ��
//    4--����������ó���δ������ͬһ·�� 5--�򿪹����ڴ�ʧ�� 6--��ȡ�����ڴ��׵�ַʧ��
DRIVER_DECLSPEC DWORD __stdcall InitAllDSPDriver(DSPDriver *dspDriver,int driverNum,bool isStrongCorrelation)
{
	wchar_t temp[20];
	int ii;

	// hu ����������򴰿ھ��
	hDriverWnd = ::FindWindow(driverClassName,NULL);
	if (NULL == hDriverWnd) 
	{
		wcscpy(lastError, L"��������δ���У�");
		return 1;
	}

	if (isStrongCorrelation)
	{
		wchar_t progPath[MAX_PATH];    // hu ������Ŀ¼
		wchar_t driverPath[MAX_PATH];  // hu ����·��

		// hu ����������·��
		GetModuleFileNameW(NULL,progPath,MAX_PATH);
		wchar_t *progNametmp = wcsrchr(progPath,L'\\');
		if (NULL == progNametmp) 
		{
			wcscpy(lastError, L"��ȡӦ�ó���·��ʧ�ܣ�");
			return 2;
		}
		progNametmp++;
		*progNametmp = L'\0';

		// hu �����������·��
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
			wcscpy(lastError, L"��ȡ��������·��ʧ�ܣ�");
			return 3;
		}
		driverNametmp++;
		*driverNametmp = L'\0';

		// hu �ж�����������ó����Ƿ�������ͬһ·��
		for (ii = 0;ii<wcslen(driverPath);ii++)
		{
			if (driverPath[ii]>='A'&&driverPath[ii]<='Z')
			{
				driverPath[ii] += 32;  // hu ���̷�ת��ΪСд
			}
			if (progPath[ii]>='A'&&progPath[ii]<='Z')
			{
				progPath[ii] += 32;  // hu ���̷�ת��ΪСд
			}
		}
		if (wcscmp(progPath,driverPath))
		{
			wcscpy(lastError, L"����������ó���δ������ͬһ·����");
			return 4;
		}
	}

	// �򿪹����ڴ�
	shareMem0.hMap = ::OpenFileMapping(FILE_MAP_ALL_ACCESS,0,shareMem0.name);
	if (NULL == shareMem0.hMap) 
	{
		wcscpy(lastError, L"�򿪹����ڴ�ʧ�ܣ�");
		return 5;
	}

	shareMem0.headAddr = ::MapViewOfFile(shareMem0.hMap,FILE_MAP_ALL_ACCESS,0,0,0);
	if (NULL == shareMem0.headAddr)
	{
		wcscpy(lastError, L"��ȡ�����ڴ��׵�ַʧ�ܣ�");
		return 6;
	}

	GetDriverStatus(driver,DEVNUM_MAX);
	for (ii=0;ii<driverNum;ii++)
	{
		GetDSPDevStatus(dspDriver,ii);
	}
	return 0;
}

// hu �������״̬(�ڲ�ʹ��)
// hu ���س�ʼ��������Ŀ
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

// hu ���ָ������״̬
DRIVER_DECLSPEC void __stdcall GetDSPDevStatus(DSPDriver *dspDriver,int index)
{
	dspDriver[index] = driver[index];
}

// hu ˢ������״̬
// hu ���س�ʼ��������Ŀ
DRIVER_DECLSPEC int __stdcall RefreshDSPDevStatus(DSPDriver *dspDriver,int driverNum)
{
	return GetDriverStatus(dspDriver,driverNum);
}

// hu ע����Ϣ���ݾ��
DRIVER_DECLSPEC void __stdcall RegistHWNDToDev(HWND hUserWnd)
{
	COPYDATASTRUCT cds;
	cds.dwData = 0;
	cds.lpData = &hUserWnd;
	cds.cbData = sizeof(hUserWnd);
	SendMessage(hDriverWnd,WM_COPYDATA,ID_SYS_RegistHWNDToDev,(LPARAM)&cds);
}

// hu ���ÿ���������
DRIVER_DECLSPEC void __stdcall SetDSPAutoRun(BOOL Enable)
{
	COPYDATASTRUCT cds;
	cds.dwData = 0;
	cds.lpData = &Enable;
	cds.cbData = sizeof(Enable);
	SendMessage(hDriverWnd,WM_COPYDATA,ID_SYS_SetAutoRun,(LPARAM)&cds);
}

// hu ��ָ��DSP��ַд32bit����
// hu 0 -- �ɹ� 1 -- PCI�豸д��ʧ��
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
		wcscpy(lastError, L"PCI�豸д��ʧ�ܣ�");
	}
	return status;
}

// hu ��ָ��DSP��ַ��32bit����
// hu 0 -- �ɹ� 1 -- PCI�豸��ȡʧ��
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
		wcscpy(lastError, L"PCI�豸��ȡʧ�ܣ�");
	}
	return status;
}

// hu ͨ���ļ���DSP����DMA����(�Ƽ�ʹ��)
// hu memIdxΪʹ�õ�����ռ�����(һ��Ϊ0)        | hEndWnd��ΪNULL,����ע�ᴰ�ڷ����жϺ�
// hu pPhyAddr.Write32,dataLen.Write32�ɵײ�д�� | endFlag��ΪNULL,�ڴ������ʱ��д�������־
// hu 1--����ռ��������ܴ��ڵ���10 2--�豸��(deviceX)������Χ 3--����ռ�����������Χ 
//    4--��������ռ�״̬���� 5--dma�����ļ��޷��� 6--������ʶ����Ϊ0
DRIVER_DECLSPEC DWORD __stdcall DmaFileToDSP(int deviceX,UINT memIdx,wchar_t *binPath,WriteToDev pPhyAddr,WriteToDev dataLen,WriteToDev *endFlag,HWND hEndWnd)
{
	COPYDATASTRUCT cds;
	DmaToDev dma2Dev;
	FILE *fp;

	dma2Dev.DeviceX = deviceX;
	dma2Dev.dmaFlag = 1;     // hu 1--ͨ���ļ� 2--ͨ���ڴ�
	if (memIdx >= 10)  
	{
		wcscpy(lastError, L"����ռ��������ܴ��ڵ���10��");
		return 1;
	}
	dma2Dev.memIdx = memIdx;

	if (deviceX>=DEVNUM_MAX || deviceX<0)  
	{
		wcscpy(lastError, L"�豸��(deviceX)������Χ��");
		return 2;
	}

	if(dma2Dev.memIdx >= driver[deviceX].memNum)
	{
		wcscpy(lastError, L"����ռ�����������Χ��");
		return 3;
	}

	if((0 != driver[deviceX].memStatus[dma2Dev.memIdx]) || (0 == driver[deviceX].memSize[dma2Dev.memIdx])) 
	{
		wcscpy(lastError, L"��������ռ�״̬����");
		return 4;
	}

	wcscpy(dma2Dev.binPath,binPath);
	fp = _wfopen(dma2Dev.binPath,L"rb");
	if (NULL == fp)   
	{
		wcscpy(lastError, L"dma�����ļ��޷��򿪣�");
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
			wcscpy(lastError, L"������ʶ����Ϊ0��");
			return 6;
		}
	}

	cds.dwData = 0;
	cds.lpData = &dma2Dev;
	cds.cbData = sizeof(dma2Dev);
	SendMessage(hDriverWnd,WM_COPYDATA,ID_SYS_DMAToDev,(LPARAM)&cds);
	return 0;
}

// hu ��DSPͨ��DMA���䵽ָ��·���ļ�(�Ƽ�ʹ��)
// hu memIdxΪʹ�õ�����ռ�����(һ��Ϊ0) | hEndWnd��ΪNULL,����ע�ᴰ�ڷ����жϺ� | pPhyAddr.Write32�ɵײ�д��
// hu endFlag��ΪNULL,�ڴ���ʱ����ȡ������־,DMA�����������ΪdataLen.Write32ʱ����DMA����
// hu endFlag����ΪNULL������DMA����ʱֻҪ��ȡ��endFlag.Read32��������DMA����(ÿ��DMA�������ȡ)
// hu 1--����ռ��������ܴ��ڵ���10 2--�豸��(deviceX)������Χ 3--����ռ�����������Χ 
//    4--��������ռ�״̬���� 5--dma�����ļ��޷��� 6--DMA�����������(Byte)����Ϊ0
DRIVER_DECLSPEC DWORD __stdcall DmaDSPToFile(int deviceX,UINT memIdx,wchar_t *binPath,WriteToDev pPhyAddr,WriteToDev dataLen,ReadFromDev *endFlag,HWND hEndWnd)
{
	COPYDATASTRUCT cds;
	DmaFromDev dma2Pc;
	FILE *fp;

	dma2Pc.DeviceX = deviceX;
	dma2Pc.dmaFlag = 1;     // hu 1--ͨ���ļ� 2--ͨ���ڴ�
	if (memIdx >= 10)  
	{
		wcscpy(lastError, L"����ռ��������ܴ��ڵ���10��");
		return 1;
	}
	dma2Pc.memIdx = memIdx;

	if (deviceX>=DEVNUM_MAX || deviceX<0)  
	{
		wcscpy(lastError, L"�豸��(deviceX)������Χ��");
		return 2;
	}

	if(dma2Pc.memIdx >= driver[deviceX].memNum) 
	{
		wcscpy(lastError, L"����ռ�����������Χ��");
		return 3;
	}

	if((0 != driver[deviceX].memStatus[dma2Pc.memIdx]) || (0 == driver[deviceX].memSize[dma2Pc.memIdx])) 
	{
		wcscpy(lastError, L"��������ռ�״̬����");
		return 4;
	}

	wcscpy(dma2Pc.binPath,binPath);
	fp = _wfopen(dma2Pc.binPath,L"wb+");
	if (NULL == fp)   
	{
		wcscpy(lastError, L"dma�����ļ��޷��򿪣�");
		return 5;
	}
	fclose(fp);

	dma2Pc.hUserWndforEnd = hEndWnd;
	dma2Pc.pPhyAddr = pPhyAddr;
	dma2Pc.pPhyAddr.Write32 = 0;
	dma2Pc.dataLen = dataLen;
	if (0 == dma2Pc.dataLen.Write32) 
	{
		wcscpy(lastError, L"DMA�����������(Byte)����Ϊ0��");
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

// hu ����ڴ������ַ(��DmaMemToDSP,DmaDSPToMem����ʹ��)
// hu 0--�ɹ� 1--����ռ��������ܴ��ڵ���10 2--�豸��(deviceX)ֻ��Ϊ0��1 3--����ռ�����������Χ
//    4--��������ռ�״̬���� 5--�򿪹����ڴ�ʧ�� 6--��ȡ�����ڴ��׵�ַʧ�� 7--�ڴ�й©
DRIVER_DECLSPEC DWORD __stdcall GetMemAddr(int deviceX,UINT memIdx,UINT **pBufAddr)
{
	unsigned int status;

	if (memIdx >= 10)  
	{
		wcscpy(lastError, L"����ռ��������ܴ��ڵ���10��");
		return 1;
	}

	if (deviceX>=DEVNUM_MAX || deviceX<0)  
	{
		wcscpy(lastError, L"�豸��(deviceX)ֻ��Ϊ0��1��");
		return 2;
	}

	if(memIdx >= driver[deviceX].memNum) 
	{
		wcscpy(lastError, L"����ռ�����������Χ��");
		return 3;
	}
	if((0 != driver[deviceX].memStatus[memIdx]) || (0 == driver[deviceX].memSize[memIdx])) 
	{
		wcscpy(lastError, L"��������ռ�״̬����");
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
		case 1:wcscpy(lastError, L"�򿪹����ڴ�ʧ�ܣ�");break;
		case 2:wcscpy(lastError, L"��ȡ�����ڴ��׵�ַʧ�ܣ�");break;
		case 3:wcscpy(lastError, L"�ڴ�й©��");break;
		}
		return status+4;
	}
	*pBufAddr = (UINT*)shareMem0.headAddr;
	return 0;
}

// hu ͨ���ڴ���DSP����DMA����(���Ƽ�ʹ��)
// hu deviceXΪ�豸��(��GetMemAddr�ж�Ӧ) | memIdxΪʹ�õ�����ռ�����(��GetMemAddr�ж�Ӧ)
// hu pBufAddrΪ�����׵�ַ(��GetMemAddr�ж�Ӧ) | hEndWnd��ΪNULL,����ע�ᴰ�ڷ����жϺ�
// hu dataLen.Write32���û�д��(Byte) | pPhyAddr.Write32�ɵײ�д�� | ������δ����,��endFlag��ΪNULL
// hu 1--����ռ��������ܴ��ڵ���10 2--�豸��(deviceX)������Χ 3--����ռ�����������Χ 
//    4--��������ռ�״̬���� 5--�ڴ�ռ��С������ռ䲻ƥ�� 6--���ݳ���(Byte)������Χ
DRIVER_DECLSPEC DWORD __stdcall DmaMemToDSP(int deviceX,UINT memIdx,UINT *pBufAddr,WriteToDev pPhyAddr,WriteToDev dataLen,WriteToDev *endFlag,HWND hEndWnd)
{
	COPYDATASTRUCT cds;
	DmaToDev dma2Dev;

	dma2Dev.DeviceX = deviceX;
	dma2Dev.dmaFlag = 2;   // hu 1--ͨ���ļ� 2--ͨ���ڴ�
	if (memIdx >= 10)  
	{
		wcscpy(lastError, L"����ռ��������ܴ��ڵ���10��");
		return 1;
	}
	dma2Dev.memIdx = memIdx;

	if (dma2Dev.DeviceX>=DEVNUM_MAX || dma2Dev.DeviceX<0) 
	{
		wcscpy(lastError, L"�豸��(deviceX)������Χ��");
		return 2;
	}
	if(dma2Dev.memIdx >= driver[dma2Dev.DeviceX].memNum) 
	{
		wcscpy(lastError, L"����ռ�����������Χ��");
		return 3;
	}
	if((0 != driver[dma2Dev.DeviceX].memStatus[dma2Dev.memIdx]) || (0 == driver[dma2Dev.DeviceX].memSize[dma2Dev.memIdx])) 
	{
		wcscpy(lastError, L"��������ռ�״̬����");
		return 4;
	}
	if (driver[dma2Dev.DeviceX].memSize[dma2Dev.memIdx] != shareMem0.memSize)
	{
		wcscpy(lastError, L"�ڴ�ռ��С������ռ䲻ƥ�䣡");
		return 5;
	}

	dma2Dev.pBufAddr = pBufAddr;
	dma2Dev.hUserWndforEnd = hEndWnd;
	dma2Dev.pPhyAddr = pPhyAddr;
	dma2Dev.pPhyAddr.Write32 = 0;
	dma2Dev.dataLen = dataLen;
	if (0==dma2Dev.dataLen.Write32 || dma2Dev.dataLen.Write32>shareMem0.memSize)
	{
		wcscpy(lastError, L"���ݳ���(Byte)������Χ��");
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

// hu ��DSPͨ��DMA���䵽ָ���ڴ��ַ(���Ƽ�ʹ��)
// hu memIdxΪʹ�õ�����ռ�����(һ��Ϊ0) | hEndWnd��ΪNULL,����ע�ᴰ�ڷ����жϺ� | pPhyAddr.Write32�ɵײ�д��
// hu 1--����ռ��������ܴ��ڵ���10 2--�豸��(deviceX)������Χ 3--����ռ�����������Χ 
//    4--��������ռ�״̬���� 5--�ڴ�ռ��С������ռ䲻ƥ�� 6--���ݳ���(Byte)������Χ
DRIVER_DECLSPEC DWORD __stdcall DmaDSPToMem(int deviceX,UINT memIdx,UINT *pBufAddr,WriteToDev pPhyAddr,WriteToDev dataLen,HWND hEndWnd)
{
	COPYDATASTRUCT cds;
	DmaFromDev dma2Pc;

	dma2Pc.DeviceX = deviceX;
	dma2Pc.dmaFlag = 2;     // hu 1--ͨ���ļ� 2--ͨ���ڴ�
	if (memIdx >= 10)  
	{
		wcscpy(lastError, L"����ռ��������ܴ��ڵ���10��");
		return 1;
	}
	dma2Pc.memIdx = memIdx;

	if (dma2Pc.DeviceX>=DEVNUM_MAX || dma2Pc.DeviceX<0) 
	{
		wcscpy(lastError, L"�豸��(deviceX)������Χ��");
		return 2;
	}
	if(dma2Pc.memIdx >= driver[deviceX].memNum) 
	{
		wcscpy(lastError, L"����ռ�����������Χ��");
		return 3;
	}
	if((0 != driver[deviceX].memStatus[dma2Pc.memIdx]) || (0 == driver[deviceX].memSize[dma2Pc.memIdx])) 
	{
		wcscpy(lastError, L"��������ռ�״̬����");
		return 4;
	}
	if (driver[deviceX].memSize[dma2Pc.memIdx] != shareMem0.memSize)
	{
		wcscpy(lastError, L"�ڴ�ռ��С������ռ䲻ƥ�䣡");
		return 5;
	}

	dma2Pc.pBufAddr = pBufAddr;
	dma2Pc.hUserWndforEnd = hEndWnd;
	dma2Pc.pPhyAddr = pPhyAddr;
	dma2Pc.pPhyAddr.Write32 = 0;
	dma2Pc.dataLen = dataLen;
	if (0==dma2Pc.dataLen.Write32 || dma2Pc.dataLen.Write32>shareMem0.memSize) 
	{
		wcscpy(lastError, L"���ݳ���(Byte)������Χ��");
		return 6;
	}

	cds.dwData = 0;
	cds.lpData = &dma2Pc;
	cds.cbData = sizeof(dma2Pc);
	SendMessage(hDriverWnd,WM_COPYDATA,ID_SYS_DMAFromDev,(LPARAM)&cds);
	return 0;
}

// hu �ر����������������Ⲣ�˳���������
DRIVER_DECLSPEC void __stdcall CloseAllDSPDriver()
{
	SendMessage(hDriverWnd,WM_DESTROY,0,0);
}

// hu �������빲���ڴ�(�ڲ�ʹ��)
// hu 0--�ɹ� 1--�򿪹����ڴ�ʧ�� 2--��ȡ�����ڴ��׵�ַʧ�� 3--�ڴ�й©
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

// hu ��ʼ��������������(���ض����������Ҫʹ��)
// hu 0 -- �ɹ� 1 -- δɨ�赽PCI�豸 2 -- ��ȡ�豸�����������ʧ�� 3 -- ��ʼ���豸������ʧ��
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
	case 1:wcscpy(lastError, L"δɨ�赽PCI�豸��");break;
	case 2:wcscpy(lastError, L"��ȡ�豸�����������ʧ�ܣ�");break;
	case 3:wcscpy(lastError, L"��ʼ���豸������ʧ�ܣ�");break;
	}
	return status;
}

// hu �ر�������������(���ض����������Ҫʹ��)
// hu 0 -- �ɹ� 1 -- �ر�PCI����ʧ�� 2 -- ж���豸������ʧ�� 3 -- �ر�PCI������ж���豸������ʧ��
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
	case 1:wcscpy(lastError, L"�ر�PCI����ʧ�ܣ�");break;
	case 2:wcscpy(lastError, L"ж���豸������ʧ�ܣ�");break;
	case 3:wcscpy(lastError, L"�ر�PCI������ж���豸������ʧ�ܣ�");break;
	}
	return status;
}

// hu ���������ַ�������ڴ�ռ�(���ض����������Ҫʹ��)
// hu 0--�ɹ� 1--�豸��(deviceX)������Χ 2--�����������ռ���Ŀ�Ѵ����� 3--����ռ��С����Ϊ0 
//    4--�����������ռ�������������10 5--��������ռ�ʧ�� 6--CPU����������ռ�ͬ��ʧ��
DRIVER_DECLSPEC DWORD __stdcall ApplyForPA(int deviceX,DWORD dwDMABufSize)
{
	COPYDATASTRUCT cds;
	unsigned int status;
	ApplyForPa applyPA = {deviceX,dwDMABufSize};

	if (applyPA.DeviceX>=DEVNUM_MAX || applyPA.DeviceX<0)  
	{
		wcscpy(lastError, L"�豸��(deviceX)������Χ��");
		return 1;  
	}
	if (10 == driver[applyPA.DeviceX].memNum)  
	{
		wcscpy(lastError, L"�����������ռ���Ŀ�Ѵ����ޣ�");
		return 2;
	}
	if (0 == applyPA.dwDMABufSize) 
	{
		wcscpy(lastError, L"����ռ��С����Ϊ0��");
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
		case 1:wcscpy(lastError, L"�����������ռ�������������10��");break;
		case 2:wcscpy(lastError, L"��������ռ�ʧ�ܣ�");break;
		case 3:wcscpy(lastError, L"CPU����������ռ�ͬ��ʧ�ܣ�");break;
		}
		return status+3;
	}

	GetDriverStatus(driver);
	return 0;
}

// hu �ͷ�������Ŀռ�(���ض����������Ҫʹ��)
// hu 0 -- �ɹ� 1--�豸��(deviceX)������Χ 2--����ռ�����������Χ 3--��������ռ�״̬����
//    4--I/O����������ռ�ͬ��ʧ�� 5--�ͷ�����ռ�ʧ��
DRIVER_DECLSPEC DWORD __stdcall FreePA(int deviceX,UINT memIdx)
{
	COPYDATASTRUCT cds;
	unsigned int status;
	FreePa freePA = {deviceX,memIdx};

	if (freePA.DeviceX>=DEVNUM_MAX || freePA.DeviceX<0)  
	{
		wcscpy(lastError, L"�豸��(deviceX)������Χ��");
		return 1;
	}
	if (freePA.memIdx >= driver[freePA.DeviceX].memNum) 
	{
		wcscpy(lastError, L"����ռ�����������Χ��");
		return 2;
	}
	if((0 != driver[freePA.DeviceX].memStatus[freePA.memIdx]) || (0 == driver[freePA.DeviceX].memSize[freePA.memIdx])) 
	{
		wcscpy(lastError, L"��������ռ�״̬����");
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
		case 1:wcscpy(lastError, L"I/O����������ռ�ͬ��ʧ�ܣ�");break;
		case 2:wcscpy(lastError, L"�ͷ�����ռ�ʧ�ܣ�");break;
		}
		return status+3;
	}

	GetDriverStatus(driver); 
	return 0;
}