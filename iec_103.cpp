
#include "iec_103.h"
//准备报文
void initUFrame(TFrame * pFme)
{
	pFme->pCrc = 0;
	pFme->pCrc = 0;
	pFme->cAsduLen = 0;
	pFme->frame.fme.pkghead._begin =  0x68;
	pFme->frame.fme.pkghead._begin_cfm = 0x68;
}

//pFrame 内部的帧没有作初始化操作
void parseIEC103(TFrame * pFrame) //解析103规约,获得内部的数据值
{
	short inLen;
	unsigned char * pBuff;
	inLen = pFrame->frame.rec.len;
	pBuff = pFrame->frame.rec.buf;
	pFrame->pCrc = pBuff + inLen - 2;//crc
	pFrame->pEnd = pFrame->pCrc + 1;
	pFrame->cAsduLen = inLen - 8;
}

//将输入内容形成发送报文
short makeIEC103(TFrame *  pFrame) 
{
	unsigned char i,ilen = 0,cs = 0,*pStr;
	ilen = pFrame->cAsduLen;
	pFrame->pCrc = pFrame->frame.fme.asduBuff + ilen;
	pFrame->pEnd = pFrame->pCrc + 1;
	ilen += 2; //控制域+地址域 共两个字节
	pFrame->frame.fme.pkghead._len = ilen;
	pFrame->frame.fme.pkghead._len_cfm =  ilen;
	pStr = pFrame->frame.rec.buf+4; //68 len len 68 不参与crc的计算
	for(i=0;i<ilen;i++,pStr++)
	{
		cs += *pStr;
	}
	*(pFrame->pCrc)=cs;
	*(pFrame->pEnd)=0x16;
	ilen +=sizeof(TDLY_HEAD); // 加上 //68 len len 68 ctrl addr 的才长度
	pFrame->frame.rec.len = ilen;
	return 1;
}

//是否是Iec103报文格式
short  isIEC103(char * sBuff)
{
	unsigned char sFrameBuff[512];
	TDLY_HEAD * pHead;
	int ilen;

	//将可见字符转换为不可见字符                                     
	ilen  = ascToHex((unsigned char *)sBuff,sFrameBuff, strlen((char*)sBuff));  

	pHead = (TDLY_HEAD *)sFrameBuff;


	if ((pHead->_begin !=0x68)||(pHead->_begin_cfm !=0x68)||(pHead->_len!=pHead->_len_cfm))
		return 0;

	return 1;
}

//将可见字符转换为不可见字符
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

//将不可见字符转换为可见字符
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
