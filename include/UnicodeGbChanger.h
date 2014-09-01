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
	long Fsize; 	// GB->Unicode 编码表的长度
	long Fullen; 	// 整个GB文件长度
	char FFileName[128];
	int  FMapLen;
	char FMap[51200]; //32K的数据映射表
	char * FMapAddr;
	
	static CUnicodeGbChanger * m_Instance;

	CUnicodeGbChanger(char *Filename);
	virtual ~CUnicodeGbChanger();
protected:

	unsigned short UnicodeToGB(unsigned short code); // 将unicode 转为GB
	unsigned short GBToUnicode(unsigned short code); // 将GB转为unicode
	int Error; // 判断类创建是否成功，成功0，失败非0



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

	int loadMapFile();//将文件放置在普通内存中

	static int MultiByteToWideChar(
		unsigned int     CodePage, //指定要转换成的字符集代码页
		unsigned long    dwFlags,  //指定如何处理没有转换的字符
		const char *   lpMultiByteStr,//接收转换后输出新串的缓冲区
		int      cchMultiByte, //待转换字符串的长度
		const unsigned short *   lpWideCharStr,//转换的宽字符串
		int      cchWideChar); //转换宽字符串的长度
							   
							   /*
							   CodePage： 指定要转换成的字符集代码页，它可以是任何已经安装的或系统自带的字符集，你也可以使用如下所示代码页之一。
							   CP_ACP 当前系统ANSI代码页
							   CP_MACCP 当前系统Macintosh代码页
							   CP_OEMCP 当前系统OEM代码页，一种原始设备制造商硬件扫描码
							   CP_SYMBOL Symbol代码页，用于Windows 2000及以后版本，我不明白是什么
							   CP_THREAD_ACP 当前线程ANSI代码页，用于Windows 2000及以后版本，我不明白是什么
							   CP_UTF7 UTF-7，设置此值时lpDefaultChar和lpUsedDefaultChar都必须为NULL
							   CP_UTF8 UTF-8，设置此值时lpDefaultChar和lpUsedDefaultChar都必须为NULL
	我想最常用的应该是CP_ACP和CP_UTF8了，前者将宽字符转换为ANSI，后者转换为UTF8。 */
	
	/*
	dwFlags： 指定如何处理没有转换的字符， 但不设此参数函数会运行的更快一些，我都是把它设为0。 可设的值如下表所示：
	WC_NO_BEST_FIT_CHARS 把不能直接转换成相应多字节字符的Unicode字符转换成lpDefaultChar指定的默认字符。也就是说，如果把Unicode转换成多字节字符，然后再转换回来，你并不一定得到相同的Unicode字符，因为这期间可能使用了默认字符。此选项可以单独使用，也可以和其他选项一起使用。
	WC_COMPOSITECHECK 把合成字符转换成预制的字符。它可以与后三个选项中的任何一个组合使用，如果没有与他们中的任何一个组合，则与选项WC_SEPCHARS相同。
	WC_ERR_INVALID_CHARS 此选项会致使函数遇到无效字符时失败返回，并且GetLastError会返回错误码ERROR_NO_UNICODE_TRANSLATION。否则函数会自动丢弃非法字符。此选项只能用于UTF8。
	WC_DISCARDNS 转换时丢弃不占空间的字符，与WC_COMPOSITECHECK一起使用
	WC_SEPCHARS 转换时产生单独的字符，此是默认转换选项，与WC_COMPOSITECHECK一起使用
	WC_DEFAULTCHAR 转换时使用默认字符代替例外的字符，(最常见的如’?’)，与WC_COMPOSITECHECK一起使用。
	
	  当指定WC_COMPOSITECHECK时，函数会将合成字符转换成预制字符。合成字符由一个基字符和一个不占空间的字符(如欧洲国家及汉语拼音的音标)组成，每一个都有不同的字符值。预制字符有一个用于表示基字符和不占空间字符的合成体的单一的字符值。 当指定WC_COMPOSITECHECK选项时，也可以使用上表列出的最后3个选项来定制预制字符的转换规则。这些选项决定了函数在遇到宽字符串的合成字符没有对应的预制字符时的行为，他们与WC_COMPOSITECHECK一起使用，如果都没有指定，函数默认WC_SEPCHARS。 对于下列代码页，dwFlags必须为0，否则函数返回错误码ERROR_INVALID_FLAGS。 50220 50221 50222 50225 50227 50229 52936 54936 57002到57011 65000(UTF7) 42(Symbol) 
	  
		对于UTF8，dwFlags必须为0或WC_ERR_INVALID_CHARS，否则函数都将失败返回并设置错误码ERROR_INVALID_FLAGS，你可以调用GetLastError获得。  
	*/
	
	/*
	lpWideCharStr：待转换的宽字符串。 
	cchWideChar：待转换宽字符串的长度，-1表示转换到字符串结尾。 
	lpMultiByteStr：接收转换后输出新串的缓冲区。 
	cbMultiByte：输出缓冲区大小，如果为0，lpMultiByteStr将被忽略，函数将返回所需缓冲区大小而不使用lpMultiByteStr。 
	lpDefaultChar：指向字符的指针， 在指定编码里找不到相应字符时使用此字符作为默认字符代替。 
	如果为NULL则使用系统默认字符。对于要求此参数为NULL的dwFlags而使用此参数，函数将失败返回并设置错误码ERROR_INVALID_PARAMETER。 
	lpUsedDefaultChar：开关变量的指针，用以表明是否使用过默认字符。
	对于要求此参数为NULL的dwFlags而使用此参数，函数将失败返回并设置错误码ERROR_INVALID_PARAMETER。
	lpDefaultChar和lpUsedDefaultChar都设为NULL，函数会更快一些。 
	*/
	
	
	static int WideCharToMultiByte(
		unsigned int     CodePage,//指定要转换成的字符集代码页
		unsigned long    dwFlags,//指定如何处理没有转换的字符
		const unsigned short *  lpWideCharStr,//转换的宽字符串
		int      cchWideChar,//转换宽字符串的长度
		char *    lpMultiByteStr,//接收转换后输出新串的缓冲区
		int      cchMultiByte,//待转换字符串的长度
		const char *   lpDefaultChar, //指向字符的指针
		int *   lpUsedDefaultChar); //开关变量的指针


	static CUnicodeGbChanger * GetInstance();
};

#endif // !defined(AFX_UNICODEGBCHANGER_H__65F39DD8_E186_489A_8644_6B1E38751CEE__INCLUDED_)
