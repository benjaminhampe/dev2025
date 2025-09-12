#include <de/os/win32/ComInit.h>

#if defined(_WIN32) || defined(WIN32) 

#ifndef UNICODE
#define UNICODE
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif
#define _WIN32_WINNT 0x0600 // CreateEventEx() needs atleast this API version = WinXP or so.

#include <windows.h>
#include <synchapi.h>
#include <process.h>
#include <mmdeviceapi.h>
#include <audioclient.h>
//#include <commdlg.h>

ComInit::ComInit()  
{ 
	CoInitializeEx(nullptr, COINIT_MULTITHREADED); 
}

ComInit::~ComInit() 
{ 
	CoUninitialize(); 
}

#else
	

ComInit::ComInit()  
{ 
	// CoInitializeEx(nullptr, COINIT_MULTITHREADED); 
}

ComInit::~ComInit() 
{ 
	// CoUninitialize(); 
}

#endif