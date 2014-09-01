// SmsPDU.cpp: implementation of the CSmsPDU class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SmsPDU.h"
#include "UnicodeGbChanger.h"
#include "LoggerTask.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

int CSmsPDU::isUtf8 = 0;

CSmsPDU::CSmsPDU()
{

}

CSmsPDU::~CSmsPDU()
{

}

int CSmsPDU::getEnvLang()
{
	char * e=getenv("LANG");
	if (strstr(e,"utf"))
		isUtf8 = 1;
	return 0;
}


/* convert unicode to UTF-8 */
unsigned char *CSmsPDU::unicode2Utf8(unsigned long unicode, unsigned char *p)
{
	unsigned char *e = NULL;

	if((e = p))
	{
		if(unicode < 0x80) *e++ = unicode;
		else if(unicode < 0x800)
		{
			/*<11011111> < 000 0000 0000>*/
			*e++ = ((unicode >> 6) & 0x1f)|0xc0;
			*e++ = (unicode & 0x3f)|0x80; 
		}
		else if(unicode < 0x10000)
		{
			/*<11101111> <0000 0000 0000 0000>*/
			*e++ = ((unicode >> 12) & 0x0f)|0xe0; 
			*e++ = ((unicode >> 6) & 0x3f)|0x80;
			*e++ = (unicode & 0x3f)|0x80; 
		}
		else if(unicode < 0x200000)
		{
			/*<11110111> <0 0000 0000 0000 0000 0000>*/
			*e++ = ((unicode >> 18) & 0x07)|0xf0; 
			*e++ = ((unicode >> 12) & 0x3f)|0x80;
			*e++ = ((unicode >> 6) & 0x3f)|0x80;
			*e++ = (unicode & 0x3f)|0x80; 
		}
		else if(unicode < 0x4000000)
		{
			/*<11111011> <00 0000 0000 0000 0000 0000 0000>*/
			*e++ = ((unicode >> 24) & 0x03)|0xf8 ; 
			*e++ = ((unicode >> 18) & 0x3f)|0x80;
			*e++ = ((unicode >> 12) & 0x3f)|0x80;
			*e++ = ((unicode >> 6) & 0x3f)|0x80;
			*e++ = (unicode & 0x3f)|0x80; 
		}
		else
		{
			/*<11111101> <0000 0000 0000 0000 0000 0000 0000 0000>*/
			*e++ = ((unicode >> 30) & 0x01)|0xfc; 
			*e++ = ((unicode >> 24) & 0x3f)|0x80;
			*e++ = ((unicode >> 18) & 0x3f)|0x80;
			*e++ = ((unicode >> 12) & 0x3f)|0x80;
			*e++ = ((unicode >> 6) & 0x3f)|0x80;
			*e++ = (unicode & 0x3f)|0x80; 
		}
	}
	return e;
}

/* convert UTF-8 to unicode */
int CSmsPDU::utf82Unicode(unsigned char *ch, unsigned long *unicode)
{
	unsigned char *p = NULL;
	int e = 0, n = 0;

	if((p = ch) && unicode)
	{
		if(*p >= 0xfc)
		{
			/*6:<11111100>*/
			e = (p[0] & 0x01) << 30;
			e |= (p[1] & 0x3f) << 24;
			e |= (p[2] & 0x3f) << 18;
			e |= (p[3] & 0x3f) << 12;
			e |= (p[4] & 0x3f) << 6;
			e |= (p[5] & 0x3f);
			n = 6;
		}
		else if(*p >= 0xf8) 
		{
			/*5:<11111000>*/
			e = (p[0] & 0x03) << 24;
			e |= (p[1] & 0x3f) << 18;
			e |= (p[2] & 0x3f) << 12;
			e |= (p[3] & 0x3f) << 6;
			e |= (p[4] & 0x3f);
			n = 5;
		}
		else if(*p >= 0xf0)
		{
			/*4:<11110000>*/
			e = (p[0] & 0x07) << 18;
			e |= (p[1] & 0x3f) << 12;
			e |= (p[2] & 0x3f) << 6;
			e |= (p[3] & 0x3f);
			n = 4;
		}
		else if(*p >= 0xe0)
		{
			/*3:<11100000>*/
			e = (p[0] & 0x0f) << 12;
			e |= (p[1] & 0x3f) << 6;
			e |= (p[2] & 0x3f);
			n = 3;
		}
		else if(*p >= 0xc0) 
		{
			/*2:<11000000>*/
			e = (p[0] & 0x1f) << 6;
			e |= (p[1] & 0x3f);
			n = 2;
		}
		else 
		{
			e = p[0];
			n = 1;
		}
		*unicode = e;
	}
	return n;
}


