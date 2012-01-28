#pragma once
#include "stdafx.h"
//#include "afxcoll.h"
class XBAddress :	public CByteArray
{
public:
	bool operator ==(XBAddress &addr);
	XBAddress & operator =(const XBAddress &addr);
};

