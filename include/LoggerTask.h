// LoggerTask.h: interface for the CLoggerTask class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOGGERTASK_H__853DB960_EFCF_4615_A8C8_3B4CB4C6860B__INCLUDED_)
#define AFX_LOGGERTASK_H__853DB960_EFCF_4615_A8C8_3B4CB4C6860B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "baseTask.h"
#include "ace/Singleton.h"

class CLoggerTask : public CBaseTask  
{
public:
	typedef std::map<std::string,FILE *> TFileMap;
    typedef std::map<std::string,FILE *>::iterator TFileMapIter;

	enum E_LOG_FMT_TYPE 
	{ 
			LOG_F_STR=0, 
			LOG_F_BIN=1,
			LOG_F_ASC=2,
	}; 
	
	enum E_LOG_TYPE 
	{ 
		LOG_T_DATA=0,// 普通数据
		LOG_T_ERR=3, //错误数据
		LOG_T_SQL=4, //错误sql
	}; 


private:
	TFileMap m_fileMap;
	int writelog(FILE * pFp,char * headinfo,int msglen,char * pMsg,char * sName);
	FILE * getLogFilePtr(E_LOG_TYPE logType,E_LOG_FMT_TYPE logfmt,char * sOutName);

	void closeAllFile();
public:
	// 
	CLoggerTask(int msgSize = 2500,int msgCount = 100); //空间主要用来组装下发语句，所以不需要太大
	virtual ~CLoggerTask();
	
	int log(int msglen,char * msg,char * headinfo = NULL ,E_LOG_FMT_TYPE logfmt = LOG_F_STR,E_LOG_TYPE logType = LOG_T_DATA);
	virtual int deal(ACE_Message_Block *&mb, ACE_Time_Value *timeout = 0);
};

typedef ACE_Singleton <CLoggerTask, ACE_Thread_Mutex> theLoggerTask;

#endif // !defined(AFX_LOGGERTASK_H__853DB960_EFCF_4615_A8C8_3B4CB4C6860B__INCLUDED_)
