
#include "iec_103.h"
//׼������
void initUFrame(TFrame * pFme)
{
	pFme->pCrc = 0;
	pFme->pCrc = 0;
	pFme->cAsduLen = 0;
	pFme->frame.fme.pkghead._begin =  0x68;
	pFme->frame.fme.pkghead._begin_cfm = 0x68;
}

//pFrame �ڲ���֡û������ʼ������
void parseIEC103(TFrame * pFrame) //����103��Լ,����ڲ�������ֵ
{
	short inLen;
	unsigned char * pBuff;
	inLen = pFrame->frame.rec.len;
	pBuff = pFrame->frame.rec.buf;
	pFrame->pCrc = pBuff + inLen - 2;//crc
	pFrame->pEnd = pFrame->pCrc + 1;
	pFrame->cAsduLen = inLen - 8;
}

//�����������γɷ��ͱ���
short makeIEC103(TFrame *  pFrame) 
{
	unsigned char i,ilen = 0,cs = 0,*pStr;
	ilen = pFrame->cAsduLen;
	pFrame->pCrc = pFrame->frame.fme.asduBuff + ilen;
	pFrame->pEnd = pFrame->pCrc + 1;
	ilen += 2; //������+��ַ�� �������ֽ�
	pFrame->frame.fme.pkghead._len = ilen;
	pFrame->frame.fme.pkghead._len_cfm =  ilen;
	pStr = pFrame->frame.rec.buf+4; //68 len len 68 ������crc�ļ���
	for(i=0;i<ilen;i++,pStr++)
	{
		cs += *pStr;
	}
	*(pFrame->pCrc)=cs;
	*(pFrame->pEnd)=0x16;
	ilen +=sizeof(TDLY_HEAD); // ���� //68 len len 68 ctrl addr �Ĳų���
	pFrame->frame.rec.len = ilen;
	return 1;
}

//�Ƿ���Iec103���ĸ�ʽ
short  isIEC103(char * sBuff)
{
	unsigned char sFrameBuff[512];
	TDLY_HEAD * pHead;
	int ilen;

	//���ɼ��ַ�ת��Ϊ���ɼ��ַ�                                     
	ilen  = ascToHex((unsigned char *)sBuff,sFrameBuff, strlen((char*)sBuff));  

	pHead = (TDLY_HEAD *)sFrameBuff;


	if ((pHead->_begin !=0x68)||(pHead->_begin_cfm !=0x68)||(pHead->_len!=pHead->_len_cfm))
		return 0;

	return 1;
}

//���ɼ��ַ�ת��Ϊ���ɼ��ַ�
short ascToHex(unsigned char * asc, unsigned char * hex, int asclen)
{
	short i;
	char base_char;
	short len = asclen;
	if(len%2) return -1;
	for(i=0; i<len; i++)
	{
		if( asc[i]>='0' && asc[i]<='9' ) base_char='0';
		else if(asc[i]>='A' && asc[i]<='F' ) base_char='A'-10;
		else if(asc[i]>='a' && asc[i]<='f' ) base_char='a'-10;
		else return -1;
		if( !(i%2) ) hex[i/2]=(unsigned char)(asc[i]-base_char)<<4; /*High*/
		else hex[i/2]|=(unsigned char)(asc[i]-base_char); /*Low*/
	}
	return asclen>>1;
}

//�����ɼ��ַ�ת��Ϊ�ɼ��ַ�
short hexToAsc(unsigned char * hex, unsigned char * asc, int hexlen)
{
	short i;
	short len = hexlen;
	for(i=0; i<len; i++)
	{
		sprintf((char *)(asc+2*i), "%02X", *(hex+i));
	}
	return len<<1;
}
