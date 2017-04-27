// PCIDriverTestDlg.cpp : ʵ���ļ�
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

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

// CPCIDriverTestDlg �Ի���




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
	ON_MESSAGE(WM_CALLBACKMESSAGE_0, OnCALLBACKMESSAGE_0)    // hu �����Ϣӳ�䴦��
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


// CPCIDriverTestDlg ��Ϣ�������

BOOL CPCIDriverTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
#ifndef XP_HU
	// hu ���ڽ���
	MainDlgFlash = new CDlgFlash(this,9,10,11,12);  // hu ��ʱ��IDΪ9,10,11,12
	MainDlgFlash->SetFlashCreateSpeed(1,5);
	MainDlgFlash->SetFlashDestroySpeed(5,3);
	MainDlgFlash->SetFlashCR2NCRSpeed(12);
	MainDlgFlash->StartFlashCR2NCRDlg();
#endif

	// hu ����tab�ӽ���
	Bar_wr.InsertItem(0,_T("FPGA"));  
	Bar_wr.InsertItem(1,_T("DSP")); 
	Bar_FPGA.Create(IDD_DIALOG_FPGA,GetDlgItem(IDC_BAR_WR));
	Bar_DSP.Create(IDD_DIALOG_DSP,GetDlgItem(IDC_BAR_WR));

	// hu ��ÿͻ�����С  
	CRect rs;  
	Bar_wr.GetClientRect(&rs);

	// hu �����ӶԻ����ڸ������е�λ�ã�����ʵ���޸�  
	rs.top+=25;  
	rs.bottom-=1;  
	rs.left+=1;  
	rs.right-=1;

	// hu �����ӶԻ���ߴ粢�ƶ���ָ��λ��  
	Bar_FPGA.MoveWindow(&rs);  
	Bar_DSP.MoveWindow(&rs);

	// hu �ֱ��������غ���ʾ  
	Bar_FPGA.ShowWindow(true);  
	Bar_DSP.ShowWindow(false);  

	// hu ����Ĭ�ϵ�ѡ�  
	Bar_wr.SetCurSel(0);

	// hu FPGA��ַ������
	((CEdit*)Bar_FPGA.GetDlgItem(IDC_FPGA_STARTADDR))->SetLimitText(7); // hu ��������Ϊ7���ֽ�
	((CEdit*)Bar_FPGA.GetDlgItem(IDC_FPGA_ENDADDR))->SetLimitText(7);
	((CEdit*)Bar_FPGA.GetDlgItem(IDC_REPEAT_FPGADATA))->SetLimitText(2); // hu ����ظ�д��99��

	Bar_FPGA.GetDlgItem(IDC_FPGA_STARTADDR)->SetWindowText(_T("0x")); // hu ��enchange��Ϣ�г�ʼ��
	Bar_FPGA.GetDlgItem(IDC_DATA_FPGA)->SetWindowText(_T("1"));
	Bar_FPGA.GetDlgItem(IDC_REPEAT_FPGADATA)->SetWindowText(_T("1"));
	//Bar_FPGA.GetDlgItem(IDC_FPGA_ENDADDR)->SetWindowText(_T("0x"));
	/*FileBinColor.CreateSolidBrush(RGB(255,255,0)); // hu �ı�༭�򱳾���ɫ*/

	// hu DSP��ַ������
	((CEdit*)Bar_DSP.GetDlgItem(IDC_DSP_OFFSADDR))->SetLimitText(10); // hu ��������Ϊ7���ֽ�

	Bar_DSP.GetDlgItem(IDC_DSP_OFFSADDR)->SetWindowText(_T("0x0"));
	Bar_DSP.GetDlgItem(IDC_DATAW_DSP)->SetWindowText(_T("1"));

	// hu ���������ͣ��ʾ
	EnableToolTips(TRUE);
	Bubble_Tip.Create(this);
	Bubble_Tip.Activate(TRUE);
	Bubble_Tip.SetMaxTipWidth(600);         // hu ����������ʾ����ȣ����ú��ʵ�ֶ�����ʾ
	Bar_FPGA.Bubble_Tip_FPGA.Create(this);
	Bar_FPGA.Bubble_Tip_FPGA.Activate(TRUE);
	Bar_FPGA.Bubble_Tip_FPGA.SetMaxTipWidth(600);  
	Bar_DSP.Bubble_Tip_DSP.Create(this);
	Bar_DSP.Bubble_Tip_DSP.Activate(TRUE);
	Bar_DSP.Bubble_Tip_DSP.SetMaxTipWidth(600); 
	
	// hu ��ʼ����ѡ��
	((CButton *)Bar_FPGA.GetDlgItem(IDC_FIX_SELECT))->SetCheck(TRUE);
	((CButton *)Bar_DSP.GetDlgItem(IDC_DDR_SELECT))->SetCheck(TRUE);

	// hu ��ʼ��������
	Bar_FPGA.Progress_FPGA.SetRange(0,100);  // hu ���ý������ķ�Χ
	Bar_FPGA.Progress_FPGA.SetStep(10);       // hu ���ý�������ÿһ��������
	Bar_FPGA.Progress_FPGA.SetPos(0);        // hu ���ý������ĵ�ǰλ��

	// hu ��ʼ���б���ͼ
	CRect rect;
	haveeditcreate = false;//��ʼ����־λ����ʾ��û�д����༭��
	// hu ��ȡ��������б���ͼ�ؼ���λ�úʹ�С   
	dataList.GetClientRect(&rect);
	dataList.SetExtendedStyle(dataList.GetExtendedStyle()|LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_CHECKBOXES); 
	// hu Ϊ�б���ͼ�ؼ��������   
	dataList.InsertColumn(0,_T(""),LVCFMT_LEFT,25,0);
	dataList.InsertColumn(1,_T("�����ַ"),LVCFMT_CENTER,rect.Width()/3+4,1); 
	dataList.InsertColumn(2,_T("ѡ��"),LVCFMT_CENTER,rect.Width()/3-24,2);  
	dataList.InsertColumn(3,_T("ȡ��"),LVCFMT_CENTER,rect.Width()/3-24,3); 
	// hu ���б���ͼ�ؼ��в����б���������б������ı�
	dataList.InsertItem(0,L"");
	dataList.SetItemText(0,1,_T("0x"));   
	dataList.SetItemText(0,2,_T("1"));   
	dataList.SetItemText(0,3,_T("0"));  
	dataList.SetTextBkColor(RGB(152,251,152)); 
	dataList.SetBkColor(RGB(152,251,152));  

	// hu ��ʼ��PCI6455
	DWORD status;

