//iec_sys.h
//�豸��

#ifndef _IEC_SYSTEM_H
#define _IEC_SYSTEM_H
#pragma pack(1) 
#include "iec_def.h"
#include "iec_103.h" //��Լ

//���˶��д��ڴ���copy������,�ڴ�����½��д���,ȷ�������޸����ڲ�������,ֻ���޸ĸ���������,
//��������Ϊһ�������������ݽ�����װ,���ս�������ȫ�������
//��������������
//���ı������Եݹ����Ϊ��λ���н�����

typedef struct 
{
	unsigned char  grp; //���
	unsigned char  num; //���
	unsigned char  sid[16]; //������� ,�����γ����ݿ��¼
	unsigned char * subFld; //�Ƿ�������� �����������ʱ,����Ž���������ʾ��
}TFldID;

typedef struct 
{
	unsigned char  type; //�������� ͬIEC103�涨
	unsigned char  size; //���ݿ��
}TFldType;

typedef struct 
{
	unsigned char  pric; //���ݾ��� С��λ��
	unsigned char  rate; //���ݱ���
	unsigned char  unit[8];  //���ݵ�λ
	//���ݿ�ֵ״̬
}TFldDefs;





//�ڲ��������ݵ������ֵ�
typedef  union 
{
	unsigned char  sval[32];//Ĭ��ֵ ����ֵʱ
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
	//���ݿ�ֵ״̬
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
//�����ڲ��������ֵ�,��ǰ��̨ʹ��
//�ڲ��������ݵ������ֵ�
typedef  __packed struct
{
	TGIN  gin;  //1��ʶ���� 1
	TGDD  gdd;  //3���Ͷ��� 1
	TGID  gid; //4����ֵ 4 
}TDICT; //���ṹ����6���ֽ�
*/

#ifdef __cplusplus
extern "C" {
#endif 

//������յ���������,
//�����Ҫ��������,�ͷ���һ���ַ���,������һ������0��ֵ,����Ͳ�����
//����ֵ<=0 ˵��û�б��ķ���
	short sysDealRecvInfo(int inlen ,unsigned char * sRecv,TValue * outRecs);

	//������λ������������Ϣ����,����֡��ʽȷ����Ҫ�������� 2h
	short sysDealFrameCtrl(TFrame * pFrame,char * sBuffer,int &curIdx);


	//�������ҵ������,��ÿ��item �ֽ⵽��С��,�γ�һ���������Ŀɼ��ַ���,�����б��� 2h
	short sysDealAsduFunc(TASDU * pAsdu,char * sBuffer,int &curIdx);


	//���bx,ϵͳ��֯����, ȷ��ϵͳ�Ѿ���Ȩ,�ظ�ȷ��״̬, 1h
	short sysConfimGrantStats(unsigned char cmd,TASDU * pAsdu);	

	//�����յ��ı��ı��浽���ݿ���,���պ�����������صĲ�������ϵ 2h
	short sysStore(char * telno,char recType,char * sBuff);

	//ϵͳ�����ݿ��ж�ȡ��¼,�����Ҫ��ɵĲ��� 2h
	short sysGetCmd(char * cmdBuff);
	//�·�����,��ϵͳ����Ҫ�·��Ĳ���,���浽 ����״̬����, ϵͳ�Զ�ɨ��ÿ������,����ѭ����кϳɱ��ġ�

	//û�о����������,�����򵥶���


	//��ÿ������������ϳ�ͳһ�ı���ITEM,���ĳ��ITEM©��,ϵͳ����Ĭ�ϲ��������·� 3h
	short combinCmd2Item();




#ifdef __cplusplus
}
#endif
#endif //_IEC_SYSTEM_H

