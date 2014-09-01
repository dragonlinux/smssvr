// GrpsSms.cpp: implementation of the CGprsSms class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "GprsSms.h"
#include "commTask.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CGprsSms::CGprsSms()
{
}

CGprsSms::~CGprsSms()
{
}

int CGprsSms::WriteComm(char* pData, int nLength)
{
	return TheCommTask::instance()->WriteComm(pData,nLength);
}

int CGprsSms::ReadComm(char* pData, int nLength,int rType,int nSec)
{
	return TheCommTask::instance()->ReadComm(pData,nLength,rType,nSec);
}
//获得设备的响应
int  CGprsSms::GetResponse(SM_BUFF* pBuff,int rType,int nSec)
{
	int nLength;		// 串口收到的数据长度
	int nState;
	char * pFind;
	// 从串口读数据，追加到缓冲区尾部
	nLength = ReadComm(&pBuff->data[pBuff->len], 160,rType,nSec);
	pBuff->len += nLength;
	pBuff->data[pBuff->len]=0;
	// 确定GSM MODEM的应答状态  可能出现接收短信情况CMTI....
	nState = GSM_WAIT;
	if ((nLength > 0) && (pBuff->len >= 4))
	{
		if ((pFind = strstr(pBuff->data, "\x0aOK")) != NULL)
		{
			*pFind=0; //!!!! OK after ok string appear, the modem maybe send the other chars to fill the buff such as 160
			nState = GSM_OK;
		}
		else if ((pFind = strstr(pBuff->data, "ERROR")) != NULL) 
		{
			*pFind=0;
			nState = GSM_ERR;
		}
		else nState = GSM_READING;
	} 
	
	return nState;
}
//ATQ0=0 DCE发送结果码
//如果是采用厦门四信的，需要先断开网络 发送命令 ***COMMIT CONFIG**** 
// 等待返回指令后，发送 sssssss 到 返回进行配置模式 ---serite
//短信方式初始化
int CGprsSms::InitSms()
{
	int i;
	char ans[160];		// 应答串AT+MODE=TRNS
	memset((char *)ans, 0, sizeof(ans));
	char sAsk[6][32]={"AT\r\n","ATE0\r\n","ATQ0\r\n","ATV1\r\n","AT+CMGF=0\r\n","AT+CNMI=2,0,0,0,1\r\n"};//,"AT+CPIN?\r\n",};//,"AT+MODE=TRNS\r"};
	//AT 测试GSM-MODEM的存在性
	//ATQ0V1E0 等效 ATQ0\rATV1\rATE0\r
	/* ATQ0 设备需要返回结果码, ATQ1无响应
	ATV0 响应格式数字, ATV1 字符响应
	ATE0 不回显,ATE1 回显
	*/
	//AT+CMGF=0 0:PDU模式  1:text模式
	//AT+CNMI=0,0,0,0,1\r
	////设置接收短信不通知，直接存放在sim卡上,等待主动查询删除
	for(i=0;i<6;i++)
	{
		WriteComm(sAsk[i],strlen(sAsk[i]));
		Sleep(200);
		ReadComm(ans, 160,0,1);
		if (strstr(ans, "OK") == 0) 
		{
			//return 0;
			Sleep(1200);
		}
	}
	CSmsPDU::getEnvLang();
	return 1;
}
// 如果包含，则系统只能按照一个英文按照两个字节计算
int CGprsSms::splitSms(int msgLen,char* pMsgBuf ,int &smscnt,char  ** smslist)
{
	int i,j,icnt,bWchar,iMsgLen;
	unsigned char *pSrc,*uDes;
	pSrc = (unsigned char *)pMsgBuf;
	j = 0;
	smscnt = 0;
	uDes = (unsigned char *)smslist[0];
	bWchar = 0;
	for(i=0;i<msgLen;i++)
	{
		if (*(pSrc+i) > 0x80) //说明中间存在中文，全部都要按照双字节处理
		{
			bWchar = 1;
			break;
		}
	}
	iMsgLen = 0;
	for(i=0;i<msgLen;i++)
	{
		if (iMsgLen % SMS_PKG_MAXLEN ==0)  // 从140更改为134，留6个字节给自定义的包头
		{
			if ((j % 2)!=0) //说明存在半个汉字
			{
				i--; //将前面的字符复位
				*(uDes-1) = 0;
				j = 0;
			}
			uDes = (unsigned char *)smslist[smscnt++];
		}
		*uDes = *(pSrc+i);
		if (*uDes > 0x80) //发现汉字,要保证成对出现
		{
			j++;
		}else
		{
			if (bWchar)
			{ //一个asii要算两个字节
				iMsgLen ++;
			}
		}
		uDes ++ ;
		iMsgLen ++;
	}
	return smscnt;
}
//发送短信
/******************************************************************
* Name: writeSms
* Description: 发送短信
* Parameter: 
pMsc : 短信中心号码8613800591500
pMobileNo : 对方号码
pMsg :短信息内容
* Return: int >0 成功;0 -1 失败
* Cautions:  　　　　　　　　　　　　　　　　　　　　　　　　    
******************************************************************/
int CGprsSms::SndSms(const char* pMsc,const char* pMobileNo,const char* pMsgBuf , int msgLen)
{
	char smslist[64][144];
	char sPhone[64];
	char * pSms[64];
	int  i,smscnt=0,iRet;
	TPDU_UDHI vUdhi={6,8,4,0,0,0};
	for(i=0;i<64;i++)
	{
		pSms[i] = smslist[i];
	}
	memset((char *)smslist,0,sizeof(smslist));
	splitSms(msgLen,(char *)pMsgBuf ,smscnt,pSms);
	SmsSeqNum ++ ;
	vUdhi.amt = smscnt;
	vUdhi.batid = SmsSeqNum;
	vUdhi.seq = 0;
	strcpy(sPhone,pMobileNo);
	//判断号码长度是否满足13位长度
	if (strlen(pMobileNo) < 13)
	{
		sprintf(sPhone,"86%s",pMobileNo);
	}
	for(i=0;i<smscnt;i++)
	{
		vUdhi.seq=i+1;
		iRet= _SndSms(pMsc,sPhone,smslist[i] , strlen(smslist[i]),&vUdhi);
		if (iRet <0 ) return iRet;
	}
	return smscnt;
}
//发送短信
/******************************************************************
* Name: writeSms
* Description: 发送短信
* Parameter: 
pMsc : 短信中心号码8613800591500
pMobileNo : 对方号码
pMsg :短信息内容
* Return: int >0 成功;0 -1 失败
* Cautions:  　　　　　　　　　　　　　　　　　　　　　　　　    
******************************************************************/
int CGprsSms::_SndSms(const char* pMsc,const char* pMobileNo,const char* pMsgBuf , int msgLen,TPDU_UDHI *pUdhi)
{
	//	CSingleLock sLock( &rMailBuffLock );
	//  sLock.Lock();
	//	EnterCriticalSection(&m_csSend);
	int i,irealLen,iwaitcnt;
	SM_PARAM SmParam;
	SM_BUFF buff;			// 接收短消息列表的缓冲区
	int ret;
	int icnt=5;
	irealLen = msgLen > SMS_PKG_MAXLEN ? SMS_PKG_MAXLEN : msgLen;
	memset(&SmParam, 0, sizeof(SM_PARAM));
	SmParam.TP_UDHI = *pUdhi;
	// 填充短消息结构
	strcpy(SmParam.SCA, pMsc);
	strcpy(SmParam.TPA, pMobileNo);
	memcpy((char *)SmParam.TP_UD,pMsgBuf,irealLen);
	SmParam.Len = irealLen;
	SmParam.TP_PID = 0;
	SmParam.TP_DCS = GSM_7BIT;
	for(i=0;i<SmParam.Len;i++)
	{
		//一旦发现出现了中文，就采用uncode发送
		if ((unsigned char)SmParam.TP_UD[i]>0x80)
		{
			SmParam.TP_DCS = GSM_UCS2;
			break;
		}
	}

	while (icnt -- )
	{
		memset((char *)&buff, 0, sizeof(buff));
		ret = PutSmsCmd(&SmParam);  //发送成功结果>0
		if (ret == GSM_ERR) 
		{
			return ret;
		}
		//------------------------------------------------------------------
		Sleep(200);
		iwaitcnt = 15;
		while ((ret = GetResponse(&buff)) == GSM_WAIT)
		{
			if (iwaitcnt-- < 0) return GSM_ERR;
			Sleep(200);
		}
		if (GSM_ERR == ret)  //如果返回是GSM_ERR，则重新发送	
		{
			continue;
		}
		return ret;
	}
	//	sLock.Unlock();
	//	LeaveCriticalSection(&m_csSend);
	return GSM_ERR;
}
//读取短信内容
/******************************************************************
* Name: ReceiveMsg
* Description: 接收短信
* Parameter:
* Return:  
* Cautions:  　　　　　　　　　　　　　　　　　　　　　　　　    
******************************************************************/
int CGprsSms::ReadSms(TSmsInfo * SmsList )
{
	//	CSingleLock sLock( &rMailBuffLock );
	//	sLock.Lock();
	//	EnterCriticalSection(&m_csSend);
	//SM_PARAM* ret;
	//memset(&ret, 0, sizeof(ret));
	char* pStrContent=0;
	SM_BUFF buff;			// 接收短消息列表的缓冲区
	SM_PARAM param[256];	// 发送/接收短消息缓冲区
	TSmsInfo aSms;
	int nMsg;				// 收到短消息条数
	int i =0,nsec=0;
	int ret=0; 
	int cnt=500;  //当前短信（50条未读) about 2
	GetSmsCmd();
	memset(&buff, 0, sizeof(buff));  //初始化返回结果
	ret = GSM_WAIT; 
	
	//需要读取返回结果，直到GSM_OK状态为止
	while ((ret != GSM_OK) && (cnt --)  )  //&& (cnt --) 
	{
		nsec = 3;
		ret = GetResponse(&buff,100,nsec);
		
		if (GSM_WAIT == ret)
		{
			cnt = cnt >3 ? 3 :cnt;
		}else if (GSM_READING == ret)
		{
			nsec = 1;
			continue;
		}	
		printf(".");fflush(stdout);
	}

	if (ret==GSM_OK) //成功后进行删除
	{

		nMsg = ParseMessageList(param, &buff); //返回条数（目前一条一条读取）

		if (nMsg>0)
		{
			ClearSms(nMsg,param); //删除所有信息
		}
	}else
	{
		return GSM_ERR;
	}

	for(i=0;i<nMsg;i++)
	{
		strcpy(aSms.datatime,param[i].TP_SCTS);
		strcpy(aSms.phone,param[i].TPA);
		strcpy(aSms.info,param[i].TP_UD);
		SmsList[i] = aSms;
	}
	//	sLock.Unlock();
	//	LeaveCriticalSection(&m_csSend);
	return nMsg;
}
//删除指定序号的短信
int CGprsSms::DeleteSmsCmd(int index)
{
	char cmd[32];		// 命令串

	if (index == -1)
	    strcpy(cmd,"AT+CMGD=1,2\r");	// 生成命令
	else
  	   sprintf(cmd,"AT+CMGD=%d\r", index);	// 生成命令
	// 输出命令串
	return WriteComm(cmd, strlen(cmd));
}
// 读取短消息，仅发送命令，不读取应答
// 用+CMGL代替+CMGR，可一次性读出全部短消息
int CGprsSms::GetSmsCmd()
{
	//return WriteComm("AT+CMGL\r", 8);
	char cmd[16] = "AT+CMGL=4\r";		// 命令串  4 是针对pdu 方式 
	// 输出命令串
	return WriteComm(cmd, strlen(cmd));
}
//清除所有的短信 
int CGprsSms::ClearSms(int icnt,SM_PARAM* pMsg)
{
	int nDelete,i,rCnt,iRet;
	SM_BUFF smsBuff;
	//return 0;//????for test
	for(i=0;i<icnt;i++)
	{
		if (i == -1)
		{
		    nDelete = -1 ; //索引
		}
		else
		{
			nDelete = pMsg[i].index ; //索引
		}
		ACE_DEBUG((LM_DEBUG,"ClearSms:Index = %d\n",nDelete));
	
		DeleteSmsCmd(nDelete);		
		memset((char *)&smsBuff, 0, sizeof(SM_BUFF));
		Sleep(200);
		//得到删除的返回结果
		rCnt = 20;
		while (((iRet = GetResponse(&smsBuff)) != 1) && (rCnt --) )  //直到删除成功为止
		{
			Sleep(200);				
		}
		if ((GSM_OK == iRet)&&(i == -1)) break;
	}
	return 0;
}