#ifdef DEBUG_HU
	status = 99;
#else
	status = InitAllDSPDriver(&dspDriver[0]);
#endif
	if (status)   GetDlgItem(IDC_SHOW)->SetWindowText(GetLastDriverError());
	else          GetDlgItem(IDC_SHOW)->SetWindowText(_T("��ʼ����������ɹ���"));

	if (0 == status)
	{
		CString temp;
		int ii,jj;

		for (ii=0;ii<DEVNUM_MAX;ii++)
		{
			GetDlgItem(IDC_SHOW)->GetWindowText(temp);
			temp.Format(_T("%s\r\nPCI�豸%d:\r\n%s"),temp,ii,DriverStatus2Str(dspDriver[ii].status));
			GetDlgItem(IDC_SHOW)->SetWindowText(temp);

			if (!dspDriver[ii].status)  deviceNum = ii + 1;

			for (jj=0;jj<dspDriver[ii].memNum;jj++)
			{
				GetDlgItem(IDC_SHOW)->GetWindowText(temp);
				temp.Format(_T("%s\r\ndmaMem%d:%s"),temp,jj,DriverMemStatus2Str(dspDriver[ii].memStatus[jj],dspDriver[ii].memSize[jj]));
				GetDlgItem(IDC_SHOW)->SetWindowText(temp);
			}
		}

		// hu ��ʼ����Ͽ�
		if (0 == deviceNum)
		{
			((CComboBox *)GetDlgItem(IDC_DEVICE_SELECT))->AddString(L"���豸");
		}
		else
		{
			for (ii=0;ii<deviceNum;ii++)
			{
				temp.Format(L"�豸%d",ii);
				((CComboBox *)GetDlgItem(IDC_DEVICE_SELECT))->AddString(temp);
			}
		}
		((CComboBox *)GetDlgItem(IDC_DEVICE_SELECT))->SetCurSel(deviceX);

		// hu ע����Ϣ����
		m_hWnd = AfxGetMainWnd()->m_hWnd;
		RegistHWNDToDev(m_hWnd);
		SetDSPAutoRun(TRUE);
	}
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CPCIDriverTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ��������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù����ʾ��
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
	MainDlgFlash->StartFlashDestroyDlg(); //��ʼ�����رնԻ���
