// SmsPDU.h: interface for the CSmsPDU class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SMSPDU_H__536C1E67_8AF5_496F_ACC1_CFCB609AFDE0__INCLUDED_)
#define AFX_SMSPDU_H__536C1E67_8AF5_496F_ACC1_CFCB609AFDE0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#pragma pack(1)

// 用户信息编码方式
#define GSM_7BIT		0
#define GSM_8BIT		4
#define GSM_UCS2		8

// 应答状态
#define GSM_WAIT		0		// no data wait
#define GSM_R_WAIT		3		// hav data wait
#define GSM_READING		2		// 
#define GSM_OK			1		// OK
#define GSM_ERR			-1		// ERROR

#define SMS_PKG_MAXLEN 132

typedef struct 
{
	unsigned char len1;   //05 表示剩余协议头的长度
	unsigned char sze; //00  标识位pid 的长度
	unsigned char len2;   //03 剩余协议头的长度
	unsigned char batid;   //xx 这批短信的唯一标志
	unsigned char amt;   //xx 这批短信的数量
	unsigned char seq;  //序号 这批短信的数量
} TPDU_UDHI;
// 短消息参数结构，编码/解码共用
// 其中，字符串以'\0'结尾
typedef struct {
	char SCA[32];			// 短消息服务中心号码(SMSC地址)
	char TPA[32];			// 目标号码或回复号码(TP-DA或TP-RA)
	char TP_PID;			// 用户信息协议标识(TP-PID)
	char TP_DCS;			// 用户信息编码方式(TP-DCS)
	char TP_SCTS[16];		// 服务时间戳字符串(TP_SCTS), 接收时用到
	int  Len;
	TPDU_UDHI TP_UDHI;      // 长报文数据头
	char TP_UD[512];		// 原始用户信息(编码前或解码后的TP-UD)
	short index;			// 短消息序号，在读取时用到
} SM_PARAM;




// 读取应答的缓冲区
typedef struct {
	int len;
	char data[32768];
} SM_BUFF;

class CSmsPDU  
{
public:

	struct TSCA
	{
		unsigned char Len;
		unsigned char Type;
		unsigned char Addr[14];
	}; //短消息服务中心号码


	struct TDCS
	{
		unsigned char  CLS:2; //class 0:短消息直接显示到用户终端　1:存储在SIM卡上, 
		//2:必须存在SIM卡上禁止直接传到终端上，3:存储在用户设备上
		unsigned char  COD:2; //0 默认　1:8位　2:usc2 3:rsv
		unsigned char  BCLS:1; //0 CLS是否有效0: 1:
		unsigned char  EPR:1; //0 是否压缩
		unsigned char  RSV:2;
	};

	struct  TUD
	{
		unsigned char  UDL;     // 用户数据长度
		unsigned char  UD[140];	// 原始用户信息(编码前或解码后的TP-UD)
	};

	struct TSCTS
	{
		unsigned char data[7];

		void getReal();

		unsigned char getVFlen(unsigned char VPF)
		{
			switch(VPF)
			{
			case 0:
			case 1:
				return 0;
			case 2:
				return 1;
			case 3:
				return 7;
			}
		}
	};

	struct TRCV_PDU
	{
		struct TPDUTYPE
		{
			unsigned char  RP:1;        //应答路径　0:未设置 1:设置
			unsigned char  UDHI:1;      //用户数据头标示　0:不包含 1:包含
			unsigned char  SRI:1;       //状态报告指示
			unsigned char   RSV:2; //保留
			unsigned char   MMS:1; //更多信息
			unsigned char   MTI:2; //信息类型　
		}; //PDU 类型

		TSCA SCA;               // 短消息服务中心号码(SMSC地址)
		TSCA OA;                // 原地址
		TPDUTYPE TYPE;		    // 目标号码或回复号码(TP-DA或TP-RA)
		unsigned char PID;	    // 用户信息协议标识(TP-PID)
		TDCS DCS;			    // 用户信息编码方式(TP-DCS)
		TSCTS SCTS;	            // 服务时间戳字符串(TP_SCTS), 接收时用到
		TUD UD;     // 用户数据
	};

	struct TSND_PDU
	{
		struct TPDUTYPE
		{
			unsigned char  RP:1;        //应答路径　0:未设置 1:设置
			unsigned char  UDHI:1;      //用户数据头标示　0:不包含 1:包含
			unsigned char  SRR:1;       //请求状态报告指示
			unsigned char   VPF:2;      //有效格式 00:无VP段 01:保留 10:VP段以整型提供 11:VP以8位组的一半提供
			unsigned char   RD:1; //更多信息
			unsigned char   MTI:2; //信息类型　
		}; //PDU 类型

		TSCA SCA;               // 短消息服务中心号码(SMSC地址)
		TPDUTYPE TYPE;		    // 目标号码或回复号码(TP-DA或TP-RA)
		unsigned char MR;       // 默认00
		TSCA DA;                // 原地址
		unsigned char PID;	    // 用户信息协议标识(TP-PID)
		TDCS DCS;			    // 用户信息编码方式(TP-DCS)
		TSCTS VP;	            // 信息有效期
		TUD UD;					// 用户数据
		short index;			// 短消息序号，在读取时用到
	}; // 上述结构是用于理解协议的，具体的采用 SM_PARAM 直接进行操作
	//下述结构是用于报文合并使用的用户定义头




public:
	CSmsPDU();
	virtual ~CSmsPDU();

public:
	static int isUtf8 ;

