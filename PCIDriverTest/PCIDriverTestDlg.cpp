// PCIDriverTestDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PCIDriverTest.h"
#include "PCIDriverTestDlg.h"
#include "DialogFPGA.h"
#include "PciDriverFun.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

int CPCIDriverTestDlg::deviceX = 0;
int CPCIDriverTestDlg::deviceNum = 0;

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

// CPCIDriverTestDlg 对话框




CPCIDriverTestDlg::CPCIDriverTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPCIDriverTestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPCIDriverTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BAR_WR, Bar_wr);
	DDX_Control(pDX, IDC_LIST_DATA, dataList);
}

BEGIN_MESSAGE_MAP(CPCIDriverTestDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
//	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_CALLBACKMESSAGE_0, OnCALLBACKMESSAGE_0)    // hu 添加消息映射处理
	ON_BN_CLICKED(IDOK, &CPCIDriverTestDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_SELECT_BIN, &CPCIDriverTestDlg::OnBnClickedSelectBin)
	ON_BN_CLICKED(IDC_SEND_DATA, &CPCIDriverTestDlg::OnBnClickedSendData)
	ON_BN_CLICKED(IDC_FPGA_INI, &CPCIDriverTestDlg::OnBnClickedFpgaIni)
	ON_NOTIFY(TCN_SELCHANGE, IDC_BAR_WR, &CPCIDriverTestDlg::OnTcnSelchangeBarWr)
	ON_EN_CHANGE(IDC_SHOW_BIN, &CPCIDriverTestDlg::OnEnChangeShowBin)
	ON_EN_CHANGE(IDC_EDIT_CREATEID, &CPCIDriverTestDlg::OnEnChangeFpgaAddr) // hu
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()
	ON_WM_DRAWITEM()
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_DATA, &CPCIDriverTestDlg::OnNMDblclkListData)
	ON_NOTIFY(NM_CLICK, IDC_LIST_DATA, &CPCIDriverTestDlg::OnNMClickListData)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_DATA, &CPCIDriverTestDlg::OnNMRclickListData)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_DATA, &CPCIDriverTestDlg::OnLvnItemchangedListData)
	ON_CBN_SELCHANGE(IDC_DEVICE_SELECT, &CPCIDriverTestDlg::OnCbnSelchangeDeviceSelect)
	ON_COMMAND(ID_LIST_ADD, &CPCIDriverTestDlg::OnListAdd)
	ON_COMMAND(ID_LIST_DELETE, &CPCIDriverTestDlg::OnListDelete)
	ON_COMMAND(ID_LIST_SAVE, &CPCIDriverTestDlg::OnListSave)
	ON_COMMAND(ID_LIST_USE, &CPCIDriverTestDlg::OnListUse)
END_MESSAGE_MAP()


// CPCIDriverTestDlg 消息处理程序

BOOL CPCIDriverTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
#ifndef XP_HU
	// hu 窗口渐变
	MainDlgFlash = new CDlgFlash(this,9,10,11,12);  // hu 定时器ID为9,10,11,12
	MainDlgFlash->SetFlashCreateSpeed(1,5);
	MainDlgFlash->SetFlashDestroySpeed(5,3);
	MainDlgFlash->SetFlashCR2NCRSpeed(12);
	MainDlgFlash->StartFlashCR2NCRDlg();
