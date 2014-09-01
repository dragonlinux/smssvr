//iec_sys.h
//设备端

#ifndef _IEC_SYSTEM_H
#define _IEC_SYSTEM_H
#pragma pack(1) 
#include "iec_def.h"
#include "iec_103.h" //规约

//将此队列从内存中copy出来后,在此情况下进行处理,确保不得修改其内部的内容,只能修改复本的内容,
//并将其作为一个个独立的内容进行组装,最终将内容完全解析完毕
//当存在子类型是
//报文必须是以递归深度为单位进行解析的

typedef struct 
{
	unsigned char  grp; //组号
	unsigned char  num; //序号
	unsigned char  sid[16]; //编号名称 ,用来形成数据库记录
	unsigned char * subFld; //是否有子序号 当存在子序号时,父序号将不进行显示了
}TFldID;

typedef struct 
{
	unsigned char  type; //数据类型 同IEC103规定
	unsigned char  size; //数据宽度
}TFldType;

typedef struct 
{
	unsigned char  pric; //数据精度 小数位数
	unsigned char  rate; //数据倍率
	unsigned char  unit[8];  //数据单位
	//数据库值状态
}TFldDefs;





//内部所有数据的数据字典
typedef  union 
{
	unsigned char  sval[32];//默认值 当赋值时
	unsigned long  lval;
	float		   fval;
	double		   dval;
	TFmt08         b1s32;
	TFmt09         b2s16;
}TFldValue;


typedef struct 
{
	 TFldID id;
	 TFldType type;
	 TFldValue val;
	//数据库值状态
}TField;

typedef struct 
{
	char sID[8];
	char sValue[16];
}TValue;
/*
typedef struct TField
{
 TFldID id;
 TFldType type;
 TFldValue val;
};*/
/*
//定义内部的数据字典,供前后台使用
//内部所有数据的数据字典
typedef  __packed struct
{
	TGIN  gin;  //1标识定义 1
	TGDD  gdd;  //3类型定义 1
	TGID  gid; //4数据值 4 
}TDICT; //本结构最大就6个字节
*/

#ifdef __cplusplus
extern "C" {
#endif 

//处理接收到短信内容,
//如果需要返回内容,就返回一串字符串,并返回一个大于0的值,否则就不返回
//返回值<=0 说明没有报文返回
	short sysDealRecvInfo(int inlen ,unsigned char * sRecv,TValue * outRecs);

	//处理下位机发送来的信息内容,根据帧格式确定需要后续处理 2h
	short sysDealFrameCtrl(TFrame * pFrame,char * sBuffer,int &curIdx);


	//处理具体业务内容,将每个item 分解到最小端,形成一个个完整的可见字符串,并进行保存 2h
	short sysDealAsduFunc(TASDU * pAsdu,char * sBuffer,int &curIdx);


	//针对bx,系统组织报文, 确认系统已经授权,回复确认状态, 1h
	short sysConfimGrantStats(unsigned char cmd,TASDU * pAsdu);	

	//将接收到的报文保存到数据库中,按照号码关联获得相关的参数新体系 2h
	short sysStore(char * telno,char recType,char * sBuff);

	//系统从数据库中读取记录,获得需要完成的参数 2h
	short sysGetCmd(char * cmdBuff);
	//下发参数,由系统将需要下发的参数,保存到 参数状态表中, 系统自动扫描每个参数,按照循序进行合成报文。

	//没有具体的子项了,这样简单多了


	//将每个完整的命令合成统一的报文ITEM,如果某个ITEM漏了,系统将用默认参数进行下发 3h
	short combinCmd2Item();




#ifdef __cplusplus
}
#endif
#endif //_IEC_SYSTEM_H

