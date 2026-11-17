#pragma once
#include <de/gpu/Uniform.h>

namespace de {
namespace gpu {

// ===========================================================================
struct Shader
// ===========================================================================
{
    uint32_t id = 0;
    std::string name;
    Uniforms uniforms;

    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setUInt(const std::string& name, uint32_t value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec1f(const std::string& name, float value) const;
    void setVec2f(const std::string& name, glm::vec2 const & value ) const;
    void setVec3f(const std::string& name, glm::vec3 const & value ) const;
    void setVec4f(const std::string& name, glm::vec4 const & value ) const;
    void setMat2f(const std::string& name, glm::mat2 const & mat) const;
    void setMat3f(const std::string& name, glm::mat3 const & mat) const;
    void setMat4f(const std::string& name, glm::mat4 const & mat) const;
};

/*
inline std::string
GT_getShaderHeaderVersionString()
{
    return "#version 330 core\n";
}

void
GT_init();
*/

std::string
GT_getShaderTypeStr( uint32_t shaderType );

uint32_t
GT_compileShader(
    const std::string& shaderName,
    uint32_t shaderType,
    const std::string& srcText,
    bool debug );

uint32_t
GT_createShader(
    const std::string& shaderName,
    const std::string& vsText,
    const std::string& fsText,
    bool debug );



} // end namespace gpu.
} // end namespace de.
