// MyTabCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "PCIDriverTest.h"
#include "MyTabCtrl.h"


// CMyTabCtrl

IMPLEMENT_DYNAMIC(CMyTabCtrl, CTabCtrl)

CMyTabCtrl::CMyTabCtrl()
{

}

CMyTabCtrl::~CMyTabCtrl()
{
}


BEGIN_MESSAGE_MAP(CMyTabCtrl, CTabCtrl)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()



// CMyTabCtrl message handlers



void CMyTabCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{

	// TODO:  Add your code to draw the specified item
	// hu 创建画刷
	CBrush      cbr;
	cbr.CreateSolidBrush(RGB(255,236,139));

	// hu 获取选项卡文字内容
	char        szTabText[100];
	TC_ITEM     tci;
	memset(szTabText, '\0', sizeof(szTabText));
	tci.mask        = TCIF_TEXT;
	tci.pszText     = (LPWSTR)szTabText;
	tci.cchTextMax  = sizeof(szTabText)-1;
	GetItem(lpDrawItemStruct->itemID, &tci);
	// hu 填充选项卡背景
	CDC *dc = CDC::FromHandle(lpDrawItemStruct->hDC);
	dc->FillRect(&lpDrawItemStruct->rcItem, &cbr);

	// hu 绘制选项卡文字
	dc->SetBkColor(RGB(255,236,139));
	dc->SetTextColor(RGB(0,0,0));
	RECT rc;
	rc=lpDrawItemStruct->rcItem;
	rc.top+=3;
	dc->DrawText(tci.pszText,lstrlen(tci.pszText),&rc,DT_CENTER);
}

BOOL CMyTabCtrl::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	// hu 获取控件矩形
	CRect   rect;
	GetClientRect(&rect);

	// hu 创建画刷
	CBrush   brush;
	brush.CreateSolidBrush(RGB(84,255,159));

	// hu 填充控件背景
	pDC-> FillRect(&rect,&brush);

	//return CTabCtrl::OnEraseBkgnd(pDC);
	return true;
}
