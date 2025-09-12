#pragma once
#include <GL/glew.h>
#include <de_glm.hpp>
// libDeutschland
#include <DarkImage.h>
#include <de/gpu/Camera.h>
#include <de/gpu/GPU.h>
#include <de/gpu/smesh/SMeshLibrary.h>
#include <de/gpu/GL_debug_layer.h>

namespace de {
namespace gl {

typedef gpu::SO SO;

// =========================================================================
struct Tex
// =========================================================================
{
    uint32_t textureId = 0;
    uint32_t targetTyp = 0;
    uint32_t samplerId = 0;
    int32_t boundUnit = -1;
    std::string name;
};

// GLSL Type	GLenum Constant             Description
// float            GL_FLOAT                    Single float
// vec2             GL_FLOAT_VEC2               2-component float vector
// vec3             GL_FLOAT_VEC3               3-component float vector
// vec4             GL_FLOAT_VEC4               4-component float vector
// int              GL_INT                      Single integer
// ivec2            GL_INT_VEC2                 2-component integer vector
// ivec3            GL_INT_VEC3                 3-component integer vector
// ivec4            GL_INT_VEC4                 4-component integer vector
// uint             GL_UNSIGNED_INT             Unsigned integer
// uvec2            GL_UNSIGNED_INT_VEC2        2-component unsigned int vector
// uvec3            GL_UNSIGNED_INT_VEC3        3-component unsigned int vector
// uvec4            GL_UNSIGNED_INT_VEC4        4-component unsigned int vector
// bool             GL_BOOL                     Boolean value
// bvec2            GL_BOOL_VEC2                2-component boolean vector
// bvec3            GL_BOOL_VEC3                3-component boolean vector
// bvec4            GL_BOOL_VEC4                4-component boolean vector
// mat2             GL_FLOAT_MAT2               2×2 float matrix
// mat3             GL_FLOAT_MAT3               3×3 float matrix
// mat4             GL_FLOAT_MAT4               4×4 float matrix
// mat2x3           GL_FLOAT_MAT2x3             2×3 float matrix
// mat2x4           GL_FLOAT_MAT2x4             2×4 float matrix
// mat3x2           GL_FLOAT_MAT3x2             3×2 float matrix
// mat3x4           GL_FLOAT_MAT3x4             3×4 float matrix
// mat4x2           GL_FLOAT_MAT4x2             4×2 float matrix
// mat4x3           GL_FLOAT_MAT4x3             4×3 float matrix
// sampler2D        GL_SAMPLER_2D               2D texture sampler
// samplerCube      GL_SAMPLER_CUBE             Cube map texture sampler
// sampler3D        GL_SAMPLER_3D               3D texture sampler
// sampler2DShadow	GL_SAMPLER_2D_SHADOW        Depth comparison sampler
// image2D          GL_IMAGE_2D                 Image load/store access
// atomic_uint      GL_UNSIGNED_INT_ATOMIC_COUNTER	Atomic counter



// =========================================================================
struct Uniform
// =========================================================================
{
    std::string name;
    int32_t length;
    int32_t size;
    uint32_t type;
    int32_t location;

    Uniform()
        : name()
        , length(0)
        , size(0)
        , type(0)
        , location(-1)
    {

    }

    //glGetActiveUniform(programID, i, sizeof(name), &length, &size, &type, name);

    //GLint location = glGetUniformLocation(programID, name);

};

// =========================================================================
struct Uniforms
// =========================================================================
{
    std::unordered_map<std::string, Uniform> m_uniforms;
};

// =========================================================================
struct Shader
// =========================================================================
{
    uint32_t programId = 0;
    std::string name;

    std::unordered_map<std::string, Uniform> m_uniforms;



    void queryUniforms()
    {
        m_uniforms.clear();

        if (!programId)
        {
            DE_ERROR("No shader")
            return;
        }

        GLint numUniforms = 0;
        glGetProgramiv(programId, GL_ACTIVE_UNIFORMS, &numUniforms);

        for (GLint i = 0; i < numUniforms; ++i)
        {
            char nameBuf[256] = { 0 };
            GLsizei length = 0;
            GLint size = 0;
            GLenum type = 0;
            glGetActiveUniform(programId, i, 256, &length, &size, &type, nameBuf);

            GLint location = glGetUniformLocation(programId, nameBuf);

            std::string name = nameBuf;

            auto it = m_uniforms.find(name);
            if (it != m_uniforms.end())
            {
                DE_ERROR("Already got uniform ", name)
            }

            Uniform uniform;
            uniform.name = name;
            uniform.length = length;
            uniform.size = size;
            uniform.type = type;
            uniform.location = location;
            m_uniforms[name] = uniform;

            DE_DEBUG("Uniform[",i,"] ",name," (Location: ",location,", Type: ",type,")")
        }

    }

};

/*

GLint numUniforms = 0;
glGetProgramiv(programID, GL_ACTIVE_UNIFORMS, &numUniforms);

for (GLint i = 0; i < numUniforms; ++i)
{
    char name[256];
    GLsizei length;
    GLint size;
    GLenum type;
    glGetActiveUniform(programID, i, sizeof(name), &length, &size, &type, name);

    GLint location = glGetUniformLocation(programID, name);
    std::cout << "Uniform #" << i << ": " << name << " (Location: " << location << ", Type: " << type << ")\n";
}


GLint numAttribs = 0;
glGetProgramiv(programID, GL_ACTIVE_ATTRIBUTES, &numAttribs);

for (GLint i = 0; i < numAttribs; ++i) {
    char name[256];
    GLsizei length;
    GLint size;
    GLenum type;
    glGetActiveAttrib(programID, i, sizeof(name), &length, &size, &type, name);

    GLint location = glGetAttribLocation(programID, name);
    std::cout << "Attribute #" << i << ": " << name << " (Location: " << location << ", Type: " << type << ")\n";
}

*/

// =========================================================================
class Bridge // OpenGL context, renderstates, camera, lights
// =========================================================================
{
public:
    void init(); // Push state
    void restore(); // Pop state.
    void useProgram( uint32_t programId );
    void useTexture( const Tex& texHandle, int stage );
    uint32_t getMaxTessGenLevel() const { return m_maxTessGenLevel; }
    uint32_t getMaxTex2DSize() const { return m_maxTex2DSize; }
    float getMaxAnisotropy() const { return m_maxAnisotropy; }
    Tex createTex2D( const std::string& name, int unit, const Image& img, const SO& so ) const;
    bool uploadTex2D( int unit, Tex& tex, const Image& img, const SO& so ) const;
    bool uploadTex2D( int unit, Tex& tex, int w, int h, void const* const pixels, PixelFormat fmt, const SO& so ) const;

    gpu::Camera* getCamera() { return &m_camera; }
    gpu::Lights* getLights() { return &m_lights; }

private:
    gpu::Lights m_lights;
    gpu::Camera m_camera;

    int32_t m_shaderVersionMajor = 0;
    int32_t m_shaderVersionMinor = 0;

    uint32_t m_initialProgramId = 0;
    uint32_t m_currentProgramId = 0;

    uint32_t m_maxTessGenLevel = 0;
    uint32_t m_maxTex2DSize = 0;
    float m_maxAnisotropy = 0.0f;
    // float m_initialAnisotropy = 0.0f;
    // float m_currentAnisotropy = 0.0f;

    std::vector<Tex> m_initialTexUnits;
    std::vector<Tex> m_currentTexUnits;

    gpu::Culling m_initialCullMode;
    gpu::Culling m_currentCullMode;

    gpu::Depth m_initialDepthTest;
    gpu::Depth m_currentDepthTest;

    gpu::Blend m_initialBlendMode;
    gpu::Blend m_currentBlendMode;

protected:
    void applySamplerOptions( uint32_t & samplerId, const SO& so ) const;

    void applyTexParameters( const SO& so ) const;

    static uint32_t
    convertTexTargetToBindingEnum(uint32_t target);
};

} // namespace gl
} // namespace de
