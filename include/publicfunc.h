//publicfunc.h

//
///////////////////////////////////////////////////////////////////////////////
#if !defined(_publicfunc_)
#define _publicfunc_
//publicfunc.h
#pragma once

#define Sleep(x) ACE_OS::sleep(ACE_Time_Value(0, x))
#define LOG_FILE_SEQ 99

//#include <windows.h>

#include <time.h>
#include <assert.h>
#include <stdio.h> 
#include <string.h> 
#include <stdlib.h> 
#include <math.h>
#include <stdarg.h>
#include <sys/stat.h>
#include "public_template.h"

#define CK_OK 1
#define CK_CONFIRM 2
#define CK_NAGETIVE 3
#define CK_LOGININ 4
#define CK_LOGINOFF 5
#define CK_HEART 6

#define CK_NO_ERR    1
#define CK_ERR_HEAD -1
#define CK_ERR_CRC  -101
#define CK_ERR_HEAD_LEN  -102
#define CK_ERR_FULL_LEN  -103
#define CK_ERR_END  -104
#define CK_OVER_READ_LEN -105
#define CK_OVER_WRITE_LEN -106


#pragma warning( push,3 )
#include <map>
#include <vector>
#include <list>
#include <string>
#include <functional> // for greater<> and less<>
#include <algorithm> //for sort()
#pragma warning( pop)

// A fix to get rid of level 4 warnings comming from STL
#pragma warning(push)
//#include <yvals.h>
#pragma warning(disable: 4018)
#pragma warning(disable: 4146)
#pragma warning(disable: 4244)
#pragma warning(disable: 4663)
#pragma warning(disable: 4097)
#pragma warning(disable: 4786)
// #include <iostream>
// #include <ostream>
#include <string>
#pragma warning(pop)

#if defined(PUBOPT_MEMFIL) | defined(PUBOPT_NONSEG)
#	include <assert.h>
#endif

#define PUBAPI_INTERNAL_VERSION_MINOR 2

#define PUBAPI_INTERNAL_VERSION ((PUBAPI_INTERNAL_VERSION_MINOR&0xFFFF)|PUBAPI_INTERNAL_VERSION_MAJOR<<16)

//#define LEN_ID 80
//#define LEN_EID 24
//#define LEN_VAL 128

#define LOG_FILE_NAME "test"
#define BIN_LOG_FILE_NAME "test_bin.log"

#define SQL_REC_SIGN ";\n"

//<summary>The publicfunc Parser namespace.</summary>
namespace pub
{
#pragma pack(1)		
	
	// ����Ϣ����stru_pipe_pkg data��ͷ10���ֽ�
	struct stru_pipe_pkg_head
	{
		SHORT len;
		SHORT objid; //����ĸ��������Ϣ �̶�ֵ����������в����޸ģ������0˵���������ϱ�������
		SHORT taskno;//����Ǵη������Ϣ
		SHORT  func;   //������ 0: ��· 1:��ѯ 2:֪ͨ 3:��Ӧ
		CHAR  ishead; //�Ƿ���ͷ��
		CHAR  isend;  //�Ƿ���ĩ��
		
		stru_pipe_pkg_head()
		{
			len = 0;
			objid = 0;
			taskno = 0;
			ishead = 1;
			isend = 1;
			func = 2;
		}
	};
	
	struct stru_pipe_pkg
	{
		stru_pipe_pkg_head head;
		char buffer[60000];
		
		stru_pipe_pkg()
		{
			buffer[0]=0;
		}
	};
	
	enum TDataType
	{
		DT_NULL = 0X00,
			DT_String = 0X01,
			DT_Int = 0X02,
			DT_Float = 0X03,
			DT_Date = 0X04
	};
	
	//������������ 0:ʵʱ 1:�� 2:Сʱ 3:�� 4:�� 5:�� 6:���� 7:������
	
	enum TDateDefineType
	{
		DA_NULL = 0XFF,
			DA_REAL = 0X00,
			DA_MINU = 0X01,
			DA_HOUR = 0X02,
			DA_DAY =  0X03,
			DA_MONTH =0X04,
			DA_YEAR  =0X05,
			DA_CURVE =0X06,
			DA_CPDAY =0x07
	};
	
	struct _S_Err			/*��������*/
	{
		char Id;
		char Msg[256];
	};
	
	class TError
	{
	public:
		TError(const char *err = "",unsigned int err_no = 0)
		{
			if (err==NULL&&err_no==0)
			{
				strcpy(msg,"unknowed reason!please contact author.");
			}else  	strncpy(msg,err,256);
			no = err_no;
		}
		
		TError(unsigned int err_no,const char *fmt, ...)
		{
			no = err_no;
			va_list args;
			va_start(args, fmt);
			//vsnprintf(char *, size_t, const char *, va_list);
			vsnprintf(msg,sizeof(msg)-1, fmt, args); //_vsnprintf YYY
			msg[sizeof(msg)-1] = 0;
			va_end(args);
//			printf("\nERROR:%s\n",msg);
		}
		
		
		virtual ~TError(){};
		
		virtual const char* what() const
		{
			return msg;
		}
		
		
		virtual const int whatno() const
		{
			return no;
		}
		
		
		virtual const char* whatmsg() const
		{
			return msg;
		}
	protected:
		char msg[5120];
		int  no;
	};
	
	struct TValStr
	{
	private:
		double fVal;
	private:
		char * sVal;
		char m_buff[MAX_WORDAMT+2];
	public:
		TValStr()
		{
			sVal = m_buff;
			clear();
		}
		
		~TValStr()
		{
			if (sVal != m_buff)
			{
				if (sVal!=NULL)
				{
					printf("-[%x]%s\n",sVal,sVal);
				//	ACE_OS::free(sVal);
					delete [] sVal;
				}
			}

			sVal = NULL;
		}
		
		TValStr & operator=(TValStr & theVal)
		{
			//if (this == &theVal) return *this; ���ֵĸ��ʱȽϵͣ�û��ʲô��ϵ
			
			if (theVal.sVal == theVal.m_buff)
				strcpy(sVal,theVal.sVal);
			else
  			    setString(theVal.sVal);
			
					
			fVal = theVal.fVal;
			return *this; 
		}
		
		inline void clear()
		{
			fVal = 0;
			sVal[0] = 0;
			//	memset(sVal,0,sizeof(sVal));	
		}
		
		TValStr & operator=(double fValue)
		{
			fVal = fValue;
			sprintf(sVal,"%.4f",fValue);
			return *this;
		}
		
		TValStr & operator=(float fValue)
		{
			fVal = fValue;
			sprintf(sVal,"%.4f",fValue);
			return *this;
		}
		
		TValStr & operator=(int fValue)
		{
			fVal = fValue;
			sprintf(sVal,"%d",fValue);
			return *this;
		}
		
		TValStr & operator=(long fValue)
		{
			fVal = fValue;
			sprintf(sVal,"%d",fValue);
			return *this;
		}
		
		inline void setNull()
		{
			sVal[0]=0;
		}
		
		int isNull()
		{
			if ((sVal[0]==0) || (strcmp(sVal,"NULL")==0)) //strcmpi YYY
				return 1;
			else
				return 0;
		}
		
		inline void setString(char * sValue)
		{
			if (sValue == NULL)
			{
				sVal[0]=0;
				return ;
			}
			
			assert(sValue!=NULL);
			
			int nlen = strlen(sValue);
			
		//	printf("len=%d %s\n",nlen,sValue);
			
			if (nlen > sizeof(m_buff))
			{
				if (sVal != m_buff)
				{
					//ACE_OS::free(sVal);
					delete [] sVal;
					sVal = NULL;
					printf("~");
				}
				
				sVal = new char[nlen+1];//(char*)ACE_OS::malloc(nlen+1);
				if (sVal == NULL) 
				{ 
					throw TError("Not enough memory to allocate buffer\n"); 
					
				} 

				printf("+[%d][%x]\n",nlen+1,sVal);
				memset(sVal,0,nlen+1);
			}
			
			strcpy(sVal,sValue);
		}
		
		void operator=(const char * sValue)
		{
			setString((char *)sValue);
		}
		
		void operator=(char * sValue)
		{
			setString(sValue);
		}
	/*	
		operator char * ()
		{
			return sVal;
		}
	*/	

		char * c_str()
		{
		    return sVal;
		}

		operator double()
		{
			//		char *endptr;
			if (fVal != 0) return fVal;
			fVal = atof(sVal);
			//		fVal = strtod (sVal,&endptr);
			
			return fVal;
		}
		
