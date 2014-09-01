//iec_def.h
#ifndef _IEC_DEF_H
#define _IEC_DEF_H

//#ifdef WIN32
//#define code 
//#define xdata
//#define data
//#define idata
//#endif 

/*
//#define ARM_PACK __packed ;//具体由语言体系来确定
#define __packed;
*/

#define  ARM_PACK
#ifndef __packed
#define __packed
#endif
//#define  ARM_PACK __packed  


#if !defined(BYTE)
typedef  unsigned char BYTE,*PBYTE ;
#endif // BYTE 

//typedef  unsigned char byte,*pbyte ;
typedef  unsigned short ushort;
typedef  unsigned short uchar ;
typedef  unsigned short word ;

//#define byte unsigned char;
#if !defined(WORD)
typedef  unsigned short WORD;
#endif // WORD 

#if !defined(LONG)
typedef  long  LONG;
#endif // LONG 


//#define USE_IN_SYS  //在系统中使用
//#define USE_IN_JSX  //在解锁箱中使用
#define USE_IN_WELL //在井盖中使用

#ifdef USE_IN_SYS  //确定只能用在系统中

#undef USE_IN_JSX
#undef USE_IN_WELL

#endif

#ifdef USE_IN_JSX  //确定只能用在解锁箱中

#undef USE_IN_SYS
#undef USE_IN_WELL

#endif

#ifdef USE_IN_WELL //确定只能用在井盖中

#undef USE_IN_SYS
#undef USE_IN_JSX

#endif


typedef __packed struct 
{
	unsigned char len;
	unsigned char buf[180]; //记录所指向的内存 //每条报文 最大的长度为47 才能保证缓存5条记录
}TRecord;

#endif //AFX_IEC_PUBLIC_DEF_INCLUDED_
