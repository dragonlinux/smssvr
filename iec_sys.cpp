// ��ϵͳ���õı���,���ܵ�����Ϣת��Ϊ���ݿ�����
#include "stdafx.h"
#include "iec_sys.h"
#include <stdlib.h>
#include <time.h>
//�ͺ�̨��ͬ��,���ñ�ӿڷ�ʽ,ͨ������һ��״̬,����������ݿ��н������γ�һ������ַ���,ӳ�䵽�ڲ�����,
//Ȼ��ϳ��ۺϱ������·���
TField  _TAB_X1X[6] =
{
	{{0x1,0x01,"1.1.0",0},{7,12},{"13800591500"}},    //�������ĺ��� ��ʾ�ڻ�����_x1x ��0��ָ��λ�ÿ�ʼ ������12ȷ��
	{{0x1,0x02,"1.2.0",0},{7,12},{"13799416858"}},    //��վ���ź���1 ��ʾ�ڻ�����_x1x ��12��ָ��λ�ÿ�ʼ ������12ȷ��
	{{0x1,0x03,"1.3.0",0},{7,12},{"13799416858"}},    //��վ���ź���bak ��18��λ��
	{{0x1,0x04,"1.4.0",0},{7,12},{"15199416858"}},    //�������� ��32��λ��
	{{0x1,0x05,"1.5.0",0},{7,12},{"172.168.198.188:12345"}},    //GPRS��վ&�˿�
	{{0x1,0x06,"1.6.0",0},{7,12},{"192.168.198.188:12345"}},    //GPRS��վ&�˿� ����
};
//�ն����в���
TField  _TAB_X2X[6] =
{
	{{0x2,0x01,"2.1.0",0},{3,4},{0XFF,0XFF,0XFF,0XFF}},    //ͨѶά��ʱ�� 15��65535 //��
	{{0x2,0x02,"2.2.0",0},{1,4},{0X00}},    //ͨѶ�ռ��        
	{{0x2,0x03,"2.3.0",0},{1,4},{0X3008}},	 //����ʱ��� ÿ��08:{{0x2,0x03,"2.3.0",ADDR230},{7,8},{""}},        //tfmt07 ϵͳ�ڲ���ʱ,ǰ��4���ֽ�Ϊ��������м������ۼ��� �� ��4Ϊ Ϊϵͳ���͹�����������ʾ��׼��ʱ���ǩ,��������ʱ������ֵ
	{{0x2,0x04,"2.4.0",0},{1,4},{0XFF}},    //�ź�ȷ���� ���256      
	{{0x2,0x05,"2.5.0",0},{3,4},{0X00}},     // ��Ƭ����ǰ�ӻ�׼������?
	{{0x2,0x06,"2.6.0",0},{5,4},{0X08,0X30}},// ��׼ʱ��� YYMMDDHHMISS
};
//�ն����м�¼
TField  _TAB_X3X[6] =
{
	{{0x03,0x01,"3.1.0",0},{5,4},{0}}, //�״�Ͷ��ʱ�� pack_time ��ʽ,��4���ֽڱ��YYMMDDHHMISS
	{{0x03,0x02,"3.2.0",0},{5,4},{0}}, //�ϴ�����ʱ�� pack_time ��ʽ,��4���ֽڱ��YYMMDDHHMISS
	{{0x03,0x03,"3.3.0",0},{2,4},{0}}, //�ϴ�����ʱ�� ��
	{{0x03,0x03,"3.4.0",0},{2,4},{0}}, //�ϴ�����ʱ�� ��
	{{0x03,0x03,"3.5.0",0},{2,4},{0}}, //���ι���gprs����״̬  �յ��Ķ�������,���͵Ķ�������
	{{0x03,0x04,"3.6.0",0},{2,4},{0}}, //�ۼƸ�����״̬ 	       �յ��Ķ�������,���͵Ķ�������
};
//�ź�����Ҫ�ȼ�
TField  _TAB_X410_SUB[16] =
{
	{{0x00,0x01,"4.1.0",0},{9,2},{0}},   //һ�Ǵ�
	{{0x00,0x02,"4.1.1",0},{9,2},{0}},   //���࿪
	{{0x00,0x03,"4.1.2",0},{9,2},{0}},   //���Ǵ�
	{{0x00,0x04,"4.1.3",0},{9,2},{0}},   //��������
	{{0x00,0x05,"4.1.4",0},{9,2},{0}},   //ˮλ1Խ��
	{{0x00,0x06,"4.1.5",0},{9,2},{0}}, //�¶�1Խ��
	{{0x00,0x07,"4.1.6",0},{9,2},{0}}, //ˮλ2����
	{{0x00,0x08,"4.1.7",0},{9,2},{0}}, //�¶�2����
	{{0x00,0x09,"4.1.8",0},{9,2},{0}}, //����1Խ��
	{{0x00,0x0a,"4.1.9",0},{9,2},{0}}, //����2Խ��
	{{0x00,0x0b,"4.1.a",0},{9,2},{0}}, //����3Խ��
	{{0x00,0x0c,"4.1.b",0},{9,2},{0}}, //����4Խ��
	{{0x00,0x0d,"4.1.c",0},{9,2},{0}}, //�������
	{{0x00,0x0e,"4.1.d",0},{9,2},{0}}, //��Ȩ״̬
	{{0x00,0x0f,"4.1.e",0},{9,2},{0}}, //ϵͳ��Ȩ����
	{{0x00,0x10,"4.1.f",0},{9,2},{0}}, //�������ڿ���
};
//ģ��������Ҫ�ȼ�
TField  _TAB_X420_SUB[16] =
{
	{{0x00,0x01,"4.2.0",0},{8,2},{0}}, //��б�Ƕ�
	{{0x00,0x02,"4.2.1",0},{8,2},{0}}, //�������
	{{0x00,0x03,"4.2.2",0},{8,2},{0}}, //ˮλֵ
	{{0x00,0x04,"4.2.3",0},{8,2},{0}}, //�¶�ֵ
	{{0x00,0x05,"4.2.4",0},{8,2},{0}}, //��ص�ѹ
	{{0x00,0x06,"4.2.5",0},{8,2},{0}}, //����1
	{{0x00,0x07,"4.2.6",0},{8,2},{0}}, //����2
	{{0x00,0x08,"4.2.7",0},{8,2},{0}}, //����3
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
//״̬Ϊ�趨,�����ȼ��趨
TField  _TAB_X4X[2] =
{
	{{0x04,0x01,"4.1.0",ADDR410},{8,4},{0XFF,0XFF,0XFF,0XFF}}, // �ź�����Ҫ�ȼ� ��16��״̬,ÿ��λ����λ���,���ڲ�״̬��λʱ, 0:��ʾ�޴˹���,1:��ʾ��ͨ,��ʱ���� 2:��ʾΪ��Ҫ,��λʱ��Ҫ�������� 3:����
	{{0x04,0x02,"4.2.0",ADDR420},{8,4},{0XFF,0XFF,0XFF,0XFF}}, // �ɼ�����Ҫ�ȼ� ͬ��,�ɼ�ֵԽ��ʱ,��ʾ����״̬��λʱ
};
//�ⶨֵ�ı߽��趨ֵ
TField _TAB_X5X[9] =
{
	{{0x05,0x01,"5.2.0",0},{11,4},{0X10}}, 		// ��б�Ƕ�
	{{0x05,0x02,"5.2.0",0},{11,4},{0X10}}, 		// �������
	{{0x05,0x03,"5.3.0",0},{11,4},{0X10,0X20,0X30,0X40}}, // �¶�
	{{0x05,0x04,"5.4.0",0},{11,4},{0X10,0X20,0X30,0X40}}, // ˮλ
	{{0x05,0x05,"5.5.0",0},{11,4},{0X10,0X20,0X30,0X40}}, // ��ص�ѹ
	{{0x05,0x06,"5.6.0",0},{10,4},{0X10,0X20,0X30,0X40}}, // ����1
	{{0x05,0x07,"5.7.0",0},{10,4},{0X10,0X20,0X30,0X40}}, // ����2
	{{0x05,0x08,"5.8.0",0},{10,4},{0X10,0X20,0X30,0X40}}, // ����3
};
//����
//����״̬λ,Ϊ1��ʾ�����˸澯
TField _TAB_X8F0_SUB[8] =
{
	{{0x00, 0,"8.f.1", 0},{8,1},{0}}, //һ�Ǵ�
	{{0x00, 2,"8.f.2", 0},{8,1},{0}}, //���Ǵ�
	{{0x00, 3,"8.f.3", 0},{8,1},{0}}, //��������
	{{0x00, 4,"8.f.4", 0},{8,1},{0}}, //ˮλ1Խ��
	{{0x00, 5,"8.f.5", 0},{8,1},{0}}, //�¶�1Խ��
	{{0x00, 6,"8.f.6", 0},{8,1},{0}}, //ˮλ2����
	{{0x00, 7,"8.f.7", 0},{8,1},{0}}, //�¶�2����
	{{0x00, 8,"8.f.8", 0},{8,1},{0}}, //����1Խ��
/*	{{0x00, 9,"8.f.9", 0},{8,1},{0}}, //����2Խ��
	{{0x00,10,"8.f.a", 0},{8,1},{0}}, //����3Խ��
	{{0x00,11,"8.f.b", 0},{8,1},{0}}, //����4Խ��
	{{0x00,12,"8.f.c", 0},{8,1},{0}}, //�������
	{{0x00,13,"8.f.d", 0},{8,1},{0}}, //��Ȩ״̬
	{{0x00,14,"8.f.e", 0},{8,1},{0}}, //ϵͳ��Ȩ����
	{{0x00,15,"8.f.f", 0},{8,1},{0}}, //�������ڿ���
	{{0x00,16,"8.f.10",0},{8,1},{0}}, //��б�Ƕ�
	{{0x00,17,"8.f.11",0},{8,1},{0}}, //�������
	{{0x00,18,"8.f.12",0},{8,1},{0}}, //ˮλֵ
	{{0x00,19,"8.f.13",0},{8,1},{0}}, //�¶�ֵ
	{{0x00,20,"8.f.14",0},{8,1},{0}}, //��ص�ѹ
	{{0x00,21,"8.f.15",0},{8,1},{0}}, //����1
	{{0x00,22,"8.f.16",0},{8,1},{0}}, //����2
	{{0x00,23,"8.f.17",0},{8,1},{0}}, //����3
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
//�ⶨֵʵ��
TField _TAB_X8X[9] =
{
	{{0x08,0x01,"8.1.0",0},{3,4},{0}}, // ��б�Ƕ�
	{{0x08,0x02,"8.2.0",0},{3,4},{0}}, // �������
	{{0x08,0x03,"8.3.0",0},{3,4},{0}}, // �¶�
	{{0x08,0x04,"8.4.0",0},{3,4},{0}}, // ˮλ
	{{0x08,0x05,"8.5.0",0},{3,4},{0}}, // ��ص�ѹ
	{{0x08,0x06,"8.6.0",0},{3,4},{0}}, // ����1��ȼ
	{{0x08,0x07,"8.7.0",0},{3,4},{0}}, // ����2����
	{{0x08,0x08,"8.8.0",0},{3,4},{0}}, // ����3������̼
	{{0x08,0x0F,"8.f.0",ADDR8F0},{8,4},{0XFFFFFFFF}}, // ���幤��״̬ ��16���ź�״̬ 16���ɼ�״̬�ĵ�ǰ״̬,��λ���
};
TField _TAB_XAX[] =
{
	{{0x0A,0x01,"a.1.0",0},{5,4},{0}}, //��Ի�׼����ʱ��
	{{0x0A,0x02,"a.2.0",0},{1,4},{0}}, //��λ��־λ
	{{0x0A,0x03,"a.3.0",0},{3,4},{0}}, //��ֵ����
};
TField  _TAB_XB10_SUB[3] =
{
	{{0x0,0x01,"b.1.1",0},{1,12},{0X00}},//�ֻ�����
	{{0x0,0x02,"b.1.2",0},{3,4},{0}},		 //����ʱ��
	{{0x0,0x03,"b.1.3",0},{3,4},{0}},		 //�������
};
#define ADDRB10 (unsigned char *)&_TAB_XB10_SUB[0]
TField _TAB_XBX[9] =
{
	{{0x0B,0x01,"b.1.0",ADDRB10},{12,20},{0}}, //B1H ϵͳ��Ȩ
	{{0x0B,0x02,"b.2.0",ADDRB10},{12,20},{0}}, //B2H ����ϵͳ��Ȩ
	{{0x0B,0x03,"b.3.0",ADDRB10},{12,20},{0}}, //B3H ϵͳ��Ȩȷ��
	{{0x0B,0x04,"b.4.0",ADDRB10},{12,20},{0}}, //B4H ϵͳ�����Ȩ
	{{0x0B,0x05,"b.5.0",ADDRB10},{12,20},{0}}, //B5H �򾮸����뿪����Ȩ
	{{0x0B,0x06,"b.6.0",ADDRB10},{12,20},{0}}, //B6H ������Ȩȷ��
	{{0x0B,0x07,"b.7.0",ADDRB10},{12,20},{0}}, //B7H ����
	{{0x0B,0x08,"b.8.0",ADDRB10},{12,20},{0}}, //B8H ����
	{{0x0B,0x09,"b.9.0",ADDRB10},{12,20},{0}}, //B9H ִ��ȷ��???��Ҫ���µ�����ʽ
};
#define SZ(x) sizeof(x)/sizeof(TField)
TField * _TAB_PXX[8]={_TAB_X1X,_TAB_X2X,_TAB_X3X,_TAB_X4X,_TAB_X5X,_TAB_X8X,_TAB_XAX,_TAB_XBX};
unsigned char _TAB_NXX[8]={SZ(_TAB_X1X),SZ(_TAB_X2X),SZ(_TAB_X3X),SZ(_TAB_X4X),SZ(_TAB_X5X),SZ(_TAB_X8X),SZ(_TAB_XAX)}; //�ڲ������ݶ���ֵ
TField * _TAB_SUB_PXX[4]={_TAB_X410_SUB,_TAB_X420_SUB,_TAB_X8F0_SUB,_TAB_XB10_SUB};
unsigned char _TAB_SUB_NXX[8] = {SZ(_TAB_X410_SUB),SZ(_TAB_X420_SUB),SZ(_TAB_X8F0_SUB),SZ(_TAB_XB10_SUB)};
//ȫ�ֱ���
TFrame aTempFme = 
{
	{
		0,{
			0x68,0,0,0x68,{0x08},0x00
		},{""}
	},	0,0,0
};
//Ѱ��ָ����������ڴ�ӳ����е����
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
//������ź͵�ַѰ�ұ��
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
			return pFld; //���aGin.num ��ʾѰ�ҵ����ڲ�����,�����Ǿ�����ĸ�ֵ,�ʷ��ش�����׵�ַ
		for(i=0;i<icnt;i++)
		{
			if (aID.num == pFld->id.num) //���һ��,˵���ҵ���
				return pFld;
			pFld +=1;
		}
	}
	return 0;
}
//���ڴ������ݴ�����Ϊ�����ַ�,��ͨѶʹ��
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
	//���صĳ���Ϊȥ�� TGID ���Ⱥ�,TGID�ڵ�ʵ�����ݵĳ���
	return 	sizeof(TGIN) + iTmpLen;
}
//���ݲ����γ������,������������ȥ
//���������Ϊ�����б�
//���������ĸ�ʽ
//"8.1 13799416858"
//8.2 223
//8.4
//���÷ֺ���Ϊ�ָ����,��ð����Ϊǰ������
//short sysMake
//������Ʋ�������̷���
short sysDealFrameCtrl(TFrame * pFrame,char * sBuffer,int &curIdx)
{
	TASDU *	pAsdu;
	short iRet;
	unsigned char cFunc,cAsdulen;
	pAsdu =(TASDU *) pFrame->frame.fme.asduBuff;
	cAsdulen = pFrame->cAsduLen;
	//��ù�����
	cFunc = pFrame->frame.fme.pkghead._ctrl.down._func;
	//��ʱ�����Ǵ�������
	iRet =  sysDealAsduFunc(pAsdu,sBuffer,curIdx);
	//ֱ�ӻ�ȡ asdu type �;��庬��ֵ
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
//�����ֽ�ѭ��
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
//����λѭ��
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
//�������������� ���浽�ڴ��У��������õ������ݳ���
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
	//�ж��Ƿ������λ���������λ������Ͳ������� //��λ���������ȷ����
	if (pFld->id.subFld!=0)
	{
		if (pFld->type.type ==8) //Ϊλ����
		{
			sysSetDataBitItem((TField *)pFld->id.subFld,pSrc->data.lval,sBuffer,curIdx);
		}
		else //Ϊ�ֽڲ���
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
//����ֵ���ݶ����浽�ڲ�����
short sysSave2Table(TASDU * pAsdu,char * sBuffer,int &curIdx)
{
	short iAllLen=0,i,icnt,iItemLen=0;
	char cRet=1;
	TItem * pSrcItem;
	icnt = pAsdu->head._vsq.num;

	for(i=0;i<icnt;i++)
	{
		pSrcItem = (TItem*)((char *)&(pAsdu->aItem) +iAllLen);
		iItemLen = sysSetDataToDef(pSrcItem,sBuffer,curIdx)+sizeof(TGIN); //TItem��ͷ
		iAllLen += iItemLen;
	}
	return curIdx; //����ʵ�ʵĶ�����Ŀ
}
//����ҵ��ASDU���������func����
//���� ����� �� pAsdu ʹ�õĳ���
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
//������յ���������,
//�����Ҫ��������,�ͷ���һ���ַ���,������һ������0��ֵ,����Ͳ�����
//����ֵ<=0 ˵��û�б��ķ���
short sysDealRecvInfo(int inlen ,unsigned char * sRecv,TValue * outRecs)
{
	short i,iRet,ilen;
	TRecord * pRec;
	char sFrameBuff[1024];
	TValue ValueList[200];
	int curIdx=0,icnt,curval;
	TFrame aFrame = aTempFme;
	pRec = &aFrame.frame.rec;
	//���ɼ��ַ�ת��Ϊ���ɼ��ַ�          

	ilen  = ascToHex((unsigned char *)sRecv,(unsigned char *)sFrameBuff, strlen((char*)sRecv));  

	memcpy(pRec->buf,sFrameBuff,ilen);

	//memcpy(pRec->buf,sRecv,inlen);
	pRec->len = inlen;
	//�������͵�����
	parseIEC103(&aFrame);//����aRcvFme.rec->buf, aRcvFme.rec->Len
	//������վ����������Ϣ����

	//1.������������
	iRet = sysDealFrameCtrl(&aFrame,(char*)ValueList,curIdx);
	icnt = curIdx / sizeof(TValue);

	memcpy((char*)outRecs,(char*)ValueList,icnt*sizeof(TValue));
	return icnt;
}