		operator long()
		{
			//			char *endptr;
			//			long i=0;
			//			i = strtol(sVal,&endptr,i); //0x
			return strtol(sVal,NULL,(sVal[1]=='X'||sVal[1]=='x') ? 16 :10);
			//			return atol(sVal);
		}
		
		operator int()
		{
			return (long)*this;
		}
		
		operator float()
		{
			return (double)*this;
		}
		
	};
	
/* YYY	
	static void disableDosClose()
	{
		HWND hWnd;												// ���洰�ھ���ı���
		HMENU hMenu;											// ���洰�ڲ˵��ı���
		char strTitle[255];										// ���洰�ڱ���ı���
		GetConsoleTitle(strTitle, 255);							// ��ȡ���ڱ���
		hWnd=FindWindow("ConsoleWindowClass",strTitle);			// ��ȡ���ھ��
		hMenu=GetSystemMenu(hWnd,false);						// ��ȡ���ڲ˵�
		EnableMenuItem(hMenu,SC_CLOSE,MF_GRAYED|MF_BYCOMMAND);	// ��Ч���رհ�ť
	}
*/	
	/*ͷβ���ո�ȥ������*/
	static char * strTrim_lr(char *p)
	{
		char *tmp;
		char *tmp1;
		tmp1=p;
		tmp=tmp1+strlen(tmp1);
		
		if (p[0]==0) return p;
		
		while(*(--tmp)<=' '&&strlen(tmp1)>0)
		{
			*tmp='\0';
		}
		while((*tmp1<=' ')&&(strlen(tmp1)>0))
		{
			tmp1++;
		}
		strcpy(p,tmp1);
		return p;
	}
	
	static char * strUpper(char * spSour)
	{
		char * sStart=spSour;
		if (spSour==NULL) return NULL;
		while( *sStart)
		{
			*sStart = toupper(*sStart);
			sStart ++ ;
		}
		return spSour;
	}
	
	static char * strLower(char * spSour)
	{
		char * sStart=spSour;
		if (spSour==NULL) return NULL;
		while( *sStart)
		{
			*sStart = tolower(*sStart);
			sStart ++ ;
		}
		return spSour;
	}

	static void ByteRev(char *src, unsigned char *targ, int len)
	{
		int i,icnt;
		
		icnt = len /2;
		for (i=0;i<icnt;i++)
		{
			targ[len-i*2-2] = src[i*2];
			targ[len-i*2-1]=src[i*2+1];
		}
	}
	
	
	static char * desc_swap(unsigned char * buff,int len)
	{
		int i;
		char c;
		for(i=0;i<len / 2;i++)
		{
			c=buff[i];
			buff[i]=buff[len-1-i];
			buff[len-1-i] = c;
		}
		
		return (char * )buff;
	};
	
	void static string_replace( std::string &strBig, const std::string &strsrc, const std::string &strdst )
	{
		std::string::size_type pos = 0;
		std::string::size_type srclen = strsrc.size();
		std::string::size_type dstlen = strdst.size();
		
		while( (pos=strBig.find(strsrc, pos)) != std::string::npos )
		{
			strBig.replace( pos, srclen, strdst );
			pos += dstlen;
		}
	} 
	
	//���ɼ��ַ�ת��Ϊ���ɼ��ַ�
	static int AscToHex(char *asc, unsigned char *hex, int asclen)
	{
		int i;
		char base_char;
		int len = asclen;
		
		if(len%2) return -1;
		for(i=0; i<len; i++)
		{
			if( asc[i]>='0' && asc[i]<='9' ) base_char='0';
			else if(asc[i]>='A' && asc[i]<='F' ) base_char='A'-10;
			else if(asc[i]>='a' && asc[i]<='f' ) base_char='a'-10;
			else return -1;
			if( !(i%2) ) hex[i/2]=(unsigned char)(asc[i]-base_char)<<4; /*High*/
			else hex[i/2]|=(unsigned char)(asc[i]-base_char); /*Low*/
		}
		return 1;
	}
	
	//�����ɼ��ַ�ת��Ϊ�ɼ��ַ�
	static void HexToAsc(unsigned char *hex, char *asc, int hexlen)
	{
		int i;
		int len = hexlen;
		
		for(i=0; i<len; i++)
		{
			sprintf(asc+2*i, "%02X", *(hex+i));
		}
	}
	
	int static strsplit(char * srcstr,const char * s, int maxSize, char ** starg) 
	{ 
		char *pBegin,*pEnd; 
		char string2[24];
		
		strcpy(string2,s);
		
		pBegin =  srcstr;
		int i=0;
		
		starg[0]=pBegin;
		int isignlen=strlen(string2);
		
		while (*pBegin!=0)
		{
			i++;
			pEnd = strstr(pBegin,string2); 
			if (pEnd != NULL)
			{
				*pEnd=0;
				if (i>=maxSize) 
					break; //����������˸����ķ�Χ����ʱ��Ҫ�����������Ĳ����д�����
				
				pBegin = pEnd + isignlen;
				starg[i]=pBegin;
			}else break;
		}
		
		starg[i+1]= srcstr+strlen(srcstr);
		
		return i; 
	} 
	
	
	//sSrc-���滻�ַ���
	//sOld-�ɡ�Ƭ�Ρ�
	//sNew-�¡�Ƭ�Ρ�
	//char*-�������ַ������ǵ��������free()�ͷ�
	static	char* replaceString(const char * sDestMem,const char*sSrc,
		const char*sOld,
		const char*sNew)
	{
		//����sOld��sNew���Ȳ�
		size_t nOldLen=strlen(sOld);
		size_t nNewLen=strlen(sNew);
		signed nDiff=(signed)(nNewLen)-(signed)(nOldLen);
		
		//����ͳ��pOldStr���ж��ٸ�sOld
		int nMatch=0;
		for(char*p=(char*)sSrc;;)
		{
			p=strstr(p,sOld);//����ƥ��Ƭ��Match
			if(p!=NULL)//����ҵ�һ��Match
			{
				nMatch++;//����
				p+=nOldLen;//����ָ��ָ�����Match֮��
			}
			else//�����Ҳ�Ҳ���
			{
				break;//��������
			}
		}
		
		//����ռ�
		char* pDestMem=(char *)sDestMem;
		if(pDestMem==NULL)
		{
			return NULL;
		}
		
		//�ֶο���
		char* pDest=(char *)pDestMem;
		char* pSrc=(char*)sSrc;
		for(;;)
		{
			char* pMatch=strstr(pSrc,sOld);//����ƥ��Ƭ��Match
			if(pMatch!=NULL)
			{
				size_t nInterLen=pMatch-pSrc;//����Match��Ĳ���Inter�ĳ���
				if(nInterLen>0)
				{
					memcpy(pDest,pSrc,nInterLen);//����Inter
					pDest+=nInterLen;//������ָ��ָ��Inter֮��
				}
				memcpy(pDest,sNew,nNewLen);//����sNew��Ϊ���Ч�ʣ��˴�����strcat()
				pDest+=nNewLen;//������ָ��ָ��sNew֮��
				pSrc=pMatch+nOldLen;//����Դָ��ָ��sOld֮��
			}
			else
			{
				break;
			}
		}
		strcpy(pDest,pSrc);//�������һ��Match���һ�Σ�����ֻ��һ��null-terminator��Ҳ����������sSrc
		
		//���ؽ��
		return (char *)sDestMem;
	}
	
	//sSrc-���滻�ַ���
	//sOld-�ɡ�Ƭ�Ρ�
	//sNew-�¡�Ƭ�Ρ�
	//char*-�������ַ������ǵ��������free()�ͷ�
	static char* replaceString(const char*sSrc,
		const char*sOld,
		const char*sNew)
	{
		char  sDestMem[10240];
		
		replaceString(sDestMem,sSrc,sOld,sNew);
		
		strcpy((char*)sSrc,sDestMem);
		
		return (char*)sSrc;
	}
	
	
	/*����У����*/
	static unsigned char getCrc8(unsigned char *msg, int len)
	{
		int i;
		unsigned char chk_byte=0;
		for(i=0; i<len; i++)
		{
			chk_byte+=*(msg+i);
		}
		return chk_byte;
	}
	
	static unsigned short CRC_Generate(unsigned short wSrcKey, const char* pdata, int len)
	{ 
		unsigned short ret=0;
		int j,k;
		unsigned char crc16Lo = 0x00;
		unsigned char crc16Hi = 0x00;
		unsigned char Cl = wSrcKey&0x00ff;
		unsigned char Ch = wSrcKey>>8;
		for (j=0; j<len; j++) 
		{ 
			crc16Lo ^= pdata[j];
			for (k=0; k<8; k++) 
			{
				unsigned char save16Hi = crc16Hi; 	
				unsigned char save16Lo = crc16Lo;
				crc16Lo = crc16Lo>>1; 
				crc16Hi = crc16Hi>>1; 
				if (save16Hi & 0x01) 
					crc16Lo = crc16Lo | 0x80;
				if (save16Lo & 0x01) 
				{ 
					crc16Hi ^= Ch;
					crc16Lo ^= Cl;
				}
			}
		}
		ret=crc16Hi;
		ret = ret<<8;
		ret+=crc16Lo;
		return ret;
	} 
	
