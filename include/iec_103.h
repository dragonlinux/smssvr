// iec103.h: interface for the CSF6IEC103 class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _IEC_103_H
#define _IEC_103_H

//#if _MSC_VER > 1000
//#pragma once
//#endif // _MSC_VER > 1000
#include <stdio.h> 
#include <string.h> 
#include "iec_def.h"
#pragma pack(1) 
#ifdef __cplusplus
extern "C" {
#endif 

	//namespace iec103
	//{
#ifdef WIN32
#pragma pack(1) 
#endif

#define BS_1(x) val##x

#define BIT(x) BS_1(x)

	typedef  __packed struct
	{
		char BIT(0):1;
		char BIT(1):1;
		char BIT(2):1;
		char BIT(3):1;
		char BIT(4):1;
		char BIT(5):1;
		char BIT(6):1;
		char BIT(7):1;
		char BIT(8):1;
		char BIT(9):1;
		char BIT(10):1;
		char BIT(11):1;
		char BIT(12):1;
		char BIT(13):1;
		char BIT(14):1;
		char BIT(15):1;
		char BIT(16):1;
		char BIT(17):1;
		char BIT(18):1;
		char BIT(19):1;
		char BIT(20):1;
		char BIT(21):1;
		char BIT(22):1;
		char BIT(23):1;
		char BIT(24):1;
		char BIT(25):1;
		char BIT(26):1;
		char BIT(27):1;
		char BIT(28):1;
		char BIT(29):1;
		char BIT(30):1;
		char BIT(31):1;
	}TFmt08; //格式06	


#define BIT2(x) BIT(x)

	typedef  __packed struct
	{
		char BIT2(0):2;
		char BIT2(1):2;
		char BIT2(2):2;
		char BIT2(3):2;
		char BIT2(4):2;
		char BIT2(5):2;
		char BIT2(6):2;
		char BIT2(7):2;
		char BIT2(8):2;
		char BIT2(9):2;
		char BIT2(10):2;
		char BIT2(11):2;
		char BIT2(12):2;
		char BIT2(13):2;
		char BIT2(14):2;
		char BIT2(15):2;
	}TFmt09; //格式06		


	//传输格式定义	
	typedef __packed struct  //下行	下发
	{
		BYTE _func:4;// 原来这里是对命令进行分组处理,用于对任务进行预先准备　用于功能准备
		BYTE _fcv:1; //帧计数有效位FCV
		//FCV=1：表示FCB位有效；FCV=0：表示FCB位无效

		BYTE _fcb:1; //帧计数位FCB
		//当帧计数有效位FCV=1时,FCB表示每个站连续的发送/确认或者请求/响应服务的变化位。
		//FCB位用来防止信息传输的丢失和重复

		BYTE _prm:1; //启动标志位PRM
		//PRM =1：表示此帧报文来自启动站；PRM =0：表示此帧报文来自从动站

		BYTE _dir:1; //传输方向位DIR DIR=0：表示此帧报文是由主站发出的下行报文； 
		//DIR=1：表示此帧报文是由终端发出的上行报文
	}TCTRL_DOWN;

	typedef __packed struct  //上行	 上传
	{
		BYTE _func:4; //1:功能
		BYTE _dfc:1; //DFC(数据流控制位)。 DFC=0表示从站可以接受数据,
		//DF =1表示从站缓冲区已满,无法接受新数据。
		//本规约规定(SF6)从站上送报文中DFC始终为0,即可以接受新数据,主站应能够处理DFC=1的报文

		BYTE _acd:1; //请求访问位ACD
		//ACD位用于上行响应报文中.ACD=1 表示终端有重要事件等待访问
		//ACD=0表示终端无事件数据等待访问.

		BYTE _prm:1;
		//PRM =1：表示此帧报文来自启动站；PRM =0：表示此帧报文来自从动站
		BYTE _dir:1;
	}TCTRL_UP; 

	typedef __packed union //_T_CTRL
	{
		TCTRL_UP up;
		TCTRL_DOWN down;
		BYTE val;
	} TCTRL,*PCTRL;//TGB_CTRL

	typedef __packed struct //_TFt_pkg_head 
	{
		BYTE _begin;
		BYTE _len;
		BYTE _len_cfm;
		BYTE _begin_cfm;
		TCTRL _ctrl;
		BYTE _addr;
	}TDLY_HEAD,PDLY_HEAD; //68 可变


	typedef __packed struct 
	{
		BYTE num:7;
		BYTE sq:1; //sq = 1
		//SQ = 1,信息体中只含单个元素或综合信息元,信息体地址(FUN和INF)即信息元地址；
		//SQ = 0,信息体地址是顺序排列的信息元素的第一个信息元地址,后续信息元地址从这个地址起顺序加1。
	}TVSQ;

	typedef  __packed struct 
	{
		BYTE _typ; //1类型标识
		TVSQ _vsq; //2可变结构限定词
	}TASDU_HEAD;

	typedef __packed struct 
	{
		unsigned char len;
		TDLY_HEAD pkghead;
		unsigned char asduBuff[sizeof(TRecord) - sizeof(TDLY_HEAD) - sizeof(unsigned char) ];
	}T68Frame;


	typedef __packed union 
	{
		T68Frame  fme;
		TRecord   rec;
	}UFrame;

	typedef __packed struct
	{
		UFrame frame;
		unsigned char cAsduLen;
		BYTE * pCrc; //校验码
		BYTE * pEnd; //结束码
	}TFrame,*PFrame;


	//内部数据格式定义

	typedef __packed struct 
	{
		BYTE grp:4;  //组号
		BYTE num:4;  //编号
	}TGIN,*PGIN;


	typedef __packed  struct
	{
		BYTE type:4;  //描述类型 0:整型,1:浮点,2:字符串,3:位,4:结构体
		BYTE size:4;  //数据宽度 //最大 32 个字节
	}TGDD,*PGDD;

#define COMPILE_MPLAB

#ifdef COMPILE_MPLAB	

	typedef  __packed struct
	{
		unsigned char * p[2];
	}TWord;
	//	#define P_VAL  pval.p[0]
	//	typedef unsigned char * pointer;
#else

	typedef  __packed struct
	{
		unsigned char * p[2];
	}TWord; 
#endif 
#define P_VAL  pval.p[0]
	typedef unsigned char * pointer;



	//内部所有数据的数据字典
	typedef __packed union 
	{
		TWord pval;
		unsigned long  lval;
		unsigned char  val[4];
		unsigned short wval[2];
		float          fval;
		TFmt08         b1s32;
		TFmt09         b2s16;
	}TGID;

	//内部所有数据的数据字典
	typedef  __packed struct
	{
		TGIN  def;  //1标识定义 1
		TGDD  typ;  //3类型定义 1
		TGID  val; //4数据值 4 
	}TDICT; //本结构最大就6个字节



	typedef  unsigned char TFmt01;
	typedef  unsigned short TFmt02;	
	typedef  unsigned long TFmt03;	
	typedef  float TFmt04;


	//string; 6


	//共4个字节
	typedef __packed struct 
	{
		BYTE sec:6;							//64 个数字
		BYTE min:6 ;						//64 个数字
		BYTE hour:5;						//24 个数字
		BYTE day:5;							//32 位
		BYTE mon:4;			        //12 二位月
		BYTE year:6;						//两位 年 00～64 ,年份小于2064年
	}TPK_DATETIME,*PPK_DATETIME; //压缩时间格式 共四个字节 YYYYMMDDMISS


	typedef TGID TFmt11;


	//事件 状态变位告警
	typedef  __packed struct
	{
		TGIN   HA1;   //固定为 A1H 
		unsigned long occTime;   //发生时刻  4 后台计时器
		TGIN   HA2;   //固定为 A2H 
		unsigned char occSite;   //变位位置  1 后台根据这个就知道是发生了什么变化
		TGIN   H8F;   //固定为 08FH
		TFmt08 curStatu; //当前状态  4
		TGIN   dataID;   //报警数据编号 2 //如果为00 表示无效
		TFmt03 curVal ;  //当前告警值 4  告警的内容,通过curStatu 的告警位AI编号能够发现
	}TFmt0F; //格式12 共 15个字节

	typedef  TFmt0F TEvent;

	//内部所有数据的数据字典
	typedef  __packed struct
	{
		TGIN  def;  //1标识定义 1
		TGID  data;  //4数据值 4 
	}TItem; //本结构最大就6个字节

	//最大只传送20个数据组
	typedef __packed struct
	{
		TASDU_HEAD head;
		TItem  aItem;
	}TASDU,*PASDU;



#ifdef WIN32	
#pragma pack()
#endif	
	//} //namespace pub
	//准备报文
	void initUFrame(TFrame * pFme);


	//是否是Iec103报文格式
	short isIEC103(char * sBuff);

	void parseIEC103(TFrame * pFrame); //解析103规约,获得内部的数据值
	short makeIEC103(TFrame *  pFrame); //将输入内容形成发送报文

	//将不可见字符转换为可见字符                                     
	short hexToAsc(unsigned char * hex, unsigned char * asc, int hexlen);         

	//将可见字符转换为不可见字符                                     
	short ascToHex(unsigned char * asc,unsigned char * hex, int asclen);

#ifdef __cplusplus
}
#endif
#endif //_IEC_103_H
