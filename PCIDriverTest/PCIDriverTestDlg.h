// PCIDriverTestDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"
#include "DialogFPGA.h"
#include "DialogDSP.h"
#include "MyTabCtrl.h"
#include "CDlgFlash.h"
#include "DSPDriverDataDef.h"
#include "PciDriverFun.h"
#include "afxwin.h"


// CPCIDriverTestDlg �Ի���
class CPCIDriverTestDlg : public CDialog
{
// ����
public:
	CPCIDriverTestDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_PCI6455DRIVERTEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
//	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg LRESULT OnCALLBACKMESSAGE_0(WPARAM wParam, LPARAM lParam); // hu ���ɵ���Ϣӳ�亯��
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedSelectBin();
	afx_msg void OnBnClickedSendData();
	afx_msg void OnBnClickedFpgaIni();
	afx_msg void OnTcnSelchangeBarWr(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeShowBin();
	afx_msg void OnEnChangeFpgaAddr();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnTimer(UINT_PTR nIDEvent);    // hu ��Ӷ�ʱ����Ϣӳ�亯��
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CMyTabCtrl Bar_wr;
	CDialogFPGA Bar_FPGA;
	CDialogDSP Bar_DSP;
	CToolTipCtrl Bubble_Tip;
	DSPDriver dspDriver[DEVNUM_MAX];
	dmaDeliver dmaSource0;
	CDlgFlash *MainDlgFlash;
	static int deviceX;
	static int deviceNum;
	CListCtrl dataList;
	int e_Item;    //�ձ༭����  
	int e_SubItem; //�ձ༭����  
	CEdit m_Edit;  //���ɵ�Ԫ�༭�����
	bool haveeditcreate;//��־�༭���Ѿ�������
	void createEdit(NM_LISTVIEW  *pEditCtrl, CEdit *createdit, int &Item, int &SubItem, bool &havecreat);//������Ԫ��༭����
	void distroyEdit(CListCtrl *list, CEdit* distroyedit, int &Item, int &SubItem);//���ٵ�Ԫ��༭�����
	afx_msg void OnNMDblclkListData(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickListData(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRclickListData(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedListData(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCbnSelchangeDeviceSelect();
	afx_msg void OnListAdd();
	afx_msg void OnListDelete();
	afx_msg void OnListSave();
	afx_msg void OnListUse();
};
