// ScanDbTask.cpp: implementation of the CScanDbTask class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ScanDbTask.h"
#include "commTask.h"
#include "publicfunc.h"
#include "DataAnalyzeTask.h"
#include "iec_sys.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CScanDbCmdTask::CScanDbCmdTask(int msgSize,int msgCount):CBaseTask("CScanDbCmdTask",msgSize,msgCount)
{
	m_scanCount = 0;
	m_befgetq_count = 0;
	this->m_wait_milliseconds = 200;//0.2秒扫描一次
}

CScanDbCmdTask::~CScanDbCmdTask()
{
	ACE_DEBUG((LM_DEBUG,"free CScanDbCmdTask...\n"));
}

void CScanDbCmdTask::init(TOCIDatabase * poraDb)
{
	m_pDataQry = new TOCIQuery(poraDb);
}

//扫描数据库，每批取得命令
int CScanDbCmdTask::beforeGetMq(int idx)
{
	Sleep(10);

	if ((m_befgetq_count % 10) != 0)
	{
		m_befgetq_count ++ ;
		return 0;
		//不需要太频繁，按照扫描时间的1500倍读取即可
		//原来是0.2秒读一次，改为每2秒读一次
	}else //==0
	{
		if ((m_befgetq_count % 500) == 0)
		{
			m_befgetq_count = 0;
			printf("\n");
		}
		
		printf(":"); //说明读数据库一次
		fflush(stdout);
	}


	m_befgetq_count ++ ;

	if (getAppCommandList()>0)
	{
		return -1;
	}
	//继续处理，直所有记录都发送结束
	return 0;
}

int CScanDbCmdTask::getAppCommandList()
{
	char sLockNum[24];
	pub::CDateTime::GetCurDateTime(sLockNum);


	int amount;

	amount = lockSmsCmd(sLockNum);

	if (amount > 0)
	{
		getDbSmsCmd(sLockNum);
		preSmsCmd(sLockNum);
		Sleep(1000); //发现存在记录，就休眠1秒，保证批次号发生变化 
	}

	return amount;
}

//处理收到的信息,将相应的内容设置数据库对应的命令序列的序列号，以形成一个完整的闭环操作
int CScanDbCmdTask::deal(ACE_Message_Block *&mb, ACE_Time_Value *timeout)
{
	//解析出客户端的状态,将相应的客户端状态设置到数据库中


	//获得指令，将其保存到数据中

	char * pMsg = NULL;
	int msglen = 0;
	int i,icnt,iusedlen=0;
	char buffer[5120];//缓存区
	TSmsInfo oSmsInfo;

	TMessage_Block * pBlk = (TMessage_Block *)mb;

	//下面将内存保存到本地，并释放所占资源
	pMsg = mb->rd_ptr();
	msglen = mb->length();
	memcpy((char *)&oSmsInfo,pMsg,msglen);

	pBlk->release();//释放自己的锁定资源 ,这样处理，能够防止后面非正常退出造成某个处理没有正常释放资源
printf("--------------------------????\n");
	if (oSmsInfo.batchid[0]==0)
	{

		saveRcvSms(oSmsInfo.phone,oSmsInfo.info);
		if(isIEC103(oSmsInfo.info))
		{
			TheDataAnalyzeTask::instance()->putMsg(0,0,(char *)&(oSmsInfo),sizeof(oSmsInfo));
		}

	}else
	{
		//变更状态
		setSmsCmdStatus(oSmsInfo.batchid,oSmsInfo.phone,oSmsInfo.status);
	}

	return 0;
}

//已测
int CScanDbCmdTask::lockSmsCmd(char * sLockNum)
{
	char sVal[1024];
	int iRet;

	char sqlString[1024] = 
		" update t_sm_sendmsg set locknum = :locknum,sendtime=sysdate,status=10 \n" 
		" WHERE sysdate>plantime and status = 1 and rownum < 100 \n";
	try
	{
		m_pDataQry->Close();
		m_pDataQry->SetSQL(sqlString);					
		m_pDataQry->SetParameter("locknum", sLockNum);	//锁定编号
		m_pDataQry->Execute();

		iRet = m_pDataQry->RowsAffected();
//printf("ret=%d %s\n",iRet,sqlString);
		m_pDataQry->Commit();
	}
	catch(TOCIException oe)						
	{
		ACE_DEBUG((LM_DEBUG,"error:%s [%s]\n",oe.GetErrSrc(),oe.GetErrMsg()));
		//throw pub::TError(-5,"%s [%s]",oe.GetErrSrc(),oe.GetErrMsg());
	}
	return iRet;
}