	static int User_CheckPW(char iRule,char * sPwd, char *pdata,char *password)
	{
		int bRight=1;
		
		switch (iRule)
		{
		default:
		case 0:// ��  
			//		if (memcmp(password,Para.Unit.Password,2))
			//			bRight=FALSE;
			break;
			
		case 1://CRC
			{
				unsigned short CRC;
				CRC=CRC_Generate(*(unsigned short *)sPwd,pdata,12);
				if (memcmp(password,&CRC,2))
					bRight=0;
				break;
			}
		}
		if (!bRight)
		{
			return -1;
		}
		return 0;
	}
	
	class CDateTime
	{
	public:
		
		static int GetWeekDay(int year,int month,int day)
		{
			if(month<=2)
			{
				year-=1;
				month+=12;
			}
			int week=(day+((8+(13*month))/5)+
				(year+(year/4)-(year/100)+(year/400)))%7;
			return week;
		}
		
		/*************************** 
		Function Name: IsLeapYear
		Function desc: 
		�ж����������Ƿ�����(�����������жϸö�������)
		Parameters:
		
		  Input:
		  ������				����			����˵��
		  year				long			Ҫ�жϵ����(���Բ���)
		  Output:
		  ��
		  Return Value:
		  �������귵��1�����򷵻�0
		****************************/
		static int IsLeapYear(long year)
		{
			if(year%100 == 0)
			{
				if(year%400 == 0)
					return 1;
				else
					return 0;
			}
			else if(year%4 == 0)
			{
				return 1;
			}
			else
				return 0;
		}
		
		static void  localtime_tm( time_t time, long timezone, struct tm *tm_time)
		{
			char Days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
			long n32_hpery;
			long ndays;
			long isLeap;
			
			//����ʱ�� 
			time=time-timezone;
			if(time < 0) {
				time = 0;
			}
			
			ndays = time;
			
			//ȡ��ʱ�� 
			tm_time->tm_sec=(int)(ndays % 60);
			ndays = ndays/60;
			
			//ȡ����ʱ�� 
			tm_time->tm_min=(int)(ndays % 60);
			ndays = ndays/60;
			
			//Сʱ�� 
			tm_time->tm_hour=(int)(ndays % 24);
			ndays = ndays/24+1; //��1�ſ�ʼ
			
			//���� 
			
			tm_time->tm_year = 70;
			for (;;) 
			{	
				n32_hpery = 365 ;
				if ((isLeap=IsLeapYear(tm_time->tm_year+1900))==1)
				{
					n32_hpery = 366 ;
				}
				
				if (ndays < n32_hpery) 
				{
					break;
				}
				
				ndays -= n32_hpery;
				
				tm_time->tm_year ++ ;
			}
			
			//ndays Ϊ ���һ������� ������鲢����
			//�������� 
			
			if (isLeap ) Days[1] = 29;
			
			for (tm_time->tm_mon = 0;	Days[tm_time->tm_mon] < ndays;	tm_time->tm_mon++) 
			{
				ndays -= Days[tm_time->tm_mon];
			}
			
			tm_time->tm_mday = (int)(ndays);
			return;
		}
		
		//����ָ����ʽ���������ͽ���ת��
		// YYYYMMDDHHMISS
		static struct tm setDateTime(const char * sValue,const char * sFmt)
		{
			struct tm tm_time;
			char strFmt[32];
			char sTmp[8];
			char * pStr ;
			
			tm_time.tm_year = 100;
			tm_time.tm_mon = 0;
			tm_time.tm_mday = 1;
			tm_time.tm_hour = 0;
			tm_time.tm_min = 0;
			tm_time.tm_sec = 0;
			
			//	printf("setDateTime %s %s %d %d \n",sValue,sFmt,strlen(sValue),strlen(sFmt));
			assert(strlen(sValue) == strlen(sFmt));
			
			strcpy(strFmt,sFmt);
			pub::strUpper(strFmt);
			
			pStr = strstr(strFmt,"YYYY");//Ѱ�������ڵ�λ��
			if (pStr != NULL)
			{
				memset(sTmp,0,sizeof(sTmp));
				strncpy(sTmp,sValue+(pStr-strFmt),4);
				tm_time.tm_year=atoi(sTmp)-1900;
			}else
			{
				pStr = strstr(strFmt,"YY");//Ѱ�������ڵ�λ��
				
				memset(sTmp,0,sizeof(sTmp));
				
				if (pStr != NULL)
				{
					strncpy(sTmp,sValue+(pStr-strFmt),2);
					tm_time.tm_year=atoi(sTmp)+2000-1900;
				}else
				{
					GetCurYear(sTmp);
					tm_time.tm_year = atoi(sTmp)-1900;
				}
			}
			
			pStr = strstr(strFmt,"MM");//��
			if (pStr != NULL)
			{
				memset(sTmp,0,sizeof(sTmp));
				strncpy(sTmp,sValue+(pStr-strFmt),2);
				tm_time.tm_mon=atoi(sTmp)-1;
			}
			
			pStr = strstr(strFmt,"DD");//��
			if (pStr != NULL)
			{
				memset(sTmp,0,sizeof(sTmp));
				strncpy(sTmp,sValue+(pStr-strFmt),2);
				tm_time.tm_mday=atoi(sTmp);
			}
			
			
			pStr = strstr(strFmt,"HH");//ʱ
			if (pStr != NULL)
			{
				memset(sTmp,0,sizeof(sTmp));
				strncpy(sTmp,sValue+(pStr-strFmt),2);
				tm_time.tm_hour=atoi(sTmp);
			}
			
			
			pStr = strstr(strFmt,"MI");//��
			if (pStr != NULL)
			{
				memset(sTmp,0,sizeof(sTmp));
				strncpy(sTmp,sValue+(pStr-strFmt),2);
				tm_time.tm_min=atoi(sTmp);
			}
			
			
			pStr = strstr(strFmt,"SS");//��
			if (pStr != NULL)
			{
				memset(sTmp,0,sizeof(sTmp));
				strncpy(sTmp,sValue+(pStr-strFmt),2);
				tm_time.tm_sec=atoi(sTmp);
			}
			
			
			tm_time.tm_wday = GetWeekDay(tm_time.tm_year+1900,tm_time.tm_mon+1,tm_time.tm_mday);
			
			return tm_time;
			//time_t t = mktime(&tm_time);
		}
		
		//��ָ����ʽ������ת����Ϊ �����ʽ������
		static char * getDataTime(const char * inData,const char * inDateFmt,const char * wantDateFmt,char *outDate)
		{
			outDate[0]=0;
			
			struct tm  vTm = setDateTime(inData,inDateFmt);
			
			setDateTime(vTm,wantDateFmt,outDate);
			
			return outDate;
		}
		
		//sDataFmt YYYYMMDDHHMISS
		static void setDateTime(struct tm &value,const char * sDataFmt,char * sTarg)
		{
			std::string strDate = sDataFmt;
			char sTmp[8];
			
			//			std::replace(strDate.find("YY"),1,"DD");
			//str.replace(find("   "),1,"   abc   ");   
			
			char * pStr = strstr((char *)sDataFmt,"YYYY");//Ѱ�������ڵ�λ��
			if (pStr != NULL)
			{
				sprintf(sTmp,"%04d",value.tm_year+1900); //�Դ�2000�꿪ʼ
				string_replace(strDate,"YYYY",sTmp);
			}else
			{
				sprintf(sTmp,"%02d",value.tm_year+1900-2000); //�Դ�2000�꿪ʼ
				string_replace(strDate,"YY",sTmp);
			}
			
			sprintf(sTmp,"%02d",value.tm_mon+1); //��1��ʼ
			string_replace(strDate,"MM",sTmp);
			
			sprintf(sTmp,"%02d",value.tm_mday);
			string_replace(strDate,"DD",sTmp);
			
			sprintf(sTmp,"%02d",value.tm_hour);
			string_replace(strDate,"HH",sTmp);
			
			sprintf(sTmp,"%02d",value.tm_min);
			string_replace(strDate,"MI",sTmp);
			
			sprintf(sTmp,"%02d",value.tm_sec);
			string_replace(strDate,"SS",sTmp);
			
			strcpy(sTarg,strDate.c_str());
			
			
			
		}
		
