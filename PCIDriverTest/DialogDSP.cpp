// DialogDSP.cpp : implementation file
//

#include "stdafx.h"
#include "PCIDriverTest.h"
#include "DialogDSP.h"
#include "PCIDriverTestDlg.h"
#include "PciDriverFun.h"

// CDialogDSP dialog

IMPLEMENT_DYNAMIC(CDialogDSP, CDialog)

CDialogDSP::CDialogDSP(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogDSP::IDD, pParent)
{

}

CDialogDSP::~CDialogDSP()
{
}

void CDialogDSP::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDialogDSP, CDialog)
	ON_EN_CHANGE(IDC_DSP_OFFSADDR, &CDialogDSP::OnEnChangeDspOffsaddr)
	ON_BN_CLICKED(IDC_SEND_DSP, &CDialogDSP::OnBnClickedSendDsp)
	ON_EN_SETFOCUS(IDC_DSP_OFFSADDR, &CDialogDSP::OnEnSetfocusDspOffsaddr)
	ON_BN_CLICKED(IDC_RECV_DSP, &CDialogDSP::OnBnClickedRecvDsp)
	ON_WM_CTLCOLOR()
	ON_WM_DRAWITEM()
END_MESSAGE_MAP()


// CDialogDSP message handlers

void CDialogDSP::OnEnChangeDspOffsaddr()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	CString temp,temp1;
	int len;

	GetDlgItem(IDC_DSP_OFFSADDR)->GetWindowText(temp);

	if (temp.Find(_T("0x"))) // hu 0x必须存在且在最开始
	{
		temp = _T("0x");
		GetDlgItem(IDC_DSP_OFFSADDR)->SetWindowText(temp);
		((CEdit*)GetDlgItem(IDC_DSP_OFFSADDR))->SetSel(2,2);  // hu 将光标设置到最后
	}

	// hu 只能输入16进制数
	len = temp.GetLength();  // hu 获得字符串长度
	temp1 = temp.Right(1);   // hu 提取最后一个字符
	temp1.MakeLower();       // hu 转化为小写字母
	if ((len>2) && !((temp1>=_T("0")&&temp1<=_T("9"))||(temp1>=_T("a")&&temp1<=_T("f"))))
	{
		temp.Delete(len-1,1);
		GetDlgItem(IDC_DSP_OFFSADDR)->SetWindowText(temp);
		((CEdit*)GetDlgItem(IDC_DSP_OFFSADDR))->SetSel(len-1,len-1);  // hu 将光标设置到最后
	}
}

BOOL CDialogDSP::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if(pMsg->message == WM_MOUSEMOVE)  // hu 显示气泡提示
	{
		if (pMsg->hwnd == GetDlgItem(IDC_DSP_OFFSADDR)->m_hWnd)
		{
			Bubble_Tip_DSP.AddTool(GetDlgItem(IDC_DSP_OFFSADDR),_T("写入相对于DDR或L2RAM起始地址的偏移地址以用来写入或读取数据"));//为此控件添加tip
			Bubble_Tip_DSP.RelayEvent(pMsg);
		}
		else if (pMsg->hwnd == GetDlgItem(IDC_DATAW_DSP)->m_hWnd)
		{
			Bubble_Tip_DSP.AddTool(GetDlgItem(IDC_DATAW_DSP),_T("填写要向指定地址写入的数据"));//为此控件添加tip
			Bubble_Tip_DSP.RelayEvent(pMsg);
		}
		else if (pMsg->hwnd == GetDlgItem(IDC_DATAR_DSP)->m_hWnd)
		{
			Bubble_Tip_DSP.AddTool(GetDlgItem(IDC_DATAR_DSP),_T("显示在指定地址读取到数据"));//为此控件添加tip
			Bubble_Tip_DSP.RelayEvent(pMsg);
		}
		else if (pMsg->hwnd == GetDlgItem(IDC_SEND_DSP)->m_hWnd)
		{
			Bubble_Tip_DSP.AddTool(GetDlgItem(IDC_SEND_DSP),_T("向指定地址写入数据"));//为此控件添加tip
			Bubble_Tip_DSP.RelayEvent(pMsg);
		}
		else if (pMsg->hwnd == GetDlgItem(IDC_RECV_DSP)->m_hWnd)
		{
			Bubble_Tip_DSP.AddTool(GetDlgItem(IDC_RECV_DSP),_T("在指定地址读取数据"));//为此控件添加tip
			Bubble_Tip_DSP.RelayEvent(pMsg);
		}
		else if (pMsg->hwnd == GetDlgItem(IDC_DDR_SELECT)->m_hWnd)
		{
			Bubble_Tip_DSP.AddTool(GetDlgItem(IDC_DDR_SELECT),_T("选择DDR作为读写对象"));//为此控件添加tip
			Bubble_Tip_DSP.RelayEvent(pMsg);
		}
		else if (pMsg->hwnd == GetDlgItem(IDC_LRAM_SELECT)->m_hWnd)
		{
			Bubble_Tip_DSP.AddTool(GetDlgItem(IDC_LRAM_SELECT),_T("选择L2RAM作为读写对象"));//为此控件添加tip
			Bubble_Tip_DSP.RelayEvent(pMsg);
		}
	}

	if(pMsg->message==WM_KEYDOWN&&pMsg->wParam==VK_RETURN)   // hu 屏蔽ENTER  
		return TRUE; 
	if(pMsg->message==WM_KEYDOWN&&pMsg->wParam==VK_ESCAPE)   // hu 屏蔽ESC
		return TRUE;
	return CDialog::PreTranslateMessage(pMsg);
}

