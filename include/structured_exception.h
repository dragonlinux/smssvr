#if   !defined   inc_CPubException_   
#define     inc_CPubException_   

//#include   "windows.h" 
#include <exception>  
#define EXCEPTION_POINTERS char*
#define EXCEPTION_RECORD  long
#define EXCEPTION_POINTERS char*
class   CPubException   
{   
public:   
	
	CPubException   
		(EXCEPTION_POINTERS   const   &info)   throw()   
	{   
	//	EXCEPTION_RECORD   const   &exception   =   *(info.ExceptionRecord);   
	//	address_ = exception.ExceptionAddress;
	//	code_ = exception.ExceptionCode;
	}   
	
	static void   install()   throw()   
	{   
	//	_set_se_translator(my_translator);   
	}   
	
	unsigned   what()   const   throw()   
	{   
		return   code_;   
	}   
	
	void   const   *where()   const   throw()   
	{   
		return   address_;   
	}   
	
	
private:   
	void   const   *address_;   
	unsigned   code_;   
	
public:	

static   void   my_translator(unsigned,   EXCEPTION_POINTERS   *info)   
{   
	throw   CPubException(*info);   
}  	
	
};   




#endif   //   !defined   inc_CPubException_   
