/*=======================================================================================
description :
用于按照规定格式读写配置文件的类。同时可用于写日志
envrionment:
in linux or other supporse the c++ env unix.
history :
serite 	2001-09-04 create
/*=======================================================================================*
功能描述
[域名称1]
键值名称1=键值
键值名称2=键值
[域名称2]
键值名称1=键值
键值名称2=键值
....
例:
[DATABASE]
UserName=fjjf
Password=fjjf
Service=jfora
[LOG]
LogType=0
LogFilePath=/usr/users/fjzh4th/boc/log
[NETWORK]
PortNumber=8010
Timeout=15
对应的上述结构可以专门产生结构类型.形成对应的结构类型.iniparm GetIniInfo(char * banktype,char * inifile)
*=======================================================================================*/
#ifndef  _CFG_INIFILE
#define  _CFG_INIFILE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <ctype.h>
#include <time.h>
#include <stdarg.h>
#include <sys/stat.h>
#include "publicfunc.h"

//#include "BaseTypes.h"
//#include "common.h"

#define PROFILE_MAX_APPNAMELEN 	50
#define PROFILE_MAX_KEYNAMELEN 	50
#define PROFILE_MAX_VALUELEN   	255
#define PROFILE_MAX_LINELEN 	1024


#define	FILENAMELEN		256		/*file name length*/
#define	BUSINAMELEN		32		/*business name length*/
#define	BANKTYPELEN		8		/*bank type name lengh*/
#define	IPADDRSIZE		24		/*ip address length*/

#define	COMMENTCHAR1	"#"	/*注释字符串*/


#define GETOK        0
#define FILENOTFOUND -1
#define APPNOTFOUND  -2
#define KEYNOTFOUND  -3

#define LOG_STD		0
#define LOG_FILE	1
#define LOG_BOTH	2

#include "publicfunc.h"

#define     TRIMTEXT "\t\f\n\r\x20"
//#include <Shlwapi.h>

#define SUCCESS	0
#define FAILURE -1

#define CLOSED 0
#define OPENED 1

typedef pub::_S_Err S_Err;

static S_Err CfgFileErr[] =
{
	{	0	,"无错误，命令正确"	},
	{	1	,"配置文件不存在"	},
	{	2	,"域名为定义"	},
	{	3	,"键名未定义"	},
};

//##ModelId=3C02656201AD
class CConfigIni
{
private:
	
	char * Trim_LR(char * sStr)
	{
//		TCHAR trim[] = TEXT(TRIMTEXT);
//		StrTrim(sStr,trim);
		return pub::strTrim_lr(sStr);
//		return sStr;
	}
	
    //##ModelId=3C02656201D4
	char	_COMMENTSTR[8];		//注释字符串
	
    //##ModelId=3C02656201DF
	char	_LOGPATH[255] ;		/* 默认记录文件名 */
	
	
	
	
	/* 以下是GetIniFieldKeyValue模块 --得到声明的配置文件域说明符*/
	
	/*#################################################################################################*/
	/*动态读配置文件产生参数*/
	/*#################################################################################################*/
	/*读出以回车或者为TAB结尾的每一行纪录,以及在注释符之前的数据*/
	//##ModelId=3C02656201F2
	char * _getline(FILE *fp,char * buf) /*读出以回车或者为TAB结尾的每一行纪录,以及在注释符之前的数据*/
	{
		char *	pTemp;
		memset(buf,'\0',PROFILE_MAX_VALUELEN);					/*初始化缓存*/
		fgets(buf,PROFILE_MAX_VALUELEN,fp);						/*得到纪录*/
		if(
			(pTemp=strstr(buf,_COMMENTSTR))||
			(pTemp=strstr(buf,"+"))
			)	
			*pTemp='\0';		/*过滤注释信息*/
		
		return Trim_LR(buf);									/*返回正文*/
	}
	
	
	/*忽略大小写,比较域名称appname,appline固定长度的比较:返回1表示为比较结果正确*/
	//##ModelId=3C0265620206
	int _check_appline(char * appname,char * appline, int size)	/*取出[]之内的数据*/
	{
		int i;
		
		if (appline[0]!='[') return 0;					/*保证比较的数据为[开头*/
		appline ++;
		
		while (*appline==' ') appline ++;				/* filter the space 过滤[之后开头的的空格*/
		for (i=0;i<size;i++)							/* compare 比较appname,appline的值*/
		{
			if (appline[i]=='\0') return 0;
			if(tolower(appname[i])!=tolower(appline[i])) return 0;
		}
		appline += size;
		while (*appline==' ') appline ++;				/* filter the space 过滤]之前的空格*/
		
		return (*appline==']'?1:0);						/* find ']' */
	}
	
