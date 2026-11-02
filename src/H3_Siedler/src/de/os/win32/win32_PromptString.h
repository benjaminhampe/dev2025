#pragma once
#include <string>

namespace de {

struct PromptStringParamsW
{
    int fontSize = 28;
    std::wstring title;
    std::wstring description;
    std::wstring initPrompt;
    void* parent = nullptr;
};

} // end namespace de.

std::wstring dbPromptString( const de::PromptStringParamsW& params );
