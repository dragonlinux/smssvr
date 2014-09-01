// DataAnalyzeTask.cpp: implementation of the CDataAnalyzeTask class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "DataAnalyzeTask.h"
#include "commTask.h"
#include "publicfunc.h"
#include "iec_sys.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CDataAnalyzeTask::CDataAnalyzeTask(int msgSize,int msgCount):CBaseTask("CDataAnalyzeTask",msgSize,msgCount)
{
	m_scanCount = 0;
	m_befgetq_count = 0;
	this->m_wait_seconds = 0;
	this->m_wait_milliseconds = 200;//0.2��ɨ��һ��
}
CDataAnalyzeTask::~CDataAnalyzeTask()
{
	ACE_DEBUG((LM_DEBUG,"free CDataAnalyzeTask...\n"));
}
void CDataAnalyzeTask::init(TOCIDatabase * poraDb)
{
	m_pDataQry = new TOCIQuery(poraDb);
}
//ɨ�����ݿ⣬ÿ��ȡ������
int CDataAnalyzeTask::beforeGetMq(int idx)
{
	Sleep(10);

	//makeAlarmSms("1","20140512054345");
	//sleep(20);
	return 0;
}
//�����յ�����Ϣ,����Ӧ�������������ݿ��Ӧ���������е����кţ����γ�һ�������ıջ�����
int CDataAnalyzeTask::deal(ACE_Message_Block *&mb, ACE_Time_Value *timeout)
{
	//�������ͻ��˵�״̬,����Ӧ�Ŀͻ���״̬���õ����ݿ���
	//���ָ����䱣�浽������
	char * pMsg = NULL;
	int msglen = 0;
	int i,icnt,iusedlen=0;
	char buffer[5120];//������
	TSmsInfo oSmsInfo;
	TValue ValueList[100];
	TMessage_Block * pBlk = (TMessage_Block *)mb;
	//���潫�ڴ汣�浽���أ����ͷ���ռ��Դ
	pMsg = mb->rd_ptr();
	msglen = mb->length();
	memcpy((char *)&oSmsInfo,pMsg,msglen);
	pBlk->release();//�ͷ��Լ���������Դ ,���������ܹ���ֹ����������˳����ĳ������û�������ͷ���Դ

	icnt=sysDealRecvInfo(strlen(oSmsInfo.info),(unsigned char *)oSmsInfo.info,ValueList);
	
	//����һ�������¼��ȷ������״̬�Ƿ�����
	strcpy(ValueList[icnt].sID,"8.f.0");
	strcpy(ValueList[icnt].sValue,"0");
	for(i=0;i<icnt;i++)
	{
		//ACE_DEBUG((LM_DEBUG,"AnalyzeTask:id=[%s],val=[%s]\n",ValueList[i].sID,ValueList[i].sValue));
		if (ValueList[i].sID[0]=='8')
		{
			if (strtol(ValueList[i].sValue,0,0) >0)
			{
				strcpy(ValueList[icnt].sValue,"1");
				break;
			}
		}
	}
	icnt ++;
	//	return 0;	
	//int saveRcvAlarmInfo(char * phoneNum,int recCount,TValue * outRecs);	
	//������������Ҫ���������� 2014,��ӡ����Ļ���������ݽ������¼���б��棡����
	//	int saveRcvAlarmInfo(TSmsInfo * pSmsInfo,int recCount,TValue * outRecs);
	saveRcvAlarmInfo(&oSmsInfo,icnt,ValueList);
	return 0;
}
/*
1.����������Ҫ���͵ļ�¼
2.��ȡ�����ļ�¼�������з���
3.��ȡ���ڴ�֮�󣬱��״̬
//�Ѳ�
*/
int CDataAnalyzeTask::selectSql(char * sqlString,int icolindex,TRec * reclist)
{
	int k,i=0,icolcnt;

//printf("sql = %s\n",sqlString);
	try
	{
		m_pDataQry->Close();
		m_pDataQry->SetSQL(sqlString);
		m_pDataQry->Open();
		icolcnt = m_pDataQry->FieldCount();
		while(!m_pDataQry->Eof())					
		{
			if (icolindex <0)
			{
				for(k=0;k<icolcnt;k++)
				{
					sprintf(reclist[i].sVal+strlen(reclist[i].sVal),"%s",m_pDataQry->Field(k).asString());
				}
			}
			else
			{
				if (icolindex < icolcnt)
				sprintf(reclist[i].sVal+strlen(reclist[i].sVal),"%s",m_pDataQry->Field(icolindex).asString());
			}
			//ACE_DEBUG((LM_DEBUG,"%s %s %s \n",aInfo.batchid,aInfo.phone,aInfo.info));
			m_pDataQry->Next();
			i++;
		}
		m_pDataQry->Close();
	}
	catch(TOCIException oe)						
	{
		ACE_DEBUG((LM_DEBUG,"selectSql:error:%s [%s]\n",oe.GetErrSrc(),oe.GetErrMsg()));
		//throw pub::TError(-5,"%s [%s]",oe.GetErrSrc(),oe.GetErrMsg());
	}	
	return i;
}

