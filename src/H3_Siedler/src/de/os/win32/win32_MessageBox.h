#pragma once
#include <string>

namespace de {

struct MessageBoxParamsW
{
    int fontSize = 28;
    std::wstring title;
    std::wstring text;
    void* parent = nullptr;
};

} // end namespace de.

void dbMessageBox( const de::MessageBoxParamsW& params );

void dbMessageBox( const std::wstring& title, const std::wstring& text);

bool dbQuestionBox(const std::wstring& title, const std::wstring& text);
