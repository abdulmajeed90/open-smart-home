#pragma once
#include "XBDataPacket.h"

class XBProtocol
{
private:
	CByteArray m_bData;
public:
	XBProtocol(void);

	//	save data into buffer according to protocol structure
	size_t PutXBPacket(XBDataPacket *p);
	//	extract data from buffer according to protocol structure
	size_t GetXBPacket(XBDataPacket *p);
	//	get size of data stored in memory buffer
	size_t GetSize(void);
	//	get data from memory buffer
	BYTE * GetData(void);
	//	get data from memory buffer as TEXT
	int GetTextData(char * strBuffer, size_t nBufferSize);
	//	clear all data
	void Clear(void);
	//	add data
	size_t PutData(CByteArray *pData);
	//	check for packet ready to extract
	static enum XBPacketSignatures IsPacket(CByteArray *pData);
};

