#pragma once
#include <de/Core.h> // dbStrW(), split()

void LogEvent(const std::wstring& msg);

void LogEvents(const StringListW& msg);

std::wstring EightZip_Registry_readExePath();

void EightZip_runCompressor(const StringListW& files);

void EightZip_runExtractor(const StringListW& files);


bool IsArchive(const std::wstring& uri);

StringListW GetFilteredArchives(const StringListW& inputList);

// 🧩 Step 2 — Classify selection
/*
StringListW EightZip_classifySelection(
    const StringListW& inputList,
    uint32_t& numDirs,
    uint32_t& numFiles,
    uint32_t& numArchives );
*/
