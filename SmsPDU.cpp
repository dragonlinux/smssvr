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

// �ɴ�ӡ�ַ���ת��Ϊ�ֽ�����
// �磺"C8329BFD0E01" --> {0xC8, 0x32, 0x9B, 0xFD, 0x0E, 0x01}
// ����: pSrc - Դ�ַ���ָ��
//       nSrcLen - Դ�ַ�������
// ���: pDst - Ŀ������ָ��
// ����: Ŀ�����ݳ���
int CSmsPDU::String2Bytes(const char* pSrc, unsigned char* pDst, int nSrcLen)
{
	for (int i = 0; i < nSrcLen; i += 2)
	{
		// �����4λ
		if ((*pSrc >= '0') && (*pSrc <= '9'))
		{
			*pDst = (*pSrc - '0') << 4;
		}
		else
		{
			*pDst = (*pSrc - 'A' + 10) << 4;
		}

		pSrc++;

		// �����4λ
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

	// ����Ŀ�����ݳ���
	return (nSrcLen / 2);
}

// �ֽ�����ת��Ϊ�ɴ�ӡ�ַ���
// �磺{0xC8, 0x32, 0x9B, 0xFD, 0x0E, 0x01} --> "C8329BFD0E01" 
// ����: pSrc - Դ����ָ��
//       nStrLen - Դ���ݳ���
// ���: pDst - Ŀ���ַ���ָ��
// ����: Ŀ���ַ�������
int CSmsPDU::Bytes2String(const unsigned char* pSrc, char* pDst, int nStrLen)
{
	const char tab[]="0123456789ABCDEF";	// 0x0-0xf���ַ����ұ�

	for (int i = 0; i < nStrLen; i++)
	{
		*pDst++ = tab[*pSrc >> 4];		// �����4λ
		*pDst++ = tab[*pSrc & 0x0f];	// �����4λ
		pSrc++;
	}

	// ����ַ����Ӹ�������
	*pDst = '\0';

	// ����Ŀ���ַ�������
	return (nStrLen * 2);
}


//// �磺{0xC8, 0x32, 0x9B, 0xFD, 0x0E, 0x01} --> "C8329BFD0E01" 
/*��Դ�ַ��ռ����������ת��δһ��2�������ַ���*/
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

/*���ַ��е������ַ�ת��Ϊ��Ӧ�ڴ��е�ÿһ��ASCII��*/
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

// 7bit����
// ����: pSrc - Դ�ַ���ָ��
//       nStrLen - Դ�ַ�������
// ���: pDst - Ŀ����봮ָ��
// ����: Ŀ����봮����
int CSmsPDU::Encode7bit(const char* pSrc, unsigned char* pDst, int nStrLen)
{
	int nSrc;		// Դ�ַ����ļ���ֵ
	int nDst;		// Ŀ����봮�ļ���ֵ
	int nChar;		// ��ǰ���ڴ���������ַ��ֽڵ���ţ���Χ��0-7
	unsigned char nLeft;	// ��һ�ֽڲ��������

	// ����ֵ��ʼ��
	nSrc = 0;
	nDst = 0;

	// ��Դ��ÿ8���ֽڷ�Ϊһ�飬ѹ����7���ֽ�
	// ѭ���ô�����̣�ֱ��Դ����������
	// ������鲻��8�ֽڣ�Ҳ����ȷ����
	while (nSrc < nStrLen)
	{
		// ȡԴ�ַ����ļ���ֵ�����3λ
		nChar = nSrc & 7;

		// ����Դ����ÿ���ֽ�
		if(nChar == 0)
		{
			// ���ڵ�һ���ֽڣ�ֻ�Ǳ�����������������һ���ֽ�ʱʹ��
			nLeft = *pSrc;
		}
		else
		{
			// ���������ֽڣ������ұ߲��������������ӣ��õ�һ��Ŀ������ֽ�
			*pDst = (*pSrc << (8-nChar)) | nLeft;

			// �����ֽ�ʣ�µ���߲��֣���Ϊ�������ݱ�������
			nLeft = *pSrc >> nChar;

			// �޸�Ŀ�괮��ָ��ͼ���ֵ
			pDst++;
			nDst++;
		}

		// �޸�Դ����ָ��ͼ���ֵ
		pSrc++;
		nSrc++;
	}

	// ����Ŀ�괮����
	return nDst;
}

// 7bit����
// ����: pSrc - Դ���봮ָ��
//       nStrLen - Դ���봮����
// ���: pDst - Ŀ���ַ���ָ��
// ����: Ŀ���ַ�������
int CSmsPDU::Decode7bit(const unsigned char* pSrc, char* pDst, int nStrLen)
{
	int nSrc;		// Դ�ַ����ļ���ֵ
	int nDst;		// Ŀ����봮�ļ���ֵ
	int nByte;		// ��ǰ���ڴ���������ֽڵ���ţ���Χ��0-6
	unsigned char nLeft;	// ��һ�ֽڲ��������

	// ����ֵ��ʼ��
	nSrc = 0;
	nDst = 0;

	// �����ֽ���źͲ������ݳ�ʼ��
	nByte = 0;
	nLeft = 0;

	// ��Դ����ÿ7���ֽڷ�Ϊһ�飬��ѹ����8���ֽ�
	// ѭ���ô�����̣�ֱ��Դ���ݱ�������
	// ������鲻��7�ֽڣ�Ҳ����ȷ����
	while(nSrc<nStrLen)
	{
		// ��Դ�ֽ��ұ߲��������������ӣ�ȥ�����λ���õ�һ��Ŀ������ֽ�
		*pDst = ((*pSrc << nByte) | nLeft) & 0x7f;

		// �����ֽ�ʣ�µ���߲��֣���Ϊ�������ݱ�������
		nLeft = *pSrc >> (7-nByte);

		// �޸�Ŀ�괮��ָ��ͼ���ֵ
		pDst++;
		nDst++;

		// �޸��ֽڼ���ֵ
		nByte++;

		// ����һ������һ���ֽ�
		if(nByte == 7)
		{
			// ����õ�һ��Ŀ������ֽ�
			*pDst = nLeft;

			// �޸�Ŀ�괮��ָ��ͼ���ֵ
			pDst++;
			nDst++;

			// �����ֽ���źͲ������ݳ�ʼ��
			nByte = 0;
			nLeft = 0;
		}

		// �޸�Դ����ָ��ͼ���ֵ
		pSrc++;
		nSrc++;
	}

	// ����ַ����Ӹ�������
	*pDst = '\0';

	// ����Ŀ�괮����
	return nDst;
}

// 8bit����
// ����: pSrc - Դ�ַ���ָ��
//       nStrLen - Դ�ַ�������
// ���: pDst - Ŀ����봮ָ��
// ����: Ŀ����봮����
int CSmsPDU::Encode8bit(const char* pSrc, unsigned char* pDst, int nStrLen)
{
	// �򵥸���
	memcpy(pDst, pSrc, nStrLen);

	return nStrLen;
}

// 8bit����
// ����: pSrc - Դ���봮ָ��
//       nStrLen -  Դ���봮����
// ���: pDst -  Ŀ���ַ���ָ��
// ����: Ŀ���ַ�������
int CSmsPDU::Decode8bit(const unsigned char* pSrc, char* pDst, int nStrLen)
{
	// �򵥸���
	memcpy(pDst, pSrc, nStrLen);

	// ����ַ����Ӹ�������
	*pDst = '\0';

	return nStrLen;
}

// UCS2���� GBK 
// ����: pSrc - Դ�ַ���ָ��
//       nStrLen - Դ�ַ�������
// ���: pDst - Ŀ����봮ָ��
// ����: Ŀ����봮����
int CSmsPDU::EncodeUcs2(const char* pSrc, unsigned char* pDst, int nStrLen)
{
	int nDstLength;		// UNICODE���ַ���Ŀ
	unsigned short wchar[1024];	// UNICODE��������128--->1280

	// �ַ���-->UNICODE��
	nDstLength = CUnicodeGbChanger::MultiByteToWideChar(0, 0, pSrc, nStrLen, wchar, 1024);

	// �ߵ��ֽڶԵ������
	for(int i=0; i<nDstLength; i++)
	{
		*pDst++ = wchar[i] >> 8;		// �������λ�ֽ�
		*pDst++ = wchar[i] & 0xff;		// �������λ�ֽ�
	}

	// ����Ŀ����봮����
	return nDstLength * 2;
}

// UCS2���� UNCODE->GBK
// ����: pSrc - Դ���봮ָ��
//       nStrLen -  Դ���봮����
// ���: pDst -  Ŀ���ַ���ָ��
// ����: Ŀ���ַ������� 
int CSmsPDU::DecodeUcs2(const unsigned char* pSrc, char* pDst, int nStrLen)
{
	int nDstLength;		// UNICODE���ַ���Ŀ
	unsigned short wchar[512];	// UNICODE��������  128-->512

	// �ߵ��ֽڶԵ���ƴ��UNICODE
	for(int i=0; i<nStrLen/2; i++)
	{
		wchar[i] = *pSrc++ << 8;	// �ȸ�λ�ֽ�
		wchar[i] |= *pSrc++;		// ���λ�ֽ�
	}

	//	memcpy((char *)wchar,pSrc,nStrLen);
	// UNICODE��-->�ַ���
	nDstLength = CUnicodeGbChanger::WideCharToMultiByte(0, 0, wchar, nStrLen/2, pDst, 512, NULL, NULL);  //160=--->1024

	// ����ַ����Ӹ�������
	pDst[nDstLength] = '\0';

	// ����Ŀ���ַ�������
	return nDstLength;
}


// ����˳����ַ���ת��Ϊ�����ߵ����ַ�����������Ϊ��������'F'�ճ�ż��
// �磺"8613851872468" --> "683158812764F8"
// ����: pSrc - Դ�ַ���ָ��
//       nStrLen - Դ�ַ�������
// ���: pDst - Ŀ���ַ���ָ��
// ����: Ŀ���ַ�������
int CSmsPDU::InvertNumbers(const char* pSrc, char* pDst, int nStrLen)
{
	int nDstLength;		// Ŀ���ַ�������
	char ch;			// ���ڱ���һ���ַ�

	// ���ƴ�����
	nDstLength = nStrLen;

	// �����ߵ�
	for(int i=0; i<nStrLen;i+=2)
	{
		ch = *pSrc++;		// �����ȳ��ֵ��ַ�
		*pDst++ = *pSrc++;	// ���ƺ���ֵ��ַ�
		*pDst++ = ch;		// �����ȳ��ֵ��ַ�
	}

	// Դ��������������
	if(nStrLen & 1)
	{
		*(pDst-2) = 'F';	// ��'F'
		nDstLength++;		// Ŀ�괮���ȼ�1
	}

	// ����ַ����Ӹ�������
	*pDst = '\0';

	// ����Ŀ���ַ�������
	return nDstLength;
}

// �����ߵ����ַ���ת��Ϊ����˳����ַ���
// �磺"683158812764F8" --> "8613851872468"
// ����: pSrc - Դ�ַ���ָ��
//       nStrLen - Դ�ַ�������
// ���: pDst - Ŀ���ַ���ָ��
// ����: Ŀ���ַ�������
int CSmsPDU::SerializeNumbers(const char* pSrc, char* pDst, int nStrLen)
{
	int nDstLength;		// Ŀ���ַ�������
	char ch;			// ���ڱ���һ���ַ�

	// ���ƴ�����
	nDstLength = nStrLen;

	// �����ߵ�
	for(int i=0; i<nStrLen;i+=2)
	{
		ch = *pSrc++;		// �����ȳ��ֵ��ַ�
		*pDst++ = *pSrc++;	// ���ƺ���ֵ��ַ�
		*pDst++ = ch;		// �����ȳ��ֵ��ַ�
	}

	// �����ַ���'F'��
	if(*(pDst-1) == 'F')
	{
		pDst--;
		nDstLength--;		// Ŀ���ַ������ȼ�1
	}

	// ����ַ����Ӹ�������
	*pDst = '\0';

	// ����Ŀ���ַ�������
	return nDstLength;
}


// PDU���룬���ڱ��ơ����Ͷ���Ϣ
// ����: pSrc - ԴPDU����ָ��
// ���: pDst - Ŀ��PDU��ָ��
// ����: Ŀ��PDU������
// ���������
int CSmsPDU::EncodeFull(const SM_PARAM* pSrc, char* pDst)
{
	int nLength,isite,nTmplen=0,nReallen=0;			// �ڲ��õĴ�����
	int nDstLength;			// Ŀ��PDU������
	unsigned char buf[1024];	// �ڲ��õĻ�����
	unsigned short *ucsDst;
	memset(buf,0,sizeof(buf));

	//1. SMSC��ַ��Ϣ��
	nLength = strlen(pSrc->SCA);	// SMSC��ַ�ַ����ĳ���	
	buf[0] = (char)((nLength & 1) == 0 ? nLength : nLength + 1) / 2 + 1;	// SMSC��ַ��Ϣ����
	buf[1] = 0x91;		// �̶�: �ù��ʸ�ʽ����
	nDstLength = Bytes2String(buf, pDst, 2);		// ת��2���ֽڵ�Ŀ��PDU��
	nDstLength += InvertNumbers(pSrc->SCA, &pDst[nDstLength], nLength);	// ת��SMSC���뵽Ŀ��PDU��

	//2. TPDU�λ���������Ŀ���ַ��
	nLength = strlen(pSrc->TPA);	// TP-DA��ַ�ַ����ĳ���
	buf[0] = 0x11;					// �Ƿ��Ͷ���(TP-MTI=01)��TP-VP����Ը�ʽ(TP-VPF=10)

	ACE_DEBUG((LM_DEBUG,"EncodeFull:pkgamt=%d,cur seq=%d len=%d\n",pSrc->TP_UDHI.amt,pSrc->TP_UDHI.seq,nLength));

	if (pSrc->TP_UDHI.amt != 1) // add by srite 20130702
	{
		buf[0] = 0x11|0X40; // TP-UDHI �Ƿ�����û�����ͷ��ʶ
	}

	buf[1] = 0;						// TP-MR=0
	buf[2] = (char)nLength;			// Ŀ���ַ���ָ���(TP-DA��ַ�ַ�����ʵ����)
	buf[3] = 0x91;					// �̶�: �ù��ʸ�ʽ����
	nDstLength += Bytes2String(buf, &pDst[nDstLength], 4);		// ת��4���ֽڵ�Ŀ��PDU��
	nDstLength += InvertNumbers(pSrc->TPA, &pDst[nDstLength], nLength);	// ת��TP-DA��Ŀ��PDU��

	//3. TPDU��Э���ʶ�����뷽ʽ���û���Ϣ��
	nLength = strlen(pSrc->TP_UD);	// �û���Ϣ�ַ����ĳ���
	buf[0] = pSrc->TP_PID;			// Э���ʶ(TP-PID)
	buf[1] = pSrc->TP_DCS;			// �û���Ϣ���뷽ʽ(TP-DCS)
	buf[2] = 0;						// ��Ч��(TP-VP)Ϊ5����

	isite = 4;
	//3.0 ������ͷ��Ϣ add by serite 20130702

//	printf("EncodeFull ====GSM_7BIT[%X]=GSM_UCS2[%X]=TP_DCS[%X]\n",GSM_7BIT,GSM_UCS2,pSrc->TP_DCS);

	if (pSrc->TP_UDHI.amt != 1)
	{
		memcpy(&buf[isite],(unsigned char *)&(pSrc->TP_UDHI),sizeof(TPDU_UDHI)); //�� �౨��ͷ����ȥ
		nTmplen = sizeof(TPDU_UDHI);
		isite += nTmplen;
	}

	if(pSrc->TP_DCS == GSM_7BIT)	
	{

		// 7-bit���뷽ʽ
		
		nReallen = Encode7bit(pSrc->TP_UD, &buf[isite], nLength+1);
		buf[3] = nLength;
	}
	else if(pSrc->TP_DCS == GSM_UCS2)
	{
		// UCS2���뷽ʽ
		if (isUtf8 ) //`��utf-8 ��ʽת��Ϊunicode��ʽ
		{
			ucsDst=  (unsigned short *)&buf[isite];
			buf[3] = utf8ToUcs2((unsigned char*)pSrc->TP_UD,ucsDst)*2;

		}
		else
		{
			//	nLength /= 2;
			buf[3] = EncodeUcs2(pSrc->TP_UD, &buf[isite], nLength);	// ת��TP-DA��Ŀ��PDU��
		}
	}
	else
	{
		// 8-bit���뷽ʽ
		nLength = pSrc->Len;
		buf[3] = Encode8bit(pSrc->TP_UD, &buf[isite], nLength);	// ת��TP-DA��Ŀ��PDU��
	}

	buf[3] += nTmplen;
	
	nLength = buf[3] + 4 ; //ͷ����4���ֽ�
	if (nReallen > 0)
	nLength = nReallen + 4 ; //ͷ����4���ֽ�

	nDstLength += Bytes2String(buf, &pDst[nDstLength], nLength);// ת���ö����ݵ�Ŀ��PDU��

	// ����Ŀ���ַ�������
	return nDstLength;
}


// PDU���룬���ڽ��ա��Ķ�����Ϣ
// ����: pSrc - ԴPDU��ָ��
// ���: pDst - Ŀ��PDU����ָ��
// ����: �û���Ϣ������
int CSmsPDU::Decode(const char* pSrc, SM_PARAM* pDst)
{
	int nDstLength,hasAmt=1,textLen;			// Ŀ��PDU������ hasAmt:has a lot of msg
	char * pText;
	unsigned char tmp;		// �ڲ��õ���ʱ�ֽڱ���
	unsigned char buf[512];	// �ڲ��õĻ�����
	unsigned short * ucsSrc;
	TDCS dcs;
	
	memset(buf,0,sizeof(buf));
	// SMSC��ַ��Ϣ��
	String2Bytes(pSrc, &tmp, 2);	// ȡ����
	tmp = (tmp - 1) * 2;	// SMSC���봮����

	if (tmp > sizeof( pDst->SCA)) return -1;

	pSrc += 4;			// ָ����ƣ�������SMSC��ַ��ʽ
	SerializeNumbers(pSrc, pDst->SCA, tmp);	// ת��SMSC���뵽Ŀ��PDU��
	pSrc += tmp;		// ָ�����

	// TPDU�λ�������
	String2Bytes(pSrc, &tmp, 2);	// ȡ��������
//printf("hasAmt=%d------------------------------%x\n",hasAmt,tmp);
	if (tmp&0X40) hasAmt = 2;

	pSrc += 2;		// ָ�����

	// ȡ�ظ�����
	String2Bytes(pSrc, &tmp, 2);	// ȡ����
	if(tmp & 1) tmp += 1;	// ������ż��
	pSrc += 4;			// ָ����ƣ������˻ظ���ַ(TP-RA)��ʽ

	if (tmp > sizeof( pDst->TPA)) return -2;

	SerializeNumbers(pSrc, pDst->TPA, tmp);	// ȡTP-RA����
	pSrc += tmp;		// ָ�����

	// TPDU��Э���ʶ�����뷽ʽ���û���Ϣ��
	String2Bytes(pSrc, (unsigned char*)&pDst->TP_PID, 2);	// ȡЭ���ʶ(TP-PID)
	pSrc += 2;		// ָ�����
	String2Bytes(pSrc, (unsigned char*)&pDst->TP_DCS, 2);	// ȡ���뷽ʽ(TP-DCS)
	pSrc += 2;		// ָ�����
	SerializeNumbers(pSrc, pDst->TP_SCTS, 14);		// ����ʱ����ַ���(TP_SCTS) 
	pSrc += 14;		// ָ�����
	String2Bytes(pSrc, &tmp, 2);	// �û���Ϣ����(TP-UDL)

	if (tmp > 144) return -3;

	pSrc += 2;		// ָ�����

	*((char *)&dcs) = pDst->TP_DCS;

//	printf("aDcs.COD = %x\n",dcs.COD);
//	printf("EncodeFull ====GSM_7BIT[%X]=GSM_UCS2[%X]=TP_DCS[%X]\n",GSM_7BIT,GSM_UCS2,pDst->TP_DCS);
//	printf("a[%s]",pSrc);
	if (pDst->TP_DCS == GSM_7BIT || dcs.COD == 0)
	{
		// 7-bit����
		nDstLength = String2Bytes(pSrc, buf, tmp & 7 ? (int)tmp * 7 / 4 + 2 : (int)tmp * 7 / 4);	// ��ʽת��
		Decode7bit(buf, pDst->TP_UD, nDstLength);	// ת����TP-DU
//		printf("b[%s]\n",buf);
//		printf("c[%s]\n",pDst->TP_UD);
		nDstLength = tmp;
	}
	else if (pDst->TP_DCS == GSM_UCS2 || dcs.COD == 2)
	{
		// UCS2����
		pText = (char *)pSrc;
		textLen = tmp;
		if (hasAmt>1)
		{
			pText += sizeof(TPDU_UDHI)*2;
			textLen -= sizeof(TPDU_UDHI);
			sleep(1);
		}
		nDstLength = String2Bytes(pText, buf, textLen * 2);		// ��ʽת��
	
		ucsSrc = (unsigned short * )buf;
		if (isUtf8 ) //`��utf-8 ��ʽת��Ϊunicode��ʽ
		{
			nDstLength= ucs2ToUtf8(nDstLength,ucsSrc,(unsigned char *)pDst->TP_UD);
		}else
		{
			nDstLength = DecodeUcs2(buf, pDst->TP_UD, nDstLength);	// ת����TP-DU
		}
	}
	else
	{
		// 8-bit����
		nDstLength = String2Bytes(pSrc, buf, tmp * 2);			// ��ʽת��
		nDstLength = Decode8bit(buf, pDst->TP_UD, nDstLength);	// ת����TP-DU
		printf("d[%s]\n",buf);
		printf("e[%s]\n",pDst->TP_UD);
		//memset(pDst->TP_UD,0,512); //160---512
		//memcpy(pDst->TP_UD,pSrc,tmp * 2);
	}

	// ����Ŀ���ַ�������
	return nDstLength;
}