void CDialogDSP::OnBnClickedSendDsp()
{
	// TODO: Add your control notification handler code here
	CString addr_offs_str,data_str,status_show;
	UINT addr_offs,data,barx;

	GetDlgItem(IDC_DSP_OFFSADDR)->GetWindowText(addr_offs_str);
	if ("0x" == addr_offs_str)
	{
		AfxMessageBox(_T("偏移地址栏不能为空!"));
		return;
	}

	addr_offs_str.Delete(0,2);
	_stscanf_s(addr_offs_str.GetBuffer(0),_T("%x"),&addr_offs);

	GetDlgItem(IDC_DATAW_DSP)->GetWindowText(data_str);
	if ("" == data_str)
	{
		AfxMessageBox(_T("写入数据栏不能为空!"));
		return;
	}
	_stscanf_s(data_str.GetBuffer(0),_T("%d"),&data);

	barx = ((CButton *)GetDlgItem(IDC_DDR_SELECT))->GetCheck();// hu 返回1表示选上，0表示没选上
	if (barx)
	{
		barx += 4;
	}

	status_show.Format(L"设备%d:\r\n开始写入...\r\n偏移地址：",((CPCIDriverTestDlg*)AfxGetMainWnd())->deviceX);
	status_show.Format(_T("%s0x%s: %d"),status_show,addr_offs_str,data);
	((CPCIDriverTestDlg*)AfxGetMainWnd())->GetDlgItem(IDC_SHOW)->SetWindowText(status_show);

#ifndef DEBUG_HU
	WriteToDSP(((CPCIDriverTestDlg*)AfxGetMainWnd())->deviceX,barx,addr_offs,data);
#endif

	status_show += "\r\n写入成功!";
	((CPCIDriverTestDlg*)AfxGetMainWnd())->GetDlgItem(IDC_SHOW)->SetWindowText(status_show);
}

void CDialogDSP::OnEnSetfocusDspOffsaddr()
{
	// TODO: Add your control notification handler code here
	CString temp;
	UINT len;
	GetDlgItem(IDC_DSP_OFFSADDR)->GetWindowText(temp);
	len = temp.GetLength();
	if (len > 2)
	{
		((CEdit*)GetDlgItem(IDC_DSP_OFFSADDR))->SetSel(0,len);
	}
}

void CDialogDSP::OnBnClickedRecvDsp()
{
	// TODO: Add your control notification handler code here
	CString addr_offs_str,data_str,status_show;
	UINT addr_offs,data,barx;

	GetDlgItem(IDC_DSP_OFFSADDR)->GetWindowText(addr_offs_str);
	if ("0x" == addr_offs_str)
	{
		AfxMessageBox(_T("偏移地址栏不能为空!"));
		return;
	}

	addr_offs_str.Delete(0,2);
	_stscanf_s(addr_offs_str.GetBuffer(0),_T("%x"),&addr_offs);

	barx = ((CButton *)GetDlgItem(IDC_DDR_SELECT))->GetCheck();// hu 返回1表示选上，0表示没选上
	if (barx)
	{
		barx += 4;
	}

	status_show.Format(L"设备%d:\r\n开始读取...",((CPCIDriverTestDlg*)AfxGetMainWnd())->deviceX);
	((CPCIDriverTestDlg*)AfxGetMainWnd())->GetDlgItem(IDC_SHOW)->SetWindowText(status_show);

#ifdef DEBUG_HU
	data = 0;
#else
	ReadFromDSP(((CPCIDriverTestDlg*)AfxGetMainWnd())->deviceX,barx,addr_offs,&data);
#endif

	data_str.Format(_T("%d"),data);
	GetDlgItem(IDC_DATAR_DSP)->SetWindowText(data_str);
	status_show += "\r\n偏移地址：";
	status_show.Format(_T("%s0x%s: %s"),status_show,addr_offs_str,data_str);
	status_show += "\r\n读取成功!";
	((CPCIDriverTestDlg*)AfxGetMainWnd())->GetDlgItem(IDC_SHOW)->SetWindowText(status_show);
}

HBRUSH CDialogDSP::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Change any attributes of the DC here
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

void CDialogDSP::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
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
