//////////////////////////////////////////////////////////////////////////////
/// @author Benjamin Hampe <benjamin.hampe@gmx.de>
//////////////////////////////////////////////////////////////////////////////
#pragma once
#include <string>

// ===========================================================================

// New

// ===========================================================================

void dbMessageBoxA(const std::string& msg, const std::string& title = "Alert!");

void dbMessageBoxW(const std::wstring& msg, const std::wstring& title = L"Alert!");

inline void dbMessageBox(const std::string& msg, const std::string& title = "Alert!") { dbMessageBoxA(msg,title); }

inline void dbMessageBox(const std::wstring& msg, const std::wstring& title = L"Alert!") { dbMessageBoxW(msg,title); }

// ===========================================================================

bool dbPromtBoolA(const std::string& msg, const std::string& title);

bool dbPromtBoolW(const std::wstring& msg, const std::wstring& title);

inline bool dbPromtBool(const std::string& msg, const std::string& title) { return dbPromtBoolA(msg,title); }

inline bool dbPromtBool(const std::wstring& msg, const std::wstring& title) { return dbPromtBoolW(msg,title); }

// ===========================================================================

std::string dbPromtStrA(const std::string& msg, const std::string& title);

std::wstring dbPromtStrW(const std::wstring& msg, const std::wstring& title);

inline std::string dbPromtStr(const std::string& msg, const std::string& title) { return dbPromtStrA(msg,title); }

inline std::wstring dbPromtStr(const std::wstring& msg, const std::wstring& title) { return dbPromtStrW(msg,title); }

// ===========================================================================

/*
void
dbMessageDlg(
   std::string const & msg,
   std::string const & caption = "Info" );

bool
dbQuestionDlg(
   std::string const & msg,
   std::string const & caption = "Question" );
*/

std::string
dbOpenFolderDlg(
   std::string caption, int x=50, int y=50, int w=800, int h=600,
   std::string initDir = "", bool newui = true );

std::string
dbOpenFileDlg(
   std::string caption,
   int x=50,
   int y=50,
   int w=800,
   int h=600,
   std::string filter = "",
   std::string initDir = "",
   bool newui = true );

std::string
dbSaveFileDlg(
   std::string caption, 
   int x=50, 
   int y=50, 
   int w=800,
   int h=600,
   std::string filter = "",
   std::string initDir = "",
   std::string initFileName = "Untitled.txt", 
   bool newui = true );

std::wstring
dbOpenFileDlgW(
   std::wstring caption,
   int x=50,
   int y=50,
   int w=800,
   int h=600,
   std::wstring filter = L"",
   std::wstring initDir = L"",
   bool newui = true );

std::wstring
dbSaveFileDlgW( 
	std::wstring caption = L"Save file dialog",
	std::wstring filter = L"All files (*.*)|*.*",
	std::wstring initDir = L"",
	std::wstring initFileName = L"Untitled",
	bool newui = true,
	int x = -1,
	int y = -1,
	int w = -1,
	int h = -1 );

/*

uint32_t
dbColorDialog(
   std::string lpCaption,
   int x, int y,
   uint32_t c0,
   uint32_t c1,
   uint32_t c2,
   uint32_t c3,
   uint32_t c4,
   uint32_t c5,
   uint32_t c6,
   uint32_t c7,
   uint32_t c8,
   uint32_t c9, uint32_t cA, uint32_t cB, uint32_t cC, uint32_t cD, uint32_t cE, uint32_t cF, bool parent);

std::string
dbFontDialog( std::string lpCaption, int x, int y, std::string lpFontname, uint32_t dwFontcolor, int iFontsize, int iStyle, bool parent);

*/
