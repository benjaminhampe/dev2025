/*
** This file is rebuilt and substitited every time you run a build.
** Things which need to be per-build should be defined here, declared
** in the version.h header, and then used wherever you want
*/
#include "version.h"

// clang-format off
namespace Surge
{
   const char* Build::MajorVersionStr = "1";
   const int   Build::MajorVersionInt = 1;
   
   const char* Build::SubVersionStr = "3";
   const int   Build::SubVersionInt = 3;
   
   const char* Build::ReleaseNumberStr = "3";
   const char* Build::ReleaseStr = "3";

   const bool Build::IsRelease = true;
   const bool Build::IsNightly = ! Build::IsRelease;

   const char* Build::BuildNumberStr = "Benni"; // Build number to be sure that each result could identified.
   
   const char* Build::FullVersionStr = "1.3.3";
   const char* Build::BuildHost = "Benni's MinGW-llvm-1706";
   const char* Build::BuildArch = "Win64";
   const char *Build::BuildCompiler = "Clang++";

   const char* Build::BuildLocation = "YourMomsHome";

   const char* Build::BuildDate = "27.09.2025";
   const char* Build::BuildTime = "23:27";
   const char* Build::BuildYear = "2025";

   const char* Build::GitHash = "Nope";
   const char* Build::GitBranch = "Main";

   const char* Build::CMAKE_INSTALL_PREFIX = "";
}
// clang-format on