int CGprsSms::ParseMessageList(SM_PARAM* pMsg, SM_BUFF* pBuff)
{
	int nMsg,i,iret;			// 短消息计数值
	char* ptr;			// 内部用的数据指针
	char * sList[64];
	int lstCnt=0;

	nMsg = 0;
	ptr = pBuff->data;
	i = 0;
	
	lstCnt = pub::strsplit(ptr,"+CMGL",64,sList); //用这个作为每个记录的串的分割字符串
	for(i=0;i<lstCnt;i++)
	{
		ptr = sList[i];
		if ((*ptr)!=':') continue; //+CMGL: seek the head of the msg
		if (strlen(ptr) < 6) continue;

		sscanf(ptr+1, "%d", &pMsg->index);	// 读取序号
		ptr = strstr(ptr, "\x0a");	// 找下一行 z
		//PC机 用 CRLF,苹果机用CR,unix 用 LF,CR -- 回车符,c语言'\r',LF -- 换行符, c语言'\n'
		if (ptr ==NULL) continue;

		while (*ptr== 0XA) ptr ++;
		
		
		iret=CSmsPDU::Decode(ptr,pMsg);	// PDU串解码
		ACE_DEBUG((LM_DEBUG,"ParseMessageLis:iret=%d IDX[%d]TIME[20%s]sPHONE[%s]\nMSG(%s)\n",iret,pMsg->index,pMsg->TP_SCTS,pMsg->TPA,pMsg->TP_UD));

		pMsg++;		// 准备读下一条短消息
		nMsg++;		// 短消息计数加1

	}

	return nMsg;
}

