// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once
#define   _WIN32_WINNT   0x0500 


#define WIN32_LEAN_AND_MEAN		// �� Windows ͷ���ų�����ʹ�õ�����
#include <stdio.h>
//#include <tchar.h>
//#include <windows.h>
#pragma warning(disable: 4100)
#pragma warning(disable: 4663)
#pragma warning(disable: 4503 4511 4512)

#pragma warning(disable: 4018)
#pragma warning(disable: 4146)
#pragma warning(disable: 4244 4267 4313)
#pragma warning(disable: 4663 4189 4245 4663)
#pragma warning(disable: 4097)
#pragma warning(disable: 4786)
#pragma warning(disable: 4800 4996)

#include "ace/pre.h"

#include "ace/OS.h"

struct TSmsInfo{
	char batchid[24];
	char phone[32];
	char datatime[24];
	char info[1024];
	int  status;

	TSmsInfo()
	{
		memset(batchid,0,sizeof(batchid));
		memset(phone,0,sizeof(phone));
		memset(datatime,0,sizeof(datatime));
		memset(info,0,sizeof(info));
		status = 0;
	}
} ;

typedef signed char         INT8, *PINT8;
typedef signed short        INT16, *PINT16;
typedef signed int          INT32, *PINT32;

typedef unsigned char       UINT8, *PUINT8;
typedef unsigned short      UINT16, *PUINT16;
typedef unsigned int        UINT32, *PUINT32;

#ifndef VOID
#define VOID void
typedef char CHAR;
typedef short SHORT;
typedef long LONG;
#if !defined(MIDL_PASS)
typedef int INT;
#endif
#endif

// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�
