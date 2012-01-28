#include "StdAfx.h"
#include "XBAddress.h"

bool XBAddress::operator ==(XBAddress &addr)
{
	size_t nSize = this->GetCount();
	if(nSize!=addr.GetCount())	return false;
	if(memcmp(this->GetData(),addr.GetData(),nSize)==0)	return true;
	return false;
}

XBAddress & XBAddress::operator =(const XBAddress &addr)
{
	this->Copy(addr);
	return *this;
}