#endif

	// hu 创建tab子界面
	Bar_wr.InsertItem(0,_T("FPGA"));  
	Bar_wr.InsertItem(1,_T("DSP")); 
	Bar_FPGA.Create(IDD_DIALOG_FPGA,GetDlgItem(IDC_BAR_WR));
	Bar_DSP.Create(IDD_DIALOG_DSP,GetDlgItem(IDC_BAR_WR));

	// hu 获得客户区大小  
	CRect rs;  
	Bar_wr.GetClientRect(&rs);

	// hu 调整子对话框在父窗口中的位置，根据实际修改  
	rs.top+=25;  
	rs.bottom-=1;  
	rs.left+=1;  
	rs.right-=1;

	// hu 设置子对话框尺寸并移动到指定位置  
	Bar_FPGA.MoveWindow(&rs);  
	Bar_DSP.MoveWindow(&rs);

	// hu 分别设置隐藏和显示  
	Bar_FPGA.ShowWindow(true);  
	Bar_DSP.ShowWindow(false);  

	// hu 设置默认的选项卡  
	Bar_wr.SetCurSel(0);

	// hu FPGA地址框设置
	((CEdit*)Bar_FPGA.GetDlgItem(IDC_FPGA_STARTADDR))->SetLimitText(7); // hu 字数限制为7个字节
	((CEdit*)Bar_FPGA.GetDlgItem(IDC_FPGA_ENDADDR))->SetLimitText(7);
	((CEdit*)Bar_FPGA.GetDlgItem(IDC_REPEAT_FPGADATA))->SetLimitText(2); // hu 最多重复写入99次

	Bar_FPGA.GetDlgItem(IDC_FPGA_STARTADDR)->SetWindowText(_T("0x")); // hu 在enchange消息中初始化
	Bar_FPGA.GetDlgItem(IDC_DATA_FPGA)->SetWindowText(_T("1"));
	Bar_FPGA.GetDlgItem(IDC_REPEAT_FPGADATA)->SetWindowText(_T("1"));
	//Bar_FPGA.GetDlgItem(IDC_FPGA_ENDADDR)->SetWindowText(_T("0x"));
	/*FileBinColor.CreateSolidBrush(RGB(255,255,0)); // hu 改变编辑框背景颜色*/

	// hu DSP地址框设置
	((CEdit*)Bar_DSP.GetDlgItem(IDC_DSP_OFFSADDR))->SetLimitText(10); // hu 字数限制为7个字节

	Bar_DSP.GetDlgItem(IDC_DSP_OFFSADDR)->SetWindowText(_T("0x0"));
	Bar_DSP.GetDlgItem(IDC_DATAW_DSP)->SetWindowText(_T("1"));

	// hu 设置鼠标悬停提示
	EnableToolTips(TRUE);
	Bubble_Tip.Create(this);
	Bubble_Tip.Activate(TRUE);
	Bubble_Tip.SetMaxTipWidth(600);         // hu 设置气泡提示最大宽度，设置后可实现多行显示
	Bar_FPGA.Bubble_Tip_FPGA.Create(this);
	Bar_FPGA.Bubble_Tip_FPGA.Activate(TRUE);
	Bar_FPGA.Bubble_Tip_FPGA.SetMaxTipWidth(600);  
	Bar_DSP.Bubble_Tip_DSP.Create(this);
	Bar_DSP.Bubble_Tip_DSP.Activate(TRUE);
	Bar_DSP.Bubble_Tip_DSP.SetMaxTipWidth(600); 
	
	// hu 初始化单选框
	((CButton *)Bar_FPGA.GetDlgItem(IDC_FIX_SELECT))->SetCheck(TRUE);
	((CButton *)Bar_DSP.GetDlgItem(IDC_DDR_SELECT))->SetCheck(TRUE);

	// hu 初始化进度条
	Bar_FPGA.Progress_FPGA.SetRange(0,100);  // hu 设置进度条的范围
	Bar_FPGA.Progress_FPGA.SetStep(10);       // hu 设置进度条的每一步的增量
	Bar_FPGA.Progress_FPGA.SetPos(0);        // hu 设置进度条的当前位置

	// hu 初始化列表视图
	CRect rect;
	haveeditcreate = false;//初始化标志位，表示还没有创建编辑框
	// hu 获取编程语言列表视图控件的位置和大小   
	dataList.GetClientRect(&rect);
	dataList.SetExtendedStyle(dataList.GetExtendedStyle()|LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_CHECKBOXES); 
	// hu 为列表视图控件添加三列   
	dataList.InsertColumn(0,_T(""),LVCFMT_LEFT,25,0);
	dataList.InsertColumn(1,_T("物理地址"),LVCFMT_CENTER,rect.Width()/3+4,1); 
	dataList.InsertColumn(2,_T("选中"),LVCFMT_CENTER,rect.Width()/3-24,2);  
	dataList.InsertColumn(3,_T("取消"),LVCFMT_CENTER,rect.Width()/3-24,3); 
	// hu 在列表视图控件中插入列表项，并设置列表子项文本
	dataList.InsertItem(0,L"");
	dataList.SetItemText(0,1,_T("0x"));   
	dataList.SetItemText(0,2,_T("1"));   
	dataList.SetItemText(0,3,_T("0"));  
	dataList.SetTextBkColor(RGB(152,251,152)); 
	dataList.SetBkColor(RGB(152,251,152));  

	// hu 初始化PCI6455
	DWORD status;

#ifdef DEBUG_HU
	status = 99;
#else
	status = InitAllDSPDriver(&dspDriver[0]);
#endif
	if (status)   GetDlgItem(IDC_SHOW)->SetWindowText(GetLastDriverError());
	else          GetDlgItem(IDC_SHOW)->SetWindowText(_T("初始化驱动程序成功！"));

	if (0 == status)
	{
		CString temp;
		int ii,jj;

		for (ii=0;ii<DEVNUM_MAX;ii++)
		{
			GetDlgItem(IDC_SHOW)->GetWindowText(temp);
			temp.Format(_T("%s\r\nPCI设备%d:\r\n%s"),temp,ii,DriverStatus2Str(dspDriver[ii].status));
			GetDlgItem(IDC_SHOW)->SetWindowText(temp);

			if (!dspDriver[ii].status)  deviceNum = ii + 1;

			for (jj=0;jj<dspDriver[ii].memNum;jj++)
			{
				GetDlgItem(IDC_SHOW)->GetWindowText(temp);
				temp.Format(_T("%s\r\ndmaMem%d:%s"),temp,jj,DriverMemStatus2Str(dspDriver[ii].memStatus[jj],dspDriver[ii].memSize[jj]));
				GetDlgItem(IDC_SHOW)->SetWindowText(temp);
			}
		}

		// hu 初始化组合框
		if (0 == deviceNum)
		{
			((CComboBox *)GetDlgItem(IDC_DEVICE_SELECT))->AddString(L"无设备");
		}
		else
		{
			for (ii=0;ii<deviceNum;ii++)
			{
				temp.Format(L"设备%d",ii);
				((CComboBox *)GetDlgItem(IDC_DEVICE_SELECT))->AddString(temp);
			}
		}
		((CComboBox *)GetDlgItem(IDC_DEVICE_SELECT))->SetCurSel(deviceX);

		// hu 注册消息函数
		m_hWnd = AfxGetMainWnd()->m_hWnd;
		RegistHWNDToDev(m_hWnd);
		SetDSPAutoRun(TRUE);
	}
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CPCIDriverTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CPCIDriverTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
//
//HCURSOR CPCIDriverTestDlg::OnQueryDragIcon()
//{
//	return static_cast<HCURSOR>(m_hIcon);
//}


void CPCIDriverTestDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
#ifdef XP_HU
	DestroyWindow();
#else
	MainDlgFlash->SetFlashNCR2CRSpeed(12);
	MainDlgFlash->StartFlashNCR2CRDlg();
	MainDlgFlash->StartFlashDestroyDlg(); //开始动画关闭对话框
#endif
	//OnOK();
}

// hu 响应dll上传的PostMessage()函数
LRESULT CPCIDriverTestDlg::OnCALLBACKMESSAGE_0(WPARAM wParam, LPARAM lParam)//hu wParam为中断号
{
	CString temp;
	if(wParam == 0) 
	{
		// hu ///////////添加0号中断处理程序///////////////	
		return 0; 
		// hu ///////////////////////////////////////////// 
	}
	else if(wParam == 1)
	{
		// hu ///////////添加1号中断处理程序///////////////
		return 0; 
		// hu /////////////////////////////////////////////
	}
	else if(wParam == 2)  
	{
		// hu ///////////添加2号中断处理程序///////////////
		UINT GPIO2FPGA_flag;

		dmaSource0.Deliver_Time = GetTickCount() - dmaSource0.Deliver_Time;  // hu 所花时间

#ifdef DEBUG_HU
		GPIO2FPGA_flag = 99;
#else
		ReadFromDSP(deviceX,5,0x14,&GPIO2FPGA_flag);
#endif
		GetDlgItem(IDC_SHOW)->GetWindowText(temp);
		temp.Format(L"%s\r\n设备%d:\r\n%s",temp,lParam,IniFpgaFlag2Str(GPIO2FPGA_flag));
		GetDlgItem(IDC_SHOW)->SetWindowText(temp);

		Bar_FPGA.Progress_FPGA.SetPos(100);
		temp.Format(_T("%.1f"),dmaSource0.Deliver_Time/1000.0);
		GetDlgItem(IDC_SHOW_TIME)->SetWindowText(temp);
		return 0; 
		// hu /////////////////////////////////////////////
	}
	else if(wParam == 3)  
	{
		// hu ///////////添加3号中断处理程序///////////////
		dmaSource0.Deliver_Time = GetTickCount() - dmaSource0.Deliver_Time;  // hu 所花时间

		WriteToDSP(deviceX,3,0xa0a00*addrOffset4FPGA,0xf0f0);// hu 通知fpga传输完毕 0xa0a00为fpga读取数据地址

		temp.Format(_T("%.1f"),dmaSource0.Deliver_Time/1000.0);
		GetDlgItem(IDC_SHOW_TIME)->SetWindowText(temp);

		GetDlgItem(IDC_SHOW)->GetWindowText(temp);
		temp.Format(L"%s\r\n设备%d:\r\nEDMA传输成功！",temp,lParam);
		GetDlgItem(IDC_SHOW)->SetWindowText(temp);
		// hu /////////////////////////////////////////////
	}		

	return 0; 
}

void CPCIDriverTestDlg::OnBnClickedSelectBin()
{
	// TODO: Add your control notification handler code here
	CFileDialog FileDlg(TRUE,NULL,NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,_T("文件(*.bin;*.bit)|*.bin;*.bit||"));

	FileDlg.m_ofn.lpstrInitialDir = _T(".\\files");//改初始化目录

	if (FileDlg.DoModal() == IDOK)
	{
		CString path;
		path = FileDlg.GetPathName();
		GetDlgItem(IDC_SHOW_BIN)->SetWindowText(path);
	}
	GetDlgItem(IDC_SHOW_BIN)->ShowWindow(TRUE);	
}

void CPCIDriverTestDlg::OnBnClickedSendData()
{
	// TODO: Add your control notification handler code here
	CString path,temp;

	WriteToDev pPhyAddr = {deviceX,5,0,0,1},     // hu 物理地址
		       dataLen  = {deviceX,5,4,0,1},     // hu 物理空间中数据的长度
		       endFlag  = {deviceX,5,0x10,1,1};  // hu 传输结束标志位

	GetDlgItem(IDC_SHOW_BIN)->GetWindowText(path);
	if (!dmaSource0.Deliver_FP.Open(path,CFile::typeBinary|CFile::modeRead))
	{
		AfxMessageBox(_T("请输入正确的文件路径名!"));
		return;
	}
	dmaSource0.Deliver_FP.Close();

	GetDlgItem(IDC_SHOW_TIME)->SetWindowText(_T(""));
	dmaSource0.Deliver_Time = GetTickCount();// hu 开始时间

	temp.Format(L"设备%d:\r\n开始EDMA传输...",deviceX);
	GetDlgItem(IDC_SHOW)->SetWindowText(temp);

#ifdef DEBUG_HU
	PostMessage(WM_CALLBACKMESSAGE_0,3,deviceX);
#else
	DmaFileToDSP(deviceX,0,path.GetBuffer(0),pPhyAddr,dataLen,&endFlag);
#endif
}

void CPCIDriverTestDlg::OnBnClickedFpgaIni()
{
	// TODO: Add your control notification handler code here
	CString path,temp;
	UINT binLen;
	WriteToDev pPhyAddr = {deviceX,5,8,0,1},     // hu 物理地址
		       dataLen  = {deviceX,5,0xc,0,1},   // hu 物理空间中数据的长度
	           endFlag  = {deviceX,5,0x10,1,1};  // hu 传输结束标志位

	GetDlgItem(IDC_SHOW_BIN)->GetWindowText(path);
	if (!dmaSource0.Deliver_FP.Open(path,CFile::typeBinary|CFile::modeRead))
	{
		AfxMessageBox(_T("请输入正确的文件路径名!"));
		return;
	}
	binLen = dmaSource0.Deliver_FP.GetLength();
	dmaSource0.Deliver_FP.Close();
	if (binLen >= 11*1024*1024)
	{
		AfxMessageBox(_T("加载文件不能大于11MB!"));
		return;
	}

#ifdef DEBUG_HU
	PostMessage(WM_CALLBACKMESSAGE_0,2,deviceX);
#else
	DmaFileToDSP(deviceX,0,path.GetBuffer(0),pPhyAddr,dataLen,&endFlag);
#endif
	GetDlgItem(IDC_SHOW_TIME)->SetWindowText(_T(""));
	temp.Format(L"设备%d:\r\n开始动态加载...",deviceX);
	GetDlgItem(IDC_SHOW)->SetWindowText(temp);

	dmaSource0.Deliver_Time = GetTickCount();// hu 开始时间
	SetTimer(1,500,NULL);// hu 启动定时器1,定时时间是0.5秒
}

void CPCIDriverTestDlg::OnTcnSelchangeBarWr(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	int CurSel = Bar_wr.GetCurSel();  
	switch(CurSel)  
	{  
	case 0:  
		{
			Bar_FPGA.ShowWindow(true);
			Bar_DSP.ShowWindow(false);
			break;  
		}
	case 1: 
		{
			Bar_FPGA.ShowWindow(false);  
			Bar_DSP.ShowWindow(true);  
			break;
		}
	}

	*pResult = 0;
}


void CPCIDriverTestDlg::OnEnChangeShowBin()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	CWnd* pWnd=GetDlgItem(IDC_SHOW_BIN);	// hu 获取文本控件窗口指针
	pWnd->Invalidate();          // hu 让pWnd所指向的窗口重画
}

void CPCIDriverTestDlg::OnEnChangeFpgaAddr()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	if (1 == e_SubItem)
	{
		CString temp,temp1;
		int len;
		m_Edit.GetWindowText(temp);

		if (temp.Find(_T("0x"))) // hu 0x必须存在且在最开始
		{
			m_Edit.SetWindowText(_T("0x"));
			m_Edit.SetSel(-1);  // hu 将光标设置到最后
		}

		// hu 只能输入16进制数
		len = temp.GetLength();  // hu 获得字符串长度
		temp1 = temp.Right(1);   // hu 提取最后一个字符
		temp1.MakeLower();       // hu 转化为小写字母
		if ((len>2) && !((temp1>=_T("0")&&temp1<=_T("9"))||(temp1>=_T("a")&&temp1<=_T("f"))))
		{
			temp.Delete(len-1,1);
			m_Edit.SetWindowText(temp);
			m_Edit.SetSel(-1);  // hu 将光标设置到最后
		}
	}
}

HBRUSH CPCIDriverTestDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Change any attributes of the DC here
	if(nCtlColor==CTLCOLOR_EDIT && pWnd->GetDlgCtrlID()==IDC_SHOW_BIN)//注意此处的（pWnd->），否则没效果
	{
		CString path;
		CFileStatus temp;
		GetDlgItem(IDC_SHOW_BIN)->GetWindowText(path);
		if (dmaSource0.Deliver_FP.GetStatus(path,temp))
		{
			pDC->SetTextColor(RGB(0,0,0));   // hu 若文件存在，将字设为黑色
		}
		else
		{
			pDC->SetTextColor(RGB(255,0,0)); // hu 若文件不存在，将字设为红色
		}
		//pDC->SetBkColor(RGB(255,255,255));// hu 设置文本背景色
		//pDC->SetBkMode(TRANSPARENT);// hu 设置背景透明
		//hbr = (HBRUSH)FileBinColor;
	}
    
	//if(nCtlColor==CTLCOLOR_BTN)          // hu 更改按钮颜色，会出现字体不显示的问题
	//{
	//	//pDC->SetBkMode(TRANSPARENT);
	//	//pDC->SetTextColor(RGB(0,0,0));
	//	//pDC->SetBkColor(RGB(121,121,255));   
	//	HBRUSH b=CreateSolidBrush(RGB(118,238,198));
	//	return b;
	//}

	if(nCtlColor==CTLCOLOR_EDIT)   // hu 更改编辑框
	{
		//pDC->SetBkMode(TRANSPARENT);
		//pDC->SetTextColor(RGB(0,0,0));
		pDC->SetBkColor(RGB(152,251,152));
		HBRUSH b=CreateSolidBrush(RGB(152,251,152));
		return b;
	}
	else if(nCtlColor==CTLCOLOR_STATIC)  // hu 更改静态文本
	{
		//pDC->SetTextColor(RGB(0,0,0));
		pDC->SetBkColor(RGB(84,255,159));
		HBRUSH b=CreateSolidBrush(RGB(84,255,159));
		return b;
	}
	else if(nCtlColor==CTLCOLOR_DLG)   // hu 更改对话框背景色
	{
		//pDC->SetTextColor(RGB(0,0,0));
		//pDC->SetBkColor(RGB(166,254,1));
		HBRUSH b=CreateSolidBrush(RGB(84,255,159));
		return b;
	}
	else if(nCtlColor==CTLCOLOR_SCROLLBAR)  // hu 滚动条
	{
		//pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(0,0,0));
		pDC->SetBkColor(RGB(233,233,220));
		HBRUSH b=CreateSolidBrush(RGB(233,233,220));
		return b;
	}

	// TODO:  Return a different brush if the default is not desired
	return hbr;
}

BOOL CPCIDriverTestDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if(pMsg->message == WM_MOUSEMOVE)  // hu 显示气泡提示
	{
		if (pMsg->hwnd == GetDlgItem(IDC_SHOW_BIN)->m_hWnd)
		{
			CString tmp;
			GetDlgItem(IDC_SHOW_BIN)->GetWindowText(tmp);
			if (!dmaSource0.Deliver_FP.Open(tmp,CFile::typeBinary|CFile::modeRead))
			{
				Bubble_Tip.AddTool(GetDlgItem(IDC_SHOW_BIN),_T("写入二进制文件路径"));//为此控件添加tip
				Bubble_Tip.RelayEvent(pMsg);
			}
			else
			{
				int dataLen = dmaSource0.Deliver_FP.GetLength();
				dmaSource0.Deliver_FP.Close();
				tmp.Format(L"文件大小为%dBytes",dataLen);
				Bubble_Tip.AddTool(GetDlgItem(IDC_SHOW_BIN),tmp);//为此控件添加tip
				Bubble_Tip.RelayEvent(pMsg);
			}
		}
// 		else if (pMsg->hwnd == GetDlgItem(IDC_LIST_DATA)->m_hWnd)
// 		{
// 			Bubble_Tip.AddTool(GetDlgItem(IDC_LIST_DATA),_T("单击：编辑\r\n双击：添加\r\n右击：删除"));//为此控件添加tip
// 			Bubble_Tip.RelayEvent(pMsg);
// 		}
		else if (pMsg->hwnd == GetDlgItem(IDC_SHOW_TIME)->m_hWnd)
		{
			Bubble_Tip.AddTool(GetDlgItem(IDC_SHOW_TIME),_T("显示文件写入底层FPGA所花时间"));//为此控件添加tip
			Bubble_Tip.RelayEvent(pMsg);
		}
		else if (pMsg->hwnd == GetDlgItem(IDC_SELECT_BIN)->m_hWnd)
		{
			Bubble_Tip.AddTool(GetDlgItem(IDC_SELECT_BIN),_T("选择二进制(.bin)文件或FPGA(.bit)文件"));//为此控件添加tip
			Bubble_Tip.RelayEvent(pMsg);
		}
		else if (pMsg->hwnd == GetDlgItem(IDC_SEND_DATA)->m_hWnd)
		{
			Bubble_Tip.AddTool(GetDlgItem(IDC_SEND_DATA),_T("向底层FPGA写入二进制(.bin)文件"));//为此控件添加tip
			Bubble_Tip.RelayEvent(pMsg);
		}
		else if (pMsg->hwnd == GetDlgItem(IDC_FPGA_INI)->m_hWnd)
		{
			Bubble_Tip.AddTool(GetDlgItem(IDC_FPGA_INI),_T("用FPGA(.bit)文件初始化底层FPGA"));//为此控件添加tip
			Bubble_Tip.RelayEvent(pMsg);
		}
		else if (pMsg->hwnd == GetDlgItem(IDC_SHOW)->m_hWnd)
		{
			Bubble_Tip.AddTool(GetDlgItem(IDC_SHOW),_T("显示系统状态"));//为此控件添加tip
			Bubble_Tip.RelayEvent(pMsg);
		}
		else if (pMsg->hwnd == GetDlgItem(IDOK)->m_hWnd)
		{
			Bubble_Tip.AddTool(GetDlgItem(IDOK),_T("关闭底层硬件驱动并退出软件"));//为此控件添加tip
			Bubble_Tip.RelayEvent(pMsg);
		}
	}

	if(pMsg->message==WM_KEYDOWN&&pMsg->wParam==VK_RETURN)   // hu 屏蔽ENTER  
		return TRUE; 
	if(pMsg->message==WM_KEYDOWN&&pMsg->wParam==VK_ESCAPE)   // hu 屏蔽ESC
		return TRUE;
	return CDialog::PreTranslateMessage(pMsg);
}

void CPCIDriverTestDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	static int timer_count = 0;

	switch(nIDEvent)
	{
	case 1:
		{
			Bar_FPGA.Progress_FPGA.SetPos((timer_count+1)*0.5*100/15);
			if ((timer_count+1)*0.5 == 13)
			{
				timer_count = 0;
				KillTimer(nIDEvent);
				break;
			}
			timer_count++;
			break;
		}
	default:
		{
			MainDlgFlash->OnTimer(nIDEvent);
			break;
		}
	}

	CDialog::OnTimer(nIDEvent);
}

void CPCIDriverTestDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO: Add your message handler code here and/or call default
	// hu 自绘按钮
	//if(nIDCtl==IDC_SELECT_BIN) //hu 只改变一个按钮
	
	CDC dc;
	dc.Attach(lpDrawItemStruct ->hDC);
	RECT rect;
	rect= lpDrawItemStruct ->rcItem;

	dc.Draw3dRect(&rect,RGB(255,255,255),RGB(205,155,29));
	dc.FillSolidRect(&rect,RGB(255,236,139));
	UINT state=lpDrawItemStruct->itemState;

	if((state & ODS_SELECTED))
	{
		dc.DrawEdge(&rect,EDGE_SUNKEN,BF_RECT);
	}
	else
	{
		dc.DrawEdge(&rect,EDGE_RAISED,BF_RECT);
	}

	dc.SetBkColor(RGB(255,236,139));
	//dc.SetBkColor(GetSysColor(COLOR_BTNFACE));
	dc.SetTextColor(RGB(0,0,0));


	TCHAR buffer[MAX_PATH];
	ZeroMemory(buffer,MAX_PATH );
	::GetWindowText(lpDrawItemStruct->hwndItem,buffer,MAX_PATH);
	dc.DrawText(buffer,&rect,DT_CENTER|DT_VCENTER|DT_SINGLELINE);

	dc.Detach();

	CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

void CPCIDriverTestDlg::OnNMDblclkListData(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	NM_LISTVIEW  *pEditCtrl = (NM_LISTVIEW *)pNMHDR;

	if (-1 == pEditCtrl->iItem)                             // hu 点击到非工作区
	{
		if (haveeditcreate == true)                         // hu 如果之前创建了编辑框就销毁掉
		{
			distroyEdit(&dataList,&m_Edit,e_Item,e_SubItem);// hu 销毁单元格编辑框对象
			haveeditcreate = false;
		}
	}
	else if (0 != pEditCtrl->iSubItem)
	{
		if (haveeditcreate == true)
		{
			if (!(e_Item == pEditCtrl->iItem && e_SubItem == pEditCtrl->iSubItem))// hu 如果点中的单元格不是之前创建好的
			{
				distroyEdit(&dataList,&m_Edit,e_Item,e_SubItem);
				haveeditcreate = false;
				// hu createEdit(pEditCtrl, &m_Edit,e_Item,e_SubItem,haveeditcreate);// hu 创建编辑框
			}
			else m_Edit.SetFocus();// hu 设置为焦点 
		}
		else
		{
			e_Item = pEditCtrl->iItem;          // hu 将点中的单元格的行赋值给“刚编辑过的行”以便后期处理
			e_SubItem = pEditCtrl->iSubItem;    // hu 将点中的单元格的行赋值给“刚编辑过的行”以便后期处理
			createEdit(pEditCtrl, &m_Edit, e_Item, e_SubItem, haveeditcreate);// hu 创建编辑框
		}
	}

	*pResult = 0;
}

// hu 创建单元格编辑框函数
void CPCIDriverTestDlg::createEdit(NM_LISTVIEW  *pEditCtrl, CEdit *createdit, int &Item, int &SubItem, bool &havecreat)//创建单元格编辑框函数
{
	// hu pEditCtrl为列表对象指针，createdit为编辑框指针对象，
	// hu Item为创建单元格在列表中的行，SubItem则为列，havecreat为对象创建标准
	Item = pEditCtrl->iItem;
	SubItem = pEditCtrl->iSubItem;

	 // hu 创建编辑框对象,IDC_EDIT_CREATEID为控件ID号3000
	createdit->Create(ES_AUTOHSCROLL|WS_CHILD|ES_CENTER|ES_WANTRETURN,CRect(0,0,0,0),this,IDC_EDIT_CREATEID);
	havecreat = true;
	createdit->SetFont(this->GetFont(), FALSE);// hu 设置字体,不设置这里的话上面的字会很突兀的感觉
	createdit->SetParent(&dataList);           // hu 将list control设置为父窗口,生成的Edit才能正确定位,这个也很重要
	// hu createdit->LimitText(7);                   // hu 限制文本长度

	CRect  EditRect;
	dataList.GetSubItemRect(e_Item,e_SubItem,LVIR_LABEL,EditRect);// hu 获取单元格的空间位置信息
	// hu +1和-1可以让编辑框不至于挡住列表框中的网格线
	EditRect.SetRect(EditRect.left+1,EditRect.top+1,EditRect.left+dataList.GetColumnWidth(e_SubItem)-1,EditRect.bottom-1);
	CString strItem = dataList.GetItemText(e_Item,e_SubItem);// hu 获得相应单元格字符
	createdit->SetWindowText(strItem);                       // hu 将单元格字符显示在编辑框上
	createdit->MoveWindow(&EditRect);                        // hu 将编辑框位置放在相应单元格上
	createdit->ShowWindow(SW_SHOW);                          // hu 显示编辑框在单元格上面
	createdit->SetFocus();                                   // hu 设置为焦点 
	createdit->SetSel(-1);                                   // hu 设置光标在文本框文字的最后
}

void CPCIDriverTestDlg::distroyEdit(CListCtrl *list,CEdit* distroyedit, int &Item, int &SubItem)
{
	CString meditdata;
	distroyedit->GetWindowText(meditdata);
	list->SetItemText(Item, SubItem, meditdata);// hu 获得相应单元格字符
	distroyedit->DestroyWindow();               // hu 销毁对象，有创建就要有销毁，不然会报错
}
void CPCIDriverTestDlg::OnNMClickListData(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
// 	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
// 	NM_LISTVIEW  *pEditCtrl = (NM_LISTVIEW *)pNMHDR;

// 	if (haveeditcreate == true)                         // hu 如果之前创建了编辑框就销毁掉
// 	{
// 		distroyEdit(&dataList,&m_Edit,e_Item,e_SubItem);// hu 销毁单元格编辑框对象
// 		haveeditcreate = false;
// 	}

	*pResult = 0;
}

void CPCIDriverTestDlg::OnNMRclickListData(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	CMenu   menu;   // hu 定义下面要用到的cmenu对象

	if (haveeditcreate == true)                         // hu 如果之前创建了编辑框就销毁掉
	{
		distroyEdit(&dataList,&m_Edit,e_Item,e_SubItem);// hu 销毁单元格编辑框对象
		haveeditcreate = false;
	}

	menu.LoadMenu(IDR_LIST_MENU);                    // hu 装载自定义的右键菜单 
	CMenu   *pContextMenu = menu.GetSubMenu(0);     // hu 获取第一个弹出菜单，所以第一个菜单必须有子菜单 
	CPoint point1;                                    // hu 定义一个用于确定光标位置的位置  
	GetCursorPos(&point1);                           // hu 获取当前光标的位置，以便使得菜单可以跟随光标  
	pContextMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,point1.x,point1.y,AfxGetMainWnd()); // hu 在指定位

	*pResult = 0;
}

void CPCIDriverTestDlg::OnLvnItemchangedListData(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	NM_LISTVIEW  *pListCtrl = (NM_LISTVIEW *)pNMHDR;
	CString addr_str,data_str,status_show;
	UINT addr,data;

	// hu Item被选中
	if((pNMLV->uOldState & INDEXTOSTATEIMAGEMASK(1)) /* old state : unchecked */
		&& (pNMLV->uNewState & INDEXTOSTATEIMAGEMASK(2)) /* new state : checked */
		)
	{
		addr_str = dataList.GetItemText(pListCtrl->iItem,1);
		data_str = dataList.GetItemText(pListCtrl->iItem,2);
		if (L"0x" == addr_str)
		{
			AfxMessageBox(_T("地址栏不能为空!"));
			return;
		}
		addr_str.Delete(0,2);
		_stscanf_s(addr_str.GetBuffer(0),_T("%x"),&addr);

		if (L"" == data_str)
		{
			AfxMessageBox(_T("数据栏不能为空!"));
			return;
		}
		_stscanf_s(data_str.GetBuffer(0),_T("%d"),&data);

		status_show.Format(_T("写入...\r\n0x%x：%d"),addr,data);
		GetDlgItem(IDC_SHOW)->SetWindowText(status_show);
#ifndef DEBUG_HU
		WriteToDSP(deviceX,3,addr*addrOffset4FPGA,data);
#endif
		GetDlgItem(IDC_SHOW)->GetWindowText(status_show);
		status_show += L"\r\n成功！";
		GetDlgItem(IDC_SHOW)->SetWindowText(status_show);
	}
	// hu Item未被选中
	else if((pNMLV->uOldState & INDEXTOSTATEIMAGEMASK(2)) /* old state : checked */
		&& (pNMLV->uNewState & INDEXTOSTATEIMAGEMASK(1)) /* new state : unchecked */
		)
	{
		addr_str = dataList.GetItemText(pListCtrl->iItem,1);
		data_str = dataList.GetItemText(pListCtrl->iItem,3);
		if (L"0x" == addr_str)
		{
			AfxMessageBox(_T("地址栏不能为空!"));
			return;
		}
		addr_str.Delete(0,2);
		_stscanf_s(addr_str.GetBuffer(0),_T("%x"),&addr);

		if (L"" == data_str)
		{
			AfxMessageBox(_T("数据栏不能为空!"));
			return;
		}
		_stscanf_s(data_str.GetBuffer(0),_T("%d"),&data);

		status_show.Format(_T("写入...\r\n0x%x：%d"),addr,data);
		GetDlgItem(IDC_SHOW)->SetWindowText(status_show);
#ifndef DEBUG_HU
		WriteToDSP(deviceX,3,addr*addrOffset4FPGA,data);
#endif
		GetDlgItem(IDC_SHOW)->GetWindowText(status_show);
		status_show += L"\r\n成功！";
		GetDlgItem(IDC_SHOW)->SetWindowText(status_show);
	}

	*pResult = 0;
}

void CPCIDriverTestDlg::OnCbnSelchangeDeviceSelect()
{
	// TODO: Add your control notification handler code here
	deviceX = ((CComboBox *)GetDlgItem(IDC_DEVICE_SELECT))->GetCurSel();
}

void CPCIDriverTestDlg::OnListAdd()
{
	// TODO: Add your command handler code here
	UINT listSelectNum;

	listSelectNum = dataList.GetSelectedCount();

	if (listSelectNum > 0)
	{
	    int *dataListPos = new int[listSelectNum];
		int ii;

		POSITION ListPos = dataList.GetFirstSelectedItemPosition();
		for (ii = 0;ii < listSelectNum;ii++)
		{
			dataListPos[ii] = dataList.GetNextSelectedItem(ListPos);
		}
		for (ii = listSelectNum-1;ii >= 0;ii--)
		{
			dataList.InsertItem(dataListPos[ii]+1,L"");
			dataList.SetItemText(dataListPos[ii]+1,1,dataList.GetItemText(dataListPos[ii],1));   
			dataList.SetItemText(dataListPos[ii]+1,2,dataList.GetItemText(dataListPos[ii],2));   
			dataList.SetItemText(dataListPos[ii]+1,3,dataList.GetItemText(dataListPos[ii],3));  
			dataList.EnsureVisible(dataListPos[ii]+1, FALSE); 
		}
		delete []dataListPos;
	}
	else
	{
	    int dataListPos;
		dataListPos = dataList.GetItemCount();
		dataList.InsertItem(dataListPos,L"");
		dataList.SetItemText(dataListPos,1,dataList.GetItemText(dataListPos-1,1));   
		dataList.SetItemText(dataListPos,2,dataList.GetItemText(dataListPos-1,2));   
		dataList.SetItemText(dataListPos,3,dataList.GetItemText(dataListPos-1,3));  
		dataList.EnsureVisible(dataListPos, FALSE); 
	}
}

void CPCIDriverTestDlg::OnListDelete()
{
	// TODO: Add your command handler code here
	UINT listSelectNum;

	listSelectNum = dataList.GetSelectedCount();

	if (listSelectNum > 0)
	{
		int *dataListPos = new int[listSelectNum];
		int ii;

		POSITION ListPos = dataList.GetFirstSelectedItemPosition();
		for (ii = 0;ii < listSelectNum;ii++)
		{
			dataListPos[ii] = dataList.GetNextSelectedItem(ListPos);
		}
		for (ii = listSelectNum-1;ii >= 0;ii--)
		{
			if (dataListPos[ii] > 0)
			{
				dataList.DeleteItem(dataListPos[ii]);
			}
		}

		delete []dataListPos;
	}
	else
	{
		int dataListPos;
		dataListPos = dataList.GetItemCount();
		if (dataListPos > 1)
		{
			dataList.DeleteItem(dataListPos-1);
		}
	}
}
void CPCIDriverTestDlg::OnListSave()
{
	// TODO: Add your command handler code here 
	CString defaultDir = L".\\listCfg";   //默认打开的文件路径  
	CString fileName = L"1.txt";           //默认打开的文件名  
	CString filter = L"文件 (*.txt)|*.txt||";   //文件过虑的类型  

	CFileDialog openFileDlg(FALSE, defaultDir, fileName, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, filter, NULL);  
	//openFileDlg.GetOFN().lpstrInitialDir = L"listCfg\\2.txt"; 

	INT_PTR result = openFileDlg.DoModal();  
	if(result == IDOK) 
	{  
		FILE *ListSave;
		int ii;
		UINT listNum = dataList.GetItemCount();

		fileName = openFileDlg.GetPathName(); 
		ListSave = _wfopen(fileName,L"wt+");

		for (ii = 0;ii < listNum;ii++)
		{
			fwprintf(ListSave,L"%s %s %s\n",dataList.GetItemText(ii,1),dataList.GetItemText(ii,2),dataList.GetItemText(ii,3));
		}
		fclose(ListSave);
	}
}

void CPCIDriverTestDlg::OnListUse()
{
	// TODO: Add your command handler code here
	CFileDialog FileDlg(TRUE,NULL,NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,_T("文件(*.txt)|*.txt||"));

	FileDlg.m_ofn.lpstrInitialDir = _T(".\\listCfg");//改初始化目录

	if (FileDlg.DoModal() == IDOK)
	{
		CString path,listData;
		FILE *ListUse;
		int ii=0;

		path = FileDlg.GetPathName();
		ListUse = _wfopen(path,L"rt");
		if (NULL == ListUse)
		{
			AfxMessageBox(_T("载入模版文件失败!"));
			return;
		}

		fwscanf(ListUse,L"%s",listData);
		while (!listData.Find(L"0x"))
		{
			if (0 == ii)
				dataList.DeleteAllItems();
			dataList.InsertItem(ii,L"");
			dataList.SetItemText(ii,1,listData);
			fwscanf(ListUse,L"%s",listData);
			dataList.SetItemText(ii,2,listData);
			fwscanf(ListUse,L"%s",listData);
			dataList.SetItemText(ii,3,listData);
			fwscanf(ListUse,L"%s",listData);
			ii++;		
		}
		fclose(ListUse);
	}
}
