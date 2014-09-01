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
	// 读取短消息，仅发送命令，不读取应答
// 用+CMGL代替+CMGR，可一次性读出全部短消息
	int GetSmsCmd();
	//删除指定序号的短信
	int DeleteSmsCmd(int index);

	// 发送短消息，仅发送命令，不读取应答
// 输入: pSrc - 源PDU参数指针
	int PutSmsCmd(SM_PARAM* pSrc);
public:
	//打开短信方式 0:sms 1:gprs
	void SetWordMode();

	//获得当前工作模式 0:sms 1:gprs -1:串口未打开
 	int  GetWordMode();

//获得设备的响应
    int  GetResponse(SM_BUFF* pBuff,int rType=0,int nSec=1);

	//短信方式初始化
	int InitSms(); 

	int splitSms(int msgLen,char* pMsgBuf ,int &smscnt,char  **smslist);

/******************************************************************
* Name: writeSms
* Description: 发送短信
* Parameter: 
pMsc : 短信中心号码8613800591500
pMobileNo : 对方号码
pMsg :短信息内容
* Return: int >0 成功;0 -1 失败
* Cautions:  　　　　　　　　　　　　　　　　　　　　　　　　    
******************************************************************/
	int _SndSms(const char* pMsc,const char* pMobileNo,const char* pMsgBuf , int msgLen,TPDU_UDHI *pUdhi);
    int SndSms(const char* pMsc,const char* pMobileNo,const char* pMsgBuf , int msgLen);


	//读取短信内容
	int ReadSms(TSmsInfo * SmsList );


	//清除所有的短信 
	int ClearSms(int icnt,SM_PARAM* pMsg);
	int ParseMessageList(SM_PARAM* pMsg, SM_BUFF* pBuff);
};



#endif // !defined(AFX_GRPSSMS_H__2368BDF3_D9D7_483C_8C17_95F66914007D__INCLUDED_)
