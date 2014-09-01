// UnicodeGbChanger.h: interface for the CUnicodeGbChanger class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UNICODEGBCHANGER_H__65F39DD8_E186_489A_8644_6B1E38751CEE__INCLUDED_)
#define AFX_UNICODEGBCHANGER_H__65F39DD8_E186_489A_8644_6B1E38751CEE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stdio.h>
#include <fcntl.h>


class CUnicodeGbChanger
{
private:
	long Fsize; 	// GB->Unicode �����ĳ���
	long Fullen; 	// ����GB�ļ�����
	char FFileName[128];
	int  FMapLen;
	char FMap[51200]; //32K������ӳ���
	char * FMapAddr;
	
	static CUnicodeGbChanger * m_Instance;

	CUnicodeGbChanger(char *Filename);
	virtual ~CUnicodeGbChanger();
protected:

	unsigned short UnicodeToGB(unsigned short code); // ��unicode תΪGB
	unsigned short GBToUnicode(unsigned short code); // ��GBתΪunicode
	int Error; // �ж��ഴ���Ƿ�ɹ����ɹ�0��ʧ�ܷ�0



public:	

	inline static unsigned short htons(unsigned short wVal)
	{
		unsigned char tmp ,* p;

		p = (unsigned char *)&wVal;
		tmp = *(p+1); 
		*(p+1) = *p;
		*p = tmp;
		return wVal;
	}

	int loadMapFile();//���ļ���������ͨ�ڴ���

	static int MultiByteToWideChar(
		unsigned int     CodePage, //ָ��Ҫת���ɵ��ַ�������ҳ
		unsigned long    dwFlags,  //ָ����δ���û��ת�����ַ�
		const char *   lpMultiByteStr,//����ת��������´��Ļ�����
		int      cchMultiByte, //��ת���ַ����ĳ���
		const unsigned short *   lpWideCharStr,//ת���Ŀ��ַ���
		int      cchWideChar); //ת�����ַ����ĳ���
							   
							   /*
							   CodePage�� ָ��Ҫת���ɵ��ַ�������ҳ�����������κ��Ѿ���װ�Ļ�ϵͳ�Դ����ַ�������Ҳ����ʹ��������ʾ����ҳ֮һ��
							   CP_ACP ��ǰϵͳANSI����ҳ
							   CP_MACCP ��ǰϵͳMacintosh����ҳ
							   CP_OEMCP ��ǰϵͳOEM����ҳ��һ��ԭʼ�豸������Ӳ��ɨ����
							   CP_SYMBOL Symbol����ҳ������Windows 2000���Ժ�汾���Ҳ�������ʲô
							   CP_THREAD_ACP ��ǰ�߳�ANSI����ҳ������Windows 2000���Ժ�汾���Ҳ�������ʲô
							   CP_UTF7 UTF-7�����ô�ֵʱlpDefaultChar��lpUsedDefaultChar������ΪNULL
							   CP_UTF8 UTF-8�����ô�ֵʱlpDefaultChar��lpUsedDefaultChar������ΪNULL
	������õ�Ӧ����CP_ACP��CP_UTF8�ˣ�ǰ�߽����ַ�ת��ΪANSI������ת��ΪUTF8�� */
	