/*
1.����������Ҫ���͵ļ�¼
2.��ȡ�����ļ�¼�������з���
3.��ȡ���ڴ�֮�󣬱��״̬
//�Ѳ�
*/
int CDataAnalyzeTask::selectOneSql(char * sqlString,int reccount,TRec * reclist)
{
	int i;

//printf("sql = %s\n",sqlString);
	try
	{
		m_pDataQry->Close();
		m_pDataQry->SetSQL(sqlString);
		m_pDataQry->Open();
		while(!m_pDataQry->Eof())					
		{
			for(i=0;i<reccount;i++)
			{
				strcpy(reclist[i].sVal,m_pDataQry->Field(reclist[i].sID).asString());
			}
			//ACE_DEBUG((LM_DEBUG,"%s %s %s \n",aInfo.batchid,aInfo.phone,aInfo.info));
			m_pDataQry->Next();
		}
		m_pDataQry->Close();
	}
	catch(TOCIException oe)						
	{
		ACE_DEBUG((LM_DEBUG,"selectOneSql:error:%s [%s]\n",oe.GetErrSrc(),oe.GetErrMsg()));
		//throw pub::TError(-5,"%s [%s]",oe.GetErrSrc(),oe.GetErrMsg());
	}	
	return i;
}
//�Ѳ�
int CDataAnalyzeTask::updateSql(char * sqlString)
{
	int iRet;
//printf("sql = %s\n",sqlString);

	try
	{
		m_pDataQry->Close();
		m_pDataQry->SetSQL(sqlString);					
		m_pDataQry->Execute();
		iRet = m_pDataQry->RowsAffected();
		//printf("ret=%d %s\n",iRet,sqlString);
		m_pDataQry->Commit();
	}
	catch(TOCIException oe)						
	{
		ACE_DEBUG((LM_DEBUG,"updateSql:error:%s [%s]\n",oe.GetErrSrc(),oe.GetErrMsg()));
		//throw pub::TError(-5,"%s [%s]",oe.GetErrSrc(),oe.GetErrMsg());
	}
	return iRet;
}
//��������д������Ķ���Ϣ�����ݿ���
//�Ѳ�
int CDataAnalyzeTask::insertSql(char * sqlString)
{
	int iRet;

//printf("sql = %s\n",sqlString);
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
		ACE_DEBUG((LM_DEBUG,"insertSql:error:%s [%s]\n",oe.GetErrSrc(),oe.GetErrMsg()));
		//throw pub::TError(-5,"%s [%s]",oe.GetErrSrc(),oe.GetErrMsg());
	}	
	return iRet;
}

