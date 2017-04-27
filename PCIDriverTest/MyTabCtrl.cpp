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
	// hu ������ˢ
	CBrush      cbr;
	cbr.CreateSolidBrush(RGB(255,236,139));

	// hu ��ȡѡ���������
	char        szTabText[100];
	TC_ITEM     tci;
	memset(szTabText, '\0', sizeof(szTabText));
	tci.mask        = TCIF_TEXT;
	tci.pszText     = (LPWSTR)szTabText;
	tci.cchTextMax  = sizeof(szTabText)-1;
	GetItem(lpDrawItemStruct->itemID, &tci);
	// hu ���ѡ�����
	CDC *dc = CDC::FromHandle(lpDrawItemStruct->hDC);
	dc->FillRect(&lpDrawItemStruct->rcItem, &cbr);

	// hu ����ѡ�����
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
	// hu ��ȡ�ؼ�����
	CRect   rect;
	GetClientRect(&rect);

	// hu ������ˢ
	CBrush   brush;
	brush.CreateSolidBrush(RGB(84,255,159));

	// hu ���ؼ�����
	pDC-> FillRect(&rect,&brush);

	//return CTabCtrl::OnEraseBkgnd(pDC);
	return true;
}
