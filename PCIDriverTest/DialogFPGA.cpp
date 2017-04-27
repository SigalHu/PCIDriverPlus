// DialogFPGA.cpp : implementation file
//

#include "stdafx.h"
#include "PCIDriverTest.h"
#include "DialogFPGA.h"
#include "PCIDriverTestDlg.h"
#include "PciDriverFun.h"

// CDialogFPGA dialog

IMPLEMENT_DYNAMIC(CDialogFPGA, CDialog)

CDialogFPGA::CDialogFPGA(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogFPGA::IDD, pParent)
{

}

CDialogFPGA::~CDialogFPGA()
{
}

void CDialogFPGA::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS_FPGA, Progress_FPGA);
}


BEGIN_MESSAGE_MAP(CDialogFPGA, CDialog)
	ON_EN_CHANGE(IDC_FPGA_STARTADDR, &CDialogFPGA::OnEnChangeFpgaStartaddr)
	ON_EN_SETFOCUS(IDC_FPGA_ENDADDR, &CDialogFPGA::OnEnSetfocusFpgaEndaddr)
	ON_EN_CHANGE(IDC_FPGA_ENDADDR, &CDialogFPGA::OnEnChangeFpgaEndaddr)
	ON_BN_CLICKED(IDC_SEND_FPGA, &CDialogFPGA::OnBnClickedSendFpga)
	ON_WM_CTLCOLOR()
	ON_WM_DRAWITEM()
END_MESSAGE_MAP()


// CDialogFPGA message handlers

void CDialogFPGA::OnEnChangeFpgaStartaddr()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	CString temp,temp1;
	int len;

	GetDlgItem(IDC_FPGA_STARTADDR)->GetWindowText(temp);
	
	if (temp.Find(_T("0x"))) // hu 0x必须存在且在最开始
	{
		temp = _T("0x");
		GetDlgItem(IDC_FPGA_STARTADDR)->SetWindowText(temp);
		((CEdit*)GetDlgItem(IDC_FPGA_STARTADDR))->SetSel(2,2);  // hu 将光标设置到最后
	}

	// hu 只能输入16进制数
	len = temp.GetLength();  // hu 获得字符串长度
	temp1 = temp.Right(1);   // hu 提取最后一个字符
	temp1.MakeLower();       // hu 转化为小写字母
	if ((len>2) && !((temp1>=_T("0")&&temp1<=_T("9"))||(temp1>=_T("a")&&temp1<=_T("f"))))
	{
		temp.Delete(len-1,1);
		GetDlgItem(IDC_FPGA_STARTADDR)->SetWindowText(temp);
		((CEdit*)GetDlgItem(IDC_FPGA_STARTADDR))->SetSel(len-1,len-1);  // hu 将光标设置到最后
	}
	GetDlgItem(IDC_FPGA_ENDADDR)->SetWindowText(temp);
}

void CDialogFPGA::OnEnSetfocusFpgaEndaddr()
{
	// TODO: Add your control notification handler code here
	CString temp1,temp2;
	GetDlgItem(IDC_FPGA_STARTADDR)->GetWindowText(temp1);
	GetDlgItem(IDC_FPGA_ENDADDR)->GetWindowText(temp2);
	if (!temp1.CompareNoCase(temp2))
	{
		GetDlgItem(IDC_FPGA_ENDADDR)->SetWindowText(_T("0x"));
	}
}

void CDialogFPGA::OnEnChangeFpgaEndaddr()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	CString temp,temp1;
	int len;

	GetDlgItem(IDC_FPGA_ENDADDR)->GetWindowText(temp);

	if (temp.Find(_T("0x"))) // hu 0x必须存在且在最开始
	{
		GetDlgItem(IDC_FPGA_ENDADDR)->SetWindowText(_T("0x"));
		((CEdit*)GetDlgItem(IDC_FPGA_ENDADDR))->SetSel(2,2);  // hu 将光标设置到最后
	}

	// hu 只能输入16进制数
	len = temp.GetLength();  // hu 获得字符串长度
	temp1 = temp.Right(1);   // hu 提取最后一个字符
	temp1.MakeLower();       // hu 转化为小写字母
	if ((len>2) && !((temp1>=_T("0")&&temp1<=_T("9"))||(temp1>=_T("a")&&temp1<=_T("f"))))
	{
		temp.Delete(len-1,1);
		GetDlgItem(IDC_FPGA_ENDADDR)->SetWindowText(temp);
		((CEdit*)GetDlgItem(IDC_FPGA_ENDADDR))->SetSel(len-1,len-1);  // hu 将光标设置到最后
	}
}

