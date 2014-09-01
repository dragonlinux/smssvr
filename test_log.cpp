// smspda.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include   <stdio.h> 
#include "ace/Task.h"
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>

#include "ConfigIni.h"
#include "publicfunc.h"
#include "LoggerTask.h"

#include "structured_exception.h"
#define SVERSION "VERSION----1.2.0"


char pub::CLog::m_work_dir[256];
int pub_is_show_trace;
time_t pub_nexttime;


int ACE_TMAIN(int argc, char * argv[])
{
	int icnt=0;	
	char sAceVer[128];
	char sCurPath[512]="";
	char outBodyBuff[1024];

	char errSrc[3024]="";
	char errMsg[3024]="";

	int itmpVal,nRet;
	//	std::cout<<"size of item is "<<sizeof(Item)<<std::endl;
	char sDateTime[32];
	pub::CDateTime::GetCurDateTime(sDateTime);
	char sLogFile[256];
	sprintf(sLogFile,"%s_%s.log",argv[0],sDateTime);
	//std::string(argv[0])+'_'+sDateTime+ ".log")
	std::ofstream oslog(sLogFile,ios::out | ios::trunc);
	//YYY  pub::disableDosClose();
	try
	{
		getcwd(sCurPath,sizeof(sCurPath));  //这里可能存在找不到路径的情况,此时只要在vc上设置工作目录为.exe所在目录即可
		char sIniFileName[128];
		sprintf(sIniFileName,"%s/%s",sCurPath,"smspda.ini");
		//strcpy(sIniFileName,"smspda.ini");
		CConfigIni oCfgIni("#",sIniFileName);
		//---------------------------------------------------------------------
		int iret = ACE::init();
		sprintf(sAceVer,"compile ace.dll[%s],used ace.dll[%d.%d.%d]",ACE_VERSION,ACE::major_version(),ACE::minor_version(),ACE::beta_version());

		//设置日志
		ACE_LOG_MSG->open (argv[1],
			ACE_Log_Msg::STDERR |
			ACE_Log_Msg::OSTREAM);
		//std::ofstream myostream (SERVER_LOG_FILE_NAME, ios::out | ios::trunc);
		ACE_LOG_MSG->msg_ostream (&oslog);
		//读入配置文件
		//	ACE_DEBUG((LM_DEBUG,"read config file from %s\n",SERVER_CONFIG_FILE));
		//	XMLConfig::get_instance().read_config_from_xml_file(SERVER_CONFIG_FILE);

		ACE_DEBUG((LM_DEBUG,"%s,ace init=%d\n",sAceVer,iret));
		ACE_DEBUG((LM_DEBUG,"theLoggerTask start=%d\n",iret));
		theLoggerTask::instance()->start(1,1);


		
		while(true)
		{
			icnt ++;
	strcat(errSrc,"1");
	strcat(errMsg,"2");

			ACE_DEBUG((LM_DEBUG,"error:%s [%s]\n",errSrc,errMsg));
			Sleep(100);
		}

exitmain:

		theLoggerTask::instance()->shutdown();
		ACE::fini();
	}
	catch(...)
	{
		ACE_DEBUG((LM_DEBUG,"error:..."));
		strcpy(outBodyBuff,"calc error!!!");
		pub::CLog::WriteLog(5,"\n catch(...) UnKnow Error!\n");
		nRet = -14;
	}
	ACE_DEBUG((LM_DEBUG,"请按任意键退出!\n"));
	ACE_LOG_MSG->msg_ostream (&std::cout); //防止因oslog 被释放 造成后面的资源访问失败
	ACE::fini();
	getchar();
	return nRet;
}

