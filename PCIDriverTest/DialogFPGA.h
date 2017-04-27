#pragma once
#include "afxcmn.h"

// CDialogFPGA dialog

class CDialogFPGA : public CDialog
{
	DECLARE_DYNAMIC(CDialogFPGA)

public:
	CDialogFPGA(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDialogFPGA();

// Dialog Data
	enum { IDD = IDD_DIALOG_FPGA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeFpgaStartaddr();
	afx_msg void OnEnSetfocusFpgaEndaddr();
	afx_msg void OnEnChangeFpgaEndaddr();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedSendFpga();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	CToolTipCtrl Bubble_Tip_FPGA;
	CProgressCtrl Progress_FPGA;
};
