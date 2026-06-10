#pragma once
#include <DarkImage.h>

#if 0

inline std::string
GT_getShaderHeaderVersionString()
{
    return "#version 330 core\n";
}

void
GT_init();

std::string
GT_getShaderTypeStr( uint32_t shaderType );

uint32_t
GT_compileShader(
    const std::string& shaderName,
    uint32_t shaderType,
    const std::string& srcText);

uint32_t
GT_createShader(
    const std::string& shaderName,
    const std::string& vsText,
    const std::string& fsText );

#endif
