// PCIDriverTest.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CPCIDriverTestApp:
// �йش����ʵ�֣������ PCIDriverTest.cpp
//

class CPCIDriverTestApp : public CWinApp
{
public:
	CPCIDriverTestApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CPCIDriverTestApp theApp;