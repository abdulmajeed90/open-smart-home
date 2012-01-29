// XBServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "XBServer.h"
#include "XBLedDriver.h"
#include "XBLinkDevice.h"
#include "XBLinkDeviceXBee2.h"
#include "XBGateDevice.h"
#include "XBAddress.h"

#include "..\Drivers\LedDriverPCA9635\LedDriverPCA9635.h"

#include <conio.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The one and only application object

CWinApp theApp;

using namespace std;
HMODULE hModule = NULL;
FARPROC lpfProcFunction = NULL;

int Start(void);
void Stop(XBLedDriver *led_dev);

typedef void* (*Create)(void *);
typedef void (*Delete)(void *);

Create _Create;
Delete _Delete;

XBLinkDevice* CreateLinkDevice(void);
XBLinkDevice* pLinkDev = NULL;
int StartLED(XBLedDriver *led_dev);

CPtrArray m_LinkDevices;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	HMODULE hModule = ::GetModuleHandle(NULL);

	if (hModule != NULL)
	{
		// initialize MFC and print and error on failure
		if (!AfxWinInit(hModule, NULL, ::GetCommandLine(), 0))
		{
			// TODO: change error code to suit your needs
			_tprintf(_T("Fatal Error: MFC initialization failed\n"));
			nRetCode = 1;
		}
		else
		{
			nRetCode = Start();
		}
	}
	else
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: GetModuleHandle failed\n"));
		nRetCode = 1;
	}

	return nRetCode;
}

XBGateDevice* InitGateDevices(XBLinkDevice* pLinkDev);
XBEndPointDevice* InitEndPointDevices(XBGateDevice *pGateDev);
////////////////	LINKS

XBLinkDevice* InitLinkDevices(void)
{
	XBLinkDeviceXBee2* p = new XBLinkDeviceXBee2;
	p->Config(2,CBR_57600);
	m_LinkDevices.Add(p);

	InitGateDevices(p);
	return p;
}

void DeInitLinkDevices(void)
{
	int count = m_LinkDevices.GetCount();
	for(int i=0;i<count;i++)
	{
		XBLinkDevice* p = (XBLinkDevice*)m_LinkDevices[i];
		if(p) delete p;
	}
	m_LinkDevices.RemoveAll();
}

////////////////	GATES

XBGateDevice* InitGateDevices(XBLinkDevice* pLinkDev)
{
	ASSERT(pLinkDev);
	BYTE addr[]={"GateDev"};

	XBGateDevice *pGateDev = new XBGateDevice(pLinkDev);
	if(!pGateDev)	return NULL;
	pGateDev->SetAddress(addr);
	
	//	move to  XBGateDevice constructor ???
	if(pLinkDev->AddGateDevice(pGateDev)!=S_OK)	return NULL;

//	InitEndPointDevices(pGateDev);
	
	return pGateDev;
}

/////////////////  END POINTS

XBEndPointDevice* InitEndPointDevices(XBGateDevice *pGateDev)
{
	ASSERT(pGateDev);
	BYTE addr[]={"EPDev"};
		
	//	select and load driver dll
	XBEndPointDevice* p = new XBEndPointDevice(pGateDev);
	pGateDev->AddEndPointDevice(p);

	return p;
}

const int leds[] = {1,2,3};
int limit = 64;
#define RG 1
#define GB 2
#define BR 3
unsigned int count = 0;

int cycle(XBLedDriver *dev, int which, const int * led)
{
    int a;
    int b;
	int ret = 0;
    switch(which)
    {
        case RG:
            a = led[0];
            b = led[1];
        break;
        case GB:
            a = led[1];
            b = led[2];
        break;
        default:
            a = led[2];
            b = led[0];
    }
    for(int i = 0; i < limit; i++)
    {
		dev->SetLedPWM(b, i*256);
        dev->SetLedPWM(a, (limit - i)*256);
		dev->SetLedPWM(b+4, i*256);
        dev->SetLedPWM(a+4, (limit - i)*256);
		dev->SetLedPWM(b+8, i*256);
        dev->SetLedPWM(a+8, (limit - i)*256);
		dev->SetLedPWM(b+12, i*256);
        dev->SetLedPWM(a+12, (limit - i)*256);
		_tprintf(_T("%i\r"),count++);
	//	_tprintf(_T("."));
		dev->Write();
        Sleep(100);
		ret = _kbhit();
		if(ret)	break;
    }
	return ret;
}

int Start(void)
{
	HRESULT hr = S_OK;
	int ret = 0;
	_tprintf(_T("XBServer started...\n"));

	XBLinkDevice* pLinkDev = InitLinkDevices();
	if(!pLinkDev)	return 0;

	if(pLinkDev->Open()!=S_OK)	
	{
		_tprintf(_T("Can't open XBLinkDevice.\n"));
		delete pLinkDev;
		return 1;
	}

	XBGateDevice *pGateDev = pLinkDev->GetGateDevice(0);

	hModule = LoadLibrary(_T("LedDriverPCA9635.dll"));
	if(hModule)
	{
		_Create = (Create)GetProcAddress(hModule,"CreateInstance");
		_Delete = (Delete)GetProcAddress(hModule,"DeleteInstance");
		if(_Create)
		{
			BYTE addr=0xE0;
			XBLedDriver *dev = (XBLedDriver *)(_Create(pGateDev));
			dev->SetAddress(addr);
			//	move to XBEndPointDevice constructor ?
			if(pGateDev->AddEndPointDevice(dev)==S_OK)	
			{
				hr = dev->Read();
				if(hr==S_OK)
				{
					_tprintf(_T("\nRead device ok\n"));
					dev->InitDefault();
					dev->SetSleepMode(false);
					for(int i=0;i<16;i++)
					{
							dev->SetLedState(i,LedStatePWM);
							dev->SetLedPWM(i,0);
					}
					while(!ret)
					{
						ret = cycle(dev, RG, leds);
						ret = cycle(dev, GB, leds);
						ret = cycle(dev, BR, leds);
					}
					dev->SetSleepMode(true);
					dev->Write();
				}
				else _tprintf(_T("Read device error : 0x%x\n"),hr);
			}
			if(dev)	_Delete(dev);
		}
		FreeLibrary(hModule);
		SAFE_DELETE(pGateDev);
//		SAFE_DELETE(pLinkDev);
	}
	else
	{
		DWORD error = GetLastError();
		_tprintf(_T("Error : 0x%x\n"),error);
	}

	_tprintf(_T("XBServer stopped...\n"));
	return 1;
}
/*
HRESULT OnRecieveChar(void)
{
	BYTE m_Buffer[32];
	int m_buffer_char_count = 8;

	BYTE *ptrBuffer = m_Buffer;
	int count = m_buffer_char_count;
	if(count<sizeof(XBPacketHeader))	return E_FAIL;

	XBPacketHeader *pGateHeader = (XBPacketHeader *)m_Buffer;
	if(m_buffer_char_count<sizeof(XBPacketHeader) + pGateHeader->DataSize)	return E_FAIL;

	XBGateDevice *pGateDev = pLinkDev->FindGateDevice(XBAddress(pGateHeader->Address));
	if(!pGateDev)	return E_FAIL;		//	GateDevice not found


	//	найти следующий header 
	XBPacketHeader *pEndPointHeader = (XBPacketHeader *)(m_Buffer + sizeof(XBPacketHeader) + pGateHeader->DataSize);
	XBEndPointDevice *pEndPointDev = pGateDev->FindEndPointDevice(address);
	if(!pEndPointDev)	return E_FAIL;		//	EndPointDevice not found

	pEndPointDev->Read(ptrBuffer, m_buffer_char_count);
}
*/
/*
void Stop(XBLedDriver *led_dev)
{
	_Delete(led_dev);
	SAFE_DELETE(pLinkDev);
}

int StartLED(XBLedDriver *led_dev)
{
	ASSERT(led_dev);

	int c = led_dev->GetLedCount();

	for(int i=0;i<c;i++)
		led_dev->SetLedState(i,256/(i+1));

	pLinkDev->Write(led_dev);

	pLinkDev->Write(pGateDev, pEndDev);		// отправить устройству pLinkDev через гейт pGateDev на устройство pEndDev
	return 1;
}
*/
