/*
 * �����������ȫ�ֱ���(���ļ��е�class��ΪPOD,����memcpy�Ⱥ���)  
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
	int dmaFlag;                   // hu 0--dmaδ���� 1--ͨ���ļ� 2--ͨ���ڴ�
	HWND hUserWndforEnd;           // hu dma������Ϣ���(��ΪNULL�����͵�ע����Ϣ)
	UINT memIdx;                   // hu ��ʹ�õ�����ռ�����(��0��ʼ)
	wchar_t binPath[MAX_PATH];     // hu ׼��DMA������ļ�·��
	UINT *pBufAddr;                // hu �ڴ��׵�ַ
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