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
	this->m_wait_milliseconds = 200;//0.2��ɨ��һ��
}

CScanDbCmdTask::~CScanDbCmdTask()
{
	ACE_DEBUG((LM_DEBUG,"free CScanDbCmdTask...\n"));
}

void CScanDbCmdTask::init(TOCIDatabase * poraDb)
{
	m_pDataQry = new TOCIQuery(poraDb);
}

//ɨ�����ݿ⣬ÿ��ȡ������
int CScanDbCmdTask::beforeGetMq(int idx)
{
	Sleep(10);

	if ((m_befgetq_count % 10) != 0)
	{
		m_befgetq_count ++ ;
		return 0;
		//����Ҫ̫Ƶ��������ɨ��ʱ���1500����ȡ����
		//ԭ����0.2���һ�Σ���Ϊÿ2���һ��
	}else //==0
	{
		if ((m_befgetq_count % 500) == 0)
		{
			m_befgetq_count = 0;
			printf("\n");
		}
		
		printf(":"); //˵�������ݿ�һ��
		fflush(stdout);
	}


	m_befgetq_count ++ ;

	if (getAppCommandList()>0)
	{
		return -1;
	}
	//��������ֱ���м�¼�����ͽ���
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
		Sleep(1000); //���ִ��ڼ�¼��������1�룬��֤���κŷ����仯 
	}

	return amount;
}

//�����յ�����Ϣ,����Ӧ�������������ݿ��Ӧ���������е����кţ����γ�һ�������ıջ�����
int CScanDbCmdTask::deal(ACE_Message_Block *&mb, ACE_Time_Value *timeout)
{
	//�������ͻ��˵�״̬,����Ӧ�Ŀͻ���״̬���õ����ݿ���


	//���ָ����䱣�浽������

	char * pMsg = NULL;
	int msglen = 0;
	int i,icnt,iusedlen=0;
	char buffer[5120];//������
	TSmsInfo oSmsInfo;

	TMessage_Block * pBlk = (TMessage_Block *)mb;

	//���潫�ڴ汣�浽���أ����ͷ���ռ��Դ
	pMsg = mb->rd_ptr();
	msglen = mb->length();
	memcpy((char *)&oSmsInfo,pMsg,msglen);

	pBlk->release();//�ͷ��Լ���������Դ ,���������ܹ���ֹ����������˳����ĳ������û�������ͷ���Դ
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
		//���״̬
		setSmsCmdStatus(oSmsInfo.batchid,oSmsInfo.phone,oSmsInfo.status);
	}

	return 0;
}

//�Ѳ�
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
		m_pDataQry->SetParameter("locknum", sLockNum);	//�������
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

//�Ѳ�
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

//�Ѳ�
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
		m_pDataQry->SetParameter("locknum", sLockNum);	//�������
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


//��������д������Ķ���Ϣ�����ݿ���
//�Ѳ�
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
1.����������Ҫ���͵ļ�¼
2.��ȡ�����ļ�¼�������з���
3.��ȡ���ڴ�֮�󣬱��״̬
//�Ѳ�
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
	//�Ѳ�
	try
	{
		m_pDataQry->Close();
		m_pDataQry->SetSQL(sqlString);
		m_pDataQry->SetParameter("locknum", sLockNum);	//�������
		m_pDataQry->Open();

		while(!m_pDataQry->Eof())					
		{
			strcpy(aInfo.batchid,m_pDataQry->Field("batchid").asString());
			strcpy(aInfo.phone,m_pDataQry->Field("sendnum").asString());
			strcpy(aInfo.info,m_pDataQry->Field("infomation").asString());

			pub::replaceString(aInfo.info,"' || chr(10) || '","\n"); //����һ�����ˣ���HTML����ɾ�� add by serite 20130702
 
			ACE_DEBUG((LM_DEBUG,"%s %s %s \n",aInfo.batchid,aInfo.phone,aInfo.info));
			//�ӱ���õ�һ����Ϣ����,һֱȡ����,ֱ��ȡ�����е�����,Ҳ����˵������ռ�ʹ����ϣ�
			//�����Ĵ����ȴ�����,ֱ���еط��ڳ��ռ䣬�����������д���ڴ�䶯�������˶�̬�ڴ�������ڴ���Ƭ

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