		/*�õ���ǰʱ��*/
		static void GetCurTime(char* sTime)
		{
			time_t t;
			struct tm* tm_buf;
			t = time(0);
			//	t = t - 8*60*60 ; //ʱ����8��Сʱ�����
			tm_buf = localtime(&t);
			sprintf(sTime,"%02d:%02d:%02d",tm_buf->tm_hour,tm_buf->tm_min,tm_buf->tm_sec);
		}
		
		static void GetFmtDateTime(time_t t,char * sTime)
		{
			struct tm* tm_buf;
			tm_buf = localtime(&t);
			sprintf(sTime,"%04d-%02d-%02d %02d:%02d:%02d",
				tm_buf->tm_year+ 1900,tm_buf->tm_mon+1,tm_buf->tm_mday,
				tm_buf->tm_hour,tm_buf->tm_min,tm_buf->tm_sec);
		}
		
		static void GetFmt2DateTime(int ival,char * sTime)
		{
			int ihour;
			int iminu;
			int isec;
			int iday;
			
			isec = ival % 60;
			iminu = (ival / 60) % 60;
			ihour = ival / 3600;
			iday = ihour / 24;
			ihour = ihour % 24;
			
			if (iday <= 0)
			{
				sprintf(sTime,"%4d:%02d:%02d",
					ihour,
					iminu,
					isec);
			}
			else
			{
				sprintf(sTime,"%4d-%02d:%02d",
					iday,
					ihour,
					iminu);
				
			}
			
		}
		
		static void GetFmt1DateTime(time_t t,char * sTime)
		{
			struct tm* tm_buf;
			tm_buf = localtime(&t);
			sprintf(sTime,"%02d-%02d:%02d",
				//tm_buf->tm_year+ 1900,
				//tm_buf->tm_mon+1,
				tm_buf->tm_mday,
				tm_buf->tm_hour,tm_buf->tm_min);
		}
		
		static char * AddHour(char * sTime,char * sFmt,int hour)
		{
			struct tm _tm = setDateTime(sTime,sFmt);
			time_t t = mktime(&_tm);
			
			t = t + hour*3600;
			
			localtime_tm(t,-8*3600,&_tm);
			
			setDateTime(_tm,sFmt,sTime);
			//mktime
			
			return sTime;
		}
		
		/*�õ���ǰ���·�*/
		static char * GetCurMon(char* sDate)
		{
			time_t t;
			struct tm* tm_buf;
			t = time(0);
			tm_buf = localtime(&t);
			sprintf(sDate, "%04d%02d",tm_buf->tm_year + 1900,tm_buf->tm_mon + 1);
			return sDate;
		}
		
		
		
		
		/*�õ���ǰ���·�*/
		static char * GetCurYear(char* sDate)
		{
			time_t t;
			struct tm* tm_buf;
			t = time(0);
			tm_buf = localtime(&t);
			sprintf(sDate, "%04d",tm_buf->tm_year + 1900);
			return sDate;
		}
		
		/*�õ��ϸ����·�*/
		static char * GetLastMon(char* sDate)
		{
			time_t t;
			struct tm* tm_buf;
			t = time(0);
			tm_buf = localtime(&t);
			sprintf(sDate, "%04d%02d",tm_buf->tm_year + 1900,tm_buf->tm_mon);
			return sDate;
		}
		
		
		
		/*�õ���ǰ����*/
		static char * GetCurDate(char* sDate)
		{
			time_t t;
			struct tm* tm_buf;
			t = time(0);
			//	t = t - 8*60*60 ; //ʱ����8��Сʱ�����
			tm_buf = localtime(&t);
			sprintf(sDate, "%04d%02d%02d",tm_buf->tm_year + 1900,tm_buf->tm_mon + 1,tm_buf->tm_mday);
			return sDate;
		}
		
		/*�õ����ں�ʱ��*/
		static void GetCurDayTime(char* sTime)
		{
			time_t t;
			struct tm* tm_buf;
			t = time(0);
			tm_buf = localtime(&t);
			sprintf(sTime,"%02d-%02d:%02d:%02d",tm_buf->tm_mday,tm_buf->tm_hour,tm_buf->tm_min,tm_buf->tm_sec);
		}
		
		/*�õ���ǰ������ʱ��*/
		
		static char * GetCurDateHour(char * sDT)
		{
			time_t t;
			
			struct tm* tm_buf;
			t = time(0);
			
			//	t = t - 8*60*60 ; //ʱ����8��Сʱ�����
			
			tm_buf = localtime(&t);
			
			sprintf(sDT, "%04d%02d%02d%02d",
				tm_buf->tm_year + 1900,tm_buf->tm_mon + 1,tm_buf->tm_mday,tm_buf->tm_hour);
			return sDT;
		}
		
		/*�õ���ǰ������ʱ��*/
		
		static char * GetCurDateTime(char * sDT)
		{
			time_t t;
			
			struct tm* tm_buf;
			t = time(0);
			
			//	t = t - 8*60*60 ; //ʱ����8��Сʱ�����
			
			tm_buf = localtime(&t);
			
			sprintf(sDT, "%04d%02d%02d%02d%02d%02d",
				tm_buf->tm_year + 1900,tm_buf->tm_mon + 1,tm_buf->tm_mday,tm_buf->tm_hour,tm_buf->tm_min,tm_buf->tm_sec);
			return sDT;
		}
	};//class CDatetime
	
	class CLog
	{
	public:
		static char m_work_dir[256];
	public:
		
		/*��ù���·��*/
		static char *  GetWorkDir(char * sdir=NULL)
		{
			char sDirectory[256];
			int i;
			
			memset(sDirectory,0,256);
			//YYY GetModuleFileName(NULL,sDirectory,255);
			for(i=strlen(sDirectory)-1; i>=0; i--)
			{
				if (sDirectory[i]=='\\')
				{
					sDirectory[i]='\0';
					break;
				}
			}
			
			strcpy(m_work_dir,sDirectory);
			
			if (sdir!=NULL)
			{
				strcpy(sdir,sDirectory);
			}
			return m_work_dir;
			
		}
		
		/*д��־�ļ�*/
		static bool WriteFileLog(char * sFileName,char *fmt, ...)
		{
			FILE *fLog;
			char log_filename[256];
			char sDir[256]="";
			va_list varlist;
			
			sprintf(log_filename, "%s\\%s",  GetWorkDir(sDir), sFileName);
			
			if( (fLog=fopen(log_filename,"a+"))==NULL ) return false;
			va_start(varlist, fmt);
			if( vfprintf(fLog, fmt, varlist)<=0 )
			{
				va_end(varlist);
				fclose(fLog);
				return false;
			}
			va_end(varlist);
			fclose(fLog);
			
			return true;
		}
		
		/*д��־�ļ�*/
		static bool WriteLog(int ilevel,const char *fmt, ...)
		{
			FILE *fLog;
			time_t time_now;
			struct tm *tm_now;
			struct stat stat_buf;
			char log_filename[256], bak_log_filename[256];
			va_list varlist;
			char sCurDate[32];
			char sInfo[20480];
			
			pub::CDateTime::GetCurDate(sCurDate);
			
			sprintf(log_filename, "%s\\log\\%s.%d.%s.log", GetWorkDir(), LOG_FILE_NAME,LOG_FILE_SEQ,sCurDate);
			sprintf(bak_log_filename, "%s.bak", log_filename);
			
			if(stat(log_filename, &stat_buf)!=-1)
			{
				if ( stat_buf.st_size>(5000*1024) )
				{
					remove(bak_log_filename);
					rename(log_filename, bak_log_filename);
				}
			}
			
			if( (fLog=fopen(log_filename,"a+"))==NULL ) return false;
			if( fprintf(fLog,"\n************************************\n")<=0 )
			{
				fclose(fLog);
				return false;
			}
			time(&time_now);
			tm_now=(struct tm *) localtime(&time_now);
			
			sprintf(sInfo,"Time: %4d-%02d-%02d  %02d:%02d:%02d\n",
				tm_now->tm_year+1900,tm_now->tm_mon+1,tm_now->tm_mday,
				tm_now->tm_hour,tm_now->tm_min,tm_now->tm_sec);
			
			printf(sInfo);
			
			if( fprintf(fLog,sInfo)<=0 )
			{
				fclose(fLog);
				return false;
			}
			va_start(varlist, fmt);
			
			vsprintf(sInfo,fmt, varlist);
			printf(sInfo);
			
			if( vfprintf(fLog, fmt, varlist)<=0 )
			{
				va_end(varlist);
				fclose(fLog);
				return false;
			}
			va_end(varlist);
			fclose(fLog);
			
			return true;
		}
		