BOOL CDialogFPGA::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if(pMsg->message == WM_MOUSEMOVE)  // hu 显示气泡提示
	{
		if (pMsg->hwnd == GetDlgItem(IDC_FPGA_STARTADDR)->m_hWnd)
		{
			Bubble_Tip_FPGA.AddTool(GetDlgItem(IDC_FPGA_STARTADDR),_T("FPGA起始地址"));//为此控件添加tip
			Bubble_Tip_FPGA.RelayEvent(pMsg);
		}
		else if (pMsg->hwnd == GetDlgItem(IDC_FPGA_ENDADDR)->m_hWnd)
		{
			Bubble_Tip_FPGA.AddTool(GetDlgItem(IDC_FPGA_ENDADDR),_T("FPGA结束地址\r\n若与FPGA起始地址相等，则向该地址写入数据"));//为此控件添加tip
			Bubble_Tip_FPGA.RelayEvent(pMsg);
		}
		else if (pMsg->hwnd == GetDlgItem(IDC_FIX_SELECT)->m_hWnd)
		{
			Bubble_Tip_FPGA.AddTool(GetDlgItem(IDC_FIX_SELECT),_T("向FPGA地址范围内写入固定值"));//为此控件添加tip
			Bubble_Tip_FPGA.RelayEvent(pMsg);
		}
		else if (pMsg->hwnd == GetDlgItem(IDC_SUM_SELECT)->m_hWnd)
		{
			Bubble_Tip_FPGA.AddTool(GetDlgItem(IDC_SUM_SELECT),_T("向FPGA地址范围内写入累加值"));//为此控件添加tip
			Bubble_Tip_FPGA.RelayEvent(pMsg);
		}
		else if (pMsg->hwnd == GetDlgItem(IDC_DATA_FPGA)->m_hWnd)
		{
			Bubble_Tip_FPGA.AddTool(GetDlgItem(IDC_DATA_FPGA),_T("输入十进制正整数\r\n若选择累加值，则从该值开始依次累加"));//为此控件添加tip
			Bubble_Tip_FPGA.RelayEvent(pMsg);
		}
		else if (pMsg->hwnd == GetDlgItem(IDC_SEND_FPGA)->m_hWnd)
		{
			Bubble_Tip_FPGA.AddTool(GetDlgItem(IDC_SEND_FPGA),_T("将数据写入到指定FPGA地址"));//为此控件添加tip
			Bubble_Tip_FPGA.RelayEvent(pMsg);
		}
		else if (pMsg->hwnd == GetDlgItem(IDC_REPEAT_FPGADATA)->m_hWnd)
		{
			Bubble_Tip_FPGA.AddTool(GetDlgItem(IDC_REPEAT_FPGADATA),_T("写入一个0--99的整数"));//为此控件添加tip
			Bubble_Tip_FPGA.RelayEvent(pMsg);
		}
	}

	if(pMsg->message==WM_KEYDOWN&&pMsg->wParam==VK_RETURN)   // hu 屏蔽ENTER  
		return TRUE; 
	if(pMsg->message==WM_KEYDOWN&&pMsg->wParam==VK_ESCAPE)   // hu 屏蔽ESC
		return TRUE;
	return CDialog::PreTranslateMessage(pMsg);
}

