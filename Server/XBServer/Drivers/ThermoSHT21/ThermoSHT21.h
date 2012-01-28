// ThermoSHT21.h : main header file for the ThermoSHT21 DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CThermoSHT21App
// See ThermoSHT21.cpp for the implementation of this class
//

class CThermoSHT21App : public CWinApp
{
public:
	CThermoSHT21App();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