		/*д��־�ļ�*/
		static bool WriteStringLog(char *fmt, ...)
		{
			FILE *fLog;
			
			
			struct stat stat_buf;
			char log_filename[256], bak_log_filename[256];
			va_list varlist;
			
			sprintf(log_filename, "%s\\%s", GetWorkDir(), LOG_FILE_NAME);
			sprintf(bak_log_filename, "%s\\%s.bak", m_work_dir, LOG_FILE_NAME);
			
			if(stat(log_filename, &stat_buf)!=-1)
			{
				if ( stat_buf.st_size>(5000*1024) )
				{
					remove(bak_log_filename);
					rename(log_filename, bak_log_filename);
				}
			}
			
			if( (fLog=fopen(log_filename,"a+"))==NULL ) return false;
			
			va_start(varlist, fmt);
			if( vfprintf(fLog, fmt, varlist)<=0 )
			{
				va_end(varlist);
				fclose(fLog);
				return false;
			}
			va_end(varlist);
			fclose(fLog);
			
			return true;
		}
		
		
		/*д��־�ļ�*/
		static bool WriteBinLog(int bufflen,unsigned char * pBuff )
		{
			FILE *fLog;
			struct stat stat_buf;
			char log_filename[256], bak_log_filename[256];
			
			
			sprintf(log_filename, "%s\\%s", GetWorkDir(), BIN_LOG_FILE_NAME);
			sprintf(bak_log_filename, "%s\\%s.bak", m_work_dir, BIN_LOG_FILE_NAME);
			
			if(stat(log_filename, &stat_buf)!=-1)
			{
				if ( stat_buf.st_size>(500*1024) )
				{
					remove(bak_log_filename);
					rename(log_filename, bak_log_filename);
				}
			}
			
			if( (fLog=fopen(log_filename,"ab"))==NULL ) return false;
			fseek(fLog, 0, SEEK_SET); 
			fwrite(pBuff,1,bufflen,fLog);
			fclose(fLog);
			
			return true;
		}
		
	};
	
	struct TLevel
	{
		unsigned long  index;         //�ǵڼ���������ݶ���
		unsigned long  recNo;	      //����ĵڼ�����¼�ı��
		char sID[LEN_ID]; //��ǰ��ı�� Ϊxml�����ļ��е�˳��λ��
		
		void Clear()
		{
			index = -1;
			recNo = -1;
			sID[0]=0;
		}
	};
	
	typedef std::vector<TLevel * > TLevelArray;
	typedef std::vector<TLevel * > ::iterator TLevelArrayIter;
	
	struct TLevelList
	{
	private:
		TLevelArray _arr;
		
		void Free()
		{
			int i;
			TLevel * pVal;
			for (i=0;i<_arr.size();i++)
			{
				pVal = _arr[i];
				delete pVal;
			}
			_arr.clear();
		}
	public:
		int Add(int index ,TLevel * pVal)
		{
			TLevel * pNew;
			int i;
			if (index >= _arr.size())
			{
				for (i=_arr.size();i<=index;i++) 
				{
					pNew = new TLevel;  //���������ֹһֱ���ظ����������
					_arr.push_back(pNew);
				}
			}
			pNew = _arr[index];
			*pNew = * pVal;
			
			return _arr.size();
		}
		
		void Clear()
		{
			int i;
			TLevel * pVal;
			for (i=0;i<_arr.size();i++)
			{
				pVal = _arr[i];
				pVal->Clear();
			}
		}
		
		~TLevelList()
		{
			Free();
		}
		
	public:
		TLevel * getItem(int idx)
		{
			return _arr[idx];
		}
		
		void Clone(TLevelList * pVal)
		{
			Clear();
			int i,cnt;
			cnt = pVal->_arr.size();
			for (i=0;i<cnt;i++)
			{
				Add(i,pVal->_arr[i]);
			}
		}
		
	};
	
	struct TMaskDepend
	{
		char sID[LEN_EID]; //��ǰ��ı�� Ϊxml�����ļ��е�˳��λ��
		unsigned long val ;//����ֵ
		
		inline void clear()
		{
			val = 0;
			sID[0]=0;
		}
		
		TMaskDepend()
		{
			clear();
		}
	};
	
	struct TMaskComp
	{
		char sID[LEN_EID]; //��ǰ��ı�� Ϊxml�����ļ��е�˳��λ��
		unsigned long val ;//����ֵ
		char sHint[LEN_EID];
		
		inline void clear()
		{
			val = 0;
			sID[0]=0;
			sHint[0]=0;
		}
		
		TMaskComp()
		{
			clear();
		}
	};
	
	
	typedef std::vector<TMaskDepend * > TMaskDepArray;
	typedef std::vector<TMaskComp * > TMaskCompArray;
	typedef std::vector<TMaskDepend * > ::iterator TMaskDepArrayIter;
	
	struct TMask
	{
	public:
		unsigned long val; //�����maskֵ�������������ķ���ʹ��
	private:
		TMaskDepArray _arr;
		
		inline void free()
		{
			clear();
			_arr.clear();
		}
	public:
		int getDepCount()
		{
			return _arr.size();
		}
		
		static TMaskComp * find(TMaskCompArray &depArr,char * sId)
		{
			int k,kcnt;
			TMaskComp * pcompDep;
			kcnt = depArr.size();
			
			for (k=0;k<kcnt;k++)
			{
				pcompDep = depArr[k];
				
				if (strcmp(pcompDep->sID,sId) == 0) //strcmpi YYY
					return pcompDep;
			}	
			return NULL;
		}
		
		static void freeAndNil(TMaskCompArray &depArr)
		{
			int i;
			TMaskComp * pVal;
			for (i=0;i<depArr.size();i++)
			{
				pVal = depArr[i];
				delete pVal;
			}
			depArr.clear();
		}
		
		//�ж��Ƿ�����Ҫ��
		inline int isMatch(unsigned long maskVal)
		{
			return (val & maskVal) == val ? 1 : 0;
		}
		
		static  void showMasks(TMaskCompArray &depArrMask)
		{
			char sInfo[256]="";
			int k,kcnt;
			TMaskComp * pcompDep;
			kcnt = depArrMask.size();
			
			for (k=0;k<kcnt;k++)
			{
				pcompDep = depArrMask[k];
				sprintf(sInfo+strlen(sInfo),"%d:%s[%02X]:%s_",k,pcompDep->sID,pcompDep->val,pcompDep->sHint);
			}	
			sInfo[strlen(sInfo)-1]=0;
			printf("%s\n",sInfo);
		}
		
		//TMaskDepArray &depArr �ڲ���ŵ����������������maskֵ
		inline int isMatch(TMaskCompArray &depArrMask)
		{
			//�����ڲ������ݿ����Ƿ�ƥ��,ֻҪ��һ����ƥ��,��˵����ƥ��
			int i,icnt,k,kcnt;
			icnt = _arr.size();
			TMaskDepend * pDep;
			TMaskComp * pcompDep;
			for (i=0;i<icnt;i++)
			{
				pDep = _arr[i];
				kcnt = depArrMask.size();
				for (k=0;k<kcnt;k++)
				{
					pcompDep = depArrMask[k];
					if (strcmp(pcompDep->sID,pDep->sID) == 0) //strcmpi YYY
					{
						if ((pDep->val & pcompDep->val)!=pcompDep->val)
							return 0;
					}
					
				}	
			}
			
			return 1;
		}
		
		int add(int index ,TMaskDepend & aVal)
		{
			TMaskDepend * pNew;
			int i,amt;
			amt = _arr.size();
			if (index < 0) index = amt;
			
			if (index >= amt)
			{
				for (i=amt;i<=index;i++) 
				{
					pNew = new TMaskDepend;  //���������ֹһֱ���ظ����������
					_arr.push_back(pNew);
				}
			}
			pNew = _arr[index];
			*pNew = aVal;
			
			return _arr.size();
		}
		
		inline void clear()
		{
			int i;
			TMaskDepend * pVal;
			for (i=0;i<_arr.size();i++)
			{
				pVal = _arr[i];
				//				pVal->clear();
				delete pVal;
			}
			_arr.clear();
		}
		
		TMask()
		{
			val = 0;
			clear();
		}
		
		~TMask()
		{
			free();
		}
		
		inline TMaskDepend * getItem(int idx)
		{
			if (idx >= _arr.size()) return NULL;
			return _arr[idx];
		}		
	public:
		
		
		int getItem(int idx,char * sDepId,unsigned long &depMask)
		{
			TMaskDepend * pDep = getItem(idx);
			
			if (pDep != NULL)
			{
				strcpy(sDepId,pDep->sID);
				depMask = pDep->val;
				return 1;
			}else
				return -1;
			
		}
		
