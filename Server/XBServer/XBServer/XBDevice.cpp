#include "StdAfx.h"
#include "XBDevice.h"


XBDevice::XBDevice(void)
{
	ZERO_MEM(m_Name,sizeof(TCHAR)*DEV_NAME_SIZE_MAX);
}

void XBDevice::SetName(const TCHAR *strName)
{
	_tcscpy_s(m_Name,DEV_NAME_SIZE_MAX,strName);
}

HRESULT XBDevice::InitDefault(void)
{
	return E_NOTIMPL;
}

void XBDevice::SetAddress(XBAddress &addr)
{
	m_Address.Copy(addr);
}

XBAddress& XBDevice::GetAddress(void)	
{	
	return m_Address;	
}

void XBDevice::SetAddress(BYTE addr)
{
	m_Address.SetSize(1);
	m_Address.SetAt(0,addr);
}

void XBDevice::SetAddress(BYTE *pStrAddr)
{
	m_Address.RemoveAll();
	while(*pStrAddr)
		m_Address.Add(*pStrAddr++);
}

void XBDevice::SetAddress(BYTE *pAddr, size_t nSize)
{
	m_Address.SetSize(nSize);
	for(size_t i=0;i<nSize;i++)
		m_Address.SetAt(i,pAddr[i]);
}