
#include "../stdafx.h"
#include "CDlgFlash.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CDlgFlash::CDlgFlash(CDialog* Dlg)
{
	flashDlg=Dlg;
	Timer1_ID = 9;
	Timer2_ID = 10;
	Timer3_ID = 11;
	Timer4_ID = 12;
	InitCDlg();
}

CDlgFlash::CDlgFlash(CDialog* Dlg,int Timer1_ID,int Timer2_ID,int Timer3_ID,int Timer4_ID)
{
	flashDlg=Dlg;
	this->Timer1_ID = Timer1_ID;
	this->Timer2_ID = Timer2_ID;
	this->Timer3_ID = Timer3_ID;
	this->Timer4_ID = Timer4_ID;
	InitCDlg();
}

CDlgFlash::~CDlgFlash()
{
}

 

void CDlgFlash::InitCDlg()
{
	// hu 获得窗口预设的大小
	CRect dlgRect;
	flashDlg->GetWindowRect(dlgRect);

	// hu 获取屏幕大小
	CRect desktopRect;
	flashDlg->GetDesktopWindow()->GetWindowRect(desktopRect);

	// hu 将窗口开始大小设为0，居中
	flashDlg->MoveWindow(dlgRect.left+dlgRect.Width()/2,dlgRect.top+dlgRect.Height()/2,0,0);

	// hu 数据成员初始化
	OriginalWidth=dlgRect.Width();
	OriginalHeight=dlgRect.Height();
	xCreateSpeed = 2;
	yCreateSpeed = 3;
	xDestorySpeed = 2;
	yDestorySpeed = 3;
	CR2NCRSpeed = 10;
	NCR2CRSpeed = 10;

	// hu 设定定时器Timer1_ID     

	flashDlg->SetTimer(Timer1_ID,10,NULL);
}

void CDlgFlash::OnTimer(UINT nIDEvent)
{
	if (nIDEvent == Timer1_ID)   // hu 如果是窗口弹出过程，窗口居中，逐渐增大窗口
	{
		// hu 获得此时窗口的实际大小
		CRect dlgRect;
		flashDlg->GetWindowRect(&dlgRect);

		// hu 获得桌面的大小
		CRect desktopRect;
		flashDlg->GetDesktopWindow()->GetWindowRect(desktopRect);

		flashDlg->MoveWindow((desktopRect.Width()-dlgRect.Width())/2,
			(desktopRect.Height()-dlgRect.Height())/2,
			xCreateSpeed+dlgRect.Width(),
			yCreateSpeed+dlgRect.Height());

		// hu 不要超过窗口预设的宽度
		if(dlgRect.Width() >=OriginalWidth)
			xCreateSpeed=0;

		// hu 不要超过窗口预设的高度
		if(dlgRect.Height() >=OriginalHeight)
			yCreateSpeed=0;

		// hu 停止变化，关闭定时器Timer1_ID
		if((dlgRect.Width() >=OriginalWidth) && (dlgRect.Height() >=OriginalHeight))
			flashDlg->KillTimer(Timer1_ID);
	}
	else if (nIDEvent == Timer2_ID)  // hu 如果是窗口关闭过程，则逐渐缩小窗口
	{
		// hu 获得此时窗口的实际大小
		CRect dlgRect;
		flashDlg->GetWindowRect(&dlgRect);

		flashDlg->MoveWindow(DestoryPosX,DestoryPosY,dlgRect.Width()-xDestorySpeed,dlgRect.Height()-yDestorySpeed);

		// hu 当高度等于零后高度就不在变化
		//if(dlgRect.Height() <=0)
		// hu 停止变化，关闭定时器Timer2_ID，并且关闭窗口
		if(dlgRect.Height() <=0)
		{
			yDestorySpeed=0;
			flashDlg->KillTimer(Timer1_ID);
			flashDlg->DestroyWindow();
		}
	} 
	else if (nIDEvent == Timer3_ID)  // hu 窗口从透明变为不透明
	{
		BYTE bAlpha = 0;
		flashDlg->GetLayeredWindowAttributes(0,&bAlpha,NULL);
		bAlpha += CR2NCRSpeed;
		if(bAlpha > 255-CR2NCRSpeed)
		{
			flashDlg->SetLayeredWindowAttributes(0,255,LWA_ALPHA);  
			//flashDlg->GetLayeredWindowAttributes(0,&bAlpha,&pdwFlags);
			flashDlg->KillTimer(nIDEvent);
		}
		else
		{
			flashDlg->SetLayeredWindowAttributes(0,bAlpha,LWA_ALPHA);
		}
	}
	else if (nIDEvent == Timer4_ID)  // hu 窗口从不透明变为透明
	{
		BYTE bAlpha = 255;
		flashDlg->GetLayeredWindowAttributes(0,&bAlpha,NULL);
		bAlpha -= NCR2CRSpeed;
		if(bAlpha < NCR2CRSpeed)
		{
			flashDlg->SetLayeredWindowAttributes(0,0,LWA_ALPHA);    
			flashDlg->KillTimer(nIDEvent);
			flashDlg->DestroyWindow();
		}
		else
		{
			flashDlg->SetLayeredWindowAttributes(0,bAlpha,LWA_ALPHA);
		}
	}
}

void CDlgFlash::StartFlashDestroyDlg()
{
	CRect dlgRect;

	// hu 获取窗口当前的位置并保存
	flashDlg->GetWindowRect(&dlgRect);
	DestoryPosX=dlgRect.left;
	DestoryPosY=dlgRect.top;

	// hu 开始启动定时器Timer2_ID
	flashDlg->SetTimer(Timer2_ID,10,NULL);
}

void CDlgFlash::StartFlashCR2NCRDlg()
{
	UINT a = GetWindowLong(flashDlg->GetSafeHwnd(),GWL_EXSTYLE);
	SetWindowLong(flashDlg->GetSafeHwnd(),GWL_EXSTYLE,GetWindowLong(flashDlg->GetSafeHwnd(),GWL_EXSTYLE)|WS_EX_LAYERED);
	UINT b = GetWindowLong(flashDlg->GetSafeHwnd(),GWL_EXSTYLE);

	// hu 开始启动定时器Timer3_ID
	flashDlg->SetTimer(Timer3_ID,100,NULL);
}

void CDlgFlash::StartFlashNCR2CRDlg()
{
	//UINT a = GetWindowLong(flashDlg->GetSafeHwnd(),GWL_EXSTYLE);
	SetWindowLong(flashDlg->GetSafeHwnd(),GWL_EXSTYLE,GetWindowLong(flashDlg->GetSafeHwnd(),GWL_EXSTYLE)|WS_EX_LAYERED);

	// hu 开始启动定时器Timer4_ID
	flashDlg->SetTimer(Timer4_ID,100,NULL);
}
 
void CDlgFlash::SetFlashCreateSpeed(int xSpeed,int ySpeed)
{
	xCreateSpeed = xSpeed;
	xCreateSpeed = ySpeed;
}

 

void CDlgFlash::SetFlashDestroySpeed(int xSpeed,int ySpeed)
{
	xDestorySpeed = xSpeed;
    yDestorySpeed = ySpeed;
}

void CDlgFlash::SetFlashCR2NCRSpeed(BYTE Speed)
{
	CR2NCRSpeed = Speed;
}

void CDlgFlash::SetFlashNCR2CRSpeed(BYTE Speed)
{
	NCR2CRSpeed = Speed;
}