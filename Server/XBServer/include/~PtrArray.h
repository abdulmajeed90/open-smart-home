#pragma once

#define REALLOCATE_STEP 8

//	Declaration of PtrArray class
//	Dynamic pointer array with automatic reallocation and complete memory freeing

template<class Type>
class PtrArray
{
public:
	PtrArray(int size=0);
	~PtrArray(void);
	void operator <<(Type *);
	Type& operator [](int index);
	int Size(void)	{return m_nSize;};
	void Remove(Type *);

private:
	Type *m_ptrList;
	int m_nSize;
	int m_nIndex;

	bool Reallocate();
};

