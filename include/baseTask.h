// BaseTask.h: interface for the CBaseTask class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_BASETASK_H__0690AE5E_3BFA_4682_87CF_45EC1AEA721A__INCLUDED_)
#define AFX_BASETASK_H__0690AE5E_3BFA_4682_87CF_45EC1AEA721A__INCLUDED_
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "ace/Task.h"
#include "ace/Message_Block.h"
#include "MsgBlockManager.h"
#include "publicfunc.h"
#include <vector>
class CBaseTask :
public ACE_Task<ACE_MT_SYNCH>
{
public:
	static std::vector<CBaseTask *> m_TaskPool;
protected:
	char className[32];
	int m_task_minnum_, m_task_maxnum_;
	int m_msg_count_, m_msg_size_;
	long thread_flags_; // ace thread create flag
	int m_execCount;
	int m_threadcnt;
	ACE_Recursive_Thread_Mutex m_add_lock; 
	ACE_Recursive_Thread_Mutex m_del_lock; 
	ACE_Recursive_Thread_Mutex m_threadcnt_lock; 
	
	int m_addcnt;
	int m_delcnt;
	CMsgBlockManager m_MsgMgr;  //�����������ĵط����ʹ������ݵĿռ�
	int m_taskNum;
	int m_bClosed;
	int m_wait_milliseconds; //ɨ��ȴ�������
	int m_wait_seconds;	 //ɨ��ȴ�����
public://!!!!
	inline int wait_msg_cnt () 
	{
//		return 0;
		return this->msg_queue()->message_count ();
	}
	inline int wait_milliseconds()
	{
		return m_wait_milliseconds;
	}
	void wait_milliseconds(int val)
	{
		m_wait_milliseconds = val;
	}

	inline int wait_seconds()
	{
		return m_wait_seconds;
	}
	void wait_seconds(int val)
	{
		m_wait_seconds = val;
	}

	inline int wait_msg_len()
	{
//		return 0;
		return this->msg_queue()->message_length();
	}
	inline int wait_msg_bytes()
	{
//		return 0;
		return this->msg_queue()->message_bytes();
	}
	int add_new_task (void *arg);
protected:
	//��ָ��ʱ���ڷ���
	// Ĭ��Ϊ��������¼����ȴ���������
	ACE_Message_Block *get_msg_block(ACE_Time_Value *timeout = 0)
	{
// 		ACE_Time_Value start = ACE_OS::gettimeofday ();
// 		ACE_Time_Value end ;
// 		end.sec(start.sec()+timeout->sec());
// 		end.usec(start.sec()+timeout->sec())
// 
		int icnt=0;
		ACE_Message_Block * pBlk = m_MsgMgr.get_msg_block();
		while (pBlk==NULL)
		{
			icnt ++ ;
			pBlk = m_MsgMgr.get_msg_block();
			if ((icnt % 100) == 0) // 25 ����ʾһ��
			{
				ACE_DEBUG((LM_DEBUG,"%s msg full!!!\n",className));				
			} 
			Sleep(50);
		}
		return pBlk;
	}
public:
public:
	CBaseTask(const char * clsName,int msgSize=1024,int msgCount=1024);
	virtual ~CBaseTask();
public:
	virtual int beforeGetMq(int idx)
	{
		return 0;
	}
	//���ݱ�������ȷ�� Э������
	virtual int deal(ACE_Message_Block *&mb, ACE_Time_Value *timeout = 0)=0;
// 	{
// 		//1.�������Ľ�� �ŵ���⴦��������أ����ڽ���������
// 		
// 		//2.���������Ҫ�������صļ�¼ ֱ�ӵ���ͨѶ�ӿڣ������ݷ��ͻ�ȥ
// 		
// 		//3.ͬʱ������־�������������ݵ���־����
// 		
// 	}
	
// 	int open(void*)
// 	{
// 		m_bClosed = 0;
// 		ACE_DEBUG((LM_DEBUG, "(%t) CBaseTask task opened \n"));
// 		
// 		//Activate the Task
// 		activate(THR_NEW_LWP,m_taskNum);
// 		
// 		return 0;
// 	}
	int start (int minnum=1,int maxnum=10)
	{
		m_task_minnum_ = minnum > 0 ? minnum :1;
		m_task_maxnum_ = maxnum > m_task_minnum_ ? maxnum:m_task_minnum_;
		
		int ret = this->activate (thread_flags_, m_task_minnum_);
		
		return ret;
	}
	//The Service Processing routine
	virtual int svc(void);
	virtual int close(u_long)
	{
		ACE_DEBUG((LM_DEBUG, "%s closes down \n",className));
		
		return 0;
	}
	//�˳�ֱ���ڴ������е����ݶ��������
	int waitUntilMsgEmpty();
	int shutdown();
	int putMsg(const char * pMsgType,int typelen,const char * pMsg,int msglen );
	virtual void getstatus(char * sInfo)
	{
		//sprintf(sInfo+strlen(sInfo),"�߳��������� ���� MAX MIN RES BLKLEN BLKAMT USED MAXUSED ʹ�ô���\n");
		sprintf(sInfo,"%12.12s %4d %3d %3d %3d %6d %6d %4d %7d %8d\n",
			className,this->thr_count (),m_task_maxnum_,m_task_minnum_,m_addcnt,
			m_MsgMgr.fixLen(),
			m_MsgMgr.blkAmount(),
			m_MsgMgr.usedCount(),
			m_MsgMgr.blkUsdMax(),
			this->m_execCount);
	}
};
#endif // !defined(AFX_BASETASK_H__0690AE5E_3BFA_4682_87CF_45EC1AEA721A__INCLUDED_)

