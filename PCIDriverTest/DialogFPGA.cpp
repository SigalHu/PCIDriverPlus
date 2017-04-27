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
	
	if (temp.Find(_T("0x"))) // hu 0x������������ʼ
	{
		temp = _T("0x");
		GetDlgItem(IDC_FPGA_STARTADDR)->SetWindowText(temp);
		((CEdit*)GetDlgItem(IDC_FPGA_STARTADDR))->SetSel(2,2);  // hu ��������õ����
	}

	// hu ֻ������16������
	len = temp.GetLength();  // hu ����ַ�������
	temp1 = temp.Right(1);   // hu ��ȡ���һ���ַ�
	temp1.MakeLower();       // hu ת��ΪСд��ĸ
	if ((len>2) && !((temp1>=_T("0")&&temp1<=_T("9"))||(temp1>=_T("a")&&temp1<=_T("f"))))
	{
		temp.Delete(len-1,1);
		GetDlgItem(IDC_FPGA_STARTADDR)->SetWindowText(temp);
		((CEdit*)GetDlgItem(IDC_FPGA_STARTADDR))->SetSel(len-1,len-1);  // hu ��������õ����
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

	if (temp.Find(_T("0x"))) // hu 0x������������ʼ
	{
		GetDlgItem(IDC_FPGA_ENDADDR)->SetWindowText(_T("0x"));
		((CEdit*)GetDlgItem(IDC_FPGA_ENDADDR))->SetSel(2,2);  // hu ��������õ����
	}

	// hu ֻ������16������
	len = temp.GetLength();  // hu ����ַ�������
	temp1 = temp.Right(1);   // hu ��ȡ���һ���ַ�
	temp1.MakeLower();       // hu ת��ΪСд��ĸ
	if ((len>2) && !((temp1>=_T("0")&&temp1<=_T("9"))||(temp1>=_T("a")&&temp1<=_T("f"))))
	{
		temp.Delete(len-1,1);
		GetDlgItem(IDC_FPGA_ENDADDR)->SetWindowText(temp);
		((CEdit*)GetDlgItem(IDC_FPGA_ENDADDR))->SetSel(len-1,len-1);  // hu ��������õ����
	}
}

BOOL CDialogFPGA::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if(pMsg->message == WM_MOUSEMOVE)  // hu ��ʾ������ʾ
	{
		if (pMsg->hwnd == GetDlgItem(IDC_FPGA_STARTADDR)->m_hWnd)
		{
			Bubble_Tip_FPGA.AddTool(GetDlgItem(IDC_FPGA_STARTADDR),_T("FPGA��ʼ��ַ"));//Ϊ�˿ؼ����tip
			Bubble_Tip_FPGA.RelayEvent(pMsg);
		}
		else if (pMsg->hwnd == GetDlgItem(IDC_FPGA_ENDADDR)->m_hWnd)
		{
			Bubble_Tip_FPGA.AddTool(GetDlgItem(IDC_FPGA_ENDADDR),_T("FPGA������ַ\r\n����FPGA��ʼ��ַ��ȣ�����õ�ַд������"));//Ϊ�˿ؼ����tip
			Bubble_Tip_FPGA.RelayEvent(pMsg);
		}
		else if (pMsg->hwnd == GetDlgItem(IDC_FIX_SELECT)->m_hWnd)
		{
			Bubble_Tip_FPGA.AddTool(GetDlgItem(IDC_FIX_SELECT),_T("��FPGA��ַ��Χ��д��̶�ֵ"));//Ϊ�˿ؼ����tip
			Bubble_Tip_FPGA.RelayEvent(pMsg);
		}
		else if (pMsg->hwnd == GetDlgItem(IDC_SUM_SELECT)->m_hWnd)
		{
			Bubble_Tip_FPGA.AddTool(GetDlgItem(IDC_SUM_SELECT),_T("��FPGA��ַ��Χ��д���ۼ�ֵ"));//Ϊ�˿ؼ����tip
			Bubble_Tip_FPGA.RelayEvent(pMsg);
		}
		else if (pMsg->hwnd == GetDlgItem(IDC_DATA_FPGA)->m_hWnd)
		{
			Bubble_Tip_FPGA.AddTool(GetDlgItem(IDC_DATA_FPGA),_T("����ʮ����������\r\n��ѡ���ۼ�ֵ����Ӹ�ֵ��ʼ�����ۼ�"));//Ϊ�˿ؼ����tip
			Bubble_Tip_FPGA.RelayEvent(pMsg);
		}
		else if (pMsg->hwnd == GetDlgItem(IDC_SEND_FPGA)->m_hWnd)
		{
			Bubble_Tip_FPGA.AddTool(GetDlgItem(IDC_SEND_FPGA),_T("������д�뵽ָ��FPGA��ַ"));//Ϊ�˿ؼ����tip
			Bubble_Tip_FPGA.RelayEvent(pMsg);
		}
		else if (pMsg->hwnd == GetDlgItem(IDC_REPEAT_FPGADATA)->m_hWnd)
		{
			Bubble_Tip_FPGA.AddTool(GetDlgItem(IDC_REPEAT_FPGADATA),_T("д��һ��0--99������"));//Ϊ�˿ؼ����tip
			Bubble_Tip_FPGA.RelayEvent(pMsg);
		}
	}

	if(pMsg->message==WM_KEYDOWN&&pMsg->wParam==VK_RETURN)   // hu ����ENTER  
		return TRUE; 
	if(pMsg->message==WM_KEYDOWN&&pMsg->wParam==VK_ESCAPE)   // hu ����ESC
		return TRUE;
	return CDialog::PreTranslateMessage(pMsg);
}

