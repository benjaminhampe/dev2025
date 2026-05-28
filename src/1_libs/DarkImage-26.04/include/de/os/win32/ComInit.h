#pragma once

bool win32_CoInitialize();
void win32_CoUninitialize();

struct ComInit // Initialized from MainApp or MainWindow
{
   ComInit();
   ~ComInit();
   bool bSuccess;
};

struct OleInit // Initialized from MainApp or MainWindow
{
   OleInit();
   ~OleInit();
   bool bSuccess;
};

struct CommCtlInit // Initialized from MainApp or MainWindow
{
   CommCtlInit();
   ~CommCtlInit();
   bool bSuccess;
};
