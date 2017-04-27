// PCIDriverTestDlg.h : 头文件
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


// CPCIDriverTestDlg 对话框
class CPCIDriverTestDlg : public CDialog
{
// 构造
public:
	CPCIDriverTestDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_PCI6455DRIVERTEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
//	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg LRESULT OnCALLBACKMESSAGE_0(WPARAM wParam, LPARAM lParam); // hu 生成的消息映射函数
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedSelectBin();
	afx_msg void OnBnClickedSendData();
	afx_msg void OnBnClickedFpgaIni();
	afx_msg void OnTcnSelchangeBarWr(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeShowBin();
	afx_msg void OnEnChangeFpgaAddr();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnTimer(UINT_PTR nIDEvent);    // hu 添加定时器消息映射函数
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
	int e_Item;    //刚编辑的行  
	int e_SubItem; //刚编辑的列  
	CEdit m_Edit;  //生成单元编辑框对象
	bool haveeditcreate;//标志编辑框已经被创建
	void createEdit(NM_LISTVIEW  *pEditCtrl, CEdit *createdit, int &Item, int &SubItem, bool &havecreat);//创建单元格编辑框函数
	void distroyEdit(CListCtrl *list, CEdit* distroyedit, int &Item, int &SubItem);//销毁单元格编辑框对象
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
