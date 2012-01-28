#include "StdAfx.h"
#include "xb.h"
#include "Buffer.h"

CBuffer::CBuffer()
{
	m_pData=NULL;
	m_nSize = 0;
	m_nWritePos = 0;
}

CBuffer::CBuffer(size_t nSize)
{
	m_pData=NULL;
	m_nSize = 0;
	m_nWritePos = 0;

	Allocate(nSize);
}

CBuffer::~CBuffer(void)
{
	Delete();
}

void CBuffer::Delete(void)
{
	if(m_pData)	
	{
		delete m_pData;
		m_pData = NULL;
	}
	m_nSize = 0;
	m_nWritePos = 0;
}

bool CBuffer::Allocate(size_t nSize)
{
	if(nSize<1)	return false;
	if(m_pData)	Delete();
	void *p = new void*[nSize];
	if(!p) return false;
	m_nSize = nSize;
	m_pData = p;
	m_nWritePos = 0;
	return true;
}

bool CBuffer::ReAllocate(size_t nSize)
{
	ASSERT(m_pData);
	ASSERT(nSize);

	void *p_new = new void*[nSize];
	if(!p_new)	return false;
	memcpy(p_new,m_pData,m_nSize>nSize?nSize:m_nSize);

	delete m_pData;
	m_pData = p_new;
	m_nSize = nSize;
	return true;
}

void CBuffer::operator<<(BYTE &b)
{
	ASSERT(m_pData);

	if(m_nWritePos+1>=(int)m_nSize) ReAllocate(m_nSize + m_nSize/2);

	((BYTE *)m_pData)[m_nWritePos] = b;
	m_nWritePos++;
}

bool CBuffer::Add(void *ptrData, size_t nSize)
{
	ASSERT(m_pData);
	ASSERT(ptrData);
	ASSERT(nSize);

	if(m_nWritePos+nSize>=m_nSize) 
		if(!ReAllocate(m_nSize + nSize))	return false;

	memcpy((BYTE *)m_pData + m_nWritePos, ptrData, nSize);
	m_nWritePos+=nSize;
	return true;
}

bool CBuffer::InsertAt(void *ptrData, size_t nSize, int nPos)
{
	ASSERT(m_pData);
	ASSERT(ptrData); 
	ASSERT(nSize);

	if(m_nWritePos+nSize>=m_nSize) 
		if(!ReAllocate(m_nSize + nSize))	return false;
	BYTE *p = (BYTE *)m_pData;
	BYTE *pSrc = (BYTE *)ptrData;
	
	for(int i=m_nWritePos;i>=nPos;i--)
		p[i+nSize] = p[i];
	for(size_t i=0;i<nSize;i++)
		p[i+nPos] = pSrc[i];
	m_nWritePos+=nSize;
	return true;
}