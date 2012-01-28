// ThermoDS1820.h : main header file for the ThermoDS1820 DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CThermoDS1820App
// See ThermoDS1820.cpp for the implementation of this class
//

class CThermoDS1820App : public CWinApp
{
public:
	CThermoDS1820App();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