/*

1.��zx_data�л����Ҫ�����������б������
2.��ms_ol_well �л����ϵ�˺���ϵ����,�������ƣ���ַ
3.��֯��Ϊһ����ɵ�info��䣬�������ݲ��뵽t_sm_sendmsg����

*/
int CDataAnalyzeTask::makeAlarmSms(char * targNo,char * sdatatime)
{
	int i,iRet;
	char sinfo[2048]="",ssql[3096];
	long curval,ddflag;
	char sPhone[32];
	char sDateTime[24];
	TRec  reclist[8]={{"TN_ID",""},{"LX_ID",""},{"SB_ID",""},{"CS_ID",""},{"RT_DD",""},{"DA_VAL",""},{"DA_FG",""},{"DA_DT",""}};
	
	char sqlselect[2048]=" SELECT B.MINGZI ||TRIM(SUBSTR(SHUOMING,INSTR(SHUOMING,'1')+1,INSTR(SHUOMING,'0')-INSTR(SHUOMING,'1')-1)) INFO "
		" FROM ZX_DATA A , MS_OBJCANSHU B "
		" WHERE TRIM( TO_CHAR(A.DATA,'99999')) = '1' "
		" AND A.SHEBEI_ID = '%s' "
		" AND TO_CHAR(A.DATA_TIME,'YYYYMMDDHH24MISS') = '%s' "
		" AND A.CANSHU_ID = B.CANSHU_ID "
		" AND B.LEIXING_ID = 26 "
		" AND B.CANSHU_ID <> '8.F.0' ";

	char sqlinsert[2048]= 
		"INSERT INTO T_SM_SENDMSG "
		"(BATCHID, SENDNUM, INFOMATION, PLANTIME, STATUS) "
		"SELECT T_SM_SENDMSG_SEQ.NEXTVAL,"
		" A.OWNERTEL, "
		" NVL(A.MINGZI, B.SB_ID) || A.REMARK || B.INFO INFO,"
		" SYSDATE - 0.0001, "
		" '1' "
		"FROM MS_OL_WELL A, "
		" (SELECT '%s' SB_ID, ':%s!' INFO "
		"FROM DUAL) B "
		" WHERE A.SHEBEI_ID(+) = B.SB_ID";

	sprintf(ssql,sqlselect,targNo,sdatatime);

	iRet = selectSql(ssql,0,reclist);

	strcpy(sinfo,":");
	for(i=0;i<iRet;i++)
	{
	  strcat(sinfo,reclist[i].sVal);
	}

	if (iRet > 0)
	{
		sprintf(ssql,sqlinsert,targNo,sinfo);
		insertSql(ssql);
	}

	return 0;
}

//��������д������Ķ���Ϣ�����ݿ���
//�Ѳ�
int CDataAnalyzeTask::saveRcvAlarmInfo(TSmsInfo * pSmsInfo,int recCount,TValue * outRecs)
{
	int i,iRet;
	char sqlString[4][3096];
	long curval,ddflag;
	char sPhone[32];
	char sDateTime[24];
	char * targNo, * sdatatime;
	TRec  reclist[8]={{"TN_ID",""},{"LX_ID",""},{"SB_ID",""},{"CS_ID",""},{"RT_DD",""},{"DA_VAL",""},{"DA_FG",""},{"DA_DT",""}};

	
	char sqlselect[2048]=
		"  SELECT NVL(B.TN_ID,1) TN_ID,\n"
		"         A.LX_ID,\n"
		"         NVL(NVL(B.WELL_ID, B.SMSNO), A.PHONE) SB_ID,\n"
		"         A.CS_ID,\n"
		"         TO_CHAR(A.RT,'YYYYMMDDHH24MISS') RT_DD,\n"
		"         A.DA_VAL,\n"
		"         A.DA_FG,\n"
		"         TO_CHAR(A.RT,'YYYYMMDDHH24MISS') DA_DT \n"
		"    FROM (SELECT '26' LX_ID,\n"
		"                 UPPER('%s') CS_ID,\n"
		"                 '%d' DA_VAL,\n"
		"                 '%d' DA_FG,\n"
		"                 TO_DATE('%s', 'YYMMDDHH24MISS') RT,\n" //14050711403032 YYMMDDHHMISS
		"                 SUBSTR('%s',-11,11) PHONE\n"
		"            FROM DUAL) A,\n"
		"         MS_OL_TERM B\n"
		"   WHERE A.PHONE = B.SMSNO(+)\n";

	char sqlinsert[2048]= 
		"INSERT INTO ZX_DATA\n"
		"  (TN_ID, LEIXING_ID, SHEBEI_ID, CANSHU_ID, RTIME, DATA, DATA_FLAG,DATA_TIME)\n"
		"VALUES('%s','%s','%s','%s',TO_DATE('%s','YYYYMMDDHH24MISS'),'%s','%s',TO_DATE('%s','YYYYMMDDHH24MISS'))";

	char sqlupdate[2048]= 
		"UPDATE ZX_DATA SET\n"
		" RTIME = TO_DATE('%s','YYYYMMDDHH24MISS'), DATA='%s', DATA_FLAG='%s',DATA_TIME=TO_DATE('%s','YYYYMMDDHH24MISS') \n"
		"WHERE TN_ID='%s' AND LEIXING_ID='%s' AND SHEBEI_ID='%s' AND CANSHU_ID='%s'";
 
	char sqlhisinsert[2048]= 
		"INSERT INTO LX_DATA\n"
		"  (TN_ID, LEIXING_ID, SHEBEI_ID, CANSHU_ID, RTIME, DATA, DATA_FLAG,DATA_TIME)\n"
		" SELECT TN_ID, LEIXING_ID, SHEBEI_ID, CANSHU_ID, RTIME, DATA, DATA_FLAG,DATA_TIME FROM ZX_DATA A \n"
		" WHERE A.TN_ID='%s' AND A.LEIXING_ID='%s' AND A.SHEBEI_ID='%s' "
		" AND A.RTIME = TO_DATE('%s','YYYYMMDDHH24MISS')";


	strncpy(sDateTime,pSmsInfo->datatime,12);
	sDateTime[12]=0;
	strcpy(sPhone,pSmsInfo->phone);

	////���һ������״̬�ı�������
	for(i=0;i<recCount;i++)
	{
//		printf("ID=%s,Value=%s\n",outRecs[i].sID,outRecs[i].sValue);

		//��Ҫ���屨���л����ʷ��¼������������ݣ��ͽ������뱨�Ľ��кϲ�
		curval =  strtol(outRecs[i].sValue,0,0);
		ddflag = 0;
		if (curval==1)	ddflag |=131072; //0  ���� 262144 �澯 131072 ע��

		sprintf(sqlString[0],sqlselect,
			outRecs[i].sID, //���ݲ���
			curval, //����ֵ
			ddflag	    ,	// DATA_FLAG ����λ���б��
			sDateTime,		  //���ݲ���ʱ��
			sPhone      	// �绰����
			);

		selectOneSql(sqlString[0],sizeof(reclist)/sizeof(TRec),reclist);

		sprintf(sqlString[1],sqlinsert,reclist[0].sVal,reclist[1].sVal,reclist[2].sVal,
				reclist[3].sVal,reclist[4].sVal,reclist[5].sVal,reclist[6].sVal,reclist[7].sVal);

		sprintf(sqlString[2],sqlupdate,reclist[4].sVal,reclist[5].sVal,reclist[6].sVal,reclist[7].sVal,
			reclist[0].sVal,reclist[1].sVal,reclist[2].sVal,reclist[3].sVal);
		
//		sprintf(sqlString[3],sqlhisinsert,
//			reclist[0].sVal,reclist[1].sVal,reclist[2].sVal,reclist[4].sVal);

		

		iRet = updateSql(sqlString[2]);
		if (iRet==0)
			insertSql(sqlString[1]);

		//������ʷ��¼
		
		//	insertSql(sqlString[3]);
//	static char* replaceString(const char*sSrc,const char*sOld,const char*sNew)
		//����¼������������ʷ���У��γ���������ʷ��¼

	}

	////������ʷ��¼
	sprintf(sqlString[3],sqlhisinsert,
			reclist[0].sVal,reclist[1].sVal,reclist[2].sVal,reclist[4].sVal);
	
	insertSql(sqlString[3]);

	targNo = reclist[2].sVal;
	sdatatime = reclist[4].sVal;
	
	makeAlarmSms(targNo,sdatatime);

	return 0;
}

