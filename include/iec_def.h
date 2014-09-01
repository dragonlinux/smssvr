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
//#define ARM_PACK __packed ;//������������ϵ��ȷ��
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


//#define USE_IN_SYS  //��ϵͳ��ʹ��
//#define USE_IN_JSX  //�ڽ�������ʹ��
#define USE_IN_WELL //�ھ�����ʹ��

#ifdef USE_IN_SYS  //ȷ��ֻ������ϵͳ��

#undef USE_IN_JSX
#undef USE_IN_WELL

#endif

#ifdef USE_IN_JSX  //ȷ��ֻ�����ڽ�������

#undef USE_IN_SYS
#undef USE_IN_WELL

#endif

#ifdef USE_IN_WELL //ȷ��ֻ�����ھ�����

#undef USE_IN_SYS
#undef USE_IN_JSX

#endif


typedef __packed struct 
{
	unsigned char len;
	unsigned char buf[180]; //��¼��ָ����ڴ� //ÿ������ ���ĳ���Ϊ47 ���ܱ�֤����5����¼
}TRecord;

#endif //AFX_IEC_PUBLIC_DEF_INCLUDED_
