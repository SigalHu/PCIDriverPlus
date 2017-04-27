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

	if (temp.Find(_T("0x"))) // hu 0x������������ʼ
	{
		temp = _T("0x");
		GetDlgItem(IDC_DSP_OFFSADDR)->SetWindowText(temp);
		((CEdit*)GetDlgItem(IDC_DSP_OFFSADDR))->SetSel(2,2);  // hu ��������õ����
	}

	// hu ֻ������16������
	len = temp.GetLength();  // hu ����ַ�������
	temp1 = temp.Right(1);   // hu ��ȡ���һ���ַ�
	temp1.MakeLower();       // hu ת��ΪСд��ĸ
	if ((len>2) && !((temp1>=_T("0")&&temp1<=_T("9"))||(temp1>=_T("a")&&temp1<=_T("f"))))
	{
		temp.Delete(len-1,1);
		GetDlgItem(IDC_DSP_OFFSADDR)->SetWindowText(temp);
		((CEdit*)GetDlgItem(IDC_DSP_OFFSADDR))->SetSel(len-1,len-1);  // hu ��������õ����
	}
}

BOOL CDialogDSP::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if(pMsg->message == WM_MOUSEMOVE)  // hu ��ʾ������ʾ
	{
		if (pMsg->hwnd == GetDlgItem(IDC_DSP_OFFSADDR)->m_hWnd)
		{
			Bubble_Tip_DSP.AddTool(GetDlgItem(IDC_DSP_OFFSADDR),_T("д�������DDR��L2RAM��ʼ��ַ��ƫ�Ƶ�ַ������д����ȡ����"));//Ϊ�˿ؼ����tip
			Bubble_Tip_DSP.RelayEvent(pMsg);
		}
		else if (pMsg->hwnd == GetDlgItem(IDC_DATAW_DSP)->m_hWnd)
		{
			Bubble_Tip_DSP.AddTool(GetDlgItem(IDC_DATAW_DSP),_T("��дҪ��ָ����ַд�������"));//Ϊ�˿ؼ����tip
			Bubble_Tip_DSP.RelayEvent(pMsg);
		}
		else if (pMsg->hwnd == GetDlgItem(IDC_DATAR_DSP)->m_hWnd)
		{
			Bubble_Tip_DSP.AddTool(GetDlgItem(IDC_DATAR_DSP),_T("��ʾ��ָ����ַ��ȡ������"));//Ϊ�˿ؼ����tip
			Bubble_Tip_DSP.RelayEvent(pMsg);
		}
		else if (pMsg->hwnd == GetDlgItem(IDC_SEND_DSP)->m_hWnd)
		{
			Bubble_Tip_DSP.AddTool(GetDlgItem(IDC_SEND_DSP),_T("��ָ����ַд������"));//Ϊ�˿ؼ����tip
			Bubble_Tip_DSP.RelayEvent(pMsg);
		}
		else if (pMsg->hwnd == GetDlgItem(IDC_RECV_DSP)->m_hWnd)
		{
			Bubble_Tip_DSP.AddTool(GetDlgItem(IDC_RECV_DSP),_T("��ָ����ַ��ȡ����"));//Ϊ�˿ؼ����tip
			Bubble_Tip_DSP.RelayEvent(pMsg);
		}
		else if (pMsg->hwnd == GetDlgItem(IDC_DDR_SELECT)->m_hWnd)
		{
			Bubble_Tip_DSP.AddTool(GetDlgItem(IDC_DDR_SELECT),_T("ѡ��DDR��Ϊ��д����"));//Ϊ�˿ؼ����tip
			Bubble_Tip_DSP.RelayEvent(pMsg);
		}
		else if (pMsg->hwnd == GetDlgItem(IDC_LRAM_SELECT)->m_hWnd)
		{
			Bubble_Tip_DSP.AddTool(GetDlgItem(IDC_LRAM_SELECT),_T("ѡ��L2RAM��Ϊ��д����"));//Ϊ�˿ؼ����tip
			Bubble_Tip_DSP.RelayEvent(pMsg);
		}
	}

	if(pMsg->message==WM_KEYDOWN&&pMsg->wParam==VK_RETURN)   // hu ����ENTER  
		return TRUE; 
	if(pMsg->message==WM_KEYDOWN&&pMsg->wParam==VK_ESCAPE)   // hu ����ESC
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
		AfxMessageBox(_T("ƫ�Ƶ�ַ������Ϊ��!"));
		return;
	}

	addr_offs_str.Delete(0,2);
	_stscanf_s(addr_offs_str.GetBuffer(0),_T("%x"),&addr_offs);

	GetDlgItem(IDC_DATAW_DSP)->GetWindowText(data_str);
	if ("" == data_str)
	{
		AfxMessageBox(_T("д������������Ϊ��!"));
		return;
	}
	_stscanf_s(data_str.GetBuffer(0),_T("%d"),&data);

	barx = ((CButton *)GetDlgItem(IDC_DDR_SELECT))->GetCheck();// hu ����1��ʾѡ�ϣ�0��ʾûѡ��
	if (barx)
	{
		barx += 4;
	}

	status_show.Format(L"�豸%d:\r\n��ʼд��...\r\nƫ�Ƶ�ַ��",((CPCIDriverTestDlg*)AfxGetMainWnd())->deviceX);
	status_show.Format(_T("%s0x%s: %d"),status_show,addr_offs_str,data);
	((CPCIDriverTestDlg*)AfxGetMainWnd())->GetDlgItem(IDC_SHOW)->SetWindowText(status_show);

#ifndef DEBUG_HU
	WriteToDSP(((CPCIDriverTestDlg*)AfxGetMainWnd())->deviceX,barx,addr_offs,data);
#endif

	status_show += "\r\nд��ɹ�!";
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
		AfxMessageBox(_T("ƫ�Ƶ�ַ������Ϊ��!"));
		return;
	}

	addr_offs_str.Delete(0,2);
	_stscanf_s(addr_offs_str.GetBuffer(0),_T("%x"),&addr_offs);

	barx = ((CButton *)GetDlgItem(IDC_DDR_SELECT))->GetCheck();// hu ����1��ʾѡ�ϣ�0��ʾûѡ��
	if (barx)
	{
		barx += 4;
	}

	status_show.Format(L"�豸%d:\r\n��ʼ��ȡ...",((CPCIDriverTestDlg*)AfxGetMainWnd())->deviceX);
	((CPCIDriverTestDlg*)AfxGetMainWnd())->GetDlgItem(IDC_SHOW)->SetWindowText(status_show);

#ifdef DEBUG_HU
	data = 0;
#else
	ReadFromDSP(((CPCIDriverTestDlg*)AfxGetMainWnd())->deviceX,barx,addr_offs,&data);
#endif

	data_str.Format(_T("%d"),data);
	GetDlgItem(IDC_DATAR_DSP)->SetWindowText(data_str);
	status_show += "\r\nƫ�Ƶ�ַ��";
	status_show.Format(_T("%s0x%s: %s"),status_show,addr_offs_str,data_str);
	status_show += "\r\n��ȡ�ɹ�!";
	((CPCIDriverTestDlg*)AfxGetMainWnd())->GetDlgItem(IDC_SHOW)->SetWindowText(status_show);
}

HBRUSH CDialogDSP::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Change any attributes of the DC here
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

void CDialogDSP::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
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
