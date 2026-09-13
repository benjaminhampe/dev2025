#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#define MY_EXPORT extern "C" __declspec(dllexport) HRESULT __stdcall

MY_EXPORT DllGetClassObject(REFCLSID, REFIID, void**);
MY_EXPORT DllCanUnloadNow();
MY_EXPORT DllRegisterServer();
MY_EXPORT DllUnregisterServer();