// 发送短消息，仅发送命令，不读取应答
// 输入: pSrc - 源PDU参数指针
int CGprsSms::PutSmsCmd(SM_PARAM* pSrc)
{
	int nPduLength,nCnt,iwaitcnt;		// PDU串长度
	unsigned char nSmscLength;	// SMSC串长度
	int nLength;		// 串口收到的数据长度
	char cmd[16];		// 命令串
	char pdu[512];		// PDU串
	char ans[256];		// 应答串
	
	memset(&ans, 0, sizeof(ans));
	nPduLength = CSmsPDU::EncodeFull(pSrc, pdu);	// 根据PDU参数，编码PDU串
	strcat(pdu, "\x01a");		// 以Ctrl-Z结束

	CSmsPDU::String2Bytes(pdu, &nSmscLength, 2);	// 取PDU串中的SMSC信息长度
	nSmscLength++;		// 加上长度字节本身
	// 命令中的长度，不包括SMSC信息长度，以数据字节计
	sprintf(cmd, "AT+CMGS=%d\x0a\x0a\x0d", nPduLength / 2 - nSmscLength);	// 生成命令
	ACE_DEBUG((LM_DEBUG,"CMD:{%s}\nPDU:{%s}\n", cmd,pdu));

	nCnt = 3;
	while(nCnt -- )
	{
		WriteComm(cmd, strlen(cmd));	// 先输出命令串
		Sleep(200);
		iwaitcnt = 10;
		while((nLength = ReadComm(ans, 160,2))==0) // 读应答数据  //---->应答中可能有短信回复上来  -- 被严澍关了
		{
			if (iwaitcnt-- < 0) return GSM_ERR;
			Sleep(200);
		}
		if (strstr(ans, "ERROR") != NULL)
		{
			continue;
		}
		break;
	}

	if (nCnt <= 0) return GSM_ERR;

	if (nLength>=4 && strstr(ans,">") !=NULL) //可能串口上报情况出现CMTI字符\r\n
	{
		WriteComm(pdu, strlen(pdu));		// 得到肯定回答，继续输出PDU串
	}
	return 0;
}
