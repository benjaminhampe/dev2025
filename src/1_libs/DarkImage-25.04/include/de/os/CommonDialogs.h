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

namespace de {

// ===========================================================================
struct BrowseFolderParamsA
// ===========================================================================
{
    std::string caption = "Browse for Folder";
    std::string initDir;
    int w = -1;
    int h = -1;
    int x = -1;
    int y = -1;
    bool newui = true;
};

// ===========================================================================
struct OpenFileParamsA
// ===========================================================================
{
    std::string caption = "Open file dialog";
    std::string filter = "All Files (*.*)\0*.*\0";
    std::string initFileName; // Untitled.txt
    std::string initDir;
    int x = -1;
    int y = -1;
    int w = -1;
    int h = -1;
    bool newui = true;
    bool multiSelect = false;
};

// ===========================================================================
struct OpenFileParamsW
// ===========================================================================
{
    std::wstring caption = L"Open file dialog";
    std::wstring filter = L"All Files (*.*)\0*.*\0";
    std::wstring initFileName; // Untitled.txt
    std::wstring initDir;
    int x = -1;
    int y = -1;
    int w = -1;
    int h = -1;
    bool newui = true;
    bool multiSelect = false;
};

//            "Bitmap (*.bmp)\0*.bmp\0"
//            "Portable Network Graphic (*.png)\0*.png\0"
//            "JPEG (*.jpg)\0*.jpg\0"
//            "Graphic interchange format (*.gif)\0*.gif\0"
//            "TrueVision (*.tga)\0*.tga\0"
//            "DDS (*.dds)\0*.dds\0"
//            "Tif (*.tif)\0*.tif\0"
//            "\0",

// ===========================================================================
struct SaveFileParamsA
// ===========================================================================
{
    std::string caption = "Save file dialog";
    std::string filter = "All Files (*.*)\0*.*\0\0"; // "All files (*.*)|*.*"
    std::string initFileName = "Untitled.txt";
    std::string initDir;
    int x = -1;
    int y = -1;
    int w = -1;
    int h = -1;
    bool newui = true;
};

// ===========================================================================
struct SaveFileParamsW
// ===========================================================================
{
    std::wstring caption = L"Save file dialog";
    std::wstring filter = L"All Files (*.*)\0*.*\0\0"; // "All files (*.*)|*.*"
    std::wstring initFileName = L"Untitled.txt";
    std::wstring initDir;
    int x = -1;
    int y = -1;
    int w = -1;
    int h = -1;
    bool newui = true;
};

} // namespace de


std::string dbOpenFolderDlg( de::BrowseFolderParamsA params );
std::string dbOpenFileDlg( de::OpenFileParamsA params );
std::wstring dbOpenFileDlg( de::OpenFileParamsW params );
std::string dbSaveFileDlg( de::SaveFileParamsA param );
std::wstring dbSaveFileDlg( de::SaveFileParamsW params );

// ===========================================================================

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
