/*
 * ʵ�ֽ���򿪹ر���Ч
 *
 * @author ������
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
       CDlgFlash(CDialog* Dlg);                 // hu ����Ϊ��Ҫʵ�ֶ���Ч���ĶԻ���
       CDlgFlash(CDialog* Dlg,int Timer1_ID,int Timer2_ID,int Timer3_ID,int Timer4_ID); //����1Ϊ��Ҫʵ�ֶ���Ч���ĶԻ��򣬲��������ֱ�Ϊ��ʱ��1�Ͷ�ʱ��2��ID
       virtual ~CDlgFlash();
       void StartFlashDestroyDlg();             // hu ��ʼ��������Ч��
	   void StartFlashCR2NCRDlg();              // hu �������ڴ�͸����Ϊ��͸������Ч��
	   void StartFlashNCR2CRDlg();              // hu �������ڴӲ�͸����Ϊ͸������Ч��
       void OnTimer(UINT nIDEvent);             // hu �����Ӧ��ʱ���������ں�������ʱ��ʱΪSetTimer(TimerID,10,NULL)
                                                // hu û������Ϊ�Զ���ִ�к�����ԭ����һ��Ҫ����Ҫ����Ч���Ĵ�������д��CDialog�м̳е�OnClose()��������ӳ�����ٴ���Ч����֮
       void SetFlashCreateSpeed(int xSpeed,int ySpeed);   // hu ���ô������ڱ仯�ٶ�
                                                          // hu ����x��ʾ���ڿ�ȵı仯�ٶ�
                                                          // hu ����y��ʾ���ڸ߶ȵı仯�ٶ�
       void SetFlashDestroySpeed(int xSpeed,int ySpeed);  // hu �������ٴ��ڱ仯�ٶ�
	                                                      // hu ����x��ʾ���ڿ�ȵı仯�ٶ�
                                                          // hu ����y��ʾ���ڸ߶ȵı仯�ٶ�
	   void SetFlashCR2NCRSpeed(BYTE Speed);    // hu ���ô��ڴ�͸����Ϊ��͸���ı仯�ٶ�
	   void SetFlashNCR2CRSpeed(BYTE Speed);    // hu ���ô��ڴӲ�͸����Ϊ͸���ı仯�ٶ�
protected:
       int xCreateSpeed;                        // hu ��������ʱ���ڿ�ȵı仯�ٶ�
       int yCreateSpeed;                        // hu ��������ʱ���ڸ߶ȵı仯�ٶ�
       int xDestorySpeed;                       // hu ���ٴ���ʱ���ڿ�ȵı仯�ٶ�                  
       int yDestorySpeed;                       // hu ��������ʱ���ڸ߶ȵı仯�ٶ�
	   BYTE CR2NCRSpeed;                         // hu ��͸����Ϊ��͸���ı仯�ٶ�
	   BYTE NCR2CRSpeed;                         // hu �Ӳ�͸����Ϊ͸���ı仯�ٶ�
       int OriginalWidth,OriginalHeight;        // hu ���ڵ�ԭʼ��С
       int DestoryPosX;                         // hu ���ٴ���ʱ���ڵ����Ͻ�x��λ��
       int DestoryPosY;                         // hu ���ٴ���ʱ���ڵ����Ͻ�y��λ��
       int Timer1_ID;                           // hu ��ʱ��1ID����������ʱ��֮
       int Timer2_ID;                           // hu ��ʱ��2ID�����ٴ���ʱ��֮ 
	   int Timer3_ID;                           // hu ��ʱ��3ID����͸����Ϊ��͸�����䴰��ʱ��֮ 
	   int Timer4_ID;                           // hu ��ʱ��4ID���Ӳ�͸����Ϊ͸�����䴰��ʱ��֮ 
       CDialog* flashDlg;                       // hu ָ����Ҫ����Ч���Ĵ���ָ��
       void InitCDlg();                         // hu ��ʼ�����ڲ�ʹ��
};

#endif // !defined(AFX_DLGFLASH_H__FC9C5DF7_7144_4F99_9B5A_0C5451DEA813__INCLUDED_)