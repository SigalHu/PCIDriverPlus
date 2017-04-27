#pragma once

// CDialogDSP dialog

class CDialogDSP : public CDialog
{
	DECLARE_DYNAMIC(CDialogDSP)

public:
	CDialogDSP(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDialogDSP();

// Dialog Data
	enum { IDD = IDD_DIALOG_DSP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeDspOffsaddr();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CToolTipCtrl Bubble_Tip_DSP;
	afx_msg void OnBnClickedSendDsp();
	afx_msg void OnEnSetfocusDspOffsaddr();
	afx_msg void OnBnClickedRecvDsp();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
};
