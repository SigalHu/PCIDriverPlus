/*
 * 实现界面打开关闭特效
 *
 * @author 胡续俊
 * @date 2015/12/21
 */

#if !defined(AFX_DLGFLASH_H__FC9C5DF7_7144_4F99_9B5A_0C5451DEA813__INCLUDED_)
#define AFX_DLGFLASH_H__FC9C5DF7_7144_4F99_9B5A_0C5451DEA813__INCLUDED_
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDlgFlash
{
public:
       CDlgFlash(CDialog* Dlg);                 // hu 参数为将要实现动画效果的对话框
       CDlgFlash(CDialog* Dlg,int Timer1_ID,int Timer2_ID,int Timer3_ID,int Timer4_ID); //参数1为将要实现动画效果的对话框，参数二三分别为定时器1和定时器2的ID
       virtual ~CDlgFlash();
       void StartFlashDestroyDlg();             // hu 开始启动销毁效果
	   void StartFlashCR2NCRDlg();              // hu 开启窗口从透明变为不透明渐变效果
	   void StartFlashNCR2CRDlg();              // hu 开启窗口从不透明变为透明渐变效果
       void OnTimer(UINT nIDEvent);             // hu 处理对应定时器的任务，在后面设置时钟时为SetTimer(TimerID,10,NULL)
                                                // hu 没有设置为自定义执行函数的原因是一定要在需要动画效果的窗口中重写从CDialog中继承的OnClose()函数用于映射销毁窗口效果用之
       void SetFlashCreateSpeed(int xSpeed,int ySpeed);   // hu 设置创建窗口变化速度
                                                          // hu 参数x表示窗口宽度的变化速度
                                                          // hu 参数y表示窗口高度的变化速度
       void SetFlashDestroySpeed(int xSpeed,int ySpeed);  // hu 设置销毁窗口变化速度
	                                                      // hu 参数x表示窗口宽度的变化速度
                                                          // hu 参数y表示窗口高度的变化速度
	   void SetFlashCR2NCRSpeed(BYTE Speed);    // hu 设置窗口从透明变为不透明的变化速度
	   void SetFlashNCR2CRSpeed(BYTE Speed);    // hu 设置窗口从不透明变为透明的变化速度
protected:
       int xCreateSpeed;                        // hu 创建窗口时窗口宽度的变化速度
       int yCreateSpeed;                        // hu 创建窗口时窗口高度的变化速度
       int xDestorySpeed;                       // hu 销毁窗口时窗口宽度的变化速度                  
       int yDestorySpeed;                       // hu 创建窗口时窗口高度的变化速度
	   BYTE CR2NCRSpeed;                         // hu 从透明变为不透明的变化速度
	   BYTE NCR2CRSpeed;                         // hu 从不透明变为透明的变化速度
       int OriginalWidth,OriginalHeight;        // hu 窗口的原始大小
       int DestoryPosX;                         // hu 销毁窗口时窗口的左上角x轴位置
       int DestoryPosY;                         // hu 销毁窗口时窗口的左上角y轴位置
       int Timer1_ID;                           // hu 定时器1ID，创建窗口时用之
       int Timer2_ID;                           // hu 定时器2ID，销毁窗口时用之 
	   int Timer3_ID;                           // hu 定时器3ID，从透明变为不透明渐变窗口时用之 
	   int Timer4_ID;                           // hu 定时器4ID，从不透明变为透明渐变窗口时用之 
       CDialog* flashDlg;                       // hu 指向需要动画效果的窗口指针
       void InitCDlg();                         // hu 初始化，内部使用
};

#endif // !defined(AFX_DLGFLASH_H__FC9C5DF7_7144_4F99_9B5A_0C5451DEA813__INCLUDED_)