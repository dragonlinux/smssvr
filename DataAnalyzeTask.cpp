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
	this->m_wait_milliseconds = 200;//0.2秒扫描一次
}
CDataAnalyzeTask::~CDataAnalyzeTask()
{
	ACE_DEBUG((LM_DEBUG,"free CDataAnalyzeTask...\n"));
}
void CDataAnalyzeTask::init(TOCIDatabase * poraDb)
{
	m_pDataQry = new TOCIQuery(poraDb);
}
//扫描数据库，每批取得命令
int CDataAnalyzeTask::beforeGetMq(int idx)
{
	Sleep(10);

	//makeAlarmSms("1","20140512054345");
	//sleep(20);
	return 0;
}
//处理收到的信息,将相应的内容设置数据库对应的命令序列的序列号，以形成一个完整的闭环操作
int CDataAnalyzeTask::deal(ACE_Message_Block *&mb, ACE_Time_Value *timeout)
{
	//解析出客户端的状态,将相应的客户端状态设置到数据库中
	//获得指令，将其保存到数据中
	char * pMsg = NULL;
	int msglen = 0;
	int i,icnt,iusedlen=0;
	char buffer[5120];//缓存区
	TSmsInfo oSmsInfo;
	TValue ValueList[100];
	TMessage_Block * pBlk = (TMessage_Block *)mb;
	//下面将内存保存到本地，并释放所占资源
	pMsg = mb->rd_ptr();
	msglen = mb->length();
	memcpy((char *)&oSmsInfo,pMsg,msglen);
	pBlk->release();//释放自己的锁定资源 ,这样处理，能够防止后面非正常退出造成某个处理没有正常释放资源

	icnt=sysDealRecvInfo(strlen(oSmsInfo.info),(unsigned char *)oSmsInfo.info,ValueList);
	
	//产生一条总体记录，确定整体状态是否正常
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
	//在这里增加需要解析的内容 2014,打印到屏幕，，并根据结果将记录进行保存！！！
	//	int saveRcvAlarmInfo(TSmsInfo * pSmsInfo,int recCount,TValue * outRecs);
	saveRcvAlarmInfo(&oSmsInfo,icnt,ValueList);
	return 0;
}
/*
1.首先锁定需要发送的记录
2.读取锁定的记录，并进行发送
3.读取到内存之后，变更状态
//已测
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
1.首先锁定需要发送的记录
2.读取锁定的记录，并进行发送
3.读取到内存之后，变更状态
//已测
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
//已测
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
//保存队列中传送来的短消息到数据库中
//已测
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

1.从zx_data中获得需要报警的名称列表和内容
2.从ms_ol_well 中获得联系人和联系方法,井盖名称，地址
3.组织成为一个完成的info语句，并将内容插入到t_sm_sendmsg表中

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

//保存队列中传送来的短消息到数据库中
//已测
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

	////添加一条总体状态的报警内容
	for(i=0;i<recCount;i++)
	{
//		printf("ID=%s,Value=%s\n",outRecs[i].sID,outRecs[i].sValue);

		//需要整体报文中获得历史记录，如果存在数据，就将数据与报文进行合并
		curval =  strtol(outRecs[i].sValue,0,0);
		ddflag = 0;
		if (curval==1)	ddflag |=131072; //0  正常 262144 告警 131072 注意

		sprintf(sqlString[0],sqlselect,
			outRecs[i].sID, //数据参数
			curval, //数据值
			ddflag	    ,	// DATA_FLAG 后四位进行表达
			sDateTime,		  //数据产生时间
			sPhone      	// 电话号码
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

		//创建历史记录
		
		//	insertSql(sqlString[3]);
//	static char* replaceString(const char*sSrc,const char*sOld,const char*sNew)
		//将记录产生到在线历史表中，形成完整的历史记录

	}

	////创建历史记录
	sprintf(sqlString[3],sqlhisinsert,
			reclist[0].sVal,reclist[1].sVal,reclist[2].sVal,reclist[4].sVal);
	
	insertSql(sqlString[3]);

	targNo = reclist[2].sVal;
	sdatatime = reclist[4].sVal;
	
	makeAlarmSms(targNo,sdatatime);

	return 0;
}