int CSmsPDU::ucs2ToUtf8(int inlen,const unsigned short * ucsSrc,unsigned char *utf8Dst)
{
    int i,n,ilen,k=0; 
    unsigned long unicode = 0;
    unsigned char * pstr;	

    ilen = inlen;//strlen((char*)(ucsSrc)) / 2;
    pstr = utf8Dst;

    for(i=0;i<ilen;i++)
    {
	unicode= CUnicodeGbChanger::htons(ucsSrc[i]);
	//unicode = ucsSrc[i];
	pstr = unicode2Utf8(unicode, pstr );
//	printf("%*.*s = unicode[%X]\n",3,3,pstr-3,unicode);
    }

    return strlen((char*)utf8Dst);
}

int CSmsPDU::utf8ToUcs2(unsigned char* utf8Src,unsigned short *ucsDst)
{
    int i,n,ilen,k=0; 
    unsigned long unicode = 0;	

    ilen = strlen((char*)utf8Src);

    for(i=0;i<ilen;)
    {
	n = utf82Unicode(utf8Src+i, &unicode);

//hi to low
	ucsDst[k]= CUnicodeGbChanger::htons(unicode);

//	ucsDst[k]=unicode;
//	printf("%*.*s = unicode[%X]\n",3,3,(utf8Src+i),ucsDst[k]);
	i +=n;
        k ++;
    }

    return k;
}

// 可打印字符串转换为字节数据
// 如："C8329BFD0E01" --> {0xC8, 0x32, 0x9B, 0xFD, 0x0E, 0x01}
// 输入: pSrc - 源字符串指针
//       nSrcLen - 源字符串长度
// 输出: pDst - 目标数据指针
// 返回: 目标数据长度
int CSmsPDU::String2Bytes(const char* pSrc, unsigned char* pDst, int nSrcLen)
{
	for (int i = 0; i < nSrcLen; i += 2)
	{
		// 输出高4位
		if ((*pSrc >= '0') && (*pSrc <= '9'))
		{
			*pDst = (*pSrc - '0') << 4;
		}
		else
		{
			*pDst = (*pSrc - 'A' + 10) << 4;
		}

		pSrc++;

		// 输出低4位
		if ((*pSrc>='0') && (*pSrc<='9'))
		{
			*pDst |= *pSrc - '0';
		}
		else
		{
			*pDst |= *pSrc - 'A' + 10;
		}

		pSrc++;
		pDst++;
	}

	// 返回目标数据长度
	return (nSrcLen / 2);
}

// 字节数据转换为可打印字符串
// 如：{0xC8, 0x32, 0x9B, 0xFD, 0x0E, 0x01} --> "C8329BFD0E01" 
// 输入: pSrc - 源数据指针
//       nStrLen - 源数据长度
// 输出: pDst - 目标字符串指针
// 返回: 目标字符串长度
int CSmsPDU::Bytes2String(const unsigned char* pSrc, char* pDst, int nStrLen)
{
	const char tab[]="0123456789ABCDEF";	// 0x0-0xf的字符查找表

	for (int i = 0; i < nStrLen; i++)
	{
		*pDst++ = tab[*pSrc >> 4];		// 输出高4位
		*pDst++ = tab[*pSrc & 0x0f];	// 输出低4位
		pSrc++;
	}

	// 输出字符串加个结束符
	*pDst = '\0';

	// 返回目标字符串长度
	return (nStrLen * 2);
}


