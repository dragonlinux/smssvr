// MsgBlockManager.h: interface for the CMsgBlockManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MSGBLOCKMANAGER_H__8AEA298E_8EE7_455F_BB9A_6EF92E7B9F86__INCLUDED_)
#define AFX_MSGBLOCKMANAGER_H__8AEA298E_8EE7_455F_BB9A_6EF92E7B9F86__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "ace/Asynch_IO.h"
#include "ace/Message_Block.h"
#include "ace/Proactor.h"
//#include "ace/Singleton.h"

#include "ace/Hash_Map_Manager.h"
#include "ace/Synch.h" // needed for the lock
#include "ace/Functor.h"
#include "ace/Time_Value.h"
#include "ace/OS_NS_time.h"
#include <list>
#include <queue>
#include <time.h>
#include "publicfunc.h"

template<class EXT_ID, class INT_ID>
//为了方便使用ACE_Hash_Map_Manager_Ex进行简单的封装
class Hash_Map :
public ACE_Hash_Map_Manager_Ex<EXT_ID, INT_ID,
ACE_Hash<EXT_ID>, ACE_Equal_To<EXT_ID>, ACE_Null_Mutex>
{};


#pragma pack(1)


struct TMsgAddi
{
	//由于在本系统中所有的消息对象的内容都来自于本管理器，所以，
	//可以在这里，增加一些附加控制信息

	//上次开始使用时间，为系统绝对时间
	//ACE_Time_Value _starttime;
	time_t _usedTime;
	int logType; //日子类型
	char hint[24];

	void clear()
	{
		_usedTime = 0;
		logType = 0;
		hint[0] = 0;
	}
};

#define MGR_FLAG 0X2000 //用最高位表示是由TMessage_Block 进行管理的消息队列

class CMsgBlockManager;
struct TMessage_Block
{
	ACE_Message_Block block;
	TMessage_Block * _header;
	CMsgBlockManager * _Ownner; //管理器 
	TMsgAddi _addi; //附加信息
	
	void next (TMessage_Block * pBlk)
	{
		_header = pBlk;
	}

	TMessage_Block * next ()
	{
		if (_header!=NULL)
		{
			time(&(_header->_addi._usedTime));//当前开始使用
		}
		return _header;
	}
	
	TMessage_Block()
	{
		block.set_flags(MGR_FLAG);
		_addi._usedTime = 0;
	}

	void clear()
	{
		block.reset();
		block.set_flags(MGR_FLAG);
		_addi.clear();
	}

	void release();
	
	void init(CMsgBlockManager * const pOwnner,UINT32 default_size);


};

#pragma pack()
class CMsgQueue
{
private:
	typedef std::list<TMessage_Block *, std::allocator<TMessage_Block *> > MSG_LIST;
	typedef std::queue<TMessage_Block *,MSG_LIST >  MSG_QUEUE;
	ACE_Recursive_Thread_Mutex m_queue_lock; 
	
	MSG_QUEUE m_msg_queue; //消息队列,用来处理最新到的消息
public:	

	CMsgQueue()
	{

	}

	~CMsgQueue()
	{
		clearWaitSeq();
	}

	int size()
	{
		return  m_msg_queue.size();
	}
	

	int pushCmd(TMessage_Block * pBlk)
	{
		ACE_Guard<ACE_Recursive_Thread_Mutex> locker(m_queue_lock);
		m_msg_queue.push(pBlk);
		return m_msg_queue.size();
	}

	int popCmd(TMessage_Block * &pBlk)
	{
		ACE_Guard<ACE_Recursive_Thread_Mutex> locker(m_queue_lock);

		int iRet = m_msg_queue.size(); 

		if(!m_msg_queue.empty())
		{
			pBlk = m_msg_queue.front();
			m_msg_queue.pop();
		}
		return iRet; 
	}
	

	void clearWaitSeq()
	{
		ACE_Guard<ACE_Recursive_Thread_Mutex> locker(m_queue_lock);
		TMessage_Block * pBlk;
		while (!m_msg_queue.empty())
		{
			pBlk = m_msg_queue.front();
			m_msg_queue.pop();
			pBlk->release();
		}
	}
};


class CMsgBlockManager  
{
private:
	char m_name[64];
	int  m_msgCount;
	int  m_msgSize;
	int  m_curUsed;
	int  m_maxUsed;
public:
	CMsgBlockManager():_header(NULL)
	{
		m_name[0]=0;
		m_msgCount = 0;
		m_msgSize = 0;
		m_maxUsed = 0;
		m_curUsed = 0;
	};

	virtual ~CMsgBlockManager()
	{
		this->fini();
	};
	
public:
	void init(char * sName,UINT32 nums,UINT32 default_size);
	void fini();
	ACE_Message_Block *get_msg_block();
	void release_msg_block(ACE_Message_Block *msg_block);
	inline int usedCount()
	{
		return m_curUsed;
	}

	inline int blkSize()
	{
		return m_msgSize;
	}

	inline int blkAmount()
	{
		return m_msgCount;
	}

	inline int blkUsdMax()
	{
		return m_maxUsed;
	}


	inline int fixLen()
	{
		return m_msgSize;
	}
private:
	TMessage_Block *_header;
	ACE_Recursive_Thread_Mutex _alock;
};

//typedef ACE_Singleton <CMsgBlockManager, ACE_Null_Mutex> MsgBlockManager;

#endif // !defined(AFX_MSGBLOCKMANAGER_H__8AEA298E_8EE7_455F_BB9A_6EF92E7B9F86__INCLUDED_)
