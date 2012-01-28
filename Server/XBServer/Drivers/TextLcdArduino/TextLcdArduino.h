// TextLcdArduino.h : main header file for the TextLcdArduino DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CTextLcdArduinoApp
// See TextLcdArduino.cpp for the implementation of this class
//

class CTextLcdArduinoApp : public CWinApp
{
public:
	CTextLcdArduinoApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
