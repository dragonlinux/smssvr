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

// �û���Ϣ���뷽ʽ
#define GSM_7BIT		0
#define GSM_8BIT		4
#define GSM_UCS2		8

// Ӧ��״̬
#define GSM_WAIT		0		// no data wait
#define GSM_R_WAIT		3		// hav data wait
#define GSM_READING		2		// 
#define GSM_OK			1		// OK
#define GSM_ERR			-1		// ERROR

#define SMS_PKG_MAXLEN 132

typedef struct 
{
	unsigned char len1;   //05 ��ʾʣ��Э��ͷ�ĳ���
	unsigned char sze; //00  ��ʶλpid �ĳ���
	unsigned char len2;   //03 ʣ��Э��ͷ�ĳ���
	unsigned char batid;   //xx �������ŵ�Ψһ��־
	unsigned char amt;   //xx �������ŵ�����
	unsigned char seq;  //��� �������ŵ�����
} TPDU_UDHI;
// ����Ϣ�����ṹ������/���빲��
// ���У��ַ�����'\0'��β
typedef struct {
	char SCA[32];			// ����Ϣ�������ĺ���(SMSC��ַ)
	char TPA[32];			// Ŀ������ظ�����(TP-DA��TP-RA)
	char TP_PID;			// �û���ϢЭ���ʶ(TP-PID)
	char TP_DCS;			// �û���Ϣ���뷽ʽ(TP-DCS)
	char TP_SCTS[16];		// ����ʱ����ַ���(TP_SCTS), ����ʱ�õ�
	int  Len;
	TPDU_UDHI TP_UDHI;      // ����������ͷ
	char TP_UD[512];		// ԭʼ�û���Ϣ(����ǰ�������TP-UD)
	short index;			// ����Ϣ��ţ��ڶ�ȡʱ�õ�
} SM_PARAM;




// ��ȡӦ��Ļ�����
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
	}; //����Ϣ�������ĺ���


	struct TDCS
	{
		unsigned char  CLS:2; //class 0:����Ϣֱ����ʾ���û��նˡ�1:�洢��SIM����, 
		//2:�������SIM���Ͻ�ֱֹ�Ӵ����ն��ϣ�3:�洢���û��豸��
		unsigned char  COD:2; //0 Ĭ�ϡ�1:8λ��2:usc2 3:rsv
		unsigned char  BCLS:1; //0 CLS�Ƿ���Ч0: 1:
		unsigned char  EPR:1; //0 �Ƿ�ѹ��
		unsigned char  RSV:2;
	};

	struct  TUD
	{
		unsigned char  UDL;     // �û����ݳ���
		unsigned char  UD[140];	// ԭʼ�û���Ϣ(����ǰ�������TP-UD)
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
			unsigned char  RP:1;        //Ӧ��·����0:δ���� 1:����
			unsigned char  UDHI:1;      //�û�����ͷ��ʾ��0:������ 1:����
			unsigned char  SRI:1;       //״̬����ָʾ
			unsigned char   RSV:2; //����
			unsigned char   MMS:1; //������Ϣ
			unsigned char   MTI:2; //��Ϣ���͡�
		}; //PDU ����

		TSCA SCA;               // ����Ϣ�������ĺ���(SMSC��ַ)
		TSCA OA;                // ԭ��ַ
		TPDUTYPE TYPE;		    // Ŀ������ظ�����(TP-DA��TP-RA)
		unsigned char PID;	    // �û���ϢЭ���ʶ(TP-PID)
		TDCS DCS;			    // �û���Ϣ���뷽ʽ(TP-DCS)
		TSCTS SCTS;	            // ����ʱ����ַ���(TP_SCTS), ����ʱ�õ�
		TUD UD;     // �û�����
	};

	struct TSND_PDU
	{
		struct TPDUTYPE
		{
			unsigned char  RP:1;        //Ӧ��·����0:δ���� 1:����
			unsigned char  UDHI:1;      //�û�����ͷ��ʾ��0:������ 1:����
			unsigned char  SRR:1;       //����״̬����ָʾ
			unsigned char   VPF:2;      //��Ч��ʽ 00:��VP�� 01:���� 10:VP���������ṩ 11:VP��8λ���һ���ṩ
			unsigned char   RD:1; //������Ϣ
			unsigned char   MTI:2; //��Ϣ���͡�
		}; //PDU ����

		TSCA SCA;               // ����Ϣ�������ĺ���(SMSC��ַ)
		TPDUTYPE TYPE;		    // Ŀ������ظ�����(TP-DA��TP-RA)
		unsigned char MR;       // Ĭ��00
		TSCA DA;                // ԭ��ַ
		unsigned char PID;	    // �û���ϢЭ���ʶ(TP-PID)
		TDCS DCS;			    // �û���Ϣ���뷽ʽ(TP-DCS)
		TSCTS VP;	            // ��Ϣ��Ч��
		TUD UD;					// �û�����
		short index;			// ����Ϣ��ţ��ڶ�ȡʱ�õ�
	}; // �����ṹ���������Э��ģ�����Ĳ��� SM_PARAM ֱ�ӽ��в���
	//�����ṹ�����ڱ��ĺϲ�ʹ�õ��û�����ͷ




public:
	CSmsPDU();
	virtual ~CSmsPDU();

public:
	static int isUtf8 ;

	static int getEnvLang();
	// �ɴ�ӡ�ַ���ת��Ϊ�ֽ�����
	// �磺"C8329BFD0E01" --> {0xC8, 0x32, 0x9B, 0xFD, 0x0E, 0x01}
	// ����: pSrc - Դ�ַ���ָ��
	//       nSrcLen - Դ�ַ�������
	// ���: pDst - Ŀ������ָ��
	// ����: Ŀ�����ݳ���
	static int String2Bytes(const char* pSrc, unsigned char* pDst, int nSrcLen);

	// �ֽ�����ת��Ϊ�ɴ�ӡ�ַ���
	// �磺{0xC8, 0x32, 0x9B, 0xFD, 0x0E, 0x01} --> "C8329BFD0E01" 
	// ����: pSrc - Դ����ָ��
	//       nStrLen - Դ���ݳ���
	// ���: pDst - Ŀ���ַ���ָ��
	// ����: Ŀ���ַ�������
	static int Bytes2String(const unsigned char* pSrc, char* pDst, int nStrLen);


	//// �磺{0xC8, 0x32, 0x9B, 0xFD, 0x0E, 0x01} --> "C8329BFD0E01" 
	/*��Դ�ַ��ռ����������ת��δһ��2�������ַ���*/
	static char * AscToStr(char * Sour,int Sourlen, char * Targ);

	/*���ַ��е������ַ�ת��Ϊ��Ӧ�ڴ��е�ÿһ��ASCII��*/
	static char  * StrToAsc(char * Sour,int Sourlen, char * Targ);


	// 7bit����
	// ����: pSrc - Դ�ַ���ָ��
	//       nStrLen - Դ�ַ�������
	// ���: pDst - Ŀ����봮ָ��
	// ����: Ŀ����봮����
	static int Encode7bit(const char* pSrc, unsigned char* pDst, int nStrLen);


	// 7bit����
	// ����: pSrc - Դ���봮ָ��
	//       nStrLen - Դ���봮����
	// ���: pDst - Ŀ���ַ���ָ��
	// ����: Ŀ���ַ�������
	static  int Decode7bit(const unsigned char* pSrc, char* pDst, int nStrLen);

	// 8bit����
	// ����: pSrc - Դ�ַ���ָ��
	//       nStrLen - Դ�ַ�������
	// ���: pDst - Ŀ����봮ָ��
	// ����: Ŀ����봮����
	static int Encode8bit(const char* pSrc, unsigned char* pDst, int nStrLen);


	// 8bit����
	// ����: pSrc - Դ���봮ָ��
	//       nStrLen -  Դ���봮����
	// ���: pDst -  Ŀ���ַ���ָ��
	// ����: Ŀ���ַ�������
	static int Decode8bit(const unsigned char* pSrc, char* pDst, int nStrLen);

	// UCS2����
	// ����: pSrc - Դ�ַ���ָ��
	//       nStrLen - Դ�ַ�������
	// ���: pDst - Ŀ����봮ָ��
	// ����: Ŀ����봮����
	static int EncodeUcs2(const char* pSrc, unsigned char* pDst, int nStrLen);


	// UCS2����
	// ����: pSrc - Դ���봮ָ��
	//       nStrLen -  Դ���봮����
	// ���: pDst -  Ŀ���ַ���ָ��
	// ����: Ŀ���ַ�������
	static int DecodeUcs2(const unsigned char* pSrc, char* pDst, int nStrLen);


	// �����ߵ����ַ���ת��Ϊ����˳����ַ���
	// �磺"683158812764F8" --> "8613851872468"
	// ����: pSrc - Դ�ַ���ָ��
	//       nStrLen - Դ�ַ�������
	// ���: pDst - Ŀ���ַ���ָ��
	// ����: Ŀ���ַ�������
	static int SerializeNumbers(const char* pSrc, char* pDst, int nStrLen);


	// ����˳����ַ���ת��Ϊ�����ߵ����ַ�����������Ϊ��������'F'�ճ�ż��
	// �磺"8613851872468" --> "683158812764F8"
	// ����: pSrc - Դ�ַ���ָ��
	//       nStrLen - Դ�ַ�������
	// ���: pDst - Ŀ���ַ���ָ��
	// ����: Ŀ���ַ�������
	static int InvertNumbers(const char* pSrc, char* pDst, int nStrLen);


	// PDU���룬���ڱ��ơ����Ͷ���Ϣ
	// ����: pSrc - ԴPDU����ָ��
	// ���: pDst - Ŀ��PDU��ָ��
	// ����: Ŀ��PDU������
	static int EncodeFull(const SM_PARAM* pSrc, char* pDst);


	// PDU���룬���ڽ��ա��Ķ�����Ϣ
	// ����: pSrc - ԴPDU��ָ��
	// ���: pDst - Ŀ��PDU����ָ��
	// ����: �û���Ϣ������
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