		void copyFrom(TMask * pVal)
		{
			clear();
			int i,cnt;
			cnt = pVal->_arr.size();
			for (i=0;i<cnt;i++)
			{
				add(i,*(pVal->_arr[i]));
			}
			val = pVal->val;
		}
		
		
		
		//MASK="0X01;ID2:0X03"
		void setString(char * sVal)
		{
			TMaskDepend aDep;
			char sTmpVal[256],*pStr;
			memcpy(sTmpVal,sVal,sizeof(sTmpVal));
			sTmpVal[250] = 0;
			char * tmparr[10];
			
			int i,icnt;
			
			icnt = pub::strsplit(sTmpVal,";",8,tmparr);
			
			for(i=0;i<icnt;i++)
			{
				//�ж��Ƿ����:��
				pStr=strchr(tmparr[i],':');
				if (pStr == NULL) //not found
				{
					val = strtol(tmparr[i],NULL,0);
				}else //found
				{
					*pStr = 0;
					memcpy(aDep.sID,tmparr[i],sizeof(aDep.sID));
					aDep.sID[sizeof(aDep.sID)-1] = 0;
					pStr ++ ;
					
					aDep.val = strtol(pStr,NULL,0); // 0��ʾ�Զ�����ʶ��
					
					add(-1,aDep);
					
				}
			}
		}
	};
	
	struct TAttr
	{
		char id[LEN_ID]; //������Э���� such as : afn 0.04.1.2:3 ��ʾ 04 ������,fn1 ����,��3����¼�ĵ�2����¼
		TValStr val;     //�ڲ�������
		TMask   mask;    //ӳ�����
	private:
		int  m_idLevel;
	public:
		char eId[LEN_EID]; //���ڱ��浽���ݱ��е����ݱ��,��Ӧ��xml�ļ��е�Ӣ������
		char combinid[LEN_ID];//����Э���ź�˵����ŵ�id ��ʽ [1.10.3.0,mainip] 
		char dateFmt[24];//���ڸ�ʽ YYYYMMDDHHMISS
		//		TLevel LevelID[10]; //id Э��ID,����Э���afn,fn,�͸�ʽ����˳�����ɱ��
		TLevelList levList;
		/************************************************************************/
		/* ���ｫ���ݶ�Ӧ�ı�Ű���.:������з��룬�Ի��ÿ��������������Сֵ
		/************************************************************************/
		
		int getLevelCount()
		{
			TLevel LevelID;
			if (m_idLevel == 0)
			{
				int i,len;
				char sTmpId[128];
				char * tmparr[32];
				//memset((char*)LevelID,0,sizeof(LevelID));
				levList.Clear();
				
				strcpy(sTmpId,id);
				
				m_idLevel = strsplit(sTmpId,".",32,tmparr);
				
				len = strlen(sTmpId);
				//�����еĵ� ���� Ϊ0 ����tmparr��λ
				for (i=0;i<len;i++)
				{
					if (sTmpId[i]=='.')
						sTmpId[i]=0;
				}
				
				char * pStr;
				
				//���ν����ݽ���Ϊ id + subpos�ķ�ʽ
				for (i=0;i<m_idLevel;i++)
				{
					LevelID.index = i;
					LevelID.recNo = 0;
					
					pStr = strchr(tmparr[i],':');
					if (pStr != NULL)
					{
						LevelID.recNo = atoi(pStr+1);
						
						*pStr=0;
					}
					strcpy(LevelID.sID,tmparr[i]);//���ﱣ����Э����ʵ�ʵ�λ��
					levList.Add(i,&LevelID);
				}
			}
			return m_idLevel;
		}
		
		TLevel * getLevelID(int idx)
		{
			return (levList.getItem(idx));
		}
		
		// 		
		// 		int setId(char * sMapid,char * sVal)
		// 		{
		// 			clear();
		// 			strcpy(id,sMapid);
		// 			strcpy(val.sVal,sVal);
		// 			
		// 			return getLevelCount();
		// 			
		// 		}
		
		char * getDataId(char * sBuff,int & nRetNo,int level=-1)
		{
			TLevel LevelID;
			sBuff[0] = 0;
			int iNo=0;
			if (m_idLevel == 0) getLevelCount();
			
			int tmpCnt;
			char sTmp[64];
			
			if ((level <= 0)||(level > m_idLevel  )) tmpCnt = m_idLevel;
			else  tmpCnt = level  ;
			
			for (int i=0;i<tmpCnt;i++)
			{
				LevelID = *(this->levList.getItem(i));
				iNo = 0;
				if (i == 0)
					sprintf(sTmp,"%s",LevelID.sID);
				else
					sprintf(sTmp,".%s",LevelID.sID);
				
				if (LevelID.recNo > 0) 
				{
					iNo = LevelID.recNo;
					//sprintf(sTmp+strlen(sTmp),":%d",iNo); //��ӵڼ�����¼		
				}
				strcat(sBuff,sTmp);
			}
			
			nRetNo = iNo;
			return sBuff;
		}
		
		//��õ�ǰԪ�ص�Э����(����Э���afn,˳��Ŷ�������ı��)
		char * getMapId(char * sBuff,int level = -1)
		{
			TLevel LevelID;
			sBuff[0] = 0;
			int iNo=0;
			if (m_idLevel == 0) getLevelCount();
			
			int tmpCnt;
			char sTmp[64];
			
			if ((level <= 0)||(level > m_idLevel  )) tmpCnt = m_idLevel;
			else  tmpCnt = level  ;
			
			for (int i=0;i<tmpCnt;i++)
			{
				LevelID = *(this->levList.getItem(i));
				iNo = 0;
				if (i == 0)
					sprintf(sTmp,"%s",LevelID.sID);
				else
					sprintf(sTmp,".%s",LevelID.sID);
				
				iNo = LevelID.recNo;
				if (iNo > 0)
				{
					sprintf(sTmp+strlen(sTmp),":%d",iNo); //��ӵڼ�����¼	
				}else if (iNo < 0)
					sprintf(sTmp+strlen(sTmp),":*"); //��ӵڼ�����¼
				
				strcat(sBuff,sTmp);
			}
			
			return sBuff;
		}
		
		// > 0 targ > sour
		// �Ƚ����������Ƿ�һ��
		int compareId(TAttr & targ)
		{
			TLevel LevelID,targLvId;
			int i,iRet;
			
			// ����ͬһ����ν��бȽ�
			int sourcnt = getLevelCount();
			int targcnt = targ.getLevelCount();
			
			
			int n = targcnt > sourcnt ? targcnt : sourcnt;
			
			
			//���ȱȽϵڼ�����¼�����������ͬ�ļ�¼���
			for (i=0;i<n;i++)
			{
				targLvId = *(targ.levList.getItem(i));
				LevelID = *(this->levList.getItem(i));
				iRet = targLvId.recNo - LevelID.recNo;//�Ӽ�¼��� �ж��Ƿ�����һ����¼
				
				if (iRet==0)
				{  //��¼��ͬ�����жϼ�¼�������ֶ�λ���Ƿ���ͬ
					iRet = strcmp(targLvId.sID,LevelID.sID);
				//ʵ�ʵı����ipaddr,port //strcmpi YYY
					if (iRet == 0)
						continue;
					else
						return iRet;
				}
				
				return iRet;
			}
			
			return 0;
		}
		
		TAttr & operator=(TAttr & theVal)
		{
			//if (this == &theVal) return *this; ���ֵĸ��ʱȽϵͣ�û��ʲô��ϵ
			strcpy(id,theVal.id);
			m_idLevel = theVal.m_idLevel;
			val = theVal.val;
			//			memcpy(LevelID,theVal.LevelID,sizeof(LevelID));
			levList.Clone(&(theVal.levList));
			return *this; 
		}
		
		TAttr()
		{
			id[0]=0;
			val.clear();
			mask.clear();
			m_idLevel = 0;
			//memset((char*)LevelID,0,sizeof(LevelID));
			clear();
		}	
		
		void clear()
		{
			m_idLevel = 0;
			id[0] = 0;
			val.clear();
			combinid[0] = 0;
			mask.clear();
			this->levList.Clear();
		}
		
		TAttr * clone()
		{
			TAttr * pNew;
			pNew = new TAttr();
			strcpy(pNew->id,id);
			strcpy(pNew->combinid,combinid);
			pNew->m_idLevel = m_idLevel;
			pNew->val = val;
			//memcpy(pNew->LevelID,LevelID,sizeof(LevelID));
			pNew->levList.Clone(&levList);
			return pNew;
		}
		
		
	};
	
	
	typedef std::map<std::string,TAttr * > ::iterator TAttrMapIter;
	