//// 如：{0xC8, 0x32, 0x9B, 0xFD, 0x0E, 0x01} --> "C8329BFD0E01" 
/*将源字符空间的所有内容转换未一个2倍长的字符串*/
char *	CSmsPDU::AscToStr(char * Sour,int Sourlen, char * Targ)
{
	int	i;
	char *	Pstr;
	char	lcAsc[] ="0123456789ABCDEF";

	Pstr=(char *)Sour;
	Targ[0]='\0';

	for(i=0; i<Sourlen; i++)
	{
		Targ[2*i]=lcAsc[(Pstr[i]>>4)&0x0f];
		Targ[2*i+1]=lcAsc[Pstr[i]&0x0f];
	}

	Targ[2*i] = '\0';
	return Targ;
}

/*将字符中的所有字符转换为对应内存中的每一个ASCII码*/
char  * CSmsPDU::StrToAsc(char * Sour,int Sourlen, char * Targ)
{
	int	i;
	int	liTemp;
	char	lcTemp;

	liTemp = Sourlen;	
	for(i=0; i<liTemp; i++)
	{
		if((Sour[i] >= '0') && (Sour[i] <= '9')) 	lcTemp = Sour[i]-'0';
		else if((Sour[i] >= 'A') && (Sour[i] <= 'F')) 	lcTemp = Sour[i] - 'A' + 0x0A;
		else if((Sour[i] >= 'a') && (Sour[i] <= 'f')) 	lcTemp = Sour[i] - 'a' + 0x0A;

		if(i%2==0) 	Targ[i/2] = (lcTemp)<<4;
		else 		Targ[i/2] += (lcTemp); 
	}
	return Targ;
}

// 7bit编码
// 输入: pSrc - 源字符串指针
//       nStrLen - 源字符串长度
// 输出: pDst - 目标编码串指针
// 返回: 目标编码串长度
int CSmsPDU::Encode7bit(const char* pSrc, unsigned char* pDst, int nStrLen)
{
	int nSrc;		// 源字符串的计数值
	int nDst;		// 目标编码串的计数值
	int nChar;		// 当前正在处理的组内字符字节的序号，范围是0-7
	unsigned char nLeft;	// 上一字节残余的数据

	// 计数值初始化
	nSrc = 0;
	nDst = 0;

	// 将源串每8个字节分为一组，压缩成7个字节
	// 循环该处理过程，直至源串被处理完
	// 如果分组不到8字节，也能正确处理
	while (nSrc < nStrLen)
	{
		// 取源字符串的计数值的最低3位
		nChar = nSrc & 7;

		// 处理源串的每个字节
		if(nChar == 0)
		{
			// 组内第一个字节，只是保存起来，待处理下一个字节时使用
			nLeft = *pSrc;
		}
		else
		{
			// 组内其它字节，将其右边部分与残余数据相加，得到一个目标编码字节
			*pDst = (*pSrc << (8-nChar)) | nLeft;

			// 将该字节剩下的左边部分，作为残余数据保存起来
			nLeft = *pSrc >> nChar;

			// 修改目标串的指针和计数值
			pDst++;
			nDst++;
		}

		// 修改源串的指针和计数值
		pSrc++;
		nSrc++;
	}

	// 返回目标串长度
	return nDst;
}