void CDialogFPGA::OnBnClickedSendFpga()
{
	// TODO: Add your control notification handler code here
	CString addr_start_str,addr_end_str,data_str,repeat_count_str,status_show;
	UINT addr_start,addr_end,data,repeat_count,data_add,idx;
	int nLower,nUpper;

	Progress_FPGA.GetRange(nLower,nUpper); // hu ��ȡ��������λ�÷�Χ�����ֵ����Сֵ
	Progress_FPGA.SetPos(nLower);

	GetDlgItem(IDC_FPGA_STARTADDR)->GetWindowText(addr_start_str);
	GetDlgItem(IDC_FPGA_ENDADDR)->GetWindowText(addr_end_str);
	if ("0x" == addr_start_str || "0x" == addr_end_str)
	{
		AfxMessageBox(_T("��ַ������Ϊ��!"));
		return;
	}

	addr_start_str.Delete(0,2);
	_stscanf_s(addr_start_str.GetBuffer(0),_T("%x"),&addr_start);
	addr_end_str.Delete(0,2);
	_stscanf_s(addr_end_str.GetBuffer(0),_T("%x"),&addr_end);
	if (addr_end < addr_start)
	{
		AfxMessageBox(_T("�����FPGA������ַ����С��FPGA��ʼ��ַ!"));
		return;
	}

	GetDlgItem(IDC_DATA_FPGA)->GetWindowText(data_str);
	if ("" == data_str)
	{
		AfxMessageBox(_T("����������Ϊ��!"));
		return;
	}
	_stscanf_s(data_str.GetBuffer(0),_T("%d"),&data);

	GetDlgItem(IDC_REPEAT_FPGADATA)->GetWindowText(repeat_count_str);
	if ("" == repeat_count_str)
	{
		AfxMessageBox(_T("д���������Ϊ��!"));
		return;
	}
	_stscanf_s(repeat_count_str.GetBuffer(0),_T("%d"),&repeat_count);

	data_add = ((CButton *)GetDlgItem(IDC_SUM_SELECT))->GetCheck();// hu ����1��ʾѡ�ϣ�0��ʾûѡ��

	status_show.Format(L"�豸%d:\r\n��ʼд��...\r\n��ʼ��ַ��",((CPCIDriverTestDlg*)AfxGetMainWnd())->deviceX);
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

	status_show += "\r\n������ַ��";
	status_show.Format(_T("%s0x%x: %dx %d"),status_show,addr_end,repeat_count,data-data_add);
	status_show += "\r\nд��ɹ�!";
	((CPCIDriverTestDlg*)AfxGetMainWnd())->GetDlgItem(IDC_SHOW)->SetWindowText(status_show);

	// hu tabҳ֮����ʾ ((CPCIDriverTestDlg*)AfxGetMainWnd())->Bar_DSP.GetDlgItem(IDC_DSP_STARTADDR)->SetWindowText(data_str);
}

HBRUSH CDialogFPGA::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Change any attributes of the DC here
	// hu ��������ַС����ʼ��ַ����������Ϊ��ɫ
	if(nCtlColor==CTLCOLOR_EDIT && pWnd->GetDlgCtrlID()==IDC_FPGA_ENDADDR)//ע��˴��ģ�pWnd->��������ûЧ��
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

void CDialogFPGA::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
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