	//	typedef std::map<std::string,TAttr * > TAttrMap;
	
	
	//typedef TAttr TVal;
	
	struct TEleVal
	{
		unsigned long id;
		char hint[LEN_ID]; //������Э���� such as : afn 04.1.2:3 ��ʾ 04 ������,fn1 ����,��3����¼�ĵ�2����¼
		TMask mask;
		
		TEleVal()
		{
			clear();
		}
		
		inline void clear()
		{
			id = 0;
			hint[0]=0;
			mask.clear();
			
		}
		
		TEleVal * clone()
		{
			TEleVal * pNew = new TEleVal();
			pNew->id = id;
			strcpy(pNew->hint,hint);
			pNew->mask.copyFrom(&mask);
			
			return pNew;
		}
	};
	
	typedef TEleVal TVal;
	typedef std::vector<TVal * > ::iterator TValArrayIter;
	
	//����һ��ģ��
	typedef indexmap<unsigned long ,TVal> TValArray;
	
	class CFindUnit 
	{
	private:
		char _id[LEN_ID];
	public:
		
		CFindUnit(const char * sId )
		{
			strcpy(_id,sId);
		}
		
		bool operator () (TAttr * pItem) 
		{
			return strcmp(pItem->id,_id)==0; //strcmpi YYYY
		}  
	};
	
	
	typedef std::vector<TAttr * > ::iterator TAttrArrayIter;
	
	struct TAttrMap
	{
	private:
		std::map<std::string,TAttr * > valMap;
		std::vector<TAttr * >  valArr;
	private:
		
		// 		bool sameAttr(TAttr * pItem)
		// 		{
		// 			return strcmpi(pItem->id,_id)==0;
		// 		}
		
		TAttr *find_arr(std::string sKey)
		{
			
			TAttrArrayIter ItemItor =std::find_if (valArr.begin(), valArr.end(), 
				CFindUnit(sKey.c_str()));
			if (ItemItor==valArr.end()) 
			{ 
				return NULL;
			}
			else 
			{
				return *ItemItor;
			}
		}
	public:		
		//ɾ����ǰ����������
		inline void   del_arr(const  char * sId)
		{
			if (valArr.size() == 0) return;
			char vKey[LEN_ID];
			
			strcpy(vKey,sId);
			
			pub::strUpper(vKey);
			
			TAttrArrayIter ItemItor =std::find_if (valArr.begin(), valArr.end(), 
				CFindUnit(vKey));
			if (ItemItor!=valArr.end()) 
			{ 
				valArr.erase(ItemItor);
			}
			
			return ;
		}
		
		
		
		
		inline TAttr * find(const  char * sKey)
		{
			TAttr * pAttr=NULL;
			char vKey[LEN_ID];
			
			strcpy(vKey,sKey);
			
			pub::strUpper(vKey);
			
			TAttrMapIter mapIter;
			
			mapIter = valMap.find(vKey);
			
			if(mapIter != valMap.end() )
			{
				pAttr =  mapIter->second;
			}
			
			return pAttr;
		}
		
		inline TAttr * find(int index)
		{
			TAttr * pAttr=NULL;
			
			if (index < valArr.size())
				pAttr = valArr[index];
			
			return pAttr;
		}
		
		
		inline TAttr * operator[](int i)
		{
			return find(i);
		}
		
		inline TAttr * operator[](const  char * sKey)
		{
			return find(sKey);
		}
		
		
		inline int size()
		{
			return valMap.size();
		}
		
		//������е���������
		inline void   clear()
		{
			TAttrMapIter mapIter;
			TAttr * pAttr;
			for(mapIter = valMap.begin();mapIter!=valMap.end();mapIter++)
			{
				pAttr = mapIter->second;
				delete 	pAttr;	
			}
			
			valMap.clear();
			valArr.clear();
		}
		
		//���һ����������
		//��ʱ��Ҫ���� һ��ԭ��������
		inline int   add(TAttr * pVal)//������ھ͸���
		{
			TAttrMapIter mapIter;
			TAttr * pAttr=NULL;
			
			mapIter = valMap.find(pVal->id);
			
			if(mapIter != valMap.end() )
			{
				pAttr = mapIter->second;
				del(pAttr->id);
			}
			
			valMap[pVal->id] = pVal;
			
			valArr.push_back(pVal);
			
			return 0;
		}
		
		//ɾ����ǰ����������
		inline void   del(const  char * sId)
		{
			TAttrMapIter mapIter;
			TAttr * pAttr;
			
			mapIter = valMap.find(sId);
			
			if(mapIter != valMap.end() )
			{
				pAttr =  mapIter->second;
				valMap.erase(mapIter);
				del_arr(sId);
				delete pAttr;
			}
			
			return ;
		}
		
		inline int copyInto(TAttrMap & aMap,bool bReplace = false)
		{
			TAttrMapIter mapIter;
			TAttr * pAttr,*pTmp;
			int i,icnt;
			icnt = valArr.size();
			
			for(i=0;i<icnt;i++)
			{
				//pAttr = mapIter->second;
				pAttr = find(i);
				
				//��aMap Ѱ��,������� �͸��ݲ������ͽ��д���
				pTmp = aMap.find(pAttr->id);
				
				if (pTmp !=NULL)
				{
					if (bReplace) //���������,����Ҫɾ�����������
					{
						aMap.del(pAttr->id);	
					}else //����,���β���������
						continue;
					
				}				
				//���ڲ�Ѱ�ң�����ҵ��˾ͽ��и���
				
				aMap.add(pAttr->clone());
			}
			return aMap.size();
		}
		
	};
	
	
	class CDataProp
	{
	public:
		typedef std::map<std::string,TAttr *> TItemInfo; //
		typedef std::map<std::string,TAttr *>::iterator TItemInfoItr;
		typedef std::vector<TAttr *> TItemList;
		
		//
		TItemInfo  _Items; //����map id���� ��λ
		TItemInfo  _IdItems; //�������ݱ������ ���� ��λ
		TItemList  _list;
	public:
		CDataProp()
		{
			
		}
		
		~CDataProp()
		{
			clear();
		}
		
		inline int size()
		{
			return _list.size();
		}
		
		TAttr * operator[](int index)
		{
			return find(index);
		}		
		
		// 		void operator=(CDataProp & value)
		// 		{
		// 			if (this == &value) return;
		// 			*this = value;		
		// 		}
		
		void operator=(CDataProp & value)
		{
			TAttr * pAttr=NULL;
			int i,icnt=0;
			
			if (this == &value) return;
			clear();
			
			icnt = value._list.size();
			
			for (i=0;i<icnt;i++)
			{
				pAttr = value._list[i];
				add(pAttr->combinid,pAttr->val.c_str());
			}
		}
		
		inline void clear()
		{
			if (!_Items.empty())
				_Items.clear();
			
			if (!_IdItems.empty())
				_IdItems.clear();
			
			TAttr * pAttr;
			int i,cnt;
			cnt = _list.size();
			for (i=0;i<cnt;i++)
			{
				pAttr = _list[i];
				delete pAttr;
			}
			
			_list.clear();
			
		}
		
		
		// ��ʽ���� key = 1.2.3.4,9010 ,val='123'
		int add(std::string key, std::string val)
		{
			//		std::string sKey=(CHAR *)key;
			
			//�ж�id�Ƿ����,�ŷָ���������ڣ�˵����Ҫ������һ���ķ��ŷָ�����
			TAttr * pAttr;
			char sTmp[1024]="";
			
			strcpy(sTmp,key.c_str());
			char * sList[40];
			int lstCnt=0;
			
			lstCnt = pub::strsplit(sTmp,",",32,sList);//1.10.74.0,TARG_PR,=098.0 ���,id����,��ʽ
			
			if (lstCnt >=2)
			{ 
				//�����ж��Ƿ���ڣ�������ڣ��򱨴�
				pAttr = find(sList[0]);
				
				if (pAttr != NULL)
				{
	//strcmpi YYY
	
					if ((strcmp(pAttr->val.c_str(),val.c_str())!=0)&&(val.length()>0))
						throw TError(-3,"add a duplicate value!(%s) ",sList[0]);
				}else
				{
					pAttr = new TAttr;
					
					strcpy(pAttr->id,sList[0]);
					
					pAttr->val = val.c_str();
					//strcpy((char * )pAttr->val,val.c_str());
					strcpy(pAttr->eId,sList[1]);
					
					if (lstCnt >= 3)
						strcpy(pAttr->dateFmt,sList[2]);
					
					_Items[sList[0]] = pAttr;  //���mapid  1.1.1.3
					_IdItems[sList[1]] = pAttr; //���data id  9010
					strcpy(pAttr->combinid,key.c_str());
					_list.push_back(pAttr);
					
				}
			}else
			{
				pAttr = find(key);
				
				if (pAttr != NULL)
				{
					if (strcmp(pAttr->val.c_str(),val.c_str())!=0) //strcmpi YYY
						throw TError(-3,"add a duplicate value!(%s) ",sList[0]);
				}else
				{
					pAttr = new TAttr;
					
					strcpy(pAttr->id,key.c_str());
					pAttr->val = val.c_str();
					//strcpy((char * )pAttr->val,val.c_str());
					
					_Items[key] = pAttr;
					strcpy(pAttr->combinid,key.c_str());
					_list.push_back(pAttr);
					
				}
			}
			
			
			return 0;
		}
		
