#include "StdAfx.h"
#include "..\include\PtrArray.h"

#include "tchar.h"
#define DEBUG_FUNCTION_NAME _tprintf(TEXT(__FUNCTION__)); _tprintf(_T("\n"))

template <class Type> 
PtrArray<Type>::PtrArray(int size)
{
	DEBUG_FUNCTION_NAME;
	m_nIndex = 0;
	m_nSize = 0;
	if(!size)	size = REALLOCATE_STEP;
	m_ptrList = new Type[size];
	if(m_ptrList)	m_nSize = size;
}
template <class Type> PtrArray<Type>::~PtrArray(void)
{
	if(!m_ptrList)	return;
	DEBUG_FUNCTION_NAME;
	for(int i=0;i<m_nIndex;i++)
		if(m_ptrList[i]!=NULL)	
		{
			_tprintf(_T("0x%08X "),m_ptrList[i]);
			delete m_ptrList[i];
		}
	delete m_ptrList;
	_tprintf(_T("\n"));
}

template <class Type> 
Type& PtrArray<Type>::operator [](int index)
{
	if(!m_ptrList)	return NULL;
	if(index>=m_nIndex)	return NULL;
	DEBUG_FUNCTION_NAME;
	return m_ptrList[index];
}

template <class Type> 
void PtrArray<Type>::operator <<(Type *p)
{
	if(!m_ptrList)	return;

	if(m_nIndex>=m_nSize)	
	{
		if(Reallocate())	
			m_ptrList[m_nIndex++] = p;
	}
	else m_ptrList[m_nIndex++] = p;
}

template <class Type> 
bool PtrArray<Type>::Reallocate(void)
{
	if(!m_ptrList)	return false;
	Type *p = new Type[m_nSize + REALLOCATE_STEP];
	if(!p) return false;

	memcpy(p,m_ptrList,sizeof(m_ptrList[0])*m_nSize);
	m_nSize = m_nSize + REALLOCATE_STEP;
	if(m_ptrList)	delete m_ptrList;
	m_ptrList = p;

	return true;
}

template <class Type> 
void PtrArray<Type>::Remove(Type *p)
{
	if(!p)	return;

	for(int i=0;i<m_nIndex;i++)
		if(m_ptrList[i]==p)	
		{
			delete m_ptrList[i];
			m_ptrList[i] = NULL;
		}
}