#endif
	//OnOK();
}

// hu ��Ӧdll�ϴ���PostMessage()����
LRESULT CPCIDriverTestDlg::OnCALLBACKMESSAGE_0(WPARAM wParam, LPARAM lParam)//hu wParamΪ�жϺ�
{
	CString temp;
	if(wParam == 0) 
	{
		// hu ///////////���0���жϴ������///////////////	
		return 0; 
		// hu ///////////////////////////////////////////// 
	}
	else if(wParam == 1)
	{
		// hu ///////////���1���жϴ������///////////////
		return 0; 
		// hu /////////////////////////////////////////////
	}
	else if(wParam == 2)  
	{
		// hu ///////////���2���жϴ������///////////////
		UINT GPIO2FPGA_flag;

		dmaSource0.Deliver_Time = GetTickCount() - dmaSource0.Deliver_Time;  // hu ����ʱ��

#ifdef DEBUG_HU
		GPIO2FPGA_flag = 99;
#else
		ReadFromDSP(deviceX,5,0x14,&GPIO2FPGA_flag);
#endif
		GetDlgItem(IDC_SHOW)->GetWindowText(temp);
		temp.Format(L"%s\r\n�豸%d:\r\n%s",temp,lParam,IniFpgaFlag2Str(GPIO2FPGA_flag));
		GetDlgItem(IDC_SHOW)->SetWindowText(temp);

		Bar_FPGA.Progress_FPGA.SetPos(100);
		temp.Format(_T("%.1f"),dmaSource0.Deliver_Time/1000.0);
		GetDlgItem(IDC_SHOW_TIME)->SetWindowText(temp);
		return 0; 
		// hu /////////////////////////////////////////////
	}
	else if(wParam == 3)  
	{
		// hu ///////////���3���жϴ������///////////////
		dmaSource0.Deliver_Time = GetTickCount() - dmaSource0.Deliver_Time;  // hu ����ʱ��

		WriteToDSP(deviceX,3,0xa0a00*addrOffset4FPGA,0xf0f0);// hu ֪ͨfpga������� 0xa0a00Ϊfpga��ȡ���ݵ�ַ

		temp.Format(_T("%.1f"),dmaSource0.Deliver_Time/1000.0);
		GetDlgItem(IDC_SHOW_TIME)->SetWindowText(temp);

		GetDlgItem(IDC_SHOW)->GetWindowText(temp);
		temp.Format(L"%s\r\n�豸%d:\r\nEDMA����ɹ���",temp,lParam);
		GetDlgItem(IDC_SHOW)->SetWindowText(temp);
		// hu /////////////////////////////////////////////
	}		

	return 0; 
}

