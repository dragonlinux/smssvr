// smspda.cpp : 定义控制台应用程序的入口点。
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

int kbhit() 
{
	static const int STDIN = 0;
	int bytesWaiting;

	static bool initialized = false; 

	if (! initialized) 
	{
		// Use termios to turn off line buffering
		termios term;
		tcgetattr(STDIN, &term);
		term.c_lflag &= ~ICANON;
		tcsetattr(STDIN, TCSANOW, &term);
		setbuf(stdin, NULL);
		initialized = true;
	} 

	ioctl(STDIN, FIONREAD, &bytesWaiting);
	return bytesWaiting;
}

 
int getCmdinTime(char * sCmd,int iwaitsec)   
{   
	int isnotover=0;
	time_t   begin,end;
	int   i=0;   
	std::string   inputcmd;

	begin=time(NULL);   
	do   
	{   
//		if(kbhit())
		{
			std::getline(std::cin,inputcmd);
			strcpy(sCmd,inputcmd.c_str());
			pub::strLower(sCmd);
			isnotover = 1;
			break;
		}
		end=time(NULL);
		sleep(100);
	}while(difftime(end,begin)<=iwaitsec);   

	return   isnotover;   
}   
int handle_console_cmd(std::string paramsCmd)
{
	char sCmd[556]="";
	int i;
	u_long maskFlag;
	pub_nexttime = 0;
	strcpy(sCmd,paramsCmd.c_str());
	char sDateTime[32],sTime[32]="";
	pub::CDateTime::GetCurDateTime(sDateTime);
	pub::CDateTime::getDataTime(sDateTime,"YYYYMMDDHHMISS","YYYY-MM-DD HH:MI:SS",sTime);
	ACE_DEBUG((LM_DEBUG,"\ncmd=[%s][%s]\n",sTime,sCmd));
	pub::strTrim_lr(sCmd);
	char sMsg[10220]="";
	if (sCmd[0] == '@')
	{
		system(sCmd+1);
		return 1;
	}
	if (paramsCmd=="?")
	{
		//	system("cls");
		printf("Usage: \n"
			"	version 1.2.1 \n"
			"	? : show this text. \n"
			"	@[system cmd]: execute window command .\n"
			"	exit : exit this process. \n"
			"	db : show cur db link list. \n"
			"	status : show block state. \n"
			"	show [on|off] [ALL|TRACE|DEBUG |INFO|NOTICE|WARNING]; the log trace info is on or off.\n"
			"	run interval cmd... ; when show is on,auto exec cmd every interval.\n"
			"   [^]task wait blk [<]task get blk [>]task release blk [$]task exec a sql [~]sync state [&]snd cmd\n"
			"	map : show memory map info. \n");
	}
	else
		if (paramsCmd == "exit") return 0;
		else if (paramsCmd == "db")
		{
			return 1;
		}
		else if (paramsCmd == "map")
		{
			return 1;
		}
		else if (paramsCmd == "status")
		{
			//	system("cls");
			CBaseTask * pTask;
			CBaseTask::m_TaskPool;
			std::vector<CBaseTask * > ::iterator vIter;
			sprintf(sMsg,"%s-------------------------------------------------\n",SVERSION);
			strcat(sMsg,"TASKNAME THREAD MAX MIN RES BLKLEN BLKAMT USED MAXUSED USEDTIMES\n");
			for (vIter=CBaseTask::m_TaskPool.begin();vIter!=CBaseTask::m_TaskPool.end();vIter++)
			{
				pTask = *vIter;
				pTask->getstatus(sMsg+strlen(sMsg));
			}
			strcat(sMsg,"-----------------------------------------------------------------\n");
			ACE_DEBUG((LM_DEBUG,"%s\n",sMsg));
		}if (strstr(sCmd,"show") == sCmd) //KILL 在前面
		{
			//		system("cls");
			char stmp[256];
			char sFlag[10][24]={""};
			char * pCont;
			/*
			char * tmparr[12];			
			int i,iinterval,iarrcnt = 0;

			iarrcnt = pub::strsplit(sCmd," ",6,tmparr); //用这个作为每个记录的串的分割字符串
			sscanf("1|2|3|4 5|6 7|8 9","%[^|]|%[^|]|%[^|]|%[^|]|%[^|]",stmp[0],stmp[1],stmp[2],stmp[3],stmp[4]);
			*/
			memset(stmp,0,sizeof(stmp));
			strcpy(stmp,sCmd+4);
			pub_is_show_trace = 1; //默认显示是关闭状态
			maskFlag = 0;
			if (stmp[0]!=0)
			{
				if ((pCont=strstr(sCmd,"off"))!=NULL)
				{
					pub_is_show_trace = 0;
					pCont = pCont + strlen("off");
				}else
				{
					pCont=strstr(sCmd,"on");
					if (pCont == NULL)
					{
						pCont = sCmd + strlen("show");
					}else
					{
						pCont = pCont + strlen("on");
					}
				}
				sscanf(pCont,"%[^|]|%[^|]|%[^|]|%[^|]|%[^|]%[^|]|%[^|]|%[^|]|%[^|]|%[^|]",
					sFlag[0],sFlag[1],sFlag[2],sFlag[3],sFlag[4],
					sFlag[5],sFlag[6],sFlag[7],sFlag[8],sFlag[9]);
				if (strstr(pCont,"all")!=0)
				{
					maskFlag = LM_TRACE|LM_DEBUG |LM_INFO|LM_NOTICE|LM_WARNING|LM_ERROR;
				}else
				{
					maskFlag = 0;
					for (i=0;i<10;i++)
					{
						if (strstr(sFlag[i],"trace")!=NULL)
						{
							maskFlag = maskFlag | LM_TRACE;
						}else
							if (strstr(sFlag[i],"debug")!=NULL)
							{
								maskFlag = maskFlag | LM_DEBUG;
							}else
								if (strstr(sFlag[i],"info")!=NULL)
								{
									maskFlag = maskFlag | LM_INFO;
								}else
									if (strstr(sFlag[i],"notice")!=NULL)
									{
										maskFlag = maskFlag | LM_NOTICE;
									}else
										if (strstr(sFlag[i],"warning")!=NULL)
										{
											maskFlag = maskFlag | LM_WARNING;
										}
					}
				}
				if (maskFlag == 0)
				{
					maskFlag = LM_TRACE|LM_DEBUG |LM_INFO|LM_NOTICE|LM_WARNING|LM_ERROR;
				}
			}
			u_long priority_mask =ACE_LOG_MSG->priority_mask (ACE_Log_Msg::PROCESS);
			if (pub_is_show_trace)
			{
				ACE_SET_BITS (priority_mask,maskFlag);
			}
			else
				ACE_CLR_BITS (priority_mask,maskFlag);
			//			设置只记录LM_DEBUG 或者LM_INFO级别的消息
			ACE_Log_Msg *i = ACE_Log_Msg::instance ();
			i->priority_mask(priority_mask,ACE_Log_Msg::PROCESS);
		}else if (strstr(sCmd,"run") == sCmd) //RUN 在前面
		{
			//		system("cls");
			char * tmparr[12];
			int i,iinterval,iarrcnt = 0;
			iarrcnt = pub::strsplit(sCmd," ",6,tmparr); //用这个作为每个记录的串的分割字符串
			iinterval = strtol(tmparr[1],NULL,10);
			for (i=2;i<iarrcnt;i++)
			{
				if (tmparr[i]!=NULL)
					handle_console_cmd(tmparr[i]);
			}
			pub_nexttime = time(NULL)+iinterval;
			return 2;
		}
		return 1;
}
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
		//数据库参数---------------------------------------------------------------------
		oCfgIni.IniReadStr("ORADB","USER",dbUser,dbUser,sizeof(dbUser));
		oCfgIni.IniReadStr("ORADB","PWD",dbPass,dbPass,sizeof(dbPass));
		oCfgIni.IniReadStr("ORADB","NAME",dbName,dbName,sizeof(dbName));
		//建立两个连接，一个连接用于扫描，一个连接用于更新
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
		//数据库扫描任务，用于扫描数据库，并将数据状态更新到数据库中
		//---------------------------------------------------------------------

		CCommTask * pSerialTask = TheCommTask::instance();
		//-----------------------串口及短信服务
		oCfgIni.IniReadStr("SERIAL","DEV","COM1",pSerialTask->m_dev,15);
		oCfgIni.IniReadInt("SERIAL","BAUDRATE",9600,&(pSerialTask->m_baudrate));
		oCfgIni.IniReadInt("SERIAL","PARITY",0,(int *)&(pSerialTask->m_parityenb));
		oCfgIni.IniReadInt("SERIAL","DATABIT",8,(int *)&(pSerialTask->m_databits));
		oCfgIni.IniReadInt("SERIAL","STOPBIT",1,(int *)&(pSerialTask->m_stopbits));
		oCfgIni.IniReadInt("SERIAL","READTIMEOUT",100,(int *)&(pSerialTask->m_readTimeOut));
		oCfgIni.IniReadStr("MAIN","SMSCENTER",pSerialTask->m_smsCenter,pSerialTask->m_smsCenter,sizeof(pSerialTask->m_smsCenter));
		pSerialTask->init();
		pSerialTask->start(1,1);
		TheScanDbCmdTask::instance()->start(1,1);
		TheDataAnalyzeTask::instance()->start(1,1); //启动数据分析线程
		ACE_DEBUG((LM_DEBUG,"waitting command ...\n"));
		//		handle_console_cmd("SHOW OFF");
		//		handle_console_cmd("RUN 120 @CLS LS STATUS");
		while(true)
		{
			//std::cout <<"\nInput Command=>\n" << std::flush ;
			char sCmd[256];
			std::string inputcmd;
			//std::getline(std::cin,inputcmd);
			if (0==getCmdinTime(sCmd,1)) //0 表示没有获得指令
			{
				//	ACE_DEBUG((LM_DEBUG,"waitting ...\n"));
				if ((pub_nexttime !=0) &&(oldCmd.length()>0) &&(time(NULL)-pub_nexttime>0))
				{
					strcpy(sCmd,oldCmd.c_str());
				}else
					continue;
			}
			inputcmd = sCmd;
			strcpy(sCmd,inputcmd.c_str());
			pub::strLower(sCmd);
			inputcmd = sCmd;
			int iret =0;
			iret = handle_console_cmd(inputcmd);
			if (iret == 2)
			{
				oldCmd = inputcmd;
			}
			if(!iret)
				break;
			else
			{
				//	ACE_DEBUG((LM_DEBUG,"w"));
				ACE_DEBUG((LM_DEBUG,"waitting command ...\n"));
			}
			Sleep(100);
		}

exitmain:
		TheDataAnalyzeTask::instance()->shutdown();
		// 	//停止从数据库读取数据
		TheScanDbCmdTask::instance()->shutdown();
		//
		// 	//停止串口通讯任务
		TheCommTask::instance()->shutdown();
		//
		//日志服务关闭
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
	ACE_DEBUG((LM_DEBUG,"请按任意键退出!\n"));
	ACE_LOG_MSG->msg_ostream (&std::cout); //防止因oslog 被释放 造成后面的资源访问失败
	ACE::fini();
	getchar();
	return nRet;
}