// 7bit解码
// 输入: pSrc - 源编码串指针
//       nStrLen - 源编码串长度
// 输出: pDst - 目标字符串指针
// 返回: 目标字符串长度
int CSmsPDU::Decode7bit(const unsigned char* pSrc, char* pDst, int nStrLen)
{
	int nSrc;		// 源字符串的计数值
	int nDst;		// 目标解码串的计数值
	int nByte;		// 当前正在处理的组内字节的序号，范围是0-6
	unsigned char nLeft;	// 上一字节残余的数据

	// 计数值初始化
	nSrc = 0;
	nDst = 0;

	// 组内字节序号和残余数据初始化
	nByte = 0;
	nLeft = 0;

	// 将源数据每7个字节分为一组，解压缩成8个字节
	// 循环该处理过程，直至源数据被处理完
	// 如果分组不到7字节，也能正确处理
	while(nSrc<nStrLen)
	{
		// 将源字节右边部分与残余数据相加，去掉最高位，得到一个目标解码字节
		*pDst = ((*pSrc << nByte) | nLeft) & 0x7f;

		// 将该字节剩下的左边部分，作为残余数据保存起来
		nLeft = *pSrc >> (7-nByte);

		// 修改目标串的指针和计数值
		pDst++;
		nDst++;

		// 修改字节计数值
		nByte++;

		// 到了一组的最后一个字节
		if(nByte == 7)
		{
			// 额外得到一个目标解码字节
			*pDst = nLeft;

			// 修改目标串的指针和计数值
			pDst++;
			nDst++;

			// 组内字节序号和残余数据初始化
			nByte = 0;
			nLeft = 0;
		}

		// 修改源串的指针和计数值
		pSrc++;
		nSrc++;
	}

	// 输出字符串加个结束符
	*pDst = '\0';

	// 返回目标串长度
	return nDst;
}

// 8bit编码
// 输入: pSrc - 源字符串指针
//       nStrLen - 源字符串长度
// 输出: pDst - 目标编码串指针
// 返回: 目标编码串长度
int CSmsPDU::Encode8bit(const char* pSrc, unsigned char* pDst, int nStrLen)
{
	// 简单复制
	memcpy(pDst, pSrc, nStrLen);

	return nStrLen;
}

// 8bit解码
// 输入: pSrc - 源编码串指针
//       nStrLen -  源编码串长度
// 输出: pDst -  目标字符串指针
// 返回: 目标字符串长度
int CSmsPDU::Decode8bit(const unsigned char* pSrc, char* pDst, int nStrLen)
{
	// 简单复制
	memcpy(pDst, pSrc, nStrLen);

	// 输出字符串加个结束符
	*pDst = '\0';

	return nStrLen;
}

// UCS2编码 GBK 
// 输入: pSrc - 源字符串指针
//       nStrLen - 源字符串长度
// 输出: pDst - 目标编码串指针
// 返回: 目标编码串长度
int CSmsPDU::EncodeUcs2(const char* pSrc, unsigned char* pDst, int nStrLen)
{
	int nDstLength;		// UNICODE宽字符数目
	unsigned short wchar[1024];	// UNICODE串缓冲区128--->1280

	// 字符串-->UNICODE串
	nDstLength = CUnicodeGbChanger::MultiByteToWideChar(0, 0, pSrc, nStrLen, wchar, 1024);

	// 高低字节对调，输出
	for(int i=0; i<nDstLength; i++)
	{
		*pDst++ = wchar[i] >> 8;		// 先输出高位字节
		*pDst++ = wchar[i] & 0xff;		// 后输出低位字节
	}

	// 返回目标编码串长度
	return nDstLength * 2;
}

// UCS2解码 UNCODE->GBK
// 输入: pSrc - 源编码串指针
//       nStrLen -  源编码串长度
// 输出: pDst -  目标字符串指针
// 返回: 目标字符串长度 
int CSmsPDU::DecodeUcs2(const unsigned char* pSrc, char* pDst, int nStrLen)
{
	int nDstLength;		// UNICODE宽字符数目
	unsigned short wchar[512];	// UNICODE串缓冲区  128-->512

	// 高低字节对调，拼成UNICODE
	for(int i=0; i<nStrLen/2; i++)
	{
		wchar[i] = *pSrc++ << 8;	// 先高位字节
		wchar[i] |= *pSrc++;		// 后低位字节
	}

	//	memcpy((char *)wchar,pSrc,nStrLen);
	// UNICODE串-->字符串
	nDstLength = CUnicodeGbChanger::WideCharToMultiByte(0, 0, wchar, nStrLen/2, pDst, 512, NULL, NULL);  //160=--->1024

	// 输出字符串加个结束符
	pDst[nDstLength] = '\0';

	// 返回目标字符串长度
	return nDstLength;
}


// 正常顺序的字符串转换为两两颠倒的字符串，若长度为奇数，补'F'凑成偶数
// 如："8613851872468" --> "683158812764F8"
// 输入: pSrc - 源字符串指针
//       nStrLen - 源字符串长度
// 输出: pDst - 目标字符串指针
// 返回: 目标字符串长度
int CSmsPDU::InvertNumbers(const char* pSrc, char* pDst, int nStrLen)
{
	int nDstLength;		// 目标字符串长度
	char ch;			// 用于保存一个字符

	// 复制串长度
	nDstLength = nStrLen;

	// 两两颠倒
	for(int i=0; i<nStrLen;i+=2)
	{
		ch = *pSrc++;		// 保存先出现的字符
		*pDst++ = *pSrc++;	// 复制后出现的字符
		*pDst++ = ch;		// 复制先出现的字符
	}

	// 源串长度是奇数吗？
	if(nStrLen & 1)
	{
		*(pDst-2) = 'F';	// 补'F'
		nDstLength++;		// 目标串长度加1
	}

	// 输出字符串加个结束符
	*pDst = '\0';

	// 返回目标字符串长度
	return nDstLength;
}

// 两两颠倒的字符串转换为正常顺序的字符串
// 如："683158812764F8" --> "8613851872468"
// 输入: pSrc - 源字符串指针
//       nStrLen - 源字符串长度
// 输出: pDst - 目标字符串指针
// 返回: 目标字符串长度
int CSmsPDU::SerializeNumbers(const char* pSrc, char* pDst, int nStrLen)
{
	int nDstLength;		// 目标字符串长度
	char ch;			// 用于保存一个字符

	// 复制串长度
	nDstLength = nStrLen;

	// 两两颠倒
	for(int i=0; i<nStrLen;i+=2)
	{
		ch = *pSrc++;		// 保存先出现的字符
		*pDst++ = *pSrc++;	// 复制后出现的字符
		*pDst++ = ch;		// 复制先出现的字符
	}

	// 最后的字符是'F'吗？
	if(*(pDst-1) == 'F')
	{
		pDst--;
		nDstLength--;		// 目标字符串长度减1
	}

	// 输出字符串加个结束符
	*pDst = '\0';

	// 返回目标字符串长度
	return nDstLength;
}