void CPCIDriverTestDlg::OnBnClickedSelectBin()
{
	// TODO: Add your control notification handler code here
	CFileDialog FileDlg(TRUE,NULL,NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,_T("�ļ�(*.bin;*.bit)|*.bin;*.bit||"));

	FileDlg.m_ofn.lpstrInitialDir = _T(".\\files");//�ĳ�ʼ��Ŀ¼

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

	WriteToDev pPhyAddr = {deviceX,5,0,0,1},     // hu �����ַ
		       dataLen  = {deviceX,5,4,0,1},     // hu ����ռ������ݵĳ���
		       endFlag  = {deviceX,5,0x10,1,1};  // hu ���������־λ

	GetDlgItem(IDC_SHOW_BIN)->GetWindowText(path);
	if (!dmaSource0.Deliver_FP.Open(path,CFile::typeBinary|CFile::modeRead))
	{
		AfxMessageBox(_T("��������ȷ���ļ�·����!"));
		return;
	}
	dmaSource0.Deliver_FP.Close();

	GetDlgItem(IDC_SHOW_TIME)->SetWindowText(_T(""));
	dmaSource0.Deliver_Time = GetTickCount();// hu ��ʼʱ��

	temp.Format(L"�豸%d:\r\n��ʼEDMA����...",deviceX);
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
	WriteToDev pPhyAddr = {deviceX,5,8,0,1},     // hu �����ַ
		       dataLen  = {deviceX,5,0xc,0,1},   // hu ����ռ������ݵĳ���
	           endFlag  = {deviceX,5,0x10,1,1};  // hu ���������־λ

	GetDlgItem(IDC_SHOW_BIN)->GetWindowText(path);
	if (!dmaSource0.Deliver_FP.Open(path,CFile::typeBinary|CFile::modeRead))
	{
		AfxMessageBox(_T("��������ȷ���ļ�·����!"));
		return;
	}
	binLen = dmaSource0.Deliver_FP.GetLength();
	dmaSource0.Deliver_FP.Close();
	if (binLen >= 11*1024*1024)
	{
		AfxMessageBox(_T("�����ļ����ܴ���11MB!"));
		return;
	}

#ifdef DEBUG_HU
	PostMessage(WM_CALLBACKMESSAGE_0,2,deviceX);
#else
	DmaFileToDSP(deviceX,0,path.GetBuffer(0),pPhyAddr,dataLen,&endFlag);
#endif
	GetDlgItem(IDC_SHOW_TIME)->SetWindowText(_T(""));
	temp.Format(L"�豸%d:\r\n��ʼ��̬����...",deviceX);
	GetDlgItem(IDC_SHOW)->SetWindowText(temp);

	dmaSource0.Deliver_Time = GetTickCount();// hu ��ʼʱ��
	SetTimer(1,500,NULL);// hu ������ʱ��1,��ʱʱ����0.5��
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
	CWnd* pWnd=GetDlgItem(IDC_SHOW_BIN);	// hu ��ȡ�ı��ؼ�����ָ��
	pWnd->Invalidate();          // hu ��pWnd��ָ��Ĵ����ػ�
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

		if (temp.Find(_T("0x"))) // hu 0x������������ʼ
		{
			m_Edit.SetWindowText(_T("0x"));
			m_Edit.SetSel(-1);  // hu ��������õ����
		}

		// hu ֻ������16������
		len = temp.GetLength();  // hu ����ַ�������
		temp1 = temp.Right(1);   // hu ��ȡ���һ���ַ�
		temp1.MakeLower();       // hu ת��ΪСд��ĸ
		if ((len>2) && !((temp1>=_T("0")&&temp1<=_T("9"))||(temp1>=_T("a")&&temp1<=_T("f"))))
		{
			temp.Delete(len-1,1);
			m_Edit.SetWindowText(temp);
			m_Edit.SetSel(-1);  // hu ��������õ����
		}
	}
}

