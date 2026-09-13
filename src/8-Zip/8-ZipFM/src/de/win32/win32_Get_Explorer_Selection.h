#pragma once
#include <vector>
#include <string>

struct AutoCoInitialize
{
    AutoCoInitialize();
    ~AutoCoInitialize();
};

std::vector<std::wstring> win32_Get_Explorer_Selection();
