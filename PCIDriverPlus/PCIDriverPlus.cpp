// PCIDriverPlus.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "windows.h"
#include <shellapi.h>
#include <tlhelp32.h> 
#include "stdio.h"
#include "PCIDriverPlus.h"
#include "PciDevDataDef.h"
#include "PciDriverFunID.h"
#include "pci_driver_lib_head.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// hu 全局变量 //////////////////
static HWND hDriverWnd;           // hu 驱动程序(本程序)句柄
static HWND hUserWnd;             // hu 应用程序句柄
static ShareMem shareMem0 = {NULL,L"ShareMemForPciDriver",NULL,1024};
static driver driver[DEVNUM_MAX];
static int driverNum;
static DmaSource dmaSource[DEVNUM_MAX];
static DmaDest   dmaDest[DEVNUM_MAX];
static wchar_t progName[20];        // hu 程序名称
static wchar_t progPath[MAX_PATH];  // hu 程序工作目录
static TrayShow trayShow;
static DmaToDevFifo* dma2DevFifoHead[DEVNUM_MAX] = {NULL};
static DmaFromDevFifo* dma2PcFifoHead[DEVNUM_MAX] = {NULL};
/////////////////////////////////

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;
	FILE *InfoFp,*CfgFp;
	int ii,jj;
	wchar_t strError[100];

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_PCIDRIVER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PCIDRIVER));

	// hu 初始化状态位
	for (ii = 0;ii < DEVNUM_MAX;ii++)
	{
		driver[ii].status = -1;
		for (jj = 0;jj < 10;jj++)
		{
			driver[ii].memStatus[jj] = -1;
		}
	}

	// hu 获得自身程序路径
	GetModuleFileNameW(NULL,progPath,MAX_PATH);
	wchar_t *progNametmp = wcsrchr(progPath,L'\\');
	if (NULL == progNametmp)
	{
		MessageBox(hDriverWnd,L"获取程序路径失败！",L"警告",MB_OK|MB_ICONWARNING);
		return 0;
	}
	progNametmp++;
	wcscpy(progName,progNametmp);
	*progNametmp = L'\0';

	// hu 判断该程序是否已运行
	HWND hDriverWndTmp = FindWindowEx(NULL,hDriverWnd,szWindowClass,NULL);
	if (NULL != hDriverWndTmp)
	{
		if (IDYES == MessageBox(hDriverWnd,L"该程序已运行！\n是否切换至当前程序！",L"警告",MB_YESNO|MB_ICONWARNING))
		{
            PostMessage(hDriverWndTmp,WM_DESTROY,0,0);
		}
		else  return 0;
	}

	//PROCESSENTRY32 pe32;  
	//pe32.dwSize = sizeof(pe32);   
	//HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 
	//if(INVALID_HANDLE_VALUE != hProcessSnap)  
	//{  
	//	int processNum = 0;
	//	::Process32First(hProcessSnap, &pe32);
	//	do 
	//	{
	//		if(0 == wcscmp(progName,pe32.szExeFile))  processNum++;

	//		if (processNum > 1)
	//		{
	//			if (IDYES == MessageBox(hDriverWnd,L"该程序已运行！\n是否切换至当前程序！",L"警告",MB_YESNO|MB_ICONWARNING))
	//			{
	//				HWND hDriverWndTmp = FindWindowEx(NULL,hDriverWnd,szWindowClass,NULL);
	//				if (NULL != hDriverWndTmp)
	//				{
	//					SendMessage(hDriverWndTmp,WM_DESTROY,0,0);
	//				}
	//			}
	//			else
	//			{
	//				::CloseHandle(hProcessSnap);
	//				return 0;
	//			}
	//		}
	//	} while (::Process32Next(hProcessSnap, &pe32));
	//	::CloseHandle(hProcessSnap);
	//}

	//SendMessage(hDriverWnd,WM_COPYDATA,ID_SYS_SetAutoRun,0);
	// hu 通过配置文件读取配置参数
	wchar_t filePath[MAX_PATH];
	wcscpy(filePath,progPath);
	wcscat(filePath,L"PciDriverInfo.txt");
	_wsetlocale(0,L"chs");
	InfoFp = _wfopen(filePath,L"wt+");
	wcscpy(filePath,progPath);
	wcscat(filePath,L"PciDriverCfg.txt");
	CfgFp = _wfopen(filePath,L"rt");
	if (NULL == CfgFp)
	{
		CfgFp = _wfopen(filePath,L"wt+");
		
		fwprintf(CfgFp,L"调试模式:\n");
		trayShow.status = 0;  // hu 默认不显示托盘
		fwprintf(CfgFp,L"isDebug:%d\n",trayShow.status);

		fwprintf(CfgFp,L"连续DMA空间配置参数:\n");
		driverNum = 1;
		fwprintf(CfgFp,L"driverNum:%d\n",driverNum);


		for (ii = 0;ii < DEVNUM_MAX;ii++)
		{
			fwprintf(CfgFp,L"driver:\n");
			driver[0].memNum = 1;
			fwprintf(CfgFp,L"memNum:%d\n",driver[0].memNum);
			driver[0].memSize[0] = 11*1024*1024;
			fwprintf(CfgFp,L"memSize:%d\n",driver[0].memSize[0]);
		}
	}
	else
	{
		wchar_t temp[20];

		fwscanf(CfgFp,L"%s\n",temp);
		// hu 读取托盘状态参数
		fwscanf(CfgFp,L"%s\n",temp);
		_stscanf_s(temp,L"isDebug:%d",&trayShow.status);
		temp[8] = L'\0';
		if (wcscmp(temp,L"isDebug:"))
		{
			fwprintf(InfoFp,L"Error:读取配置文件参数isDebug失败！\n");
			fclose(CfgFp);
			fclose(InfoFp);
			if (trayShow.status)
			{
				MessageBox(hDriverWnd,L"配置文件错误：读取参数isDebug失败！",L"警告",MB_OK|MB_ICONWARNING);
			}
			return 0;
		}

		fwscanf(CfgFp,L"%s\n",temp);
		// hu 读取driver连续DMA空间配置参数
		fwscanf(CfgFp,L"%s\n",temp);
		_stscanf_s(temp,L"driverNum:%d",&driverNum);
		temp[10] = L'\0';
		if (wcscmp(temp,L"driverNum:"))
		{
			fwprintf(InfoFp,L"Error:读取配置文件参数driverNum失败！\n");
			fclose(CfgFp);
			fclose(InfoFp);
			if (trayShow.status)
			{
				MessageBox(hDriverWnd,L"配置文件错误：读取参数driverNum失败！",L"警告",MB_OK|MB_ICONWARNING);
			}
			return 0;
		}

		if (driverNum > DEVNUM_MAX || driverNum < 1)
		{
			fwprintf(InfoFp,L"Error:驱动数目必须为1--5！\n");
			fclose(CfgFp);
			fclose(InfoFp);
			if (trayShow.status)
			{
				MessageBox(hDriverWnd,L"配置文件错误：驱动数目必须为1--5！",L"警告",MB_OK|MB_ICONWARNING);
			}
			return 0;
		}

		for (ii = 0;ii < driverNum;ii++)
		{
			fwscanf(CfgFp,L"%s\n",temp);
			if (wcscmp(temp,L"driver:"))
			{
				fwprintf(InfoFp,L"Error:读取driver%d连续DMA空间配置参数失败！\n",ii);
				fclose(CfgFp);
				fclose(InfoFp);
				_stprintf_s(strError,L"配置文件错误：读取driver%d连续DMA空间配置参数失败！",ii);
				if (trayShow.status)
				{
					MessageBox(hDriverWnd,strError,L"警告",MB_OK|MB_ICONWARNING);
				}
				return 0;
			}

			fwscanf(CfgFp,L"%s\n",temp);
			_stscanf_s(temp,L"memNum:%d",&driver[ii].memNum);
			temp[7] = L'\0';
			if (wcscmp(temp,L"memNum:"))
			{
				fwprintf(InfoFp,L"Error:读取配置文件driver%d参数memNum失败！\n",ii);
				fclose(CfgFp);
				fclose(InfoFp);
				_stprintf_s(strError,L"配置文件错误：读取driver%d参数memNum失败！",ii);
				if (trayShow.status)
				{
					MessageBox(hDriverWnd,strError,L"警告",MB_OK|MB_ICONWARNING);
				}
				return 0;
			}

			if (driver[ii].memNum < 11 && driver[ii].memNum >= 0)
			{
				for (jj = 0;jj < driver[ii].memNum;jj++)
				{
					fwscanf(CfgFp,L"%s\n",temp);
					_stscanf_s(temp,L"memSize:%d",&driver[ii].memSize[jj]);
					temp[8] = L'\0';
					if (wcscmp(temp,L"memSize:"))
					{
						fwprintf(InfoFp,L"Error:读取配置文件driver%d参数memSize失败！\n",ii);
						fclose(CfgFp);
						fclose(InfoFp);
						_stprintf_s(strError,L"配置文件错误：读取driver%d参数memSize失败！",ii);
						if (trayShow.status)
						{
							MessageBox(hDriverWnd,strError,L"警告",MB_OK|MB_ICONWARNING);
						}
						return 0;
					}
				}
			}
			else
			{
				fwprintf(InfoFp,L"Error:driver%d所申请的连续DMA空间数目应小于等于10！\n",ii);
				fclose(CfgFp);
				fclose(InfoFp);
				_stprintf_s(strError,L"配置文件错误：driver%d所申请的连续DMA空间数目应小于等于10！",ii);
				if (trayShow.status)
				{
					MessageBox(hDriverWnd,strError,L"警告",MB_OK|MB_ICONWARNING);
				}
				return 0;
			}
		}
	}
	fclose(CfgFp);
	
	// hu 申请共享内存
	shareMem0.hMap = ::CreateFileMapping(INVALID_HANDLE_VALUE,NULL,PAGE_READWRITE,0,shareMem0.memSize,shareMem0.name);
	if (NULL == shareMem0.hMap)
	{
		fclose(InfoFp);
		if (trayShow.status)
		{
			MessageBox(hDriverWnd,L"申请共享内存失败！",L"警告",MB_OK|MB_ICONWARNING);
		}
		return 0;
	}
	shareMem0.headAddr = ::MapViewOfFile(shareMem0.hMap,FILE_MAP_ALL_ACCESS,0,0,0);

	for (ii = 0;ii < driverNum;ii++)
	{
		fwprintf(InfoFp,L"driver%d:\n",ii);
		// hu 初始化PCI设备
#ifdef DEBUG_HU
		driver[ii].status = 99;
#else
		driver[ii].status = Initialize_DSPDriver(ii);
#endif
		switch(driver[ii].status)
		{
		case 0:fwprintf(InfoFp,L"Succeed:初始化driver%d成功！\n",ii);break;
		case 1:
			{
				fwprintf(InfoFp,L"Error:未扫描到driver%d设备！\n",ii);
				_stprintf_s(strError,L"未扫描到driver%d设备！",ii);
				if (trayShow.status)
				{
					MessageBox(hDriverWnd,strError,L"警告",MB_OK|MB_ICONWARNING);
				}
				break;
			}
		case 2:
			{
				fwprintf(InfoFp,L"Error:获取driver%d设备驱动操作句柄失败！\n",ii);
				_stprintf_s(strError,L"获取driver%d设备驱动操作句柄失败！",ii);
				if (trayShow.status)
				{
					MessageBox(hDriverWnd,strError,L"警告",MB_OK|MB_ICONWARNING);
				}
				break;
			}
		case 3:
			{
				fwprintf(InfoFp,L"Error:初始化driver%d设备驱动库失败！\n",ii);
				_stprintf_s(strError,L"初始化driver%d设备驱动库失败！",ii);
				if (trayShow.status)
				{
					MessageBox(hDriverWnd,strError,L"警告",MB_OK|MB_ICONWARNING);
				}
				break;
			}
		default:
			{
				fwprintf(InfoFp,L"Error:不是driver%d设备驱动初始化函数的默认返回值！\n",ii);
				_stprintf_s(strError,L"不是driver%d设备驱动初始化函数的默认返回值！",ii);
				if (trayShow.status)
				{
					MessageBox(hDriverWnd,strError,L"警告",MB_OK|MB_ICONWARNING);
				}
				break;
			}
		}

		// hu 申请物理空间
		if (driver[ii].status)
		{
			fwprintf(InfoFp,L"Error:driver%d当前状态出错,未申请物理空间！\n",ii);
			_stprintf_s(strError,L"driver%d当前状态出错,未申请物理空间！",ii);
			if (trayShow.status)
			{
				MessageBox(hDriverWnd,strError,L"警告",MB_OK|MB_ICONWARNING);
			}
		}
		else
		{
			for (jj = 0;jj < driver[ii].memNum;jj++)
			{
#ifdef DEBUG_HU
				driver[ii].memStatus[jj] = 99;
#else
				driver[ii].memStatus[jj] = ApplyForPA(ii,driver[ii].memSize[jj],
					&driver[ii].EDMAWinDriver_Device[jj],
					&driver[ii].EDMAPhysicalAddress_Device[jj],
					&driver[ii].EDMAHandle_Device[jj]);
#endif
				switch(driver[ii].memStatus[jj])
				{
				case 0:fwprintf(InfoFp,L"Succeed:申请%dByte连续物理空间成功！\n",driver[ii].memSize[jj]);break;
				case 1:
					{
						fwprintf(InfoFp,L"Error:所申请的物理空间数量超过上限10！\n");
						_stprintf_s(strError,L"driver%d：所申请的物理空间数量超过上限10！",ii);
						if (trayShow.status)
						{
							MessageBox(hDriverWnd,strError,L"警告",MB_OK|MB_ICONWARNING);
						}
						break;
					}
				case 2:
					{
						fwprintf(InfoFp,L"Error:申请物理空间失败！\n");
						_stprintf_s(strError,L"driver%d：申请物理空间失败！",ii);
						if (trayShow.status)
						{
							MessageBox(hDriverWnd,strError,L"警告",MB_OK|MB_ICONWARNING);
						}
						break;
					}
				case 3:
					{
						fwprintf(InfoFp,L"Error:CPU缓存与物理空间同步失败！\n");
						_stprintf_s(strError,L"driver%d：CPU缓存与物理空间同步失败",ii);
						if (trayShow.status)
						{
							MessageBox(hDriverWnd,strError,L"警告",MB_OK|MB_ICONWARNING);
						}
						break;
					}
				default:
					{
						fwprintf(InfoFp,L"Error:不是申请物理空间函数的默认返回值！\n");
						_stprintf_s(strError,L"driver%d：不是申请物理空间函数的默认返回值！",ii);
						if (trayShow.status)
						{
							MessageBox(hDriverWnd,strError,L"警告",MB_OK|MB_ICONWARNING);
						}
						break;
					}
				}
			}
		}
		// hu 注册消息句柄
		DllFun(ii,hDriverWnd);
	}
	fclose(InfoFp);

	// hu 产生托盘
	if (trayShow.status)
	{
		SendMessage(hDriverWnd,WM_COPYDATA,ID_SYS_CreatTray,0);
	}

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PCIDRIVER));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_PCIDRIVER);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   hDriverWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hDriverWnd)
   {
      return FALSE;
   }

   //ShowWindow(hDriverWnd, nCmdShow);
   //UpdateWindow(hDriverWnd);

   return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	unsigned int status0,status1;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COPYDATA:
		{
			switch(wParam)
			{
			case ID_FUN_WriteToDSP:
				{
					WriteToDev *driverDSP = (WriteToDev *)((COPYDATASTRUCT*)lParam)->lpData;

#ifdef DEBUG_HU
					status0 = 99;
					memcpy((unsigned int*)shareMem0.headAddr,&status0,sizeof(status0));
					AllocConsole();
					::freopen("CONOUT$", "w", stdout); 
					printf("driver:%d\n",driverDSP->DeviceX);
					printf("bar:%d\n",driverDSP->BarX);
					printf("Offset:%d\n",driverDSP->dwOffset);
					printf("Write32:%d\n",driverDSP->Write32);
					printf("RepeatTimes:%d\n",driverDSP->RepeatTimes);
					printf("status:%d\n",*(unsigned int*)shareMem0.headAddr);
					Sleep(5000);
					FreeConsole();
#else
					status0 = WriteToDSP(driverDSP->DeviceX,driverDSP->BarX,driverDSP->dwOffset,
						                 driverDSP->Write32,driverDSP->RepeatTimes);
					memcpy((unsigned int*)shareMem0.headAddr,&status0,sizeof(status0));
#endif
					break;
				}
			case ID_FUN_ReadFromDSP:
				{
					unsigned int read32;
					ReadFromDev *driverDSP = (ReadFromDev *)((COPYDATASTRUCT*)lParam)->lpData;

#ifdef DEBUG_HU
					status0 = 99;
					memcpy((unsigned int*)shareMem0.headAddr,&status0,sizeof(status0));
					read32 = 20160109;
					memcpy((unsigned int*)shareMem0.headAddr+1,&read32,sizeof(read32));
					AllocConsole();
					::freopen("CONOUT$", "w", stdout); 
					printf("driver:%d\n",driverDSP->DeviceX);
					printf("bar:%d\n",driverDSP->BarX);
					printf("Offset:%d\n",driverDSP->dwOffset);
					printf("status:%d\n",*(unsigned int*)shareMem0.headAddr);
					printf("read32:%d\n",*((unsigned int*)shareMem0.headAddr+1));
					Sleep(5000);
					FreeConsole();
#else
					status0 = ReadfromDSP(driverDSP->DeviceX,driverDSP->BarX,driverDSP->dwOffset,&read32);
					memcpy((unsigned int*)shareMem0.headAddr,&status0,sizeof(status0));
					memcpy((unsigned int*)shareMem0.headAddr+1,&read32,sizeof(read32));
#endif
					break;
				}
			case ID_FUN_InitDSPDriver:
				{
					int deviceX = *((int *)((COPYDATASTRUCT*)lParam)->lpData);
#ifdef DEBUG_HU
					status0 = 99;
					memcpy((unsigned int*)shareMem0.headAddr,&status0,sizeof(status0));
					AllocConsole();
					::freopen("CONOUT$", "w", stdout); 
					printf("deviceX:%d\n",deviceX);
					printf("status:%d\n",*(unsigned int*)shareMem0.headAddr);
					Sleep(5000);
					FreeConsole();
#else
					status0 = Initialize_DSPDriver(deviceX);
					memcpy((unsigned int*)shareMem0.headAddr,&status0,sizeof(status0));
#endif
					break;
				}
			case ID_FUN_CloseDSPDriver:
				{
					int deviceX = *((int *)((COPYDATASTRUCT*)lParam)->lpData);
#ifdef DEBUG_HU
					status0 = 99;
					memcpy((unsigned int*)shareMem0.headAddr,&status0,sizeof(status0));
					AllocConsole();
					::freopen("CONOUT$", "w", stdout); 
					printf("deviceX:%d\n",deviceX);
					printf("status:%d\n",*(unsigned int*)shareMem0.headAddr);
					Sleep(5000);
					FreeConsole();
#else
					status0 = Close_DSPDriver(deviceX);
					memcpy((unsigned int*)shareMem0.headAddr,&status0,sizeof(status0));
#endif
					break;
				}
			case ID_FUN_ApplyForPA:
				{
					ApplyForPa *applyPA = (ApplyForPa *)((COPYDATASTRUCT*)lParam)->lpData;
					int deviceX = applyPA->DeviceX;

					driver[deviceX].memSize[driver[deviceX].memNum] = applyPA->dwDMABufSize;
#ifdef DEBUG_HU
					driver[deviceX].memStatus[driver[deviceX].memNum] = 99;
#else
					driver[deviceX].memStatus[driver[deviceX].memNum] = ApplyForPA(deviceX,
						driver[deviceX].memSize[driver[deviceX].memNum],
						&driver[deviceX].EDMAWinDriver_Device[driver[deviceX].memNum],
						&driver[deviceX].EDMAPhysicalAddress_Device[driver[deviceX].memNum],
						&driver[deviceX].EDMAHandle_Device[driver[deviceX].memNum]);
#endif
					status0 = driver[deviceX].memStatus[driver[deviceX].memNum];
					driver[deviceX].memNum++;

					memcpy((unsigned int*)shareMem0.headAddr,&status0,sizeof(status0));
					break;
				}
			case ID_FUN_FreePA:
				{
					FreePa *freePA = (FreePa *)((COPYDATASTRUCT*)lParam)->lpData;
					int deviceX = freePA->DeviceX;

#ifdef DEBUG_HU
					driver[deviceX].memStatus[freePA->memIdx] = 99;
#else
					driver[deviceX].memStatus[freePA->memIdx] = FreePA(deviceX,
						driver[deviceX].EDMAHandle_Device[freePA->memIdx]);
#endif
					status0 = driver[deviceX].memStatus[driver[deviceX].memNum];
					if (0 == status0)
					{
						driver[deviceX].memNum--;
						for (int ii=freePA->memIdx;ii<driver[deviceX].memNum;ii++)
						{
							driver[deviceX].memStatus[ii] = driver[deviceX].memStatus[ii+1];
							driver[deviceX].memSize[ii] = driver[deviceX].memSize[ii+1];
							driver[deviceX].EDMAWinDriver_Device[ii] = driver[deviceX].EDMAWinDriver_Device[ii+1];
							driver[deviceX].EDMAPhysicalAddress_Device[ii] = driver[deviceX].EDMAPhysicalAddress_Device[ii+1];
							driver[deviceX].EDMAHandle_Device[ii] = driver[deviceX].EDMAHandle_Device[ii+1];
						}
						driver[deviceX].memStatus[driver[deviceX].memNum] = -1;
						driver[deviceX].memSize[driver[deviceX].memNum] = 0;
						driver[deviceX].EDMAWinDriver_Device[driver[deviceX].memNum] = 0;
						driver[deviceX].EDMAPhysicalAddress_Device[driver[deviceX].memNum] = 0;
						driver[deviceX].EDMAHandle_Device[driver[deviceX].memNum] = 0;
					}
					memcpy((unsigned int*)shareMem0.headAddr,&status0,sizeof(status0));
					break;
				}
			case ID_SYS_RegistHWNDToDev:
				{
					hUserWnd = *((HWND *)((COPYDATASTRUCT*)lParam)->lpData);
#ifdef DEBUG_HU
					AllocConsole();
					::freopen("CONOUT$", "w", stdout); 
					printf("hUserWnd:%d\n",hUserWnd);
					Sleep(5000);
					FreeConsole();
#endif
					break;
				}
			case ID_SYS_SetAutoRun:
				{
					HKEY   hKey;
					BOOL Enable = *((BOOL *)((COPYDATASTRUCT*)lParam)->lpData);
 
					wchar_t progNametmp[20];
					wchar_t progPathtmp[MAX_PATH];
					wcscpy(progPathtmp,progPath);
					wcscpy(progNametmp,progName);

					wcscat(progPathtmp,progNametmp);
					*(progNametmp+wcslen(progNametmp)-4) = L'\0';

					// hu 找到系统的启动项 
					LPCTSTR lpRun = L"Software\\Microsoft\\Windows\\CurrentVersion\\Run"; 
					// hu 打开启动项Key 
					long lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE,lpRun,0,KEY_WRITE,&hKey);

					if (TRUE == Enable && ERROR_SUCCESS == lRet)        // hu 开机启动
					{
						// hu 添加注册
						RegSetValueEx(hKey,progNametmp,0,REG_SZ,(const BYTE*)progPathtmp,MAX_PATH);
						RegCloseKey(hKey);
					}
					else if (FALSE == Enable && ERROR_SUCCESS == lRet)  // hu 取消开机启动
					{
						// hu 删除注册
						RegDeleteValue(hKey,progNametmp);
						RegCloseKey(hKey);
					}
					break;
				}
			case ID_SYS_GetDevStatus:
				{
					int ii;
					DSPDriver dspDriver[DEVNUM_MAX];

					for (ii = 0;ii < DEVNUM_MAX;ii++)
					{
						dspDriver[ii].status = driver[ii].status;
						dspDriver[ii].memNum = driver[ii].memNum;
						memcpy(dspDriver[ii].memSize,driver[ii].memSize,10*sizeof(driver[ii].memSize[0]));
						memcpy(dspDriver[ii].memStatus,driver[ii].memStatus,10*sizeof(driver[ii].memStatus[0]));
					}

					memcpy((DSPDriver*)shareMem0.headAddr,dspDriver,DEVNUM_MAX*sizeof(dspDriver[0]));
#ifdef DEBUG_HU
					AllocConsole();
					::freopen("CONOUT$", "w", stdout); 
					for (int ii=0;ii<22;ii++)
					{
						printf("%d\n",*((int*)shareMem0.headAddr+ii));
					}
					Sleep(5000);
					FreeConsole();
#endif
					break;
				}
			case ID_SYS_ApplyForShareMem:
				{
					// hu 释放共享内存
					status0 = ::UnmapViewOfFile(shareMem0.headAddr);
					status1 = ::CloseHandle(shareMem0.hMap);
					status0 = status0 + status1;
					if (status0)   status0 = 0;
					else   
					{
						status0 = 1;
						if (trayShow.status)
						{
							MessageBox(hDriverWnd,L"释放共享内存失败！",L"警告",MB_OK|MB_ICONWARNING);
						}
					}

					shareMem0 = *((ShareMem*)((COPYDATASTRUCT*)lParam)->lpData);

					// hu 申请共享内存
					shareMem0.hMap = ::CreateFileMapping(INVALID_HANDLE_VALUE,NULL,PAGE_READWRITE,0,shareMem0.memSize,shareMem0.name);
					if (NULL == shareMem0.hMap)
					{
						status1 = 2;
						if (trayShow.status)
						{
							MessageBox(hDriverWnd,L"申请共享内存失败！",L"警告",MB_OK|MB_ICONWARNING);
						}
					}
					else
					{
						status1 = 0;
						shareMem0.headAddr = ::MapViewOfFile(shareMem0.hMap,FILE_MAP_ALL_ACCESS,0,0,0);
					}
					status0 = status0 + status1;
					memcpy((unsigned int*)shareMem0.headAddr,&status0,sizeof(status0));
#ifdef DEBUG_HU
					AllocConsole();
					::freopen("CONOUT$", "w", stdout); 
					printf("headAddr:%d\n",shareMem0.headAddr);
					printf("name:%s\n",shareMem0.name);
					printf("memSize:%d\n",shareMem0.memSize);
					Sleep(5000);
					FreeConsole();
#endif
					break;
				}
			case ID_SYS_DMAToDev:
				{
					DmaToDev *dma2dev = (DmaToDev *)((COPYDATASTRUCT*)lParam)->lpData;
					
					if (NULL == dma2DevFifoHead[dma2dev->DeviceX])
					{
						dma2DevFifoHead[dma2dev->DeviceX] = new DmaToDevFifo(dma2dev);
						SendMessage(hDriverWnd,WM_DMAFIFO,ID_FIFO_DMAToDev,dma2dev->DeviceX);
					}
					else if(1 == dma2dev->dmaFlag)     // hu 1--通过文件
					{
						DmaToDevFifo *dma2DevFifo = dma2DevFifoHead[dma2dev->DeviceX];
						while(dma2DevFifo->pNext)
						{
							dma2DevFifo = dma2DevFifo->pNext;
						}
						dma2DevFifo->pNext = new DmaToDevFifo(dma2dev);
					}
					break;
				}
			case ID_SYS_DMAFromDev:
				{
					DmaFromDev *dma2pc = (DmaFromDev *)((COPYDATASTRUCT*)lParam)->lpData;

					if (NULL == dma2PcFifoHead[dma2pc->DeviceX])
					{
						dma2PcFifoHead[dma2pc->DeviceX] = new DmaFromDevFifo(dma2pc);
						SendMessage(hDriverWnd,WM_DMAFIFO,ID_FIFO_DMAFromDev,dma2pc->DeviceX);
					}
					else if(1 == dma2pc->dmaFlag)     // hu 1--通过文件
					{
						DmaFromDevFifo *dma2PcFifo = dma2PcFifoHead[dma2pc->DeviceX];
						while(dma2PcFifo->pNext)
						{
							dma2PcFifo = dma2PcFifo->pNext;
						}
						dma2PcFifo->pNext = new DmaFromDevFifo(dma2pc);
					}
					break;
				}
			case ID_SYS_CreatTray:
				{
					trayShow.nid.cbSize = sizeof(trayShow.nid);
					trayShow.nid.hWnd = hDriverWnd;
				    trayShow.nid.uID = 0;
					trayShow.nid.uFlags = NIF_ICON|NIF_MESSAGE|NIF_TIP;
					trayShow.nid.uCallbackMessage = WM_TRAYMSG;
					trayShow.nid.hIcon = LoadIcon(NULL, IDI_APPLICATION);
					lstrcpy(trayShow.nid.szTip,L"驱动程序");
					Shell_NotifyIcon(NIM_ADD, &trayShow.nid);
					trayShow.hmenu=CreatePopupMenu();          // hu 生成菜单
					AppendMenu(trayShow.hmenu,MF_STRING,IDR_ENABLEAUTORUN,L"开机自启");  // hu 为菜单添加选项
					AppendMenu(trayShow.hmenu,MF_STRING,IDR_DISABLEAUTORUN,L"取消自启");
					AppendMenu(trayShow.hmenu,MF_STRING,IDR_QUITPROG,L"退出程序");
					break;
				}
			}
			break;
		}
	case WM_TRAYMSG:
		{
			if (WM_RBUTTONDOWN == lParam)
			{
				int trayMenu;  // hu 用于接收菜单选项返回值
				POINT pt;      // hu 用于接收鼠标坐标

				GetCursorPos(&pt);                     // hu 取鼠标坐标
				SetForegroundWindow(hDriverWnd);       // hu 解决在菜单外单击左键菜单不消失的问题
				//EnableMenuItem(trayShow.hmenu,IDR_DISABLEAUTORUN,MF_GRAYED);// hu 让菜单中的某一项变灰

				trayMenu = TrackPopupMenu(trayShow.hmenu,TPM_RETURNCMD,pt.x,pt.y,NULL,hDriverWnd,NULL);  // hu 显示菜单并获取选项ID
				if(IDR_ENABLEAUTORUN == trayMenu)
				{
					BOOL Enable = TRUE;
					COPYDATASTRUCT cds;
					cds.dwData = 0;
					cds.lpData = &Enable;
					cds.cbData = sizeof(Enable);
					SendMessage(hDriverWnd,WM_COPYDATA,ID_SYS_SetAutoRun,(LPARAM)&cds);
				}
				else if(IDR_DISABLEAUTORUN == trayMenu)
				{
					BOOL Enable = FALSE;
					COPYDATASTRUCT cds;
					cds.dwData = 0;
					cds.lpData = &Enable;
					cds.cbData = sizeof(Enable);
					SendMessage(hDriverWnd,WM_COPYDATA,ID_SYS_SetAutoRun,(LPARAM)&cds);
				}
				else if(IDR_QUITPROG ==trayMenu)
				{
					PostMessage(hDriverWnd,WM_DESTROY,0,0);
				}
			}
			break;
		}
	case WM_DMAFIFO:
		{
			switch (wParam)
			{
			case ID_FIFO_DMAToDev:
				{
					DmaToDev *dma2dev = &dma2DevFifoHead[lParam]->dma2Dev;
					dmaDest[lParam].dmaFlag = 0;

					if (NULL == dma2dev->hUserWndforEnd) dmaSource[lParam].hUserWndforEnd = hUserWnd;
					else                                 dmaSource[lParam].hUserWndforEnd = dma2dev->hUserWndforEnd;

					dmaSource[lParam].DeviceX = dma2dev->DeviceX;
					dmaSource[lParam].dmaFlag = dma2dev->dmaFlag;
					dmaSource[lParam].pPhyAddr = dma2dev->pPhyAddr;
					dmaSource[lParam].dataLen = dma2dev->dataLen;
					dmaSource[lParam].endFlag = dma2dev->endFlag;
					dmaSource[lParam].pBufAddr = (UINT*)driver[dmaSource[lParam].DeviceX].EDMAWinDriver_Device[dma2dev->memIdx];
					dmaSource[lParam].pPhyAddr.Write32 = driver[dmaSource[lParam].DeviceX].EDMAPhysicalAddress_Device[dma2dev->memIdx];
					dmaSource[lParam].memSize = driver[dmaSource[lParam].DeviceX].memSize[dma2dev->memIdx];
#ifdef DEBUG_HU
					AllocConsole();
					::freopen("CONOUT$", "w", stdout); 
					printf("hUserWndforEnd:%d\n",dmaSource[lParam].hUserWndforEnd);
					printf("DeviceX:%d\n",dmaSource[lParam].DeviceX);
					printf("pBufAddr:%d\n",dmaSource[lParam].pBufAddr);
					printf("pPhyAddr:%d\n",dmaSource[lParam].pPhyAddr.Write32);
					printf("dmaFlag:%d\n",dmaSource[lParam].dmaFlag);
					printf("dataLen:%d\n",dmaSource[lParam].dataLen.Write32);
					printf("memSize:%d\n",dmaSource[lParam].memSize);
					Sleep(5000);
					FreeConsole();
#endif
					if (1 == dmaSource[lParam].dmaFlag)     // hu 1--通过文件
					{
						dmaSource[lParam].dmaFp = _wfopen(dma2dev->binPath,L"rb");
#ifndef DEBUG_HU
						dmaSource[lParam].dataLen.Write32 = fread(dmaSource[lParam].pBufAddr,1,dmaSource[lParam].memSize,dmaSource[lParam].dmaFp);
						if (-99 != dmaSource[lParam].endFlag.DeviceX)
						{
							if (dmaSource[lParam].dataLen.Write32 < dmaSource[lParam].memSize)  
								// hu 传输结束标志位
								WriteToDSP(dmaSource[lParam].endFlag.DeviceX,dmaSource[lParam].endFlag.BarX,dmaSource[lParam].endFlag.dwOffset,
								dmaSource[lParam].endFlag.Write32,dmaSource[lParam].endFlag.RepeatTimes);
						}
#endif
					}
					else if(2 == dmaSource[lParam].dmaFlag) // hu 2--通过内存
					{
						dmaSource[lParam].pShareAddr = dma2dev->pBufAddr;
#ifndef DEBUG_HU
						memcpy(dmaSource[lParam].pBufAddr,dmaSource[lParam].pShareAddr,dmaSource[lParam].dataLen.Write32);
						if (-99 != dmaSource[lParam].endFlag.DeviceX)
							// hu 传输结束标志位
							WriteToDSP(dmaSource[lParam].endFlag.DeviceX,dmaSource[lParam].endFlag.BarX,dmaSource[lParam].endFlag.dwOffset,
							dmaSource[lParam].endFlag.Write32,dmaSource[lParam].endFlag.RepeatTimes);
#endif
					}
#ifdef DEBUG_HU
					dmaSource[lParam].dataLen.Write32 = 99;
					dmaSource[lParam].memSize = 100;
					PostMessage(hDriverWnd,WM_CALLBACKMESSAGE_0,3,dmaSource[lParam].DeviceX);
#else
					WriteToDSP(dmaSource[lParam].pPhyAddr.DeviceX,dmaSource[lParam].pPhyAddr.BarX,dmaSource[lParam].pPhyAddr.dwOffset,
						dmaSource[lParam].pPhyAddr.Write32,dmaSource[lParam].pPhyAddr.RepeatTimes);
					WriteToDSP(dmaSource[lParam].dataLen.DeviceX,dmaSource[lParam].dataLen.BarX,dmaSource[lParam].dataLen.dwOffset,
						dmaSource[lParam].dataLen.Write32,dmaSource[lParam].dataLen.RepeatTimes);
					EnableInterruptFromDSP(dmaSource[lParam].DeviceX);
					SendIntToDSP(dmaSource[lParam].DeviceX); // hu 向DSP发送中断
#endif
					break;
				}
			case ID_FIFO_DMAFromDev:
				{
					DmaFromDev *dma2pc = &dma2PcFifoHead[lParam]->dma2Pc;
					dmaSource[lParam].dmaFlag = 0;

					if (NULL == dma2pc->hUserWndforEnd) dmaDest[lParam].hUserWndforEnd = hUserWnd;
					else                                 dmaDest[lParam].hUserWndforEnd = dma2pc->hUserWndforEnd;

					dmaDest[lParam].DeviceX = dma2pc->DeviceX;
					dmaDest[lParam].dmaFlag = dma2pc->dmaFlag;
					dmaDest[lParam].pPhyAddr = dma2pc->pPhyAddr;
					dmaDest[lParam].dataLen = dma2pc->dataLen;
					dmaDest[lParam].endFlag = dma2pc->endFlag;
					dmaDest[lParam].pBufAddr = (UINT*)driver[dmaDest[lParam].DeviceX].EDMAWinDriver_Device[dma2pc->memIdx];
					dmaDest[lParam].pPhyAddr.Write32 = driver[dmaDest[lParam].DeviceX].EDMAPhysicalAddress_Device[dma2pc->memIdx];
					dmaDest[lParam].memSize = driver[dmaDest[lParam].DeviceX].memSize[dma2pc->memIdx];
#ifdef DEBUG_HU
					AllocConsole();
					::freopen("CONOUT$", "w", stdout); 
					printf("hUserWndforEnd:%d\n",dmaDest[lParam].hUserWndforEnd);
					printf("DeviceX:%d\n",dmaDest[lParam].DeviceX);
					printf("pBufAddr:%d\n",dmaDest[lParam].pBufAddr);
					printf("pPhyAddr:%d\n",dmaDest[lParam].pPhyAddr.Write32);
					printf("dmaFlag:%d\n",dmaDest[lParam].dmaFlag);
					printf("dataLen:%d\n",dmaDest[lParam].dataLen.Write32);
					printf("memSize:%d\n",dmaDest[lParam].memSize);
					Sleep(5000);
					FreeConsole();
#endif
					if (1 == dmaDest[lParam].dmaFlag)     // hu 1--通过文件
					{
						dmaDest[lParam].dmaFp = _wfopen(dma2pc->binPath,L"wb+");

						if (dmaDest[lParam].dataLen.Write32 > dmaDest[lParam].memSize)
						{
							dmaDest[lParam].dmaLen = dmaDest[lParam].memSize;
							dmaDest[lParam].dataLen.Write32 -= dmaDest[lParam].memSize;
						}
						else
						{
							dmaDest[lParam].dmaLen = dmaDest[lParam].dataLen.Write32;
							dmaDest[lParam].dataLen.Write32 = 0;
						}
					}
					else if(2 == dmaDest[lParam].dmaFlag) // hu 2--通过内存
					{	
						dmaDest[lParam].pShareAddr = dma2pc->pBufAddr;
						dmaDest[lParam].dmaLen = dmaDest[lParam].dataLen.Write32;
						dmaDest[lParam].dataLen.Write32 = 0;
					}
#ifdef DEBUG_HU
					dmaDest[lParam].dataLen.Write32 = 0;
					PostMessage(hDriverWnd,WM_CALLBACKMESSAGE_0,3,dmaDest[lParam].DeviceX);				
#else
					WriteToDSP(dmaDest[lParam].pPhyAddr.DeviceX,dmaDest[lParam].pPhyAddr.BarX,dmaDest[lParam].pPhyAddr.dwOffset,
						dmaDest[lParam].pPhyAddr.Write32,dmaDest[lParam].pPhyAddr.RepeatTimes);
					WriteToDSP(dmaDest[lParam].dataLen.DeviceX,dmaDest[lParam].dataLen.BarX,dmaDest[lParam].dataLen.dwOffset,
						dmaDest[lParam].dmaLen,dmaDest[lParam].dataLen.RepeatTimes);
					EnableInterruptFromDSP(dmaDest[lParam].DeviceX);
					SendIntToDSP(dmaDest[lParam].DeviceX); // hu 向DSP发送中断
#endif
					break;
				}
			}
			break;
		}
	case WM_CALLBACKMESSAGE_0:
		{
			if (0 == dmaSource[lParam].dmaFlag && 0 == dmaDest[lParam].dmaFlag)
			{
				DisEnableInterruptFromDSP(lParam);    // hu 屏蔽中断
				// hu ///////////////////////////////////////////////////////////////////////////

				PostMessage(hUserWnd,WM_CALLBACKMESSAGE_0,wParam,lParam);

				// hu ///////////////////////////////////////////////////////////////////////////
				EnableInterruptFromDSP(lParam);       // hu 开启中断
				break;
			}
			else if (0 != dmaSource[lParam].dmaFlag)
			{
				if(1 == dmaSource[lParam].dmaFlag)      // hu 1--通过文件
				{
					DisEnableInterruptFromDSP(lParam);    // hu 屏蔽中断
					// hu ///////////////////////////////////////////////////////////////////////////

					if (dmaSource[lParam].dataLen.Write32 == dmaSource[lParam].memSize)
					{
						dmaSource[lParam].dataLen.Write32 = fread(dmaSource[lParam].pBufAddr,1,dmaSource[lParam].memSize,dmaSource[lParam].dmaFp);
						if (-99 != dmaSource[lParam].endFlag.DeviceX)
						{
							if (dmaSource[lParam].dataLen.Write32 < dmaSource[lParam].memSize)  
								// hu 传输结束标志位
								WriteToDSP(dmaSource[lParam].endFlag.DeviceX,dmaSource[lParam].endFlag.BarX,dmaSource[lParam].endFlag.dwOffset,
								           dmaSource[lParam].endFlag.Write32,dmaSource[lParam].endFlag.RepeatTimes);
						}
						WriteToDSP(dmaSource[lParam].pPhyAddr.DeviceX,dmaSource[lParam].pPhyAddr.BarX,dmaSource[lParam].pPhyAddr.dwOffset,
							       dmaSource[lParam].pPhyAddr.Write32,dmaSource[lParam].pPhyAddr.RepeatTimes);
						WriteToDSP(dmaSource[lParam].dataLen.DeviceX,dmaSource[lParam].dataLen.BarX,dmaSource[lParam].dataLen.dwOffset,
							       dmaSource[lParam].dataLen.Write32,dmaSource[lParam].dataLen.RepeatTimes);
						EnableInterruptFromDSP(dmaSource[lParam].DeviceX);
						SendIntToDSP(dmaSource[lParam].DeviceX); // hu 向DSP发送中断
						break;
					}
					else
					{
						fclose(dmaSource[lParam].dmaFp);
						DmaToDevFifo *dma2DevFifo = dma2DevFifoHead[lParam];
						dma2DevFifoHead[lParam] = dma2DevFifoHead[lParam]->pNext;
						delete dma2DevFifo;

						PostMessage(dmaSource[lParam].hUserWndforEnd,WM_CALLBACKMESSAGE_0,wParam,lParam);
						memset(&dmaSource[lParam],0,sizeof(DmaSource));
						memset(&dmaDest[lParam],0,sizeof(DmaDest));

						if (dma2DevFifoHead[lParam])
						{
							SendMessage(hDriverWnd,WM_DMAFIFO,ID_FIFO_DMAToDev,lParam);
						}
					}

					// hu ///////////////////////////////////////////////////////////////////////////
					EnableInterruptFromDSP(lParam);       // hu 开启中断
					break;
				}
				else if(2 == dmaSource[lParam].dmaFlag) // hu 2--通过内存
				{
					DisEnableInterruptFromDSP(lParam);    // hu 屏蔽中断
					// hu ///////////////////////////////////////////////////////////////////////////
					delete dma2DevFifoHead[lParam];
					dma2DevFifoHead[lParam] = NULL;

					PostMessage(dmaSource[lParam].hUserWndforEnd,WM_CALLBACKMESSAGE_0,wParam,lParam);
					memset(&dmaSource[lParam],0,sizeof(DmaSource));
					memset(&dmaDest[lParam],0,sizeof(DmaDest));
					// hu ///////////////////////////////////////////////////////////////////////////
					EnableInterruptFromDSP(lParam);       // hu 开启中断
					break;
				}
			}
			else if (0 != dmaDest[lParam].dmaFlag)
			{
				if(1 == dmaDest[lParam].dmaFlag)      // hu 1--通过文件
				{
					DisEnableInterruptFromDSP(lParam);    // hu 屏蔽中断
					// hu ///////////////////////////////////////////////////////////////////////////

					fwrite(dmaDest[lParam].pBufAddr,1,dmaDest[lParam].dmaLen,dmaDest[lParam].dmaFp);
					if (0 != dmaDest[lParam].dataLen.Write32)
					{
						if (-99 != dmaDest[lParam].endFlag.DeviceX)
						{
							UINT endflag;
							ReadfromDSP(dmaDest[lParam].endFlag.DeviceX,dmaDest[lParam].endFlag.BarX,dmaDest[lParam].endFlag.dwOffset,&endflag);
							if (endflag == dmaDest[lParam].endFlag.Read32)
							{
								fclose(dmaDest[lParam].dmaFp);

								DmaFromDevFifo *dma2PcFifo = dma2PcFifoHead[lParam];
								dma2PcFifoHead[lParam] = dma2PcFifoHead[lParam]->pNext;
								delete dma2PcFifo;

								PostMessage(dmaDest[lParam].hUserWndforEnd,WM_CALLBACKMESSAGE_0,wParam,lParam);
								memset(&dmaSource[lParam],0,sizeof(DmaSource));
								memset(&dmaDest[lParam],0,sizeof(DmaDest));

								if (dma2PcFifoHead[lParam])
								{
									SendMessage(hDriverWnd,WM_DMAFIFO,ID_FIFO_DMAFromDev,lParam);
								}
								EnableInterruptFromDSP(lParam);
								break;
							}
						}
						if (dmaDest[lParam].dataLen.Write32 > dmaDest[lParam].memSize)
						{
							dmaDest[lParam].dmaLen = dmaDest[lParam].memSize;
							dmaDest[lParam].dataLen.Write32 -= dmaDest[lParam].memSize;
						}
						else
						{
							dmaDest[lParam].dmaLen = dmaDest[lParam].dataLen.Write32;
							dmaDest[lParam].dataLen.Write32 = 0;
						}
						WriteToDSP(dmaDest[lParam].pPhyAddr.DeviceX,dmaDest[lParam].pPhyAddr.BarX,dmaDest[lParam].pPhyAddr.dwOffset,
							       dmaDest[lParam].pPhyAddr.Write32,dmaDest[lParam].pPhyAddr.RepeatTimes);
						WriteToDSP(dmaDest[lParam].dataLen.DeviceX,dmaDest[lParam].dataLen.BarX,dmaDest[lParam].dataLen.dwOffset,
							       dmaDest[lParam].dmaLen,dmaDest[lParam].dataLen.RepeatTimes);
						EnableInterruptFromDSP(dmaDest[lParam].DeviceX);
						SendIntToDSP(dmaDest[lParam].DeviceX); // hu 向DSP发送中断
						break;
					}
					else
					{
						fclose(dmaDest[lParam].dmaFp);

						DmaFromDevFifo *dma2PcFifo = dma2PcFifoHead[lParam];
						dma2PcFifoHead[lParam] = dma2PcFifoHead[lParam]->pNext;
						delete dma2PcFifo;

						PostMessage(dmaDest[lParam].hUserWndforEnd,WM_CALLBACKMESSAGE_0,wParam,lParam);
						memset(&dmaSource[lParam],0,sizeof(DmaSource));
						memset(&dmaDest[lParam],0,sizeof(DmaDest));

						if (dma2PcFifoHead[lParam])
						{
							SendMessage(hDriverWnd,WM_DMAFIFO,ID_FIFO_DMAFromDev,lParam);
						}
					}

					// hu ///////////////////////////////////////////////////////////////////////////
					EnableInterruptFromDSP(lParam);       // hu 开启中断
					break;
				}
				else if(2 == dmaDest[lParam].dmaFlag) // hu 2--通过内存
				{
					DisEnableInterruptFromDSP(lParam);    // hu 屏蔽中断
					// hu ///////////////////////////////////////////////////////////////////////////
					delete dma2PcFifoHead[lParam];
					dma2PcFifoHead[lParam] = NULL;

					memcpy(dmaDest[lParam].pShareAddr,dmaDest[lParam].pBufAddr,dmaDest[lParam].dmaLen);
					PostMessage(dmaDest[lParam].hUserWndforEnd,WM_CALLBACKMESSAGE_0,wParam,lParam);
					memset(&dmaSource[lParam],0,sizeof(DmaSource));
					memset(&dmaDest[lParam],0,sizeof(DmaDest));

					// hu ///////////////////////////////////////////////////////////////////////////
					EnableInterruptFromDSP(lParam);       // hu 开启中断
					break;
				}
			}
			break;
		}
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		{
			FILE *InfoFp;
			wchar_t filePath[MAX_PATH];
			int ii,jj;
			wchar_t strError[100];

			wcscpy(filePath,progPath);
			wcscat(filePath,L"PciDriverInfo.txt");
			InfoFp = _wfopen(filePath,L"at+");
			// hu 释放共享内存
			status0 = ::UnmapViewOfFile(shareMem0.headAddr);
			status1 = ::CloseHandle(shareMem0.hMap);
			if (0 == status0+status1 && trayShow.status)            
			{
				MessageBox(hDriverWnd,L"释放共享内存失败！",L"警告",MB_OK|MB_ICONWARNING);
			}

			for (ii = 0;ii < driverNum;ii++)
			{
				fwprintf(InfoFp,L"driver%d:\n",ii);
				if (0 == driver[ii].status)
				{
					// hu 释放物理空间
					for (jj = 0;jj < driver[ii].memNum;jj++)
					{
						if (driver[ii].memStatus[jj])
						{
							driver[ii].memStatus[jj] = 3;
						}
						else
						{
#ifdef DEBUG_HU
							driver[ii].memStatus[jj] = 99;
#else
							driver[ii].memStatus[jj] = FreePA(ii,driver[ii].EDMAHandle_Device[jj]);
#endif
						}

						switch(driver[ii].memStatus[jj])
						{
						case 0:fwprintf(InfoFp,L"Succeed:释放连续物理空间成功！\n");break;
						case 1:
							{
								fwprintf(InfoFp,L"Error:I/O缓存与物理空间同步失败！\n");
								_stprintf_s(strError,L"driver%d：I/O缓存与物理空间同步失败！",ii);
								if (trayShow.status)
								{
									MessageBox(hDriverWnd,strError,L"警告",MB_OK|MB_ICONWARNING);
								}
								break;
							}
						case 2:
							{
								fwprintf(InfoFp,L"Error:释放物理空间失败！\n");
								_stprintf_s(strError,L"driver%d：释放物理空间失败！",ii);
								if (trayShow.status)
								{
									MessageBox(hDriverWnd,strError,L"警告",MB_OK|MB_ICONWARNING);
								}
								break;
							}
						case 3:
							{
								fwprintf(InfoFp,L"Error:物理空间状态出错,未释放该物理空间！\n");
								_stprintf_s(strError,L"driver%d：物理空间状态出错,未释放该物理空间！",ii);
								if (trayShow.status)
								{
									MessageBox(hDriverWnd,strError,L"警告",MB_OK|MB_ICONWARNING);
								}
								break;
							}
						default:
							{
								fwprintf(InfoFp,L"Error:不是释放物理空间函数的默认返回值！\n");
								_stprintf_s(strError,L"driver%d：不是释放物理空间函数的默认返回值！",ii);
								if (trayShow.status)
								{
									MessageBox(hDriverWnd,strError,L"警告",MB_OK|MB_ICONWARNING);
								}
								break;
							}
						}
					}

					// hu 关闭设备
#ifdef DEBUG_HU
					driver[ii].status = 99;
#else
					driver[ii].status = Close_DSPDriver(ii);
#endif
					switch(driver[ii].status)
					{
					case 0:fwprintf(InfoFp,L"Succeed:关闭PCI设备成功！\n");break;
					case 1:
						{
							fwprintf(InfoFp,L"Error:关闭PCI设备失败！\n");
							_stprintf_s(strError,L"driver%d：关闭PCI设备失败！",ii);
							if (trayShow.status)
							{
								MessageBox(hDriverWnd,strError,L"警告",MB_OK|MB_ICONWARNING);
							}
							break;
						}
					case 2:
						{
							fwprintf(InfoFp,L"Error:卸载设备驱动库失败！\n");
							_stprintf_s(strError,L"driver%d：卸载设备驱动库失败！",ii);
							if (trayShow.status)
							{
								MessageBox(hDriverWnd,strError,L"警告",MB_OK|MB_ICONWARNING);
							}
							break;
						}
					case 3:
						{
							fwprintf(InfoFp,L"Error:关闭PCI驱动与卸载设备驱动库失败！\n");
							_stprintf_s(strError,L"driver%d：关闭PCI驱动与卸载设备驱动库失败！",ii);
							if (trayShow.status)
							{
								MessageBox(hDriverWnd,strError,L"警告",MB_OK|MB_ICONWARNING);
							}
							break;
						}
					default:
						{
							fwprintf(InfoFp,L"Error:不是关闭驱动函数的默认返回值！\n");
							_stprintf_s(strError,L"driver%d：不是关闭驱动函数的默认返回值！",ii);
							if (trayShow.status)
							{
								MessageBox(hDriverWnd,strError,L"警告",MB_OK|MB_ICONWARNING);
							}
							break;
						}
					}
				}
				else
				{
					fwprintf(InfoFp,L"Error:driver%d当前状态出错,未释放物理空间,未关闭设备！\n",ii);
					_stprintf_s(strError,L"driver%d：当前状态出错,未释放物理空间,未关闭设备！",ii);
					if (trayShow.status)
					{
						MessageBox(hDriverWnd,strError,L"警告",MB_OK|MB_ICONWARNING);
					}
				}
			}
			fclose(InfoFp);

			// hu 销毁托盘
			if (trayShow.status)
			{
				Shell_NotifyIcon(NIM_DELETE,&trayShow.nid);
			}

			PostQuitMessage(0);
			break;
		}
	default:
		{
			if (trayShow.WM_TASKBARCREATED == message && trayShow.status)
			{
				SendMessage(hDriverWnd,WM_COPYDATA,ID_SYS_CreatTray,0);
			    break;
			}
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	return 0;
}