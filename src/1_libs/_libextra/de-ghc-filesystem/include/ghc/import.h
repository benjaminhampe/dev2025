#pragma once
// clang-format off

/*
** This file imports filesystem and sets the fs namespace to the correct
** thing. In a distant future where we are compiling with all our compilers
** and sdks set up to have std::filesystem this will go away. But while still
** supporting VS2017 and macos 10.12 and stuff we need it.
*/
// generate_header FS_HEADER FS_NAMESPACE FS_DEFINE
// generate_header("\"filesystem/ghc-filesystem.h\"" "ghc::filesystem" "SST_PLUGINFRA_GHC_FS")    
// generate_header("<filesystem>" "std::filesystem" "SST_PLUGINFRA_PLATFORM_FS")

#include @FS_HEADER@
namespace fs = @FS_NAMESPACE@;

#define @FS_DEFINE@ 1

#include <utility>

inline std::string path_to_string(const fs::path& path)
{
#ifdef _WIN32
   return path.u8string();
#else
   return path.generic_string();
#endif
}

template<typename T>
inline fs::path string_to_path(T&& path)
{
#ifdef _WIN32
   return fs::u8path(std::forward<T>(path));
#else
   return fs::path(std::forward<T>(path));
#endif
}

void string_to_path(fs::path) = delete;

// clang-format on