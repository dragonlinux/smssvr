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
   ɨ�����ݿ�,�����Ƿ������Ҫ�·��ļ�¼��ͬʱ�����յ������ݣ����յ������ݶ�Ӧ����Ӧ�����ݿ��¼��
*/
/*
�������ǵ������ն˵Ĵ�����ܴ���
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
	//msg �ڵĸ�ʽΪ�̶����ַ�����ʽ term,state,ip,ipport,protocol
	CDataAnalyzeTask(int msgSize=3000,int msgCount= 1000); //ָ���ﻺ���ָ������Ϊ1000 
	//�������Ϣֻ��������ʾ�ն�״̬�ļ�¼״̬ ,�������ϢΪ1 ˵������ֻ�������̵߳��ã�û����������ʹ��
	virtual ~CDataAnalyzeTask();

	void init(TOCIDatabase * poraDb);
public:
	int m_befgetq_count;
	//ɨ�����ݿ�
	int beforeGetMq(int idx);
	
	//����ͻ�����Ӧ����Ϣ����
	virtual int deal(ACE_Message_Block *&mb, ACE_Time_Value *timeout = 0);
	
protected:
	int selectSql(char * sqlString,int icolindex,TRec * reclist);
	//int selectSql(char * sqlString,int icolindex,TRec * reclist);

	int selectOneSql(char * sqlString,int reccount,TRec * reclist);
	int insertSql(char * sqlString);
	int updateSql(char * sqlString);
//��������д������Ķ���Ϣ�����ݿ���
	int saveRcvAlarmInfo(TSmsInfo * pSmsInfo,int recCount,TValue * outRecs);

	int makeAlarmSms(char * targNo,char * sdatatime);

};

typedef ACE_Singleton <CDataAnalyzeTask, ACE_Thread_Mutex> TheDataAnalyzeTask;



#endif // !defined(AFX_DataAnalyzeTASK_H__D2EBE0E7_90D1_42E4_8B9B_E2213FF8965B__INCLUDED_)
