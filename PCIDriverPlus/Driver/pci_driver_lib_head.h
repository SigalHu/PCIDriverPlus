/*
 * pci6455������غ�������ӿ�
 *
 * @author SigalHu
 * @date 2015/12/21
 */

#ifndef _PCI_DRIVER_LIB_HEAD_H_
#define _PCI_DRIVER_LIB_HEAD_H_

// hu �����ض������/////////////////////////////////
#if defined(_WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(_WIN64)
#include <windows.h>
#endif
/////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

// hu //////////////////////////////////////////////////////
#define WM_CALLBACKMESSAGE_0 WM_USER+470
#define DEVNUM_MAX 5
extern HWND ghWnd[DEVNUM_MAX];
////////////////////////////////////////////////////////////

// hu ��ʼ��������������
// hu 0 -- �ɹ� 1 -- δɨ�赽PCI�豸 2 -- ��ȡ�豸�����������ʧ�� 3 -- ��ʼ���豸������ʧ��
DWORD Initialize_DSPDriver(int deviceX);

// hu ��ָ��DSP��ַд32bit����
// hu 0 -- �ɹ� 1 -- PCI�豸д��ʧ��
DWORD WriteToDSP(int deviceX,int barX, unsigned long dwoffset, unsigned int write32,unsigned int repeatTimes);

// hu ��ָ��DSP��ַд1KB����
// hu 0 -- �ɹ� 1 -- PCI�豸д��ʧ��
DWORD WriteToDSP_256Words(int deviceX,int barX, DWORD dwoffset, UINT32 *write256Words);

// hu ��ָ��DSP��ַ��32bit����
// hu 0 -- �ɹ� 1 -- PCI�豸��ȡʧ��
DWORD ReadfromDSP(int deviceX,int barX, unsigned long dwoffset, unsigned int *read32);

// hu ��ָ��DSP��ַ��1KB����
// hu 0 -- �ɹ� 1 -- PCI�豸��ȡʧ��
DWORD ReadfromDSP_256Words(int deviceX,int barX, DWORD dwoffset, UINT32 *read256Words);

// hu ���������򴫵���Ϣ���
// hu 0 -- �ɹ� 1 -- ���ݾ��ʧ��
DWORD DllFun(int deviceX,HWND hWn);

// hu ʹ������DSP���ж�
// hu 0 -- �ɹ� 1 -- ʹ��PCI�ж�ʧ��
DWORD EnableInterruptFromDSP(int deviceX);

// hu ��������DSP���ж�
// hu 0 -- �ɹ� 1 -- ����PCI�ж�ʧ��
DWORD DisEnableInterruptFromDSP(int deviceX);

// hu ��DSP�����ж�
// hu 0 -- �ɹ�
DWORD SendIntToDSP(int deviceX);

// hu ���������ַ�������ڴ�ռ�
// hu 0 -- �ɹ� 1 -- �����������ռ�������������10 2 -- ��������ռ�ʧ�� 3 -- CPU����������ռ�ͬ��ʧ��
DWORD ApplyForPA(int deviceX, DWORD dwDMABufSize, void * * pBufAddress, DWORD * pBufPhysicalAddress, DWORD * DMAHandle);

// hu �ͷ�������Ŀռ�
// hu 0 -- �ɹ� 1 -- I/O����������ռ�ͬ��ʧ�� 2 -- �ͷ�����ռ�ʧ��
DWORD FreePA(int deviceX, DWORD DMAHandle);

// hu �ر�������������
// hu 0 -- �ɹ� 1 -- �ر�PCI����ʧ�� 2 -- ж���豸������ʧ�� 3 -- �ر�PCI������ж���豸������ʧ��
DWORD Close_DSPDriver(int deviceX);
//void RandGroupToDSP_to_Buffer(void * RandGroupToDSP);
//void Buffer_to_EDMA_Receive(void * EDMA_Receive);*/

#ifdef __cplusplus
}
#endif

#endif