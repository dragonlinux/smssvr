//public_template.h

//
///////////////////////////////////////////////////////////////////////////////
#if !defined(_public_template_)
#define _public_template_
//public_template.h
#pragma once


#include <time.h>
#include <assert.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <math.h>
#include <stdarg.h>
#include <sys/stat.h>

#pragma warning( push,3 )
#include <map>
#include <vector>
#include <list>
#include <string>
#include <functional> // for greater<> and less<>
#include <algorithm> //for sort()
#pragma warning( pop)

// A fix to get rid of level 4 warnings comming from STL
#pragma warning(push)
//#include <yvals.h>
#pragma warning(disable: 4018)
#pragma warning(disable: 4146)
#pragma warning(disable: 4244)
#pragma warning(disable: 4663)
#pragma warning(disable: 4097)
#pragma warning(disable: 4786)
// #include <iostream>
// #include <ostream>
#include <string>
#pragma warning(pop)

#if defined(PUBOPT_MEMFIL) | defined(PUBOPT_NONSEG)
#	include <assert.h>
#endif

#define PUBAPI_INTERNAL_VERSION_MINOR 2

#define PUBAPI_INTERNAL_VERSION ((PUBAPI_INTERNAL_VERSION_MINOR&0xFFFF)|PUBAPI_INTERNAL_VERSION_MAJOR<<16)

#define MAX_WORDAMT 120
#define LEN_ID 80
#define LEN_EID 24
//#define LEN_VAL 128

#define LOG_FILE_NAME "test"
#define BIN_LOG_FILE_NAME "test_bin.log"

#define SQL_REC_SIGN ";\n"
using  namespace std;
//<summary>The public_template Parser namespace.</summary>
namespace pub
{
	
#pragma pack()
	//---------------------------ͨ�õ�ģ����
	template <class T> class node
	{
			private:
				T * _curr;
				node * _next;
				int _usecount,_freecount;
			public: //for pool
				node(T * curr)
				{
					_usecount = 0;
					_freecount = 0;
					_curr = curr;
				}
				
				T * This()
				{
					return _curr;
				}
				
				void next (node * pBlk)
				{
					_freecount++;
					_next = pBlk;
				}
				
				node * next ()
				{
					_usecount ++;
					if (_next!=NULL)
					{
					}
					return _next;
				}
	};
	
