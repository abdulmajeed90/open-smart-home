#pragma once
//	Use XBArray class name, becouse Array is reserved

template <class T> class XBArray
{
public:
	XBArray(void)
	{
		m_nSize = 0;
		m_nCount = 0;
		m_pList = NULL;
	}
	~XBArray()	
	{
		if(m_pList)	delete m_pList;
	}

	void operator <<(T val)	{	Add(val);	}
	T& operator [](int index)	{	return m_pList[index];	}

	int Size(void)			{	return m_nSize;	}
	int GetCount(void)		{	return m_nCount;	}
	T&	GetAt(int index)	{	return m_pList[index];	}
	void RemoveAll(void)	{	m_nCount = 0;	}

	bool Add(T val)
	{
		if(m_nCount==m_nSize)	
			if(!Reallocate(m_nCount + 8))	return false;
		if(!m_pList)	return false;

		m_pList[m_nCount] = val;
		m_nCount++;
		return true;
	}

private:
	T *m_pList;
	int m_nSize;
	int m_nCount;

	bool Reallocate(int nItems)
	{
		T *p = new T[nItems];
		if(!p)	return false;
		if(m_pList)
			memcpy(p,m_pList,sizeof(T)*nItems);
		delete m_pList;
		m_pList = p;
		m_nSize = nItems;
		return true;
	}
};