HBRUSH CPCIDriverTestDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Change any attributes of the DC here
	if(nCtlColor==CTLCOLOR_EDIT && pWnd->GetDlgCtrlID()==IDC_SHOW_BIN)//ע��˴��ģ�pWnd->��������ûЧ��
	{
		CString path;
		CFileStatus temp;
		GetDlgItem(IDC_SHOW_BIN)->GetWindowText(path);
		if (dmaSource0.Deliver_FP.GetStatus(path,temp))
		{
			pDC->SetTextColor(RGB(0,0,0));   // hu ���ļ����ڣ�������Ϊ��ɫ
		}
		else
		{
			pDC->SetTextColor(RGB(255,0,0)); // hu ���ļ������ڣ�������Ϊ��ɫ
		}
		//pDC->SetBkColor(RGB(255,255,255));// hu �����ı�����ɫ
		//pDC->SetBkMode(TRANSPARENT);// hu ���ñ���͸��
		//hbr = (HBRUSH)FileBinColor;
	}
    
	//if(nCtlColor==CTLCOLOR_BTN)          // hu ���İ�ť��ɫ����������岻��ʾ������
	//{
	//	//pDC->SetBkMode(TRANSPARENT);
	//	//pDC->SetTextColor(RGB(0,0,0));
	//	//pDC->SetBkColor(RGB(121,121,255));   
	//	HBRUSH b=CreateSolidBrush(RGB(118,238,198));
	//	return b;
	//}

	if(nCtlColor==CTLCOLOR_EDIT)   // hu ���ı༭��
	{
		//pDC->SetBkMode(TRANSPARENT);
		//pDC->SetTextColor(RGB(0,0,0));
		pDC->SetBkColor(RGB(152,251,152));
		HBRUSH b=CreateSolidBrush(RGB(152,251,152));
		return b;
	}
	else if(nCtlColor==CTLCOLOR_STATIC)  // hu ���ľ�̬�ı�
	{
		//pDC->SetTextColor(RGB(0,0,0));
		pDC->SetBkColor(RGB(84,255,159));
		HBRUSH b=CreateSolidBrush(RGB(84,255,159));
		return b;
	}
	else if(nCtlColor==CTLCOLOR_DLG)   // hu ���ĶԻ��򱳾�ɫ
	{
		//pDC->SetTextColor(RGB(0,0,0));
		//pDC->SetBkColor(RGB(166,254,1));
		HBRUSH b=CreateSolidBrush(RGB(84,255,159));
		return b;
	}
	else if(nCtlColor==CTLCOLOR_SCROLLBAR)  // hu ������
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
	if(pMsg->message == WM_MOUSEMOVE)  // hu ��ʾ������ʾ
	{
		if (pMsg->hwnd == GetDlgItem(IDC_SHOW_BIN)->m_hWnd)
		{
			CString tmp;
			GetDlgItem(IDC_SHOW_BIN)->GetWindowText(tmp);
			if (!dmaSource0.Deliver_FP.Open(tmp,CFile::typeBinary|CFile::modeRead))
			{
				Bubble_Tip.AddTool(GetDlgItem(IDC_SHOW_BIN),_T("д��������ļ�·��"));//Ϊ�˿ؼ����tip
				Bubble_Tip.RelayEvent(pMsg);
			}
			else
			{
				int dataLen = dmaSource0.Deliver_FP.GetLength();
				dmaSource0.Deliver_FP.Close();
				tmp.Format(L"�ļ���СΪ%dBytes",dataLen);
				Bubble_Tip.AddTool(GetDlgItem(IDC_SHOW_BIN),tmp);//Ϊ�˿ؼ����tip
				Bubble_Tip.RelayEvent(pMsg);
			}
		}
// 		else if (pMsg->hwnd == GetDlgItem(IDC_LIST_DATA)->m_hWnd)
// 		{
// 			Bubble_Tip.AddTool(GetDlgItem(IDC_LIST_DATA),_T("�������༭\r\n˫�������\r\n�һ���ɾ��"));//Ϊ�˿ؼ����tip
// 			Bubble_Tip.RelayEvent(pMsg);
// 		}
		else if (pMsg->hwnd == GetDlgItem(IDC_SHOW_TIME)->m_hWnd)
		{
			Bubble_Tip.AddTool(GetDlgItem(IDC_SHOW_TIME),_T("��ʾ�ļ�д��ײ�FPGA����ʱ��"));//Ϊ�˿ؼ����tip
			Bubble_Tip.RelayEvent(pMsg);
		}
		else if (pMsg->hwnd == GetDlgItem(IDC_SELECT_BIN)->m_hWnd)
		{
			Bubble_Tip.AddTool(GetDlgItem(IDC_SELECT_BIN),_T("ѡ�������(.bin)�ļ���FPGA(.bit)�ļ�"));//Ϊ�˿ؼ����tip
			Bubble_Tip.RelayEvent(pMsg);
		}
		else if (pMsg->hwnd == GetDlgItem(IDC_SEND_DATA)->m_hWnd)
		{
			Bubble_Tip.AddTool(GetDlgItem(IDC_SEND_DATA),_T("��ײ�FPGAд�������(.bin)�ļ�"));//Ϊ�˿ؼ����tip
			Bubble_Tip.RelayEvent(pMsg);
		}
		else if (pMsg->hwnd == GetDlgItem(IDC_FPGA_INI)->m_hWnd)
		{
			Bubble_Tip.AddTool(GetDlgItem(IDC_FPGA_INI),_T("��FPGA(.bit)�ļ���ʼ���ײ�FPGA"));//Ϊ�˿ؼ����tip
			Bubble_Tip.RelayEvent(pMsg);
		}
		else if (pMsg->hwnd == GetDlgItem(IDC_SHOW)->m_hWnd)
		{
			Bubble_Tip.AddTool(GetDlgItem(IDC_SHOW),_T("��ʾϵͳ״̬"));//Ϊ�˿ؼ����tip
			Bubble_Tip.RelayEvent(pMsg);
		}
		else if (pMsg->hwnd == GetDlgItem(IDOK)->m_hWnd)
		{
			Bubble_Tip.AddTool(GetDlgItem(IDOK),_T("�رյײ�Ӳ���������˳����"));//Ϊ�˿ؼ����tip
			Bubble_Tip.RelayEvent(pMsg);
		}
	}

	if(pMsg->message==WM_KEYDOWN&&pMsg->wParam==VK_RETURN)   // hu ����ENTER  
		return TRUE; 
	if(pMsg->message==WM_KEYDOWN&&pMsg->wParam==VK_ESCAPE)   // hu ����ESC
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
	// hu �Ի水ť
	//if(nIDCtl==IDC_SELECT_BIN) //hu ֻ�ı�һ����ť
	
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

	if (-1 == pEditCtrl->iItem)                             // hu ������ǹ�����
	{
		if (haveeditcreate == true)                         // hu ���֮ǰ�����˱༭������ٵ�
		{
			distroyEdit(&dataList,&m_Edit,e_Item,e_SubItem);// hu ���ٵ�Ԫ��༭�����
			haveeditcreate = false;
		}
	}
	else if (0 != pEditCtrl->iSubItem)
	{
		if (haveeditcreate == true)
		{
			if (!(e_Item == pEditCtrl->iItem && e_SubItem == pEditCtrl->iSubItem))// hu ������еĵ�Ԫ����֮ǰ�����õ�
			{
				distroyEdit(&dataList,&m_Edit,e_Item,e_SubItem);
				haveeditcreate = false;
				// hu createEdit(pEditCtrl, &m_Edit,e_Item,e_SubItem,haveeditcreate);// hu �����༭��
			}
			else m_Edit.SetFocus();// hu ����Ϊ���� 
		}
		else
		{
			e_Item = pEditCtrl->iItem;          // hu �����еĵ�Ԫ����и�ֵ�����ձ༭�����С��Ա���ڴ���
			e_SubItem = pEditCtrl->iSubItem;    // hu �����еĵ�Ԫ����и�ֵ�����ձ༭�����С��Ա���ڴ���
			createEdit(pEditCtrl, &m_Edit, e_Item, e_SubItem, haveeditcreate);// hu �����༭��
		}
	}

	*pResult = 0;
}

// hu ������Ԫ��༭����
void CPCIDriverTestDlg::createEdit(NM_LISTVIEW  *pEditCtrl, CEdit *createdit, int &Item, int &SubItem, bool &havecreat)//������Ԫ��༭����
{
	// hu pEditCtrlΪ�б����ָ�룬createditΪ�༭��ָ�����
	// hu ItemΪ������Ԫ�����б��е��У�SubItem��Ϊ�У�havecreatΪ���󴴽���׼
	Item = pEditCtrl->iItem;
	SubItem = pEditCtrl->iSubItem;

	 // hu �����༭�����,IDC_EDIT_CREATEIDΪ�ؼ�ID��3000
	createdit->Create(ES_AUTOHSCROLL|WS_CHILD|ES_CENTER|ES_WANTRETURN,CRect(0,0,0,0),this,IDC_EDIT_CREATEID);
	havecreat = true;
	createdit->SetFont(this->GetFont(), FALSE);// hu ��������,����������Ļ�������ֻ��ͻأ�ĸо�
	createdit->SetParent(&dataList);           // hu ��list control����Ϊ������,���ɵ�Edit������ȷ��λ,���Ҳ����Ҫ
	// hu createdit->LimitText(7);                   // hu �����ı�����

	CRect  EditRect;
	dataList.GetSubItemRect(e_Item,e_SubItem,LVIR_LABEL,EditRect);// hu ��ȡ��Ԫ��Ŀռ�λ����Ϣ
	// hu +1��-1�����ñ༭�����ڵ�ס�б���е�������
	EditRect.SetRect(EditRect.left+1,EditRect.top+1,EditRect.left+dataList.GetColumnWidth(e_SubItem)-1,EditRect.bottom-1);
	CString strItem = dataList.GetItemText(e_Item,e_SubItem);// hu �����Ӧ��Ԫ���ַ�
	createdit->SetWindowText(strItem);                       // hu ����Ԫ���ַ���ʾ�ڱ༭����
	createdit->MoveWindow(&EditRect);                        // hu ���༭��λ�÷�����Ӧ��Ԫ����
	createdit->ShowWindow(SW_SHOW);                          // hu ��ʾ�༭���ڵ�Ԫ������
	createdit->SetFocus();                                   // hu ����Ϊ���� 
	createdit->SetSel(-1);                                   // hu ���ù�����ı������ֵ����
}

void CPCIDriverTestDlg::distroyEdit(CListCtrl *list,CEdit* distroyedit, int &Item, int &SubItem)
{
	CString meditdata;
	distroyedit->GetWindowText(meditdata);
	list->SetItemText(Item, SubItem, meditdata);// hu �����Ӧ��Ԫ���ַ�
	distroyedit->DestroyWindow();               // hu ���ٶ����д�����Ҫ�����٣���Ȼ�ᱨ��
}
void CPCIDriverTestDlg::OnNMClickListData(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
// 	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
// 	NM_LISTVIEW  *pEditCtrl = (NM_LISTVIEW *)pNMHDR;

// 	if (haveeditcreate == true)                         // hu ���֮ǰ�����˱༭������ٵ�
// 	{
// 		distroyEdit(&dataList,&m_Edit,e_Item,e_SubItem);// hu ���ٵ�Ԫ��༭�����
// 		haveeditcreate = false;
// 	}

	*pResult = 0;
}

void CPCIDriverTestDlg::OnNMRclickListData(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	CMenu   menu;   // hu ��������Ҫ�õ���cmenu����

	if (haveeditcreate == true)                         // hu ���֮ǰ�����˱༭������ٵ�
	{
		distroyEdit(&dataList,&m_Edit,e_Item,e_SubItem);// hu ���ٵ�Ԫ��༭�����
		haveeditcreate = false;
	}

	menu.LoadMenu(IDR_LIST_MENU);                    // hu װ���Զ�����Ҽ��˵� 
	CMenu   *pContextMenu = menu.GetSubMenu(0);     // hu ��ȡ��һ�������˵������Ե�һ���˵��������Ӳ˵� 
	CPoint point1;                                    // hu ����һ������ȷ�����λ�õ�λ��  
	GetCursorPos(&point1);                           // hu ��ȡ��ǰ����λ�ã��Ա�ʹ�ò˵����Ը�����  
	pContextMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,point1.x,point1.y,AfxGetMainWnd()); // hu ��ָ��λ

	*pResult = 0;
}