//已测
int CScanDbCmdTask::setSmsCmdStatus(char * batchid,char * phoneNum,int status)
{
	char sVal[1024];
	int iRet;

	char sqlString[1024];

	sprintf(sqlString,"update t_sm_sendmsg set status = %d where batchid='%s' and sendnum='%s'\n",
		status > 0 ? 11: status,batchid,phoneNum);
	try
	{
		m_pDataQry->Close();
		m_pDataQry->SetSQL(sqlString);					
		m_pDataQry->Execute();

		iRet = m_pDataQry->RowsAffected();

		m_pDataQry->Commit();
	}
	catch(TOCIException oe)						
	{
		ACE_DEBUG((LM_DEBUG,"error:%s [%s]\n",oe.GetErrSrc(),oe.GetErrMsg()));
		//throw pub::TError(-5,"%s [%s]",oe.GetErrSrc(),oe.GetErrMsg());
	}
	return iRet;
}

//已测
int CScanDbCmdTask::preSmsCmd(char * sLockNum)
{
	char sVal[1024];

	char sqlString[1024] = 
		" update t_sm_sendmsg set locknum = :locknum,sendtime=sysdate,status=2 \n"
		" WHERE locknum = ':locknum' and status=10 and rownum < 120 \n";

	try
	{
		m_pDataQry->Close();
		m_pDataQry->SetSQL(sqlString);					
		m_pDataQry->SetParameter("locknum", sLockNum);	//锁定编号
		m_pDataQry->Execute();
		m_pDataQry->Commit();
	}
	catch(TOCIException oe)						
	{
		ACE_DEBUG((LM_DEBUG,"error:%s [%s]\n",oe.GetErrSrc(),oe.GetErrMsg()));
		//throw pub::TError(-5,"%s [%s]",oe.GetErrSrc(),oe.GetErrMsg());
	}	
	return 0;
}


//保存队列中传送来的短消息到数据库中
//已测
int CScanDbCmdTask::saveRcvSms(char * phoneNum,char * sinfo)
{
	char sVal[1024];

	char sqlString[1024];

	sprintf(sqlString,"insert into t_sm_receivemsg(receivenum,receivetime,infomation) values(substr('%s',-11,11),sysdate,substr('%s',1,980))\n",
		phoneNum,sinfo);
	try
	{
		m_pDataQry->Close();
		m_pDataQry->SetSQL(sqlString);					
		m_pDataQry->Execute();
		m_pDataQry->Commit();
	}
	catch(TOCIException oe)						
	{
		ACE_DEBUG((LM_DEBUG,"error:%s [%s]\n",oe.GetErrSrc(),oe.GetErrMsg()));
		//throw pub::TError(-5,"%s [%s]",oe.GetErrSrc(),oe.GetErrMsg());
	}	
	return 0;
}

/*
1.首先锁定需要发送的记录
2.读取锁定的记录，并进行发送
3.读取到内存之后，变更状态
//已测
*/
int CScanDbCmdTask::getDbSmsCmd(char * sLockNum)
{
	//	assert(sObjId!=NULL);
	char batchId[24];
	char phoneNum[24];
	char smgInfo[1040];
	TSmsInfo aInfo;

	//
	char sqlString[1024] = 
		" SELECT batchid,sendnum,infomation \n"
		" FROM t_sm_sendmsg \n"
		" WHERE locknum =  :locknum\n";
	//已测
	try
	{
		m_pDataQry->Close();
		m_pDataQry->SetSQL(sqlString);
		m_pDataQry->SetParameter("locknum", sLockNum);	//锁定编号
		m_pDataQry->Open();

		while(!m_pDataQry->Eof())					
		{
			strcpy(aInfo.batchid,m_pDataQry->Field("batchid").asString());
			strcpy(aInfo.phone,m_pDataQry->Field("sendnum").asString());
			strcpy(aInfo.info,m_pDataQry->Field("infomation").asString());

			pub::replaceString(aInfo.info,"' || chr(10) || '","\n"); //增加一个过滤，将HTML代码删除 add by serite 20130702
 
			ACE_DEBUG((LM_DEBUG,"%s %s %s \n",aInfo.batchid,aInfo.phone,aInfo.info));
			//从本身得到一个消息队列,一直取数据,直到取到所有的数据,也就是说，如果空间使用完毕，
			//后续的处理将等待运行,直到有地方腾出空间，这样将不会有大的内存变动，减少了动态内存引起的内存碎片

			TheCommTask::instance()->putMsg(0,0,(char *)&aInfo,sizeof(aInfo));

			m_pDataQry->Next();
		}
		m_pDataQry->Close();
	}
	catch(TOCIException oe)						
	{
		ACE_DEBUG((LM_DEBUG,"error:%s [%s]\n",oe.GetErrSrc(),oe.GetErrMsg()));
		//throw pub::TError(-5,"%s [%s]",oe.GetErrSrc(),oe.GetErrMsg());
	}	

	return 1;
}



