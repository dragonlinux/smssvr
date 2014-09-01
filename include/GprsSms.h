// GrpsSms.h: interface for the CGprsSms class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRPSSMS_H__2368BDF3_D9D7_483C_8C17_95F66914007D__INCLUDED_)
#define AFX_GRPSSMS_H__2368BDF3_D9D7_483C_8C17_95F66914007D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "stdafx.h"
#include "SmsPDU.h"

static unsigned short SmsSeqNum = 0;

class CGprsSms  
{
public:
	CGprsSms();
	virtual ~CGprsSms();
	inline int WriteComm(char* pData, int nLength);

	inline int ReadComm(char* pData, int nLength,int rType=0,int nSec=2);

protected:
	// ��ȡ����Ϣ���������������ȡӦ��
// ��+CMGL����+CMGR����һ���Զ���ȫ������Ϣ
	int GetSmsCmd();
	//ɾ��ָ����ŵĶ���
	int DeleteSmsCmd(int index);

	// ���Ͷ���Ϣ���������������ȡӦ��
// ����: pSrc - ԴPDU����ָ��
	int PutSmsCmd(SM_PARAM* pSrc);
public:
	//�򿪶��ŷ�ʽ 0:sms 1:gprs
	void SetWordMode();

	//��õ�ǰ����ģʽ 0:sms 1:gprs -1:����δ��
 	int  GetWordMode();

//����豸����Ӧ
    int  GetResponse(SM_BUFF* pBuff,int rType=0,int nSec=1);

	//���ŷ�ʽ��ʼ��
	int InitSms(); 

	int splitSms(int msgLen,char* pMsgBuf ,int &smscnt,char  **smslist);

/******************************************************************
* Name: writeSms
* Description: ���Ͷ���
* Parameter: 
pMsc : �������ĺ���8613800591500
pMobileNo : �Է�����
pMsg :����Ϣ����
* Return: int >0 �ɹ�;0 -1 ʧ��
* Cautions:  ������������������������������������������������    
******************************************************************/
	int _SndSms(const char* pMsc,const char* pMobileNo,const char* pMsgBuf , int msgLen,TPDU_UDHI *pUdhi);
    int SndSms(const char* pMsc,const char* pMobileNo,const char* pMsgBuf , int msgLen);


	//��ȡ��������
	int ReadSms(TSmsInfo * SmsList );


	//������еĶ��� 
	int ClearSms(int icnt,SM_PARAM* pMsg);
	int ParseMessageList(SM_PARAM* pMsg, SM_BUFF* pBuff);
};



#endif // !defined(AFX_GRPSSMS_H__2368BDF3_D9D7_483C_8C17_95F66914007D__INCLUDED_)
