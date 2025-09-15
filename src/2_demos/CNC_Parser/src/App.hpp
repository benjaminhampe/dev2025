#pragma once
#include <DarkImage.h>
#include <CNC_ParserTest.hpp>
//#include <strsafe.h>

#include <demo/DemoPanel.hpp>
#include <demo/DemoButton.hpp>
#include <demo/DemoEditBox.hpp>
#include <demo/DemoCheckBox.hpp>
#include <demo/DemoComboBox.hpp>
#include <demo/DemoTextArea.hpp>
#include <demo/DemoSlider.hpp>
#include <demo/DemoSpinBox.hpp>
#include <demo/DemoRadioButton.hpp>

#include <AboutDialog.hpp>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <commdlg.h>
#include <commctrl.h>

struct GCodeParserApp
{
   HINSTANCE m_hInstance = nullptr;
   HWND m_hWnd = nullptr;
   int m_clientWidth = 0;
   int m_clientHeight = 0;
   int m_mouseX = 0;
   int m_mouseY = 0;

   HWND m_edtFeedSpeed = nullptr;
   HWND m_spinBox = nullptr;
   HWND m_comboBox = nullptr;

   HWND m_taOriginal = nullptr;
   HWND m_taProcessed = nullptr;
};

extern GCodeParserApp g_app;