		TAttr * findDataId(std::string key)
		{
			//��� ���������ϵ
			TItemInfoItr mapIter;
			
			mapIter = _IdItems.find(key);
			if(mapIter != _IdItems.end() )
				return mapIter->second;
			else    // some character other than 0 - 9
				return NULL;
		}
		
		inline TAttr * find(int index)
		{
			//��� ���������ϵ
			if (index>=0 && index < _list.size())
				return _list[index];
			else
				return NULL;
		}
		
		
		TAttr * find(std::string key)
		{
			//��� ���������ϵ
			TItemInfoItr mapIter;
			
			mapIter = _Items.find(key);
			if(mapIter != _Items.end() )
				return mapIter->second;
			else    // some character other than 0 - 9
				return NULL;
		}
		
		char * asProperty(char * sBuff)
		{
			int ilen=0;
			char * pRec=sBuff;
			TAttr * pAttr;
			int i,cnt;
			cnt = _list.size();
			
			for (i=0;i<cnt;i++)
			{
				pAttr = _list[i];
				sprintf(pRec,"%s=%s\n",pAttr->combinid,pAttr->val.c_str());
				ilen = strlen(pRec);
				pRec +=ilen; 
			}
			return sBuff;
		}
		
		int addProperty(char * sBuff)
		{
			char sTmp[1024]="";
			
			strcpy(sTmp,sBuff);
			char * sList[4];
			int lstCnt=0;
			
			lstCnt = pub::strsplit(sBuff,"=",4,sList);
			if (lstCnt ==2)
				return add(sList[0],sList[1]);
			else if ((lstCnt ==1)&&(strchr(sTmp,'=')>0))
				return add(sList[0],"");
			else
				return 0;
		}
		
		int setProperty(char * sBuff)
		{
			int ilen=0;
			char sTmp[20240]="";
			strncpy(sTmp,sBuff,20200);
			sTmp[20200] = 0;
			pub::strUpper(sTmp);//��д
			
			char * sList[400];
			int i,lstCnt=0;
			
			lstCnt = pub::strsplit(sTmp,"\n",400,sList);
			
			for (i=0;i<lstCnt;i++)
			{
				//				if (i >= 290) 
				//					printf(sList[i]);
				addProperty(sList[i]);
			}
			
			return 0;
		}
		
		};
		
		//TMaskDepend
		
		typedef TMaskDepend TProperty;
		// ��� @ID1?0xFF,1,0X1,2,3�ı��ʽ���н���������������ֵ����������������
		class CDecode
		{
		private:
			typedef unsigned long  (*pFunc_getIdVal)(char * sid);
			
			// 			struct TProperty 
			// 			{
			// 				char sId[LEN_EID];
			// 				unsigned long val;
			// 
			// 				TProperty()
			// 				{
			// 					sId[0] = 0;
			// 					val = 0;
			// 				}
			// 
			// 
			// 			};
			
			typedef std::vector<TProperty * > TPropertyList;
			
			typedef std::vector<TProperty * > ::iterator TPropertyIter;
			
			TPropertyList _props;
			
			pFunc_getIdVal _getIdVal;
			
		public:
			
			int getPropCount()
			{
				return _props.size();
			}
			
			TProperty * getProp(int idx)
			{
				if (idx >= _props.size()) return NULL;
				
				
				TProperty * pProp = _props[idx];
				
				
				
				return pProp;
				
			}
			
			//�Ƿ���decode���ʽ
			static int isDecodeExpress(const char * sValue)
			{
				//����'?',@����
				
				if (strstr(sValue,"?")==NULL)
					return 0;
				
				if (strstr(sValue,",")==NULL)
					return 0;
				
				//����Ϊ�̶��������Ͳ�����@�����ɼ�������
				return 1;
			}
			
			void setCallback(pFunc_getIdVal func)
			{
				_getIdVal = func;
			}
			
			CDecode()
			{
				_getIdVal = NULL;
				clear();
			}
			
			CDecode(char * sValue)
			{
				_getIdVal = NULL;
				clear();
				setString(sValue);
			}
			
			~CDecode()
			{
				clear();
			}
			
			inline void clear()
			{
				
				int i,icnt;
				icnt = _props.size();
				TProperty * pProp;
				for (i=0;i<icnt;i++)
				{
					pProp = _props[i];
					delete pProp;
				}	
				
				_props.clear();
			}
			
			
			//���ʽ�Ƿ�Ϸ� @ID0?0XFF,5,0
			inline int setString(char * sValue)
			{
				TProperty * pProp;
				char sTmpVal[1024];
				char * sRst=NULL;
				
				memcpy(sTmpVal,sValue,sizeof(sTmpVal));
				sTmpVal[sizeof(sTmpVal)-1] = 0;
				strTrim_lr(sTmpVal);
				strUpper(sTmpVal);
				
				//1.Ѱ���ʺ�
				
				sRst = strstr(sTmpVal,"?");
				if (sRst != NULL)
				{
					pProp = new TProperty();
					sRst[0] = 0;
					
					if (sTmpVal[0]!='@')
					{
						//						strcpy(pProp->sId,sTmpVal);
						pProp->val = strtol(sTmpVal,NULL,16); //����ܹ�ֱ��תΪ�̶�ֵ,��ֱ�Ӵ������
						pProp->sID[0] = 0;
					}else
						strcpy(pProp->sID,sTmpVal+1);
					
					_props.push_back(pProp);
					
					sRst ++;
				}else
				{
					return 0;
				}
				
				char * tmparr[10];
				int i,icnt;
				icnt = pub::strsplit(sRst,",",9,tmparr);// �����������,������࿼��8������
				
				
				for (i=0;i<icnt;i++)
				{
					pProp = new TProperty();
					if (tmparr[i][0]!='@')
					{
						//						memcpy(pProp->sId,tmparr[i],LEN_EID);
						pProp->val = strtol(tmparr[i],NULL,16); //����ܹ�ֱ��תΪ�̶�ֵ,��ֱ�Ӵ������
						pProp->sID[0] = 0;
					}else
						memcpy(pProp->sID,tmparr[i]+1,LEN_EID);
					
					pProp->sID[LEN_EID-1] = 0;
					_props.push_back(pProp);
				}
				
				return _props.size();
			}
			
			int operator=(char * sValue)
			{
				return setString(sValue);
			}
			
			inline void prepare()
			{
				if (_getIdVal == NULL) return ;
				
				int i,icnt;
				icnt = _props.size();
				TProperty * pProp;
				for (i=0;i<icnt;i++)
				{
					pProp = _props[i];
					
					if ((pProp->sID[0]!='@')&&(pProp->sID[0]!=0))
						pProp->val = strtol(pProp->sID,NULL,16);
					else
						pProp->val = (*_getIdVal)(pProp->sID);
				}
			}
			
			// ����жϽ��
			int getResult(unsigned long &uRet)
			{
				uRet = 0;
				int i,icnt;
				TProperty * pCondProp,*pResult,*pMain;
				icnt = _props.size() / 2;
				
				if (_props.size() > 0)
					pMain = _props[0];
				else
					return -1; //��Ч���ʽ
				
				//ǰһ������������һ����ֵ
				for (i=1;i<icnt;i++)
				{
					pCondProp = _props[i*2-1];
					pResult =  _props[i*2];
					if (pMain->val == pCondProp->val) //
					{
						uRet = pResult->val; //�����������ͷ�����Ҫ��ֵ
						return 1;
					}
				}
				
				//��������û�м�¼
				if ((icnt *  2) == _props.size()) //ż������û������ֵ��
				{
					pResult = _props[ _props.size() -1]; //������ȡ���һ������ֵ
					uRet =  pResult->val;
					return 1;
				}
				
				return -1;
			}
		};
		
		
#pragma pack()
		
} //namespace pub

#endif //!defined(_publicfunc_)
