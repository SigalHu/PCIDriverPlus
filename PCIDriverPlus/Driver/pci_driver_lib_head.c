#include <stdio.h>
#include "pci_driver_lib.h"
#include "pci_driver_lib_head.h"

HWND ghWnd[DEVNUM_MAX];

DWORD Initialize_DSPDriver(int deviceX)
{
	return initialize_PCI(deviceX);
}

DWORD WriteToDSP(int deviceX,int barX, DWORD dwoffset, UINT32 write32, unsigned int repeatTimes)
{
	return  Write(deviceX,barX, dwoffset,write32,repeatTimes);
}
DWORD WriteToDSP_256Words(int deviceX,int barX, DWORD dwoffset, UINT32 *write256Words)
{
	return Write_256Words(deviceX, barX,  dwoffset, write256Words);
}

DWORD ReadfromDSP(int deviceX,int barX, DWORD dwoffset, UINT32 *read32)
{
	return  Read(deviceX,barX, dwoffset,read32);
}


DWORD ReadfromDSP_256Words(int deviceX,int barX, DWORD dwoffset, UINT32 *read256Words)
{
	return Read_256Words(deviceX, barX,  dwoffset,  read256Words);
}


DWORD DllFun(int deviceX,HWND hWn)  
{
	if (deviceX >= DEVNUM_MAX)
	{
		return 1;
	}
	ghWnd[deviceX] = hWn;
	return 0;
}

DWORD EnableInterruptFromDSP(int deviceX)
{
	return Interrupt_1(deviceX);
}


DWORD DisEnableInterruptFromDSP(int deviceX)
{
	return DisInterrupt_1(deviceX);
}


DWORD SendIntToDSP(int deviceX)
{
	return SendInterrupt(deviceX);
}

DWORD ApplyForPA(int deviceX, DWORD dwDMABufSize, void * * pBufAddress, DWORD * pBufPhysicalAddress, DWORD * DMAHandle)
{
	return ApplyForPhysicalAddress(deviceX, dwDMABufSize, pBufAddress, pBufPhysicalAddress, DMAHandle);
}

DWORD FreePA(int deviceX, DWORD DMAHandle)
{
	return FreePhysicalAddress(deviceX, DMAHandle);
}


DWORD Close_DSPDriver(int deviceX)
{
	return Close_PCI(deviceX);
}


/*
void RandGroupToDSP_to_Buffer(void * RandGroupToDSP) 
{
	memcpy_to_pBuffSend(RandGroupToDSP);
}

void Buffer_to_EDMA_Receive(void * EDMA_Receive)
{
	memcpy_to_EDMA_Receive(EDMA_Receive);
}
*/
