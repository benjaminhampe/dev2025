#pragma once
#include <de/gpu/VideoDriver.h>

namespace de {
namespace gpu {

std::string UniformType_str(uint32_t uniformType);

// Struktur, die alle verfügbaren Daten über eine Uniform hält
struct Uniform
{
    std::string name;
    int32_t location;
    uint32_t type;       // z.B. GL_FLOAT, GL_FLOAT_VEC3, GL_SAMPLER_2D
    int32_t size;        // Elementanzahl ( > 1 falls es ein Array ist)
    int32_t blockIndex;  // -1 falls Standard-Uniform, ansonsten Index des Uniform-Blocks (UBO)

    std::string str() const;
};

typedef std::unordered_map<std::string, Uniform> Uniforms;

Uniforms
enumerateUniforms(uint32_t programID);

} // end namespace gpu.
} // end namespace de.