// PDU编码，用于编制、发送短消息
// 输入: pSrc - 源PDU参数指针
// 输出: pDst - 目标PDU串指针
// 返回: 目标PDU串长度
// 解析输入的
int CSmsPDU::EncodeFull(const SM_PARAM* pSrc, char* pDst)
{
	int nLength,isite,nTmplen=0,nReallen=0;			// 内部用的串长度
	int nDstLength;			// 目标PDU串长度
	unsigned char buf[1024];	// 内部用的缓冲区
	unsigned short *ucsDst;
	memset(buf,0,sizeof(buf));

	//1. SMSC地址信息段
	nLength = strlen(pSrc->SCA);	// SMSC地址字符串的长度	
	buf[0] = (char)((nLength & 1) == 0 ? nLength : nLength + 1) / 2 + 1;	// SMSC地址信息长度
	buf[1] = 0x91;		// 固定: 用国际格式号码
	nDstLength = Bytes2String(buf, pDst, 2);		// 转换2个字节到目标PDU串
	nDstLength += InvertNumbers(pSrc->SCA, &pDst[nDstLength], nLength);	// 转换SMSC号码到目标PDU串

	//2. TPDU段基本参数、目标地址等
	nLength = strlen(pSrc->TPA);	// TP-DA地址字符串的长度
	buf[0] = 0x11;					// 是发送短信(TP-MTI=01)，TP-VP用相对格式(TP-VPF=10)

	ACE_DEBUG((LM_DEBUG,"EncodeFull:pkgamt=%d,cur seq=%d len=%d\n",pSrc->TP_UDHI.amt,pSrc->TP_UDHI.seq,nLength));

	if (pSrc->TP_UDHI.amt != 1) // add by srite 20130702
	{
		buf[0] = 0x11|0X40; // TP-UDHI 是否存在用户数据头标识
	}

	buf[1] = 0;						// TP-MR=0
	buf[2] = (char)nLength;			// 目标地址数字个数(TP-DA地址字符串真实长度)
	buf[3] = 0x91;					// 固定: 用国际格式号码
	nDstLength += Bytes2String(buf, &pDst[nDstLength], 4);		// 转换4个字节到目标PDU串
	nDstLength += InvertNumbers(pSrc->TPA, &pDst[nDstLength], nLength);	// 转换TP-DA到目标PDU串

	//3. TPDU段协议标识、编码方式、用户信息等
	nLength = strlen(pSrc->TP_UD);	// 用户信息字符串的长度
	buf[0] = pSrc->TP_PID;			// 协议标识(TP-PID)
	buf[1] = pSrc->TP_DCS;			// 用户信息编码方式(TP-DCS)
	buf[2] = 0;						// 有效期(TP-VP)为5分钟

	isite = 4;
	//3.0 长报文头信息 add by serite 20130702

//	printf("EncodeFull ====GSM_7BIT[%X]=GSM_UCS2[%X]=TP_DCS[%X]\n",GSM_7BIT,GSM_UCS2,pSrc->TP_DCS);

	if (pSrc->TP_UDHI.amt != 1)
	{
		memcpy(&buf[isite],(unsigned char *)&(pSrc->TP_UDHI),sizeof(TPDU_UDHI)); //把 多报文头考进去
		nTmplen = sizeof(TPDU_UDHI);
		isite += nTmplen;
	}

	if(pSrc->TP_DCS == GSM_7BIT)	
	{

		// 7-bit编码方式
		
		nReallen = Encode7bit(pSrc->TP_UD, &buf[isite], nLength+1);
		buf[3] = nLength;
	}
	else if(pSrc->TP_DCS == GSM_UCS2)
	{
		// UCS2编码方式
		if (isUtf8 ) //`将utf-8 格式转换为unicode格式
		{
			ucsDst=  (unsigned short *)&buf[isite];
			buf[3] = utf8ToUcs2((unsigned char*)pSrc->TP_UD,ucsDst)*2;

		}
		else
		{
			//	nLength /= 2;
			buf[3] = EncodeUcs2(pSrc->TP_UD, &buf[isite], nLength);	// 转换TP-DA到目标PDU串
		}
	}
	else
	{
		// 8-bit编码方式
		nLength = pSrc->Len;
		buf[3] = Encode8bit(pSrc->TP_UD, &buf[isite], nLength);	// 转换TP-DA到目标PDU串
	}

	buf[3] += nTmplen;
	
	nLength = buf[3] + 4 ; //头部的4个字节
	if (nReallen > 0)
	nLength = nReallen + 4 ; //头部的4个字节

	nDstLength += Bytes2String(buf, &pDst[nDstLength], nLength);// 转换该段数据到目标PDU串

	// 返回目标字符串长度
	return nDstLength;
}


