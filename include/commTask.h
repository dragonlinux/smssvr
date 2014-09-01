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
   扫描数据库,看看是否存在需要下发的纪录，同时监听收到的内容，将收到的内容对应到对应的数据库记录上
*/
/*
后来考虑到对于终端的处理可能存在
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

	int m_isDealPre;//是否进行预处理 ，当发现队列里面存在数据时，不进行预处理
public:
	char m_dev[16];
	int m_baudrate;
	int m_parityenb;
	int m_databits;
	int m_stopbits;
	long m_readTimeOut;
	char m_smsCenter[24];
	int m_befgetq_count;

	//msg 内的各式为固定的字符串各式 term,state,ip,ipport,protocol
	CCommTask(int msgSize=3000,int msgCount= 5000); //指这里缓存的指令数量为1000 
	//这里的消息只是用来标示终端状态的记录状态 ,这里的消息为1 说明这里只是用作线程调用，没有作其他的使用
	virtual ~CCommTask();

	void init();

	int ReadComm(char* pData, int nLength,int rType=0,int nSec=2);
	int WriteComm(char* pData, int nLength);

public:
	
	//扫描数据库
	int beforeGetMq(int idx);
	
	//处理客户端响应的消息内容
	virtual int deal(ACE_Message_Block *&mb, ACE_Time_Value *timeout = 0);
	
protected:
	
};

typedef ACE_Singleton <CCommTask, ACE_Thread_Mutex> TheCommTask;



#endif // !defined(AFX_SERIALTASK_H__D2EBE0E7_90D1_42E4_8B9B_E2213FF8965B__INCLUDED_)