void CPCIDriverTestDlg::OnLvnItemchangedListData(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	NM_LISTVIEW  *pListCtrl = (NM_LISTVIEW *)pNMHDR;
	CString addr_str,data_str,status_show;
	UINT addr,data;

	// hu Item��ѡ��
	if((pNMLV->uOldState & INDEXTOSTATEIMAGEMASK(1)) /* old state : unchecked */
		&& (pNMLV->uNewState & INDEXTOSTATEIMAGEMASK(2)) /* new state : checked */
		)
	{
		addr_str = dataList.GetItemText(pListCtrl->iItem,1);
		data_str = dataList.GetItemText(pListCtrl->iItem,2);
		if (L"0x" == addr_str)
		{
			AfxMessageBox(_T("��ַ������Ϊ��!"));
			return;
		}
		addr_str.Delete(0,2);
		_stscanf_s(addr_str.GetBuffer(0),_T("%x"),&addr);

		if (L"" == data_str)
		{
			AfxMessageBox(_T("����������Ϊ��!"));
			return;
		}
		_stscanf_s(data_str.GetBuffer(0),_T("%d"),&data);

		status_show.Format(_T("д��...\r\n0x%x��%d"),addr,data);
		GetDlgItem(IDC_SHOW)->SetWindowText(status_show);
#ifndef DEBUG_HU
		WriteToDSP(deviceX,3,addr*addrOffset4FPGA,data);
#endif
		GetDlgItem(IDC_SHOW)->GetWindowText(status_show);
		status_show += L"\r\n�ɹ���";
		GetDlgItem(IDC_SHOW)->SetWindowText(status_show);
	}
	// hu Itemδ��ѡ��
	else if((pNMLV->uOldState & INDEXTOSTATEIMAGEMASK(2)) /* old state : checked */
		&& (pNMLV->uNewState & INDEXTOSTATEIMAGEMASK(1)) /* new state : unchecked */
		)
	{
		addr_str = dataList.GetItemText(pListCtrl->iItem,1);
		data_str = dataList.GetItemText(pListCtrl->iItem,3);
		if (L"0x" == addr_str)
		{
			AfxMessageBox(_T("��ַ������Ϊ��!"));
			return;
		}
		addr_str.Delete(0,2);
		_stscanf_s(addr_str.GetBuffer(0),_T("%x"),&addr);

		if (L"" == data_str)
		{
			AfxMessageBox(_T("����������Ϊ��!"));
			return;
		}
		_stscanf_s(data_str.GetBuffer(0),_T("%d"),&data);

		status_show.Format(_T("д��...\r\n0x%x��%d"),addr,data);
		GetDlgItem(IDC_SHOW)->SetWindowText(status_show);
#ifndef DEBUG_HU
		WriteToDSP(deviceX,3,addr*addrOffset4FPGA,data);
#endif
		GetDlgItem(IDC_SHOW)->GetWindowText(status_show);
		status_show += L"\r\n�ɹ���";
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
	CString defaultDir = L".\\listCfg";   //Ĭ�ϴ򿪵��ļ�·��  
	CString fileName = L"1.txt";           //Ĭ�ϴ򿪵��ļ���  
	CString filter = L"�ļ� (*.txt)|*.txt||";   //�ļ����ǵ�����  

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
	CFileDialog FileDlg(TRUE,NULL,NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,_T("�ļ�(*.txt)|*.txt||"));

	FileDlg.m_ofn.lpstrInitialDir = _T(".\\listCfg");//�ĳ�ʼ��Ŀ¼

	if (FileDlg.DoModal() == IDOK)
	{
		CString path,listData;
		FILE *ListUse;
		int ii=0;

		path = FileDlg.GetPathName();
		ListUse = _wfopen(path,L"rt");
		if (NULL == ListUse)
		{
			AfxMessageBox(_T("����ģ���ļ�ʧ��!"));
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
