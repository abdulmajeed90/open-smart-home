#pragma once
#include "XB.h"
#include "XBAddress.h"
#include "string.h"

class XBDevice
{
protected:
	TCHAR m_Name[DEV_NAME_SIZE_MAX];
	XBAddress m_Address;
	XBAddress m_ExAddress;

public:
	XBDevice(void);

	void SetAddress(XBAddress &addr);
	void SetAddress(BYTE addr);
	void SetAddress(BYTE *pStrAddr);
	void SetAddress(BYTE *pAddr, size_t nSize);
	XBAddress& GetAddress(void);
	void SetName(const TCHAR *strName);
	
	//	init device into default state
	virtual HRESULT InitDefault(void);
};

