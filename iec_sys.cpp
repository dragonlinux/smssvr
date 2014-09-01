// 由系统调用的报文,将受到的信息转换为数据库内容
#include "stdafx.h"
#include "iec_sys.h"
#include <stdlib.h>
#include <time.h>
//和后台的同步,采用表接口方式,通过定义一个状态,我这里从数据库中将数据形成一个大的字符串,映射到内部表中,
//然后合成综合报文向下发送
TField  _TAB_X1X[6] =
{
	{{0x1,0x01,"1.1.0",0},{7,12},{"13800591500"}},    //短信中心号码 表示在缓存区_x1x 第0个指针位置开始 长度由12确定
	{{0x1,0x02,"1.2.0",0},{7,12},{"13799416858"}},    //主站短信号码1 表示在缓存区_x1x 第12个指针位置开始 长度由12确定
	{{0x1,0x03,"1.3.0",0},{7,12},{"13799416858"}},    //主站短信号码bak 第18个位置
	{{0x1,0x04,"1.4.0",0},{7,12},{"15199416858"}},    //本机号码 第32个位置
	{{0x1,0x05,"1.5.0",0},{7,12},{"172.168.198.188:12345"}},    //GPRS主站&端口
	{{0x1,0x06,"1.6.0",0},{7,12},{"192.168.198.188:12345"}},    //GPRS主站&端口 备用
};
//终端运行参数
TField  _TAB_X2X[6] =
{
	{{0x2,0x01,"2.1.0",0},{3,4},{0XFF,0XFF,0XFF,0XFF}},    //通讯维持时间 15～65535 //秒
	{{0x2,0x02,"2.2.0",0},{1,4},{0X00}},    //通讯日间隔        
	{{0x2,0x03,"2.3.0",0},{1,4},{0X3008}},	 //报告时间点 每天08:{{0x2,0x03,"2.3.0",ADDR230},{7,8},{""}},        //tfmt07 系统内部计时,前面4个字节为按照秒进行计数的累加器 后 面4为 为系统传送过来的用来表示基准的时间标签,用来进行时间计算的值
	{{0x2,0x04,"2.4.0",0},{1,4},{0XFF}},    //信号确认秒 最大256      
	{{0x2,0x05,"2.5.0",0},{3,4},{0X00}},     // 单片机当前从基准到现在?
	{{0x2,0x06,"2.6.0",0},{5,4},{0X08,0X30}},// 基准时间点 YYMMDDHHMISS
};
//终端运行记录
TField  _TAB_X3X[6] =
{
	{{0x03,0x01,"3.1.0",0},{5,4},{0}}, //首次投运时间 pack_time 格式,用4个字节表达YYMMDDHHMISS
	{{0x03,0x02,"3.2.0",0},{5,4},{0}}, //上次启动时刻 pack_time 格式,用4个字节表达YYMMDDHHMISS
	{{0x03,0x03,"3.3.0",0},{2,4},{0}}, //上次运行时长 秒
	{{0x03,0x03,"3.4.0",0},{2,4},{0}}, //上次运行时长 秒
	{{0x03,0x03,"3.5.0",0},{2,4},{0}}, //本次工作gprs发送状态  收到的短信数量,发送的短信数量
	{{0x03,0x04,"3.6.0",0},{2,4},{0}}, //累计个发送状态 	       收到的短信数量,发送的短信数量
};
//信号量重要等级
TField  _TAB_X410_SUB[16] =
{
	{{0x00,0x01,"4.1.0",0},{9,2},{0}},   //一盖打开
	{{0x00,0x02,"4.1.1",0},{9,2},{0}},   //锁舌开
	{{0x00,0x03,"4.1.2",0},{9,2},{0}},   //二盖打开
	{{0x00,0x04,"4.1.3",0},{9,2},{0}},   //井下入侵
	{{0x00,0x05,"4.1.4",0},{9,2},{0}},   //水位1越限
	{{0x00,0x06,"4.1.5",0},{9,2},{0}}, //温度1越限
	{{0x00,0x07,"4.1.6",0},{9,2},{0}}, //水位2报警
	{{0x00,0x08,"4.1.7",0},{9,2},{0}}, //温度2报警
	{{0x00,0x09,"4.1.8",0},{9,2},{0}}, //气体1越限
	{{0x00,0x0a,"4.1.9",0},{9,2},{0}}, //气体2越限
	{{0x00,0x0b,"4.1.a",0},{9,2},{0}}, //气体3越限
	{{0x00,0x0c,"4.1.b",0},{9,2},{0}}, //气体4越限
	{{0x00,0x0d,"4.1.c",0},{9,2},{0}}, //参数变更
	{{0x00,0x0e,"4.1.d",0},{9,2},{0}}, //授权状态
	{{0x00,0x0f,"4.1.e",0},{9,2},{0}}, //系统授权开锁
	{{0x00,0x10,"4.1.f",0},{9,2},{0}}, //解锁箱授开锁
};
//模拟量量重要等级
TField  _TAB_X420_SUB[16] =
{
	{{0x00,0x01,"4.2.0",0},{8,2},{0}}, //倾斜角度
	{{0x00,0x02,"4.2.1",0},{8,2},{0}}, //冲击力量
	{{0x00,0x03,"4.2.2",0},{8,2},{0}}, //水位值
	{{0x00,0x04,"4.2.3",0},{8,2},{0}}, //温度值
	{{0x00,0x05,"4.2.4",0},{8,2},{0}}, //电池电压
	{{0x00,0x06,"4.2.5",0},{8,2},{0}}, //气体1
	{{0x00,0x07,"4.2.6",0},{8,2},{0}}, //气体2
	{{0x00,0x08,"4.2.7",0},{8,2},{0}}, //气体3
	{{0x00,0x09,"4.2.8",0},{8,2},{0}}, //
	{{0x00,0x0a,"4.2.9",0},{8,2},{0}}, //
	{{0x00,0x0b,"4.2.a",0},{8,2},{0}}, //
	{{0x00,0x0c,"4.2.b",0},{8,2},{0}}, //
	{{0x00,0x0d,"4.2.c",0},{8,2},{0}}, //
	{{0x00,0x0e,"4.2.d",0},{8,2},{0}}, //
	{{0x00,0x0f,"4.2.e",0},{8,2},{0}}, //
	{{0x00,0x10,"4.2.f",0},{8,2},{0}}, //
};
#define ADDR410 (unsigned char *)&_TAB_X410_SUB[0]
#define ADDR420 (unsigned char *)&_TAB_X420_SUB[0]
//状态为设定,报警等级设定
TField  _TAB_X4X[2] =
{
	{{0x04,0x01,"4.1.0",ADDR410},{8,4},{0XFF,0XFF,0XFF,0XFF}}, // 信号量重要等级 共16个状态,每个位用两位表达,当内部状态变位时, 0:表示无此功能,1:表示普通,定时发送 2:表示为重要,变位时需要立即发送 3:备用
	{{0x04,0x02,"4.2.0",ADDR420},{8,4},{0XFF,0XFF,0XFF,0XFF}}, // 采集量重要等级 同上,采集值越限时,表示发生状态变位时
};
//测定值的边界设定值
TField _TAB_X5X[9] =
{
	{{0x05,0x01,"5.2.0",0},{11,4},{0X10}}, 		// 倾斜角度
	{{0x05,0x02,"5.2.0",0},{11,4},{0X10}}, 		// 冲击力量
	{{0x05,0x03,"5.3.0",0},{11,4},{0X10,0X20,0X30,0X40}}, // 温度
	{{0x05,0x04,"5.4.0",0},{11,4},{0X10,0X20,0X30,0X40}}, // 水位
	{{0x05,0x05,"5.5.0",0},{11,4},{0X10,0X20,0X30,0X40}}, // 电池电压
	{{0x05,0x06,"5.6.0",0},{10,4},{0X10,0X20,0X30,0X40}}, // 气体1
	{{0x05,0x07,"5.7.0",0},{10,4},{0X10,0X20,0X30,0X40}}, // 气体2
	{{0x05,0x08,"5.8.0",0},{10,4},{0X10,0X20,0X30,0X40}}, // 气体3
};
//各种
//数据状态位,为1表示发生了告警
TField _TAB_X8F0_SUB[8] =
{
	{{0x00, 0,"8.f.1", 0},{8,1},{0}}, //一盖打开
	{{0x00, 2,"8.f.2", 0},{8,1},{0}}, //二盖打开
	{{0x00, 3,"8.f.3", 0},{8,1},{0}}, //井下入侵
	{{0x00, 4,"8.f.4", 0},{8,1},{0}}, //水位1越限
	{{0x00, 5,"8.f.5", 0},{8,1},{0}}, //温度1越限
	{{0x00, 6,"8.f.6", 0},{8,1},{0}}, //水位2报警
	{{0x00, 7,"8.f.7", 0},{8,1},{0}}, //温度2报警
	{{0x00, 8,"8.f.8", 0},{8,1},{0}}, //气体1越限
/*	{{0x00, 9,"8.f.9", 0},{8,1},{0}}, //气体2越限
	{{0x00,10,"8.f.a", 0},{8,1},{0}}, //气体3越限
	{{0x00,11,"8.f.b", 0},{8,1},{0}}, //气体4越限
	{{0x00,12,"8.f.c", 0},{8,1},{0}}, //参数变更
	{{0x00,13,"8.f.d", 0},{8,1},{0}}, //授权状态
	{{0x00,14,"8.f.e", 0},{8,1},{0}}, //系统授权开锁
	{{0x00,15,"8.f.f", 0},{8,1},{0}}, //解锁箱授开锁
	{{0x00,16,"8.f.10",0},{8,1},{0}}, //倾斜角度
	{{0x00,17,"8.f.11",0},{8,1},{0}}, //冲击力量
	{{0x00,18,"8.f.12",0},{8,1},{0}}, //水位值
	{{0x00,19,"8.f.13",0},{8,1},{0}}, //温度值
	{{0x00,20,"8.f.14",0},{8,1},{0}}, //电池电压
	{{0x00,21,"8.f.15",0},{8,1},{0}}, //气体1
	{{0x00,22,"8.f.16",0},{8,1},{0}}, //气体2
	{{0x00,23,"8.f.17",0},{8,1},{0}}, //气体3
	{{0x00,24,"8.f.18",0},{8,1},{0}}, //
	{{0x00,25,"8.f.19",0},{8,1},{0}}, //
	{{0x00,26,"8.f.1a",0},{8,1},{0}}, //
	{{0x00,27,"8.f.1b",0},{8,1},{0}}, //
	{{0x00,28,"8.f.1c",0},{8,1},{0}}, //
	{{0x00,29,"8.f.1d",0},{8,1},{0}}, //
	{{0x00,30,"8.f.1e",0},{8,1},{0}}, //
	{{0x00,31,"8.f.1f",0},{8,1},{0}}, //*/
};
#define ADDR8F0 (unsigned char *)&_TAB_X8F0_SUB[0]
//测定值实际
TField _TAB_X8X[9] =
{
	{{0x08,0x01,"8.1.0",0},{3,4},{0}}, // 倾斜角度
	{{0x08,0x02,"8.2.0",0},{3,4},{0}}, // 冲击力量
	{{0x08,0x03,"8.3.0",0},{3,4},{0}}, // 温度
	{{0x08,0x04,"8.4.0",0},{3,4},{0}}, // 水位
	{{0x08,0x05,"8.5.0",0},{3,4},{0}}, // 电池电压
	{{0x08,0x06,"8.6.0",0},{3,4},{0}}, // 气体1可燃
	{{0x08,0x07,"8.7.0",0},{3,4},{0}}, // 气体2氧气
	{{0x08,0x08,"8.8.0",0},{3,4},{0}}, // 气体3二氧化碳
	{{0x08,0x0F,"8.f.0",ADDR8F0},{8,4},{0XFFFFFFFF}}, // 整体工作状态 共16个信号状态 16个采集状态的当前状态,用位表达
};
TField _TAB_XAX[] =
{
	{{0x0A,0x01,"a.1.0",0},{5,4},{0}}, //相对基准发生时间
	{{0x0A,0x02,"a.2.0",0},{1,4},{0}}, //变位标志位
	{{0x0A,0x03,"a.3.0",0},{3,4},{0}}, //变值内容
};
TField  _TAB_XB10_SUB[3] =
{
	{{0x0,0x01,"b.1.1",0},{1,12},{0X00}},//手机号码
	{{0x0,0x02,"b.1.2",0},{3,4},{0}},		 //申请时长
	{{0x0,0x03,"b.1.3",0},{3,4},{0}},		 //申请次数
};
#define ADDRB10 (unsigned char *)&_TAB_XB10_SUB[0]
TField _TAB_XBX[9] =
{
	{{0x0B,0x01,"b.1.0",ADDRB10},{12,20},{0}}, //B1H 系统授权
	{{0x0B,0x02,"b.2.0",ADDRB10},{12,20},{0}}, //B2H 申请系统授权
	{{0x0B,0x03,"b.3.0",ADDRB10},{12,20},{0}}, //B3H 系统授权确认
	{{0x0B,0x04,"b.4.0",ADDRB10},{12,20},{0}}, //B4H 系统解除授权
	{{0x0B,0x05,"b.5.0",ADDRB10},{12,20},{0}}, //B5H 向井盖申请开锁授权
	{{0x0B,0x06,"b.6.0",ADDRB10},{12,20},{0}}, //B6H 井盖授权确认
	{{0x0B,0x07,"b.7.0",ADDRB10},{12,20},{0}}, //B7H 开锁
	{{0x0B,0x08,"b.8.0",ADDRB10},{12,20},{0}}, //B8H 闭锁
	{{0x0B,0x09,"b.9.0",ADDRB10},{12,20},{0}}, //B9H 执行确认???需要重新调整格式
};
#define SZ(x) sizeof(x)/sizeof(TField)
TField * _TAB_PXX[8]={_TAB_X1X,_TAB_X2X,_TAB_X3X,_TAB_X4X,_TAB_X5X,_TAB_X8X,_TAB_XAX,_TAB_XBX};
unsigned char _TAB_NXX[8]={SZ(_TAB_X1X),SZ(_TAB_X2X),SZ(_TAB_X3X),SZ(_TAB_X4X),SZ(_TAB_X5X),SZ(_TAB_X8X),SZ(_TAB_XAX)}; //内部的数据定义值
TField * _TAB_SUB_PXX[4]={_TAB_X410_SUB,_TAB_X420_SUB,_TAB_X8F0_SUB,_TAB_XB10_SUB};
unsigned char _TAB_SUB_NXX[8] = {SZ(_TAB_X410_SUB),SZ(_TAB_X420_SUB),SZ(_TAB_X8F0_SUB),SZ(_TAB_XB10_SUB)};
//全局变量
TFrame aTempFme = 
{
	{
		0,{
			0x68,0,0,0x68,{0x08},0x00
		},{""}
	},	0,0,0
};
//寻找指定分组的在内存映射表中的组号
char findFldGrp(TFldID aID)
{
	char i,icnt,idx=-1;
	icnt = 8;
	for(i=0;i<icnt;i++)
	{
		if (_TAB_PXX[i]->id.grp == aID.grp)
		{
			idx = i;
			break;
		}
	}
	return idx;
}
//根据组号和地址寻找编号
TField * findFld(TFldID aID)
{
	char i,icnt,idx=-1;
	TField * pFld;
	idx = findFldGrp(aID);
	if (idx>=0)
	{
		pFld = _TAB_PXX[idx];
		icnt = _TAB_NXX[idx];
		if (0 == aID.num)
			return pFld; //如果aGin.num 表示寻找的是内部分组,而不是具体的哪个值,故返回此组的首地址
		for(i=0;i<icnt;i++)
		{
			if (aID.num == pFld->id.num) //编号一致,说明找到了
				return pFld;
			pFld +=1;
		}
	}
	return 0;
}
//将内存表的数据传出来为序列字符,供通讯使用
short getDataFromFld(TItem * pTarg)
{
	char iTmpLen;
	TField * pFld;
	TFldID aid;
	aid.grp = pTarg->def.grp;
	aid.num = pTarg->def.num;
	pFld = findFld(aid);
	iTmpLen = pFld->type.size;
	memcpy(pTarg->data.val,pFld->val.sval,iTmpLen);
	//返回的长度为去掉 TGID 长度后,TGID内的实际数据的长度
	return 	sizeof(TGIN) + iTmpLen;
}
//根据参数形成命令报文,将参数发送下去
//输入的输入为参数列表
//例如这样的格式
//"8.1 13799416858"
//8.2 223
//8.4
//采用分号作为分割符号,用冒号做为前导符号
//short sysMake
//处理控制层面的流程服务
short sysDealFrameCtrl(TFrame * pFrame,char * sBuffer,int &curIdx)
{
	TASDU *	pAsdu;
	short iRet;
	unsigned char cFunc,cAsdulen;
	pAsdu =(TASDU *) pFrame->frame.fme.asduBuff;
	cAsdulen = pFrame->cAsduLen;
	//获得功能码
	cFunc = pFrame->frame.fme.pkghead._ctrl.down._func;
	//暂时不考虑处理功能码
	iRet =  sysDealAsduFunc(pAsdu,sBuffer,curIdx);
	//直接获取 asdu type 和具体含义值
	return 0;
}
int findSubFld(TField * pFld)
{
	int i,cnt,idx;
	cnt = sizeof(_TAB_SUB_PXX) / sizeof(TField *);
	for(i=0;i<cnt;i++)
	{
		if (_TAB_SUB_PXX[i]==pFld) return i;
	}
	return -1;
}
short getStringByType(int type,TGID * data,char * sout)
{
	TPK_DATETIME aTime;
	switch(type)
	{
	case 1: //char
	case 2: //short
	case 3: //long
		sprintf(sout,"%d",data->lval);
		break;
	case 4://float
		sprintf(sout,"%8.3f",data->fval);
		break;
	case 5://time
		aTime = *(TPK_DATETIME *)(data->val);
		sprintf(sout,"%04.4d%02.2d%02.2d%02.2d%02.2d%02.2d",
			aTime.year+2000,aTime.mon,aTime.day,aTime.hour,aTime.min,aTime.sec);
		break;
	case 6: //bool
		sprintf(sout,"%d",data->val[0]?1:0);
		break;
	case 7: //string
		memcpy((char *)data->val,sout,sizeof(data->val));
		break;
	case 8: //
		break;
	default :
		memcpy((char *)data->val,sout,sizeof(data->val));
	}
	return 0;
}
//处理字节循环
short sysSetDataByteItem(TField * psubFld,char *sinVal,char * sBuffer,int&curIdx)
{
	int i,isubidx,isubcnt,istart,iend;
	int size;
	TField * pFld;
	char Val[16];
	TGID aGid;
	unsigned long maskVal=1;
	TValue aVal;
	isubidx = findSubFld((TField *)psubFld);
	if (isubidx <0) return -1;
	istart = 0;
	isubcnt = _TAB_SUB_NXX[isubidx];	
	for(i=0;i<isubcnt;i++)
	{
		pFld = psubFld + i;
		size = pFld->type.size;
		memcpy(aGid.val,sinVal+istart,size);
		istart +=size;
		strcpy(aVal.sID,(char*)pFld->id.subFld);
		getStringByType(pFld->type.type,&aGid,aVal.sValue);
		memcpy(sBuffer+curIdx,(char *)&aVal,sizeof(TValue));
		curIdx += sizeof(TValue);
	}
	return isubcnt;
}
//处理位循环
short sysSetDataBitItem(TField * psubFld,unsigned long lval,char * sBuffer,int &curIdx)
{
	int i,k,isubidx,isubcnt;
	int size;;
	TField * pFld;
	unsigned long maskVal=1,curval;
	TValue aVal;
	isubidx = findSubFld((TField *)psubFld);
	if (isubidx <0) return -1;
	isubcnt = _TAB_SUB_NXX[isubidx];	
	for(i=0;i<isubcnt;i++)
	{
		pFld = psubFld + i;
		size = pFld->type.size;
		curval = 0;
		maskVal = 1;
		for(k=0;k<size;k++)
		{	
			if (lval&1)
			{
				curval +=maskVal;
			}
			maskVal <<=1;
			lval >>= 1;
		}
		strcpy(aVal.sID,(char *)pFld->id.sid);
		sprintf(aVal.sValue,"%d",curval);
		memcpy(sBuffer+curIdx,(char *)&aVal,sizeof(TValue));
		curIdx +=sizeof(TValue);
	}
}
//将传送来的数据 保存到内存中，并返回用到的数据长度
short sysSetDataToDef(TItem * pSrc,char * sBuffer,int &curIdx)
{
	char iTmpLen;
	int isubcnt,isubidx;
	TField * pFld;
	TFldID aFld;
	TPK_DATETIME *aTime;
	TValue aVal;
	aFld.grp = pSrc->def.grp;
	aFld.num = pSrc->def.num;
	pFld = findFld(aFld);
	if (pFld==0) return 0;
	iTmpLen = pFld->type.size;
	//判断是否存在子位，如果有子位，本体就不处理了 //子位的数量如何确定？
	if (pFld->id.subFld!=0)
	{
		if (pFld->type.type ==8) //为位操作
		{
			sysSetDataBitItem((TField *)pFld->id.subFld,pSrc->data.lval,sBuffer,curIdx);
		}
		else //为字节操作
		{
			sysSetDataByteItem((TField *)pFld->id.subFld,(char *)pSrc->data.val,sBuffer,curIdx);
		};
	}else
	{
		getStringByType(pFld->type.type,&(pSrc->data),aVal.sValue);
		strcpy(aVal.sID,(char *)pFld->id.sid);
		memcpy(sBuffer+curIdx,(char *)&aVal,sizeof(TValue));
		curIdx +=sizeof(TValue);
	}
	return 	iTmpLen;
}
//将定值数据都保存到内部表中
short sysSave2Table(TASDU * pAsdu,char * sBuffer,int &curIdx)
{
	short iAllLen=0,i,icnt,iItemLen=0;
	char cRet=1;
	TItem * pSrcItem;
	icnt = pAsdu->head._vsq.num;

	for(i=0;i<icnt;i++)
	{
		pSrcItem = (TItem*)((char *)&(pAsdu->aItem) +iAllLen);
		iItemLen = sysSetDataToDef(pSrcItem,sBuffer,curIdx)+sizeof(TGIN); //TItem的头
		iAllLen += iItemLen;
	}
	return curIdx; //返回实际的队列数目
}
//处理业务ASDU层面的流程func服务
//返回 处理后 的 pAsdu 使用的长度
short sysDealAsduFunc(TASDU * pAsdu,char * sBuffer,int &curIdx)
{
	unsigned char cFunc;
	short iRet=0;
	TASDU_HEAD * pHead;
	TGIN * pGin;
	pHead =(TASDU_HEAD *) pAsdu;
	iRet =  sysSave2Table(pAsdu,sBuffer,curIdx);
	return iRet;
}
void GetFmtDateTime(char * sTime)
{
	struct tm* tm_buf;
	time_t t;
	t = time(0);
	tm_buf = localtime(&t);
	sprintf(sTime,"%04d-%02d-%02d %02d:%02d:%02d", //YYYY-MM-DD HH:MI:SS
		tm_buf->tm_year+ 1900,tm_buf->tm_mon+1,tm_buf->tm_mday,
		tm_buf->tm_hour,tm_buf->tm_min,tm_buf->tm_sec);
}
//处理接收到短信内容,
//如果需要返回内容,就返回一串字符串,并返回一个大于0的值,否则就不返回
//返回值<=0 说明没有报文返回
short sysDealRecvInfo(int inlen ,unsigned char * sRecv,TValue * outRecs)
{
	short i,iRet,ilen;
	TRecord * pRec;
	char sFrameBuff[1024];
	TValue ValueList[200];
	int curIdx=0,icnt,curval;
	TFrame aFrame = aTempFme;
	pRec = &aFrame.frame.rec;
	//将可见字符转换为不可见字符          

	ilen  = ascToHex((unsigned char *)sRecv,(unsigned char *)sFrameBuff, strlen((char*)sRecv));  

	memcpy(pRec->buf,sFrameBuff,ilen);

	//memcpy(pRec->buf,sRecv,inlen);
	pRec->len = inlen;
	//解析发送的数据
	parseIEC103(&aFrame);//分析aRcvFme.rec->buf, aRcvFme.rec->Len
	//处理主站发送来的信息内容

	//1.分析报文内容
	iRet = sysDealFrameCtrl(&aFrame,(char*)ValueList,curIdx);
	icnt = curIdx / sizeof(TValue);

	memcpy((char*)outRecs,(char*)ValueList,icnt*sizeof(TValue));
	return icnt;
}

