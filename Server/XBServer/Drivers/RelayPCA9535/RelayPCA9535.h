// RelayPCA9535.h : main header file for the RelayPCA9535 DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CRelayPCA9535App
// See RelayPCA9535.cpp for the implementation of this class
//

class CRelayPCA9535App : public CWinApp
{
public:
	CRelayPCA9535App();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
