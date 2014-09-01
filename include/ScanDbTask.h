// ScanDbTask.h: interface for the CScanDbTask class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCANDBTASK_H__D2EBE0E7_90D1_42E4_8B9B_E2213FF8965B__INCLUDED_)
#define AFX_SCANDBTASK_H__D2EBE0E7_90D1_42E4_8B9B_E2213FF8965B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stdafx.h"
#include "baseTask.h"
#include "ace/Singleton.h"
#include "TOCIQuery.h"

/*
   扫描数据库,看看是否存在需要下发的纪录，同时监听收到的内容，将收到的内容对应到对应的数据库记录上
*/
/*
后来考虑到对于终端的处理可能存在
*/
class CScanDbCmdTask : public CBaseTask  
{
private:
	int m_scanCount;
	ACE_Recursive_Thread_Mutex m_scanCount_lock; 
	TOCIQuery * m_pDataQry;
public:
	//msg 内的各式为固定的字符串各式 term,state,ip,ipport,protocol
	CScanDbCmdTask(int msgSize=3000,int msgCount= 1000); //指这里缓存的指令数量为1000 
	//这里的消息只是用来标示终端状态的记录状态 ,这里的消息为1 说明这里只是用作线程调用，没有作其他的使用
	virtual ~CScanDbCmdTask();

	void init(TOCIDatabase * poraDb);
public:
	int m_befgetq_count;
	//扫描数据库
	int beforeGetMq(int idx);
	
	//处理客户端响应的消息内容
	virtual int deal(ACE_Message_Block *&mb, ACE_Time_Value *timeout = 0);
	
protected:
	
	//获得数据库中需要发送的指令
	int getDbSmsCmd();

	//定时清除需要发送的指令 //建议将1年前的数据删除
	int dealDbData_Clear();

	//将队列内的命令发送到各个终端b



	int getAppCommandList();


	int lockSmsCmd(char * sLockNum);

	int setSmsCmdStatus(char * batchid,char * phoneNum,int status);


	int preSmsCmd(char * sLockNum);


//保存队列中传送来的短消息到数据库中
	int saveRcvSms(char * phoneNum,char * sinfo);
/*
1.首先锁定需要发送的记录
2.读取锁定的记录，并进行发送
3.读取到内存之后，变更状态为
*/
	int getDbSmsCmd(char * sLockNum);

};

typedef ACE_Singleton <CScanDbCmdTask, ACE_Thread_Mutex> TheScanDbCmdTask;



#endif // !defined(AFX_SCANDBTASK_H__D2EBE0E7_90D1_42E4_8B9B_E2213FF8965B__INCLUDED_)