	/*
	dwFlags�� ָ����δ���û��ת�����ַ��� ������˲������������еĸ���һЩ���Ҷ��ǰ�����Ϊ0�� �����ֵ���±���ʾ��
	WC_NO_BEST_FIT_CHARS �Ѳ���ֱ��ת������Ӧ���ֽ��ַ���Unicode�ַ�ת����lpDefaultCharָ����Ĭ���ַ���Ҳ����˵�������Unicodeת���ɶ��ֽ��ַ���Ȼ����ת���������㲢��һ���õ���ͬ��Unicode�ַ�����Ϊ���ڼ����ʹ����Ĭ���ַ�����ѡ����Ե���ʹ�ã�Ҳ���Ժ�����ѡ��һ��ʹ�á�
	WC_COMPOSITECHECK �Ѻϳ��ַ�ת����Ԥ�Ƶ��ַ����������������ѡ���е��κ�һ�����ʹ�ã����û���������е��κ�һ����ϣ�����ѡ��WC_SEPCHARS��ͬ��
	WC_ERR_INVALID_CHARS ��ѡ�����ʹ����������Ч�ַ�ʱʧ�ܷ��أ�����GetLastError�᷵�ش�����ERROR_NO_UNICODE_TRANSLATION�����������Զ������Ƿ��ַ�����ѡ��ֻ������UTF8��
	WC_DISCARDNS ת��ʱ������ռ�ռ���ַ�����WC_COMPOSITECHECKһ��ʹ��
	WC_SEPCHARS ת��ʱ�����������ַ�������Ĭ��ת��ѡ���WC_COMPOSITECHECKһ��ʹ��
	WC_DEFAULTCHAR ת��ʱʹ��Ĭ���ַ�����������ַ���(������硯?��)����WC_COMPOSITECHECKһ��ʹ�á�
	
	  ��ָ��WC_COMPOSITECHECKʱ�������Ὣ�ϳ��ַ�ת����Ԥ���ַ����ϳ��ַ���һ�����ַ���һ����ռ�ռ���ַ�(��ŷ�޹��Ҽ�����ƴ��������)��ɣ�ÿһ�����в�ͬ���ַ�ֵ��Ԥ���ַ���һ�����ڱ�ʾ���ַ��Ͳ�ռ�ռ��ַ��ĺϳ���ĵ�һ���ַ�ֵ�� ��ָ��WC_COMPOSITECHECKѡ��ʱ��Ҳ����ʹ���ϱ��г������3��ѡ��������Ԥ���ַ���ת��������Щѡ������˺������������ַ����ĺϳ��ַ�û�ж�Ӧ��Ԥ���ַ�ʱ����Ϊ��������WC_COMPOSITECHECKһ��ʹ�ã������û��ָ��������Ĭ��WC_SEPCHARS�� �������д���ҳ��dwFlags����Ϊ0�����������ش�����ERROR_INVALID_FLAGS�� 50220 50221 50222 50225 50227 50229 52936 54936 57002��57011 65000(UTF7) 42(Symbol) 
	  
		����UTF8��dwFlags����Ϊ0��WC_ERR_INVALID_CHARS������������ʧ�ܷ��ز����ô�����ERROR_INVALID_FLAGS������Ե���GetLastError��á�  
	*/
	
	/*
	lpWideCharStr����ת���Ŀ��ַ����� 
	cchWideChar����ת�����ַ����ĳ��ȣ�-1��ʾת�����ַ�����β�� 
	lpMultiByteStr������ת��������´��Ļ������� 
	cbMultiByte�������������С�����Ϊ0��lpMultiByteStr�������ԣ��������������軺������С����ʹ��lpMultiByteStr�� 
	lpDefaultChar��ָ���ַ���ָ�룬 ��ָ���������Ҳ�����Ӧ�ַ�ʱʹ�ô��ַ���ΪĬ���ַ����档 
	���ΪNULL��ʹ��ϵͳĬ���ַ�������Ҫ��˲���ΪNULL��dwFlags��ʹ�ô˲�����������ʧ�ܷ��ز����ô�����ERROR_INVALID_PARAMETER�� 
	lpUsedDefaultChar�����ر�����ָ�룬���Ա����Ƿ�ʹ�ù�Ĭ���ַ���
	����Ҫ��˲���ΪNULL��dwFlags��ʹ�ô˲�����������ʧ�ܷ��ز����ô�����ERROR_INVALID_PARAMETER��
	lpDefaultChar��lpUsedDefaultChar����ΪNULL�����������һЩ�� 
	*/
	
	
	static int WideCharToMultiByte(
		unsigned int     CodePage,//ָ��Ҫת���ɵ��ַ�������ҳ
		unsigned long    dwFlags,//ָ����δ���û��ת�����ַ�
		const unsigned short *  lpWideCharStr,//ת���Ŀ��ַ���
		int      cchWideChar,//ת�����ַ����ĳ���
		char *    lpMultiByteStr,//����ת��������´��Ļ�����
		int      cchMultiByte,//��ת���ַ����ĳ���
		const char *   lpDefaultChar, //ָ���ַ���ָ��
		int *   lpUsedDefaultChar); //���ر�����ָ��


	static CUnicodeGbChanger * GetInstance();
};

#endif // !defined(AFX_UNICODEGBCHANGER_H__65F39DD8_E186_489A_8644_6B1E38751CEE__INCLUDED_)