	template <class T,class CLock> class pool
	{
			private:
				char m_name[64];
				int m_msgCount;
				int m_msgSize;
				
				std::map<T*,node<T> *> m_map;
				
				T * getFirst()
				{
					if (m_map.size()>0)
						return m_map.begin()->first;
				}
			public:
				pool(char * sName = NULL):_header(NULL)
				{
					if (sName!=NULL)
						strcpy(this->m_name,sName);
				};
				
				~pool()
				{
					this->fini();
				};
				
			public:
				
				node<T> * find(T * aVal)
				{
					typename  std::map<T*,node<T> *>::iterator mapIter;
					
					mapIter = m_map.find(aVal);
					
					if(mapIter != m_map.end() )
						return mapIter->second;
					return NULL;
				}
				
				void add(T * aVal)
				{
					ACE_Guard<CLock> locker(_alock);
					node<T> * pNew = find(aVal);
					if (pNew != NULL) return ;
					
					pNew = new node<T>(aVal);
					
					pNew->next(_header);
					_header = pNew;
					
					m_map[aVal]=pNew;
					
				}
				
				int size()
				{
					return m_map.size();
				}
				
				T * get()
				{//ACE_Recursive_Thread_Mutex _alock;
					ACE_Guard<CLock> locker(_alock);
					if(_header==NULL)
					{
						return NULL;
					}
					
					node<T> * ret = _header;
					
					_header=_header->next();
					
					return ret->This();
				}
				
				void release(T * tr)
				{
					ACE_Guard<CLock> locker(_alock);
					
					node<T> * pNew = find(tr);
					
					if (pNew == NULL) return;
					pNew->next(_header);
					
					_header = pNew;
				}
				
				//������Ϣ���г�
				//�����Ǹ�������ŵ�����
				void init(char * sName,UINT32 nums,UINT32 default_size)
				{
					strcpy(this->m_name,sName);
					m_msgSize = default_size;
					m_msgCount = nums;
					_header = NULL;
					for(size_t i=0;i<nums;i++)
					{
						T * pNew =new T();
						
						if(pNew!=NULL)
						{
							pNew->next(_header);
							_header=pNew;
						}
					}
				}
				
				void fini()
				{
					while(_header)
					{
						node<T>  * pT=_header->next();
						delete _header;
						_header=pT;
					}
				}
				
				void FreeAndNil()
				{
					ACE_Guard<CLock> locker(_alock);
					_header = NULL;
					//			for(;;)
					{
						typename  std::map<T*,node<T> *>::iterator it;
						for(it = m_map.begin();it != m_map.end();it++)
						{
							delete it->first;
							delete it->second;
						}
						
						m_map.clear();
					}
				}
				
			private:
				node<T> *_header;
				CLock _alock;
		};
		
		
		template <class TIdx,class TCls> class indexmap
		{
			private:
				std::map<TIdx,TCls * > valMap;
				std::vector<TCls * >  valArr;
			private:
				//ɾ����ǰ����������
				inline void   del_arr(TIdx sKey)
				{
					
					typename std::vector<TCls * >::iterator ItemItor;
					
					for (ItemItor=valArr.begin();ItemItor!=valArr.end();ItemItor++)
					{
						if ((*ItemItor)->id ==  sKey)
						{
							valArr.erase(ItemItor);
							break; //��ֹ���洦�����
						}
					}
					
					return ;
				}
			public:		
				inline TCls * find(TIdx sKey)
				{
					TCls * pFind=NULL;
					
					typename std::map<TIdx,TCls * >::iterator mapIter;
					
					mapIter = valMap.find(sKey);
					
					if(mapIter != valMap.end() )
					{
						pFind =  mapIter->second;
					}
					
					return pFind;
				}
				
				inline TCls * findidx(int index)
				{
					TCls * pFind=NULL;
					
					if (index < valArr.size())
						pFind = valArr[index];
					
					return pFind;
				}
				
				
				inline TCls * operator[](int i)
				{
					return findidx(i);
				}
				/*
				inline TCls * operator[](LPCSTR sKey)
				{
					return find(sKey);
				}
				*/
				
				inline int size()
				{
					return valMap.size();
				}
				
				inline void clearNoFree()
				{
					valMap.clear();
					valArr.clear();
				}

				//������е���������
				inline void   clear()
				{
					typename  std::map<TIdx,TCls * >::iterator mapIter;
					TCls *  pObj;
					for(mapIter = valMap.begin();mapIter!=valMap.end();mapIter++)
					{
						pObj = mapIter->second;
						delete 	pObj;	
					}
					
					valMap.clear();
					valArr.clear();
				}
				
				//���һ����������
				//��ʱ��Ҫ���� һ��ԭ��������
				// class TIdx,class TCls
				inline int   add(TIdx idx,TCls * pVal)//������ھ͸���
				{
					typename  std::map<TIdx,TCls * >::iterator mapIter;
					TCls * pNew=NULL;
					
					mapIter = valMap.find(idx);
					
					if(mapIter != valMap.end() )
					{
						pNew = mapIter->second;
						del(idx);
					}
					
					valMap[idx] = pVal;
					
					valArr.push_back(pVal);
					
					return 0;
				}
				
				//ɾ����ǰ����������
				inline void   del(TIdx idx)
				{
					typename  std::map<TIdx,TCls * >::iterator mapIter;
					TCls * pNew;
					
					mapIter = valMap.find(idx);
					
					if(mapIter != valMap.end() )
					{
						pNew =  mapIter->second;
						valMap.erase(mapIter);
						del_arr(idx);
						delete pNew;
					}
					
					return ;
				}
				
				
				
				inline int copyInto(indexmap<TIdx,TCls> & aMap,bool bReplace = false)
				{
					typename std::map<TIdx,TCls * >::iterator mapIter;
					TCls * pObj,*pTmp;
					int i,icnt;
					icnt = valArr.size();
					
					for(i=0;i<icnt;i++)
					{
						//pAttr = mapIter->second;
						pObj = findidx(i);
						
						//��aMap Ѱ��,������� �͸��ݲ������ͽ��д���
						pTmp = aMap.find(pObj->id);
						
						if (pTmp !=NULL)
						{
							if (bReplace) //���������,����Ҫɾ�����������
							{
								aMap.del(pObj->id);	
							}else //����,���β���������
								continue;
							
						}				
						//���ڲ�Ѱ�ң�����ҵ��˾ͽ��и���
						
						aMap.add(pObj->id,pObj->clone());
					}
					return aMap.size();
				}
			
			//��÷��������������б�
	};
} //namespace pub

#endif //!defined(_public_template_)
