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
   ɨ�����ݿ�,�����Ƿ������Ҫ�·��ļ�¼��ͬʱ�����յ������ݣ����յ������ݶ�Ӧ����Ӧ�����ݿ��¼��
*/
/*
�������ǵ������ն˵Ĵ�����ܴ���
*/
class CScanDbCmdTask : public CBaseTask  
{
private:
	int m_scanCount;
	ACE_Recursive_Thread_Mutex m_scanCount_lock; 
	TOCIQuery * m_pDataQry;
public:
	//msg �ڵĸ�ʽΪ�̶����ַ�����ʽ term,state,ip,ipport,protocol
	CScanDbCmdTask(int msgSize=3000,int msgCount= 1000); //ָ���ﻺ���ָ������Ϊ1000 
	//�������Ϣֻ��������ʾ�ն�״̬�ļ�¼״̬ ,�������ϢΪ1 ˵������ֻ�������̵߳��ã�û����������ʹ��
	virtual ~CScanDbCmdTask();

	void init(TOCIDatabase * poraDb);
public:
	int m_befgetq_count;
	//ɨ�����ݿ�
	int beforeGetMq(int idx);
	
	//����ͻ�����Ӧ����Ϣ����
	virtual int deal(ACE_Message_Block *&mb, ACE_Time_Value *timeout = 0);
	
protected:
	
	//������ݿ�����Ҫ���͵�ָ��
	int getDbSmsCmd();

	//��ʱ�����Ҫ���͵�ָ�� //���齫1��ǰ������ɾ��
	int dealDbData_Clear();

	//�������ڵ�����͵������ն�b



	int getAppCommandList();


	int lockSmsCmd(char * sLockNum);

	int setSmsCmdStatus(char * batchid,char * phoneNum,int status);


	int preSmsCmd(char * sLockNum);


//��������д������Ķ���Ϣ�����ݿ���
	int saveRcvSms(char * phoneNum,char * sinfo);
/*
1.����������Ҫ���͵ļ�¼
2.��ȡ�����ļ�¼�������з���
3.��ȡ���ڴ�֮�󣬱��״̬Ϊ
*/
	int getDbSmsCmd(char * sLockNum);

};

typedef ACE_Singleton <CScanDbCmdTask, ACE_Thread_Mutex> TheScanDbCmdTask;



#endif // !defined(AFX_SCANDBTASK_H__D2EBE0E7_90D1_42E4_8B9B_E2213FF8965B__INCLUDED_)
