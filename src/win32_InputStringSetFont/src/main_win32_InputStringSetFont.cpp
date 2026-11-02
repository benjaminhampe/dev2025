#include <de/win32_PromptString.h>
#include <de/win32_MessageBox.h>

#include <iostream>
#include <sstream>

int main(int argc, char* argv[])
{
    de::PromptStringParamsW params;
    params.title = L"H3_PresetName";
    params.prompt = L"Insert a preset name";
    params.fontSize = 36;

    std::wstring userInput = dbPromptString( params );

    std::wcout << L"PresetName = " << userInput << std::endl;

    if (!userInput.empty())
    {
        std::wostringstream o;
        o << L"PresetName = " << userInput;
        dbMessageBox( o.str(), L"H3_PresetName Input Result");
    }
    else
    {
        dbMessageBox( L"Eingabe abgebrochen.", L"Ergebnis");
    }
    return 0;
}