// PDU解码，用于接收、阅读短消息
// 输入: pSrc - 源PDU串指针
// 输出: pDst - 目标PDU参数指针
// 返回: 用户信息串长度
int CSmsPDU::Decode(const char* pSrc, SM_PARAM* pDst)
{
	int nDstLength,hasAmt=1,textLen;			// 目标PDU串长度 hasAmt:has a lot of msg
	char * pText;
	unsigned char tmp;		// 内部用的临时字节变量
	unsigned char buf[512];	// 内部用的缓冲区
	unsigned short * ucsSrc;
	TDCS dcs;
	
	memset(buf,0,sizeof(buf));
	// SMSC地址信息段
	String2Bytes(pSrc, &tmp, 2);	// 取长度
	tmp = (tmp - 1) * 2;	// SMSC号码串长度

	if (tmp > sizeof( pDst->SCA)) return -1;

	pSrc += 4;			// 指针后移，忽略了SMSC地址格式
	SerializeNumbers(pSrc, pDst->SCA, tmp);	// 转换SMSC号码到目标PDU串
	pSrc += tmp;		// 指针后移

	// TPDU段基本参数
	String2Bytes(pSrc, &tmp, 2);	// 取基本参数
//printf("hasAmt=%d------------------------------%x\n",hasAmt,tmp);
	if (tmp&0X40) hasAmt = 2;

	pSrc += 2;		// 指针后移

	// 取回复号码
	String2Bytes(pSrc, &tmp, 2);	// 取长度
	if(tmp & 1) tmp += 1;	// 调整奇偶性
	pSrc += 4;			// 指针后移，忽略了回复地址(TP-RA)格式

	if (tmp > sizeof( pDst->TPA)) return -2;

	SerializeNumbers(pSrc, pDst->TPA, tmp);	// 取TP-RA号码
	pSrc += tmp;		// 指针后移

	// TPDU段协议标识、编码方式、用户信息等
	String2Bytes(pSrc, (unsigned char*)&pDst->TP_PID, 2);	// 取协议标识(TP-PID)
	pSrc += 2;		// 指针后移
	String2Bytes(pSrc, (unsigned char*)&pDst->TP_DCS, 2);	// 取编码方式(TP-DCS)
	pSrc += 2;		// 指针后移
	SerializeNumbers(pSrc, pDst->TP_SCTS, 14);		// 服务时间戳字符串(TP_SCTS) 
	pSrc += 14;		// 指针后移
	String2Bytes(pSrc, &tmp, 2);	// 用户信息长度(TP-UDL)

	if (tmp > 144) return -3;

	pSrc += 2;		// 指针后移

	*((char *)&dcs) = pDst->TP_DCS;

//	printf("aDcs.COD = %x\n",dcs.COD);
//	printf("EncodeFull ====GSM_7BIT[%X]=GSM_UCS2[%X]=TP_DCS[%X]\n",GSM_7BIT,GSM_UCS2,pDst->TP_DCS);
//	printf("a[%s]",pSrc);
	if (pDst->TP_DCS == GSM_7BIT || dcs.COD == 0)
	{
		// 7-bit解码
		nDstLength = String2Bytes(pSrc, buf, tmp & 7 ? (int)tmp * 7 / 4 + 2 : (int)tmp * 7 / 4);	// 格式转换
		Decode7bit(buf, pDst->TP_UD, nDstLength);	// 转换到TP-DU
//		printf("b[%s]\n",buf);
//		printf("c[%s]\n",pDst->TP_UD);
		nDstLength = tmp;
	}
	else if (pDst->TP_DCS == GSM_UCS2 || dcs.COD == 2)
	{
		// UCS2解码
		pText = (char *)pSrc;
		textLen = tmp;
		if (hasAmt>1)
		{
			pText += sizeof(TPDU_UDHI)*2;
			textLen -= sizeof(TPDU_UDHI);
			sleep(1);
		}
		nDstLength = String2Bytes(pText, buf, textLen * 2);		// 格式转换
	
		ucsSrc = (unsigned short * )buf;
		if (isUtf8 ) //`将utf-8 格式转换为unicode格式
		{
			nDstLength= ucs2ToUtf8(nDstLength,ucsSrc,(unsigned char *)pDst->TP_UD);
		}else
		{
			nDstLength = DecodeUcs2(buf, pDst->TP_UD, nDstLength);	// 转换到TP-DU
		}
	}
	else
	{
		// 8-bit解码
		nDstLength = String2Bytes(pSrc, buf, tmp * 2);			// 格式转换
		nDstLength = Decode8bit(buf, pDst->TP_UD, nDstLength);	// 转换到TP-DU
		printf("d[%s]\n",buf);
		printf("e[%s]\n",pDst->TP_UD);
		//memset(pDst->TP_UD,0,512); //160---512
		//memcpy(pDst->TP_UD,pSrc,tmp * 2);
	}

	// 返回目标字符串长度
	return nDstLength;
}
