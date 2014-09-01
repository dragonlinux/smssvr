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
//����豸����Ӧ
int  CGprsSms::GetResponse(SM_BUFF* pBuff,int rType,int nSec)
{
	int nLength;		// �����յ������ݳ���
	int nState;
	char * pFind;
	// �Ӵ��ڶ����ݣ�׷�ӵ�������β��
	nLength = ReadComm(&pBuff->data[pBuff->len], 160,rType,nSec);
	pBuff->len += nLength;
	pBuff->data[pBuff->len]=0;
	// ȷ��GSM MODEM��Ӧ��״̬  ���ܳ��ֽ��ն������CMTI....
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
//ATQ0=0 DCE���ͽ����
//����ǲ����������ŵģ���Ҫ�ȶϿ����� �������� ***COMMIT CONFIG**** 
// �ȴ�����ָ��󣬷��� sssssss �� ���ؽ�������ģʽ ---serite
//���ŷ�ʽ��ʼ��
int CGprsSms::InitSms()
{
	int i;
	char ans[160];		// Ӧ��AT+MODE=TRNS
	memset((char *)ans, 0, sizeof(ans));
	char sAsk[6][32]={"AT\r\n","ATE0\r\n","ATQ0\r\n","ATV1\r\n","AT+CMGF=0\r\n","AT+CNMI=2,0,0,0,1\r\n"};//,"AT+CPIN?\r\n",};//,"AT+MODE=TRNS\r"};
	//AT ����GSM-MODEM�Ĵ�����
	//ATQ0V1E0 ��Ч ATQ0\rATV1\rATE0\r
	/* ATQ0 �豸��Ҫ���ؽ����, ATQ1����Ӧ
	ATV0 ��Ӧ��ʽ����, ATV1 �ַ���Ӧ
	ATE0 ������,ATE1 ����
	*/
	//AT+CMGF=0 0:PDUģʽ  1:textģʽ
	//AT+CNMI=0,0,0,0,1\r
	////���ý��ն��Ų�֪ͨ��ֱ�Ӵ����sim����,�ȴ�������ѯɾ��
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
// �����������ϵͳֻ�ܰ���һ��Ӣ�İ��������ֽڼ���
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
		if (*(pSrc+i) > 0x80) //˵���м�������ģ�ȫ����Ҫ����˫�ֽڴ���
		{
			bWchar = 1;
			break;
		}
	}
	iMsgLen = 0;
	for(i=0;i<msgLen;i++)
	{
		if (iMsgLen % SMS_PKG_MAXLEN ==0)  // ��140����Ϊ134����6���ֽڸ��Զ���İ�ͷ
		{
			if ((j % 2)!=0) //˵�����ڰ������
			{
				i--; //��ǰ����ַ���λ
				*(uDes-1) = 0;
				j = 0;
			}
			uDes = (unsigned char *)smslist[smscnt++];
		}
		*uDes = *(pSrc+i);
		if (*uDes > 0x80) //���ֺ���,Ҫ��֤�ɶԳ���
		{
			j++;
		}else
		{
			if (bWchar)
			{ //һ��asiiҪ�������ֽ�
				iMsgLen ++;
			}
		}
		uDes ++ ;
		iMsgLen ++;
	}
	return smscnt;
}
//���Ͷ���
/******************************************************************
* Name: writeSms
* Description: ���Ͷ���
* Parameter: 
pMsc : �������ĺ���8613800591500
pMobileNo : �Է�����
pMsg :����Ϣ����
* Return: int >0 �ɹ�;0 -1 ʧ��
* Cautions:  ������������������������������������������������    
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
	//�жϺ��볤���Ƿ�����13λ����
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
//���Ͷ���
/******************************************************************
* Name: writeSms
* Description: ���Ͷ���
* Parameter: 
pMsc : �������ĺ���8613800591500
pMobileNo : �Է�����
pMsg :����Ϣ����
* Return: int >0 �ɹ�;0 -1 ʧ��
* Cautions:  ������������������������������������������������    
******************************************************************/
int CGprsSms::_SndSms(const char* pMsc,const char* pMobileNo,const char* pMsgBuf , int msgLen,TPDU_UDHI *pUdhi)
{
	//	CSingleLock sLock( &rMailBuffLock );
	//  sLock.Lock();
	//	EnterCriticalSection(&m_csSend);
	int i,irealLen,iwaitcnt;
	SM_PARAM SmParam;
	SM_BUFF buff;			// ���ն���Ϣ�б�Ļ�����
	int ret;
	int icnt=5;
	irealLen = msgLen > SMS_PKG_MAXLEN ? SMS_PKG_MAXLEN : msgLen;
	memset(&SmParam, 0, sizeof(SM_PARAM));
	SmParam.TP_UDHI = *pUdhi;
	// ������Ϣ�ṹ
	strcpy(SmParam.SCA, pMsc);
	strcpy(SmParam.TPA, pMobileNo);
	memcpy((char *)SmParam.TP_UD,pMsgBuf,irealLen);
	SmParam.Len = irealLen;
	SmParam.TP_PID = 0;
	SmParam.TP_DCS = GSM_7BIT;
	for(i=0;i<SmParam.Len;i++)
	{
		//һ�����ֳ��������ģ��Ͳ���uncode����
		if ((unsigned char)SmParam.TP_UD[i]>0x80)
		{
			SmParam.TP_DCS = GSM_UCS2;
			break;
		}
	}

	while (icnt -- )
	{
		memset((char *)&buff, 0, sizeof(buff));
		ret = PutSmsCmd(&SmParam);  //���ͳɹ����>0
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
		if (GSM_ERR == ret)  //���������GSM_ERR�������·���	
		{
			continue;
		}
		return ret;
	}
	//	sLock.Unlock();
	//	LeaveCriticalSection(&m_csSend);
	return GSM_ERR;
}
//��ȡ��������
/******************************************************************
* Name: ReceiveMsg
* Description: ���ն���
* Parameter:
* Return:  
* Cautions:  ������������������������������������������������    
******************************************************************/
int CGprsSms::ReadSms(TSmsInfo * SmsList )
{
	//	CSingleLock sLock( &rMailBuffLock );
	//	sLock.Lock();
	//	EnterCriticalSection(&m_csSend);
	//SM_PARAM* ret;
	//memset(&ret, 0, sizeof(ret));
	char* pStrContent=0;
	SM_BUFF buff;			// ���ն���Ϣ�б�Ļ�����
	SM_PARAM param[256];	// ����/���ն���Ϣ������
	TSmsInfo aSms;
	int nMsg;				// �յ�����Ϣ����
	int i =0,nsec=0;
	int ret=0; 
	int cnt=500;  //��ǰ���ţ�50��δ��) about 2
	GetSmsCmd();
	memset(&buff, 0, sizeof(buff));  //��ʼ�����ؽ��
	ret = GSM_WAIT; 
	
	//��Ҫ��ȡ���ؽ����ֱ��GSM_OK״̬Ϊֹ
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

	if (ret==GSM_OK) //�ɹ������ɾ��
	{

		nMsg = ParseMessageList(param, &buff); //����������Ŀǰһ��һ����ȡ��

		if (nMsg>0)
		{
			ClearSms(nMsg,param); //ɾ��������Ϣ
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
//ɾ��ָ����ŵĶ���
int CGprsSms::DeleteSmsCmd(int index)
{
	char cmd[32];		// ���

	if (index == -1)
	    strcpy(cmd,"AT+CMGD=1,2\r");	// ��������
	else
  	   sprintf(cmd,"AT+CMGD=%d\r", index);	// ��������
	// ������
	return WriteComm(cmd, strlen(cmd));
}
// ��ȡ����Ϣ���������������ȡӦ��
// ��+CMGL����+CMGR����һ���Զ���ȫ������Ϣ
int CGprsSms::GetSmsCmd()
{
	//return WriteComm("AT+CMGL\r", 8);
	char cmd[16] = "AT+CMGL=4\r";		// ���  4 �����pdu ��ʽ 
	// ������
	return WriteComm(cmd, strlen(cmd));
}
//������еĶ��� 
int CGprsSms::ClearSms(int icnt,SM_PARAM* pMsg)
{
	int nDelete,i,rCnt,iRet;
	SM_BUFF smsBuff;
	//return 0;//????for test
	for(i=0;i<icnt;i++)
	{
		if (i == -1)
		{
		    nDelete = -1 ; //����
		}
		else
		{
			nDelete = pMsg[i].index ; //����
		}
		ACE_DEBUG((LM_DEBUG,"ClearSms:Index = %d\n",nDelete));
	
		DeleteSmsCmd(nDelete);		
		memset((char *)&smsBuff, 0, sizeof(SM_BUFF));
		Sleep(200);
		//�õ�ɾ���ķ��ؽ��
		rCnt = 20;
		while (((iRet = GetResponse(&smsBuff)) != 1) && (rCnt --) )  //ֱ��ɾ���ɹ�Ϊֹ
		{
			Sleep(200);				
		}
		if ((GSM_OK == iRet)&&(i == -1)) break;
	}
	return 0;
}

int CGprsSms::ParseMessageList(SM_PARAM* pMsg, SM_BUFF* pBuff)
{
	int nMsg,i,iret;			// ����Ϣ����ֵ
	char* ptr;			// �ڲ��õ�����ָ��
	char * sList[64];
	int lstCnt=0;

	nMsg = 0;
	ptr = pBuff->data;
	i = 0;
	
	lstCnt = pub::strsplit(ptr,"+CMGL",64,sList); //�������Ϊÿ����¼�Ĵ��ķָ��ַ���
	for(i=0;i<lstCnt;i++)
	{
		ptr = sList[i];
		if ((*ptr)!=':') continue; //+CMGL: seek the head of the msg
		if (strlen(ptr) < 6) continue;

		sscanf(ptr+1, "%d", &pMsg->index);	// ��ȡ���
		ptr = strstr(ptr, "\x0a");	// ����һ�� z
		//PC�� �� CRLF,ƻ������CR,unix �� LF,CR -- �س���,c����'\r',LF -- ���з�, c����'\n'
		if (ptr ==NULL) continue;

		while (*ptr== 0XA) ptr ++;
		
		
		iret=CSmsPDU::Decode(ptr,pMsg);	// PDU������
		ACE_DEBUG((LM_DEBUG,"ParseMessageLis:iret=%d IDX[%d]TIME[20%s]sPHONE[%s]\nMSG(%s)\n",iret,pMsg->index,pMsg->TP_SCTS,pMsg->TPA,pMsg->TP_UD));

		pMsg++;		// ׼������һ������Ϣ
		nMsg++;		// ����Ϣ������1

	}

	return nMsg;
}

// ���Ͷ���Ϣ���������������ȡӦ��
// ����: pSrc - ԴPDU����ָ��
int CGprsSms::PutSmsCmd(SM_PARAM* pSrc)
{
	int nPduLength,nCnt,iwaitcnt;		// PDU������
	unsigned char nSmscLength;	// SMSC������
	int nLength;		// �����յ������ݳ���
	char cmd[16];		// ���
	char pdu[512];		// PDU��
	char ans[256];		// Ӧ��
	
	memset(&ans, 0, sizeof(ans));
	nPduLength = CSmsPDU::EncodeFull(pSrc, pdu);	// ����PDU����������PDU��
	strcat(pdu, "\x01a");		// ��Ctrl-Z����

	CSmsPDU::String2Bytes(pdu, &nSmscLength, 2);	// ȡPDU���е�SMSC��Ϣ����
	nSmscLength++;		// ���ϳ����ֽڱ���
	// �����еĳ��ȣ�������SMSC��Ϣ���ȣ��������ֽڼ�
	sprintf(cmd, "AT+CMGS=%d\x0a\x0a\x0d", nPduLength / 2 - nSmscLength);	// ��������
	ACE_DEBUG((LM_DEBUG,"CMD:{%s}\nPDU:{%s}\n", cmd,pdu));

	nCnt = 3;
	while(nCnt -- )
	{
		WriteComm(cmd, strlen(cmd));	// ��������
		Sleep(200);
		iwaitcnt = 10;
		while((nLength = ReadComm(ans, 160,2))==0) // ��Ӧ������  //---->Ӧ���п����ж��Żظ�����  -- ����������
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

	if (nLength>=4 && strstr(ans,">") !=NULL) //���ܴ����ϱ��������CMTI�ַ�\r\n
	{
		WriteComm(pdu, strlen(pdu));		// �õ��϶��ش𣬼������PDU��
	}
	return 0;
}
