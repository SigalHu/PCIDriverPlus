/*
 * ����ȫ�ֱ���
 *
 * @author ������
 * @date 2016/01/12
 */

#pragma once

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

class dmaDeliver
{
public:
	UINT  Deliver_Time;
	CFile Deliver_FP;
};