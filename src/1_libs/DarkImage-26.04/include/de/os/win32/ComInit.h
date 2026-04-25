#pragma once

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
