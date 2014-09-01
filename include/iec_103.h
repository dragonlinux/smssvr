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
	}TFmt08; //��ʽ06	


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
	}TFmt09; //��ʽ06		


	//�����ʽ����	
	typedef __packed struct  //����	�·�
	{
		BYTE _func:4;// ԭ�������Ƕ�������з��鴦��,���ڶ��������Ԥ��׼�������ڹ���׼��
		BYTE _fcv:1; //֡������ЧλFCV
		//FCV=1����ʾFCBλ��Ч��FCV=0����ʾFCBλ��Ч

		BYTE _fcb:1; //֡����λFCB
		//��֡������ЧλFCV=1ʱ,FCB��ʾÿ��վ�����ķ���/ȷ�ϻ�������/��Ӧ����ı仯λ��
		//FCBλ������ֹ��Ϣ����Ķ�ʧ���ظ�

		BYTE _prm:1; //������־λPRM
		//PRM =1����ʾ��֡������������վ��PRM =0����ʾ��֡�������ԴӶ�վ

		BYTE _dir:1; //���䷽��λDIR DIR=0����ʾ��֡����������վ���������б��ģ� 
		//DIR=1����ʾ��֡���������ն˷��������б���
	}TCTRL_DOWN;

	typedef __packed struct  //����	 �ϴ�
	{
		BYTE _func:4; //1:����
		BYTE _dfc:1; //DFC(����������λ)�� DFC=0��ʾ��վ���Խ�������,
		//DF =1��ʾ��վ����������,�޷����������ݡ�
		//����Լ�涨(SF6)��վ���ͱ�����DFCʼ��Ϊ0,�����Խ���������,��վӦ�ܹ�����DFC=1�ı���

		BYTE _acd:1; //�������λACD
		//ACDλ����������Ӧ������.ACD=1 ��ʾ�ն�����Ҫ�¼��ȴ�����
		//ACD=0��ʾ�ն����¼����ݵȴ�����.

		BYTE _prm:1;
		//PRM =1����ʾ��֡������������վ��PRM =0����ʾ��֡�������ԴӶ�վ
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
	}TDLY_HEAD,PDLY_HEAD; //68 �ɱ�


	typedef __packed struct 
	{
		BYTE num:7;
		BYTE sq:1; //sq = 1
		//SQ = 1,��Ϣ����ֻ������Ԫ�ػ��ۺ���ϢԪ,��Ϣ���ַ(FUN��INF)����ϢԪ��ַ��
		//SQ = 0,��Ϣ���ַ��˳�����е���ϢԪ�صĵ�һ����ϢԪ��ַ,������ϢԪ��ַ�������ַ��˳���1��
	}TVSQ;

	typedef  __packed struct 
	{
		BYTE _typ; //1���ͱ�ʶ
		TVSQ _vsq; //2�ɱ�ṹ�޶���
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
		BYTE * pCrc; //У����
		BYTE * pEnd; //������
	}TFrame,*PFrame;


	//�ڲ����ݸ�ʽ����

	typedef __packed struct 
	{
		BYTE grp:4;  //���
		BYTE num:4;  //���
	}TGIN,*PGIN;


	typedef __packed  struct
	{
		BYTE type:4;  //�������� 0:����,1:����,2:�ַ���,3:λ,4:�ṹ��
		BYTE size:4;  //���ݿ�� //��� 32 ���ֽ�
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



	//�ڲ��������ݵ������ֵ�
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

	//�ڲ��������ݵ������ֵ�
	typedef  __packed struct
	{
		TGIN  def;  //1��ʶ���� 1
		TGDD  typ;  //3���Ͷ��� 1
		TGID  val; //4����ֵ 4 
	}TDICT; //���ṹ����6���ֽ�



	typedef  unsigned char TFmt01;
	typedef  unsigned short TFmt02;	
	typedef  unsigned long TFmt03;	
	typedef  float TFmt04;


	//string; 6


	//��4���ֽ�
	typedef __packed struct 
	{
		BYTE sec:6;							//64 ������
		BYTE min:6 ;						//64 ������
		BYTE hour:5;						//24 ������
		BYTE day:5;							//32 λ
		BYTE mon:4;			        //12 ��λ��
		BYTE year:6;						//��λ �� 00��64 ,���С��2064��
	}TPK_DATETIME,*PPK_DATETIME; //ѹ��ʱ���ʽ ���ĸ��ֽ� YYYYMMDDMISS


	typedef TGID TFmt11;


	//�¼� ״̬��λ�澯
	typedef  __packed struct
	{
		TGIN   HA1;   //�̶�Ϊ A1H 
		unsigned long occTime;   //����ʱ��  4 ��̨��ʱ��
		TGIN   HA2;   //�̶�Ϊ A2H 
		unsigned char occSite;   //��λλ��  1 ��̨���������֪���Ƿ�����ʲô�仯
		TGIN   H8F;   //�̶�Ϊ 08FH
		TFmt08 curStatu; //��ǰ״̬  4
		TGIN   dataID;   //�������ݱ�� 2 //���Ϊ00 ��ʾ��Ч
		TFmt03 curVal ;  //��ǰ�澯ֵ 4  �澯������,ͨ��curStatu �ĸ澯λAI����ܹ�����
	}TFmt0F; //��ʽ12 �� 15���ֽ�

	typedef  TFmt0F TEvent;

	//�ڲ��������ݵ������ֵ�
	typedef  __packed struct
	{
		TGIN  def;  //1��ʶ���� 1
		TGID  data;  //4����ֵ 4 
	}TItem; //���ṹ����6���ֽ�

	//���ֻ����20��������
	typedef __packed struct
	{
		TASDU_HEAD head;
		TItem  aItem;
	}TASDU,*PASDU;



#ifdef WIN32	
#pragma pack()
#endif	
	//} //namespace pub
	//׼������
	void initUFrame(TFrame * pFme);


	//�Ƿ���Iec103���ĸ�ʽ
	short isIEC103(char * sBuff);

	void parseIEC103(TFrame * pFrame); //����103��Լ,����ڲ�������ֵ
	short makeIEC103(TFrame *  pFrame); //�����������γɷ��ͱ���

	//�����ɼ��ַ�ת��Ϊ�ɼ��ַ�                                     
	short hexToAsc(unsigned char * hex, unsigned char * asc, int hexlen);         

	//���ɼ��ַ�ת��Ϊ���ɼ��ַ�                                     
	short ascToHex(unsigned char * asc,unsigned char * hex, int asclen);

#ifdef __cplusplus
}
#endif
#endif //_IEC_103_H