void CDialogFPGA::OnBnClickedSendFpga()
{
	// TODO: Add your control notification handler code here
	CString addr_start_str,addr_end_str,data_str,repeat_count_str,status_show;
	UINT addr_start,addr_end,data,repeat_count,data_add,idx;
	int nLower,nUpper;

	Progress_FPGA.GetRange(nLower,nUpper); // hu 获取进度条的位置范围的最大值和最小值
	Progress_FPGA.SetPos(nLower);

	GetDlgItem(IDC_FPGA_STARTADDR)->GetWindowText(addr_start_str);
	GetDlgItem(IDC_FPGA_ENDADDR)->GetWindowText(addr_end_str);
	if ("0x" == addr_start_str || "0x" == addr_end_str)
	{
		AfxMessageBox(_T("地址栏不能为空!"));
		return;
	}

	addr_start_str.Delete(0,2);
	_stscanf_s(addr_start_str.GetBuffer(0),_T("%x"),&addr_start);
	addr_end_str.Delete(0,2);
	_stscanf_s(addr_end_str.GetBuffer(0),_T("%x"),&addr_end);
	if (addr_end < addr_start)
	{
		AfxMessageBox(_T("输入的FPGA结束地址不能小于FPGA起始地址!"));
		return;
	}

	GetDlgItem(IDC_DATA_FPGA)->GetWindowText(data_str);
	if ("" == data_str)
	{
		AfxMessageBox(_T("数据栏不能为空!"));
		return;
	}
	_stscanf_s(data_str.GetBuffer(0),_T("%d"),&data);

	GetDlgItem(IDC_REPEAT_FPGADATA)->GetWindowText(repeat_count_str);
	if ("" == repeat_count_str)
	{
		AfxMessageBox(_T("写入次数不能为空!"));
		return;
	}
	_stscanf_s(repeat_count_str.GetBuffer(0),_T("%d"),&repeat_count);

	data_add = ((CButton *)GetDlgItem(IDC_SUM_SELECT))->GetCheck();// hu 返回1表示选上，0表示没选上

	status_show.Format(L"设备%d:\r\n开始写入...\r\n起始地址：",((CPCIDriverTestDlg*)AfxGetMainWnd())->deviceX);
	status_show.Format(_T("%s0x%x: %dx %d"),status_show,addr_start,repeat_count,data);
	((CPCIDriverTestDlg*)AfxGetMainWnd())->GetDlgItem(IDC_SHOW)->SetWindowText(status_show);

	for (UINT ii=addr_start;ii<=addr_end;ii++)
	{
#ifndef DEBUG_HU
		WriteToDSP(((CPCIDriverTestDlg*)AfxGetMainWnd())->deviceX,3,ii*addrOffset4FPGA,data,repeat_count);
#endif
		data += data_add;
		Progress_FPGA.SetPos((ii-addr_start+1)*100/(addr_end-addr_start+1));
	}

	status_show += "\r\n结束地址：";
	status_show.Format(_T("%s0x%x: %dx %d"),status_show,addr_end,repeat_count,data-data_add);
	status_show += "\r\n写入成功!";
	((CPCIDriverTestDlg*)AfxGetMainWnd())->GetDlgItem(IDC_SHOW)->SetWindowText(status_show);

	// hu tab页之间显示 ((CPCIDriverTestDlg*)AfxGetMainWnd())->Bar_DSP.GetDlgItem(IDC_DSP_STARTADDR)->SetWindowText(data_str);
}

HBRUSH CDialogFPGA::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Change any attributes of the DC here
	// hu 若结束地址小于起始地址，将字体设为红色
	if(nCtlColor==CTLCOLOR_EDIT && pWnd->GetDlgCtrlID()==IDC_FPGA_ENDADDR)//注意此处的（pWnd->），否则没效果
	{
		CString addr_temp;
		UINT addr_start,addr_end;

		GetDlgItem(IDC_FPGA_STARTADDR)->GetWindowText(addr_temp);
		addr_temp.Delete(0,2);
		_stscanf_s(addr_temp.GetBuffer(0),_T("%x"),&addr_start);
		GetDlgItem(IDC_FPGA_ENDADDR)->GetWindowText(addr_temp);
		addr_temp.Delete(0,2);
		_stscanf_s(addr_temp.GetBuffer(0),_T("%x"),&addr_end);

		if (addr_end>=addr_start)
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

void CDialogFPGA::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
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
