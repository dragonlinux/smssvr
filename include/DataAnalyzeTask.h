// DataAnalyzeTask.h: interface for the CDataAnalyzeTask class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DataAnalyzeTASK_H__D2EBE0E7_90D1_42E4_8B9B_E2213FF8965B__INCLUDED_)
#define AFX_DataAnalyzeTASK_H__D2EBE0E7_90D1_42E4_8B9B_E2213FF8965B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stdafx.h"
#include "baseTask.h"
#include "ace/Singleton.h"
#include "TOCIQuery.h"
#include "iec_sys.h"

/*
   扫描数据库,看看是否存在需要下发的纪录，同时监听收到的内容，将收到的内容对应到对应的数据库记录上
*/
/*
后来考虑到对于终端的处理可能存在
*/
class CDataAnalyzeTask : public CBaseTask  
{
private:
	typedef struct 
	{
		char sID[8];
		char sVal[128];

	}TRec;


	int m_scanCount;
	ACE_Recursive_Thread_Mutex m_scanCount_lock; 
	TOCIQuery * m_pDataQry;
public:
	//msg 内的各式为固定的字符串各式 term,state,ip,ipport,protocol
	CDataAnalyzeTask(int msgSize=3000,int msgCount= 1000); //指这里缓存的指令数量为1000 
	//这里的消息只是用来标示终端状态的记录状态 ,这里的消息为1 说明这里只是用作线程调用，没有作其他的使用
	virtual ~CDataAnalyzeTask();

	void init(TOCIDatabase * poraDb);
public:
	int m_befgetq_count;
	//扫描数据库
	int beforeGetMq(int idx);
	
	//处理客户端响应的消息内容
	virtual int deal(ACE_Message_Block *&mb, ACE_Time_Value *timeout = 0);
	
protected:
	int selectSql(char * sqlString,int icolindex,TRec * reclist);
	//int selectSql(char * sqlString,int icolindex,TRec * reclist);

	int selectOneSql(char * sqlString,int reccount,TRec * reclist);
	int insertSql(char * sqlString);
	int updateSql(char * sqlString);
//保存队列中传送来的短消息到数据库中
	int saveRcvAlarmInfo(TSmsInfo * pSmsInfo,int recCount,TValue * outRecs);

	int makeAlarmSms(char * targNo,char * sdatatime);

};

typedef ACE_Singleton <CDataAnalyzeTask, ACE_Thread_Mutex> TheDataAnalyzeTask;



#endif // !defined(AFX_DataAnalyzeTASK_H__D2EBE0E7_90D1_42E4_8B9B_E2213FF8965B__INCLUDED_)
