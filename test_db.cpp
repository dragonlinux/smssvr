// smspda.cpp : �������̨Ӧ�ó������ڵ㡣
//
#include "stdafx.h"
#include   <stdio.h> 
//#include <conio.h>
#include "ace/Task.h"
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>

#include <sys/select.h>
#include <termios.h>
#include <sys/ioctl.h>

#include "ConfigIni.h"
#include "publicfunc.h"
#include "LoggerTask.h"
#include "commTask.h"
#include "ScanDbTask.h"
#include "DataAnalyzeTask.h"
#include "structured_exception.h"
#define SVERSION		"VERSION----1.2.0"


char pub::CLog::m_work_dir[256];
int pub_is_show_trace;
time_t pub_nexttime;


int ACE_TMAIN(int argc, char * argv[])
{
	char sAceVer[128];
	char sCurPath[512]="";
	char dbIp[24]="127.0.0.1";
	int  dbPort=3307,nRet=0;
	char dbUser[24]="gb";
	char dbPass[24]="gb";
	char dbName[24]="gbgprs";
	char bindIpPort[128];
	char outBodyBuff[1024];
	int maxVal,minVal,count;
	std::string oldCmd;
	int itmpVal;
	//	std::cout<<"size of item is "<<sizeof(Item)<<std::endl;
	char sDateTime[32];
	TSmsInfo smsList;

	pub::CDateTime::GetCurDateTime(sDateTime);
	char sLogFile[256];
	sprintf(sLogFile,"%s_%s.log",argv[0],sDateTime);
	//std::string(argv[0])+'_'+sDateTime+ ".log")
	std::ofstream oslog(sLogFile,ios::out | ios::trunc);
	//YYY  pub::disableDosClose();
	try
	{
		getcwd(sCurPath,sizeof(sCurPath));  //������ܴ����Ҳ���·�������,��ʱֻҪ��vc�����ù���Ŀ¼Ϊ.exe����Ŀ¼����
		char sIniFileName[128];
		sprintf(sIniFileName,"%s/%s",sCurPath,"smspda.ini");
		//strcpy(sIniFileName,"smspda.ini");
		CConfigIni oCfgIni("#",sIniFileName);
		//---------------------------------------------------------------------
		int iret = ACE::init();
		sprintf(sAceVer,"compile ace.dll[%s],used ace.dll[%d.%d.%d]",ACE_VERSION,ACE::major_version(),ACE::minor_version(),ACE::beta_version());

		//������־
		ACE_LOG_MSG->open (argv[1],
			ACE_Log_Msg::STDERR |
			ACE_Log_Msg::OSTREAM);
		//std::ofstream myostream (SERVER_LOG_FILE_NAME, ios::out | ios::trunc);
		ACE_LOG_MSG->msg_ostream (&oslog);
		//���������ļ�
		//	ACE_DEBUG((LM_DEBUG,"read config file from %s\n",SERVER_CONFIG_FILE));
		//	XMLConfig::get_instance().read_config_from_xml_file(SERVER_CONFIG_FILE);

		ACE_DEBUG((LM_DEBUG,"%s,ace init=%d\n",sAceVer,iret));
		ACE_DEBUG((LM_DEBUG,"theLoggerTask start=%d\n",iret));
		theLoggerTask::instance()->start(1,1);
		//���ݿ����---------------------------------------------------------------------
		oCfgIni.IniReadStr("ORADB","USER",dbUser,dbUser,sizeof(dbUser));
		oCfgIni.IniReadStr("ORADB","PWD",dbPass,dbPass,sizeof(dbPass));
		oCfgIni.IniReadStr("ORADB","NAME",dbName,dbName,sizeof(dbName));
		//�����������ӣ�һ����������ɨ�裬һ���������ڸ���
		TOCIDatabase oradb[2];
		nRet = oradb[0].Connect(dbUser, dbPass, dbName);
		if (!nRet)
		{
			ACE_DEBUG((LM_DEBUG,"can not connect oracle\n"));
			getchar();
			exit(1);
		}
		nRet = oradb[1].Connect(dbUser, dbPass, dbName);
		ACE_DEBUG((LM_DEBUG,"connect oracle ok %d\n",nRet));
		TheScanDbCmdTask::instance()->init(&(oradb[0]));
		TheDataAnalyzeTask::instance()->init(&(oradb[1]));
		//���ݿ�ɨ����������ɨ�����ݿ⣬��������״̬���µ����ݿ���
		//---------------------------------------------------------------------

		TheScanDbCmdTask::instance()->start(1,1);
		TheDataAnalyzeTask::instance()->start(1,1); //�������ݷ����߳�
		ACE_DEBUG((LM_DEBUG,"waitting command ...\n"));
		//		handle_console_cmd("SHOW OFF");
		//		handle_console_cmd("RUN 120 @CLS LS STATUS");
		while(true)
		{



	strcpy(smsList.batchid,"");
strcpy(smsList.phone,"10658033");
strcpy(smsList.datatime,"20140808121212");
strcpy(smsList.info,"������������������й��ƶ�����С���֣�Ϊ���ṩ����ȫ����Ľ���֪ʶ�����������ĺð��֡��ظ�7���ģ����72Сʱ��2Ԫ/�¡�������������������й��ƶ�����С���֣�Ϊ���ṩ����ȫ����Ľ���֪ʶ�����������ĺð��֡��ظ�7���ģ����72Сʱ��2Ԫ/�¡�������������������й��ƶ�����С���֣�Ϊ���ṩ����ȫ����Ľ���֪ʶ�����������ĺð��֡��ظ�7���ģ����72Сʱ��2Ԫ/�¡�");
smsList.status = 0;
	
	TheScanDbCmdTask::instance()->putMsg(0,0,(char *)&(smsList),sizeof(TSmsInfo));

			Sleep(10000000);
		}

exitmain:
		TheDataAnalyzeTask::instance()->shutdown();
		// 	//ֹͣ�����ݿ��ȡ����
		TheScanDbCmdTask::instance()->shutdown();

		//
		//��־����ر�
		theLoggerTask::instance()->shutdown();
		ACE::fini();
	}
	catch(pub::TError  &e)
	{
		int errlen;
		errlen = strlen(e.what());
		if (e.whatno() == -5 )
		{
			strcpy(outBodyBuff,"SQL ERROR!");
		}else
			strcpy(outBodyBuff,e.what());
		ACE_DEBUG((LM_DEBUG,"TError %s\n",e.what()));
		//		pub::CLog::WriteLog(5,"\n%s\n",e.what());
		nRet = -1;
	}
	catch (std::exception const &exception)
	{
		ACE_DEBUG((LM_DEBUG,"error:exception"));
		strcpy(outBodyBuff,exception.what());
		ACE_DEBUG((LM_DEBUG,"err %s\n",exception.what()));
		pub::CLog::WriteLog(5,"\nException\n%s\n",exception.what());
		nRet = -13;
	}
	catch (CPubException const &exception)
	{
		ACE_DEBUG((LM_DEBUG,"error:CPubException"));
		sprintf(outBodyBuff,"\nerrcode[%X]!form [%X]",
			exception.what(),exception.where());
		ACE_DEBUG((LM_DEBUG,"err %s\n",exception.what()));
		pub::CLog::WriteLog(5,"\nStructured_exception\n%s %s\n",exception.what(),exception.where());
		nRet = -11;
	}
	catch (unsigned exception)
	{
		ACE_DEBUG((LM_DEBUG,"error:exception"));
		sprintf(outBodyBuff,"caught C++ exception %x!",exception);
		pub::CLog::WriteLog(5,"\ncaught C++ exception %x!\n",exception);
		nRet = -12;
	}
	catch(...)
	{
		ACE_DEBUG((LM_DEBUG,"error:..."));
		strcpy(outBodyBuff,"calc error!!!");
		pub::CLog::WriteLog(5,"\n catch(...) UnKnow Error!\n");
		nRet = -14;
	}
	ACE_DEBUG((LM_DEBUG,"�밴������˳�!\n"));
	ACE_LOG_MSG->msg_ostream (&std::cout); //��ֹ��oslog ���ͷ� ��ɺ������Դ����ʧ��
	ACE::fini();
	getchar();
	return nRet;
}

