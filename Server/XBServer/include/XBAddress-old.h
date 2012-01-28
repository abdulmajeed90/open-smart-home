#pragma once
#include "XB.h"
#include <windows.h>

class XBAddress
{
public:
	XBAddress(void)
	{
		ZERO_MEM(m_bAddress,DEV_ADDRESS_SIZE_MAX);
	}
	
	void operator=(BYTE addr)
	{
		ZERO_MEM(m_bAddress,DEV_ADDRESS_SIZE_MAX);
		m_bAddress[0] = addr;
	}
	void operator=(WORD addr)
	{
		ZERO_MEM(m_bAddress,DEV_ADDRESS_SIZE_MAX);
		memcpy(m_bAddress,&addr,sizeof(WORD));
	}
	void operator=(DWORD addr)
	{
		ZERO_MEM(m_bAddress,DEV_ADDRESS_SIZE_MAX);
		memcpy(m_bAddress,&addr,sizeof(DWORD));
	}
	void operator=(QWORD addr)
	{
		ZERO_MEM(m_bAddress,DEV_ADDRESS_SIZE_MAX);
		memcpy(m_bAddress,&addr,sizeof(QWORD));
	}

	BYTE GetAsBYTE(void)	{	return m_bAddress[0]; }
	WORD GetAsWORD(void)	{	return *((WORD *)m_bAddress); }
	DWORD GetAsDWORD(void)	{	return *((DWORD *)m_bAddress); }
	QWORD GetAsQWORD(void)	{	return *((QWORD *)m_bAddress); }

	void operator=(XBAddress &a)
	{
		memcpy(m_bAddress, a.m_bAddress,DEV_ADDRESS_SIZE_MAX);
	}
	bool operator==(XBAddress &a)
	{
		if(memcmp(m_bAddress, a.m_bAddress,DEV_ADDRESS_SIZE_MAX)==0)	return true;
		return false;
	}
	void* GetPtr(void)
	{
		return m_bAddress;
	}
	size_t GetSize(void)
	{
		return DEV_ADDRESS_SIZE_MAX;
	}

private:
	BYTE m_bAddress[DEV_ADDRESS_SIZE_MAX];
};