	/*比较键值名称,返回对应值=后的值*/
	//##ModelId=3C0265620211
	char * _check_keyline(char * KeyName,char * KeyLine, int Size)
	{
		int i;
		char * keyline;
		
		keyline = KeyLine;
		for (i=0;i<Size;i++)				/* compare keyname*/
		{
			if (keyline[i]=='\0') return NULL;
			if (tolower(KeyName[i])!=tolower(keyline[i])) return NULL;
		}
		
		keyline += Size;
		while (*keyline==' ') keyline ++;	/* filter the head space before '='*/
		
		if (*keyline!='=') return NULL;		/* find the '=' */
		
		keyline ++;
		while(*keyline==' ') keyline ++;	/* filter the space after '='*/
		
		return keyline;						/* return the value pointer */
	}
				
	//##ModelId=3C0265620225
	int _GetIniFieldKeyValue
		(
		const char *FileName,		/*配置文件名称*/
		const char *AppName,		/*域名称*/
		const char *KeyName,		/*键值名称*/
		const char *DefaultStr,		/*默认值*/
		char *ResultStr,	/*--返回值--键值名称对应的键值*/
		int nSize			/*返回值的长度*/
		)
	{
		FILE * fp;
		char * value;
		int Acount,Kcount;
		char buf[PROFILE_MAX_LINELEN];
		char aname[PROFILE_MAX_APPNAMELEN];
		char kname[PROFILE_MAX_KEYNAMELEN];
		
		
		strncpy(aname,AppName,PROFILE_MAX_APPNAMELEN);
		aname[PROFILE_MAX_APPNAMELEN - 1] = '\0';
		Trim_LR(aname);
		strncpy(kname,KeyName,PROFILE_MAX_KEYNAMELEN);
		kname[PROFILE_MAX_KEYNAMELEN - 1] = '\0';
		Trim_LR(kname);
		Acount=strlen(aname);
		Kcount=strlen(kname);
		
		/*初始化返回串*/
		strncpy(ResultStr,DefaultStr,nSize);
		if (nSize > 0) ResultStr[nSize - 1] = '\0';
		
		if ((fp=fopen(FileName,"r"))==NULL) return FILENOTFOUND;
		
		while (!feof(fp))
		{
			_getline(fp,buf);		/*读取文件纪录*/
			if (*buf=='\0') continue;
			if (*buf=='[')
			{
				if (!_check_appline(aname,buf,Acount))	/*如果内容不匹配--域名称不一致*/
					continue;
				else
				{
					while (!feof(fp))
					{
						_getline(fp,buf);		/*取得纪录*/
						if (*buf=='\0') continue;	/*如果头为;则读取下一条纪录*/
						if (*buf=='[')			/*如果到了下一个域名位置,则说明此键值不存在*/
						{
							fclose(fp);
							return KEYNOTFOUND;
						}
						if ((value=_check_keyline(kname,buf,Kcount)))
						{
							strncpy(ResultStr,value,nSize);
							if (nSize > 0) ResultStr[nSize - 1] = '\0';
							fclose(fp);
							return GETOK;
						}
					}
					fclose(fp);
					return KEYNOTFOUND;
				}
			}
		}
		fclose(fp);
		return APPNOTFOUND;
	}
	protected:
		
		//##ModelId=3C02656201DE
		char	_FILENAME[128];		//文件名称
	public:
		//##ModelId=3C0265C20091
		S_Err ErrMsg;
		
		CConfigIni(const char * cmmentstr,const char *filename)
		{
			ErrMsg = CfgFileErr[0];
			
			memcpy(_COMMENTSTR,cmmentstr,sizeof(_COMMENTSTR));
			_COMMENTSTR[sizeof(_COMMENTSTR)-1] = 0;
			
			if (_COMMENTSTR[0]==0) strcpy(_COMMENTSTR,"//");		//默认的注释字符串
			
			memcpy(_FILENAME,filename,sizeof(_FILENAME));
			_FILENAME[sizeof(_FILENAME)-1] = 0;
			
			strcpy(_LOGPATH,"");
			
			if (IsFileExist(_FILENAME)<0) 
			{
				throw pub::TError(-1,"This file [%s] can not be found!\n",_FILENAME);
			}
		}
		
		int readSect(	char *FileName,		/*配置文件名称*/
			char *AppName,		/*域名称*/
			char * sBuff)
		{
			FILE * fp;
			int Acount;
			char buf[PROFILE_MAX_LINELEN];
			char aname[PROFILE_MAX_APPNAMELEN];
			
			strncpy(aname,AppName,PROFILE_MAX_APPNAMELEN);
			aname[PROFILE_MAX_APPNAMELEN - 1] = '\0';
			Trim_LR(aname);
			Acount=strlen(aname);
			
			if ((fp=fopen(FileName,"r"))==NULL) return FILENOTFOUND;
			
			while (!feof(fp))
			{
				_getline(fp,buf);		/*读取文件纪录*/
				if (*buf=='\0') continue;
				if (*buf=='[')
				{
					if (!_check_appline(aname,buf,Acount))	/*如果内容不匹配--域名称不一致*/
						continue;
					else
					{
						sBuff[0]=0;
						while (!feof(fp))
						{
							_getline(fp,buf);		/*取得纪录*/
							if (*buf=='\0') continue;	/*如果头为;则读取下一条纪录*/
							if (*buf=='[')			/*如果到了下一个域名位置,则说明此键值不存在*/
							{
								fclose(fp);
								return strlen(sBuff);
							}
							
							strcat(sBuff,buf);
							strcat(sBuff,"\n");
						}
						fclose(fp);
						return  strlen(sBuff);
					}
				}
			}
			fclose(fp);
			return APPNOTFOUND;
		}
		
		int readaSect(char *FileName,	/*配置文件名称*/
			char *AppName,		/*域名称*/
			char * sBuff,char * sNextSect)
		{
			FILE * fp;
			int Acount;
			char buf[PROFILE_MAX_LINELEN];
			char aname[PROFILE_MAX_APPNAMELEN];
			char * pNext;
			
			strncpy(aname,AppName,PROFILE_MAX_APPNAMELEN);
			aname[PROFILE_MAX_APPNAMELEN - 1] = '\0';
			Trim_LR(aname);
			Acount=strlen(aname);
			
			if ((fp=fopen(FileName,"r"))==NULL) return FILENOTFOUND;
			
			while (!feof(fp))
			{
				_getline(fp,buf);		/*读取文件纪录*/
				if (*buf=='\0') continue;
				if (*buf=='[')
				{
					if (!_check_appline(aname,buf,Acount))	/*如果内容不匹配--域名称不一致*/
						continue;
					else
					{
						sBuff[0]=0;
						while (!feof(fp))
						{
							_getline(fp,buf);		/*取得纪录*/
							if (*buf=='\0') continue;	/*如果头为;则读取下一条纪录*/
							if (*buf=='[')			/*如果到了下一个域名位置,则说明此键值不存在*/
							{
								pNext = strchr(buf,']');
								if (pNext != NULL)
								{
									*pNext = 0;
									strcpy(sNextSect,buf+1);
								}

								fclose(fp);
								return strlen(sBuff);
							}
							
							strcat(sBuff,buf);
							strcat(sBuff,"\n");
						}
						fclose(fp);
						return  strlen(sBuff);
					}
				}
			}
			fclose(fp);
			return APPNOTFOUND;
		}
		
		
		/* 以下是IsFileExist模块 */
		//##ModelId=3C0265620256
		int IsFileExist(char *FileName)
		{
			FILE *fp;
			
			if ((fp=fopen(FileName,"r"))!=NULL)
			{
				fclose(fp);
				ErrMsg = CfgFileErr[0];
				return SUCCESS;
			}
			else
			{
				ErrMsg = CfgFileErr[1];
				return FAILURE;
			}
		}
		
		//##ModelId=3C0265620258
		int	WriteMsg(char stat,const char *Format,...)
		{
			va_list ap;
			char sLog[2048];	/*错误记录字符串*/
			char sErrDesc[2048];
			char sTime[20];
			
			//	GetCurTime(sTime);	/*取得当前时间*/
			pub::CDateTime::GetCurDayTime(sTime);	/*取得当前时间*/
			
			
			/*生成错误记录字符串*/
			strcpy(sLog,sTime);	/*当前时间*/
			strcat(sLog, "  ");
			va_start(ap, Format);
			vsprintf(sErrDesc,Format,ap);
			va_end(ap);
			strcat(sLog, sErrDesc);
			if(sErrDesc[strlen(sErrDesc)- 1] != '\n') strcat(sLog, "\n");
			switch(stat)
			{
			case LOG_FILE:
				RecordLog(sLog,_LOGPATH);
				break;
				
			case LOG_BOTH:
				perror(sLog);
				RecordLog(sLog,_LOGPATH);
			case LOG_STD:
				puts(sLog);fflush(stdout);
				break;
			}
			return 0;
			
		}
		/*写日期对帐日志*/
		//##ModelId=3C026562026B
		int	RecordLog(char* msg,char * logpath,int flag=0)
		{
			FILE* filep;
			char LogFileName[255];	/*全路径名(格式:"./bankday/JHBANK/DZ20000125.log")*/
			char DateStr[9];	/*日期*/
			
			strcpy(LogFileName,logpath);
			
			if (strstr(LogFileName,".log")==NULL)
			{
				if (flag) pub::CDateTime::GetCurDate(DateStr);
				else pub::CDateTime::GetCurMon(DateStr);
				
				strcat(LogFileName, DateStr);
				strcat(LogFileName,".log");
			}
			filep=fopen(LogFileName,"a");
			if(filep==NULL)
			{
				fclose(filep);
				return -1;
			}else
			{
				fwrite( msg, strlen(msg),1, filep);
				fclose(filep);
				return 0;
			}
		}
		
		//##ModelId=3C0265620275
		int	IniReadStr(const char * App,const  char *Key,const char *DefaultStr,char * ResultStr,int Size)
		{
			return _GetIniFieldKeyValue(_FILENAME,App,Key,DefaultStr,ResultStr,Size);
			
		}
		//##ModelId=3C0265620292
		int	IniReadInt(const char * App,const char *Key,int DefaultInt,int *ResultInt)
		{
			char DefStr[64];
			char ResStr[24];
			
			sprintf(DefStr,"%d",DefaultInt);
			
			_GetIniFieldKeyValue(_FILENAME,App,Key,DefStr,ResStr,24);
			
			*ResultInt = atoi(ResStr);
			
			return 0;
		}
		
		//##ModelId=3C02656202A6
		int	IniReadFloat(char * App,char *Key,float DefaultF,float *ResultF)
		{
			char DefStr[64];
			char ResStr[24];
			
			sprintf(DefStr,"%f",DefaultF);
			_GetIniFieldKeyValue(_FILENAME,App,Key,DefStr,ResStr,24);
			*ResultF = atof(ResStr);
			
			return 0;
		}
		
		//##ModelId=3C02656202BB
		int	SetLogPath(char * logpath,char * head)
		{
			sprintf(_LOGPATH,"%s%s",logpath,head);
			return SUCCESS;
		}
		
		
};

#endif

