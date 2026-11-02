#pragma once
#include <string>

namespace de {

struct PromptStringParamsW
{
    int fontSize = 28;
    std::wstring title;
    std::wstring description;
    std::wstring inittext;
    void* parent = nullptr;
};

} // end namespace de.

std::wstring dbPromptString( const de::PromptStringParamsW& params );
