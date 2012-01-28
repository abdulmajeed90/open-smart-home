#pragma once
class CBuffer
{
private:
	void *m_pData;
	size_t m_nSize;
	int m_nWritePos;

public:
	CBuffer();
	CBuffer(size_t nSize);
	~CBuffer(void);
	void *GetPtr()	{	return m_pData; }
	size_t GetSize()	{	return m_nWritePos; }
	void SetSize(size_t size)	{	m_nWritePos = size;	} // ????

	void Delete(void);
	void Clear(void)	{	m_nWritePos = 0;} 
	bool Allocate(size_t nSize);
	bool ReAllocate(size_t nSize);
	bool Add(void *ptrData, size_t nSize);
	bool InsertAt(void *ptrData, size_t nSize, int nPos);
	void operator<<(BYTE &b);
};