	static int getEnvLang();
	// 可打印字符串转换为字节数据
	// 如："C8329BFD0E01" --> {0xC8, 0x32, 0x9B, 0xFD, 0x0E, 0x01}
	// 输入: pSrc - 源字符串指针
	//       nSrcLen - 源字符串长度
	// 输出: pDst - 目标数据指针
	// 返回: 目标数据长度
	static int String2Bytes(const char* pSrc, unsigned char* pDst, int nSrcLen);

	// 字节数据转换为可打印字符串
	// 如：{0xC8, 0x32, 0x9B, 0xFD, 0x0E, 0x01} --> "C8329BFD0E01" 
	// 输入: pSrc - 源数据指针
	//       nStrLen - 源数据长度
	// 输出: pDst - 目标字符串指针
	// 返回: 目标字符串长度
	static int Bytes2String(const unsigned char* pSrc, char* pDst, int nStrLen);


	//// 如：{0xC8, 0x32, 0x9B, 0xFD, 0x0E, 0x01} --> "C8329BFD0E01" 
	/*将源字符空间的所有内容转换未一个2倍长的字符串*/
	static char * AscToStr(char * Sour,int Sourlen, char * Targ);

	/*将字符中的所有字符转换为对应内存中的每一个ASCII码*/
	static char  * StrToAsc(char * Sour,int Sourlen, char * Targ);


	// 7bit编码
	// 输入: pSrc - 源字符串指针
	//       nStrLen - 源字符串长度
	// 输出: pDst - 目标编码串指针
	// 返回: 目标编码串长度
	static int Encode7bit(const char* pSrc, unsigned char* pDst, int nStrLen);


	// 7bit解码
	// 输入: pSrc - 源编码串指针
	//       nStrLen - 源编码串长度
	// 输出: pDst - 目标字符串指针
	// 返回: 目标字符串长度
	static  int Decode7bit(const unsigned char* pSrc, char* pDst, int nStrLen);

	// 8bit编码
	// 输入: pSrc - 源字符串指针
	//       nStrLen - 源字符串长度
	// 输出: pDst - 目标编码串指针
	// 返回: 目标编码串长度
	static int Encode8bit(const char* pSrc, unsigned char* pDst, int nStrLen);


	// 8bit解码
	// 输入: pSrc - 源编码串指针
	//       nStrLen -  源编码串长度
	// 输出: pDst -  目标字符串指针
	// 返回: 目标字符串长度
	static int Decode8bit(const unsigned char* pSrc, char* pDst, int nStrLen);

	// UCS2编码
	// 输入: pSrc - 源字符串指针
	//       nStrLen - 源字符串长度
	// 输出: pDst - 目标编码串指针
	// 返回: 目标编码串长度
	static int EncodeUcs2(const char* pSrc, unsigned char* pDst, int nStrLen);


	// UCS2解码
	// 输入: pSrc - 源编码串指针
	//       nStrLen -  源编码串长度
	// 输出: pDst -  目标字符串指针
	// 返回: 目标字符串长度
	static int DecodeUcs2(const unsigned char* pSrc, char* pDst, int nStrLen);


	// 两两颠倒的字符串转换为正常顺序的字符串
	// 如："683158812764F8" --> "8613851872468"
	// 输入: pSrc - 源字符串指针
	//       nStrLen - 源字符串长度
	// 输出: pDst - 目标字符串指针
	// 返回: 目标字符串长度
	static int SerializeNumbers(const char* pSrc, char* pDst, int nStrLen);


	// 正常顺序的字符串转换为两两颠倒的字符串，若长度为奇数，补'F'凑成偶数
	// 如："8613851872468" --> "683158812764F8"
	// 输入: pSrc - 源字符串指针
	//       nStrLen - 源字符串长度
	// 输出: pDst - 目标字符串指针
	// 返回: 目标字符串长度
	static int InvertNumbers(const char* pSrc, char* pDst, int nStrLen);


	// PDU编码，用于编制、发送短消息
	// 输入: pSrc - 源PDU参数指针
	// 输出: pDst - 目标PDU串指针
	// 返回: 目标PDU串长度
	static int EncodeFull(const SM_PARAM* pSrc, char* pDst);


	// PDU解码，用于接收、阅读短消息
	// 输入: pSrc - 源PDU串指针
	// 输出: pDst - 目标PDU参数指针
	// 返回: 用户信息串长度
	static int Decode(const char* pSrc, SM_PARAM* pDst);

	/* convert UTF-8 to unicode */
	static int utf82Unicode(unsigned char *ch, unsigned long *unicode);


	/* convert unicode to UTF-8 */
	static unsigned char *unicode2Utf8(unsigned long unicode, unsigned char *p);

	static int ucs2ToUtf8(int inlen,const unsigned short * ucsSrc,unsigned char *utf8Dst);
	static int utf8ToUcs2(unsigned char* utf8Src,unsigned short *ucsDst);


};

#pragma pack()

#endif // !defined(AFX_SMSPDU_H__536C1E67_8AF5_496F_ACC1_CFCB609AFDE0__INCLUDED_)
