// commTask.h: interface for the CScanDbTask class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERIALTASK_H__D2EBE0E7_90D1_42E4_8B9B_E2213FF8965B__INCLUDED_)
#define AFX_SERIALTASK_H__D2EBE0E7_90D1_42E4_8B9B_E2213FF8965B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stdafx.h"
#include "baseTask.h"
#include "ace/Singleton.h"
#include "TOCIQuery.h"
#include "GprsSms.h"
/*
   ɨ�����ݿ�,�����Ƿ������Ҫ�·��ļ�¼��ͬʱ�����յ������ݣ����յ������ݶ�Ӧ����Ӧ�����ݿ��¼��
*/
/*
�������ǵ������ն˵Ĵ�����ܴ���
*/
class CCommTask : public CBaseTask  
{
private:
	int m_scanCount;
	ACE_Recursive_Thread_Mutex m_scanCount_lock; 

	int hComm;
	
	int OpenComm(char* pPort, int nBaudRate, int nByteSize, int nStopBits, char cParity);
	int CloseComm();

	void set_speed(int speed);
	int set_Parity(int databits,int stopbits,char cParity);

	CGprsSms m_gprsSms;

	int m_isDealPre;//�Ƿ����Ԥ���� �������ֶ��������������ʱ��������Ԥ����
public:
	char m_dev[16];
	int m_baudrate;
	int m_parityenb;
	int m_databits;
	int m_stopbits;
	long m_readTimeOut;
	char m_smsCenter[24];
	int m_befgetq_count;

	//msg �ڵĸ�ʽΪ�̶����ַ�����ʽ term,state,ip,ipport,protocol
	CCommTask(int msgSize=3000,int msgCount= 5000); //ָ���ﻺ���ָ������Ϊ1000 
	//�������Ϣֻ��������ʾ�ն�״̬�ļ�¼״̬ ,�������ϢΪ1 ˵������ֻ�������̵߳��ã�û����������ʹ��
	virtual ~CCommTask();

	void init();

	int ReadComm(char* pData, int nLength,int rType=0,int nSec=2);
	int WriteComm(char* pData, int nLength);

public:
	
	//ɨ�����ݿ�
	int beforeGetMq(int idx);
	
	//����ͻ�����Ӧ����Ϣ����
	virtual int deal(ACE_Message_Block *&mb, ACE_Time_Value *timeout = 0);
	
protected:
	
};

typedef ACE_Singleton <CCommTask, ACE_Thread_Mutex> TheCommTask;



#endif // !defined(AFX_SERIALTASK_H__D2EBE0E7_90D1_42E4_8B9B_E2213FF8965B__INCLUDED_)
