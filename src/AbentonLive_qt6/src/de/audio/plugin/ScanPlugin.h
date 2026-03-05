#pragma once
#include <vector>
#include <sstream>
#include <QFileInfo>
#include <QDirIterator>

namespace de {
namespace audio {

struct PluginInfo;

std::vector< PluginInfo* >
scanPluginFolder( QString vstDir, bool recursive );

PluginInfo*
scanPlugin( std::wstring const & uri );

int
vstGetParamCount( void* vst, int32_t i );

float
vstGetParamValue( void* vst, int32_t i );

std::string
vstGetParamName( void* vst, int32_t i, std::vector< char > & buf );

std::string
vstGetParamLabel( void* vst, int32_t i, std::vector< char > & buf );

std::string
vstGetParamDisplay( void* vst, int32_t i, std::vector< char > & buf );


} // end namespace audio
} // end namespace de
