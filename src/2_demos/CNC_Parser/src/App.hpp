#pragma once
#include <de_win32api.hpp>
#include <de_win32api_CommonDialogs.hpp>
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
