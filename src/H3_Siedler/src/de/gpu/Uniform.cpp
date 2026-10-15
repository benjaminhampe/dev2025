#include <de/gpu/Uniform.h>
#include <de_opengl.h>

namespace de {
namespace gpu {

std::string UniformType_str(uint32_t uniformType)
{
    switch (uniformType)
    {
        // Scalars
        case GL_FLOAT:                         return "float";
        case GL_INT:                           return "int";
        case GL_UNSIGNED_INT:                  return "unsigned int";
        case GL_BOOL:                          return "bool";
        case GL_DOUBLE:                        return "double";

        // Vectors
        case GL_FLOAT_VEC2:                    return "vec2";
        case GL_FLOAT_VEC3:                    return "vec3";
        case GL_FLOAT_VEC4:                    return "vec4";
        case GL_INT_VEC2:                      return "ivec2";
        case GL_INT_VEC3:                      return "ivec3";
        case GL_INT_VEC4:                      return "ivec4";
        case GL_UNSIGNED_INT_VEC2:             return "uvec2";
        case GL_UNSIGNED_INT_VEC3:             return "uvec3";
        case GL_UNSIGNED_INT_VEC4:             return "uvec4";
        case GL_BOOL_VEC2:                     return "bvec2";
        case GL_BOOL_VEC3:                     return "bvec3";
        case GL_BOOL_VEC4:                     return "bvec4";

        // Matrices
        case GL_FLOAT_MAT2:                    return "mat2";
        case GL_FLOAT_MAT3:                    return "mat3";
        case GL_FLOAT_MAT4:                    return "mat4";
        case GL_FLOAT_MAT2x3:                  return "mat2x3";
        case GL_FLOAT_MAT2x4:                  return "mat2x4";
        case GL_FLOAT_MAT3x2:                  return "mat3x2";
        case GL_FLOAT_MAT3x4:                  return "mat3x4";
        case GL_FLOAT_MAT4x2:                  return "mat4x2";
        case GL_FLOAT_MAT4x3:                  return "mat4x3";

        // Samplers (Textures)
        case GL_SAMPLER_1D:                    return "sampler1D";
        case GL_SAMPLER_2D:                    return "sampler2D";
        case GL_SAMPLER_3D:                    return "sampler3D";
        case GL_SAMPLER_CUBE:                  return "samplerCube";
        case GL_SAMPLER_1D_SHADOW:             return "sampler1DShadow";
        case GL_SAMPLER_2D_SHADOW:             return "sampler2DShadow";
        case GL_SAMPLER_1D_ARRAY:              return "sampler1DArray";
        case GL_SAMPLER_2D_ARRAY:              return "sampler2DArray";
        case GL_SAMPLER_1D_ARRAY_SHADOW:       return "sampler1DArrayShadow";
        case GL_SAMPLER_2D_ARRAY_SHADOW:       return "sampler2DArrayShadow";
        case GL_SAMPLER_CUBE_SHADOW:           return "samplerCubeShadow";

        case GL_INT_SAMPLER_1D:                return "isampler1D";
        case GL_INT_SAMPLER_2D:                return "isampler2D";
        case GL_INT_SAMPLER_3D:                return "isampler3D";
        case GL_INT_SAMPLER_CUBE:              return "isamplerCube";
        case GL_INT_SAMPLER_1D_ARRAY:          return "isampler1DArray";
        case GL_INT_SAMPLER_2D_ARRAY:          return "isampler2DArray";

        case GL_UNSIGNED_INT_SAMPLER_1D:       return "usampler1D";
        case GL_UNSIGNED_INT_SAMPLER_2D:       return "usampler2D";
        case GL_UNSIGNED_INT_SAMPLER_3D:       return "usampler3D";
        case GL_UNSIGNED_INT_SAMPLER_CUBE:     return "usamplerCube";
        case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY: return "usampler1DArray";
        case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY: return "usampler2DArray";

        // Images (Compute Shaders)
        case GL_IMAGE_2D:                      return "image2D";
        case GL_INT_IMAGE_2D:                  return "iimage2D";
        case GL_UNSIGNED_INT_IMAGE_2D:         return "uimage2D";

        default:                               return "UNKNOWN_TYPE";
    }
}


std::string Uniform::str() const
{
    std::ostringstream o; o <<
    "name(" << name << "), "
    "location(" << location << "), "
    "type(" << type << "), "
    "size(" << size << "), "
    "blockIndex(" << blockIndex << ")";
    return o.str();
}

Uniforms
enumerateUniforms(uint32_t programID)
{
    Uniforms uniforms;

    // 1. Anzahl der aktiven Uniforms und die maximale Namenslänge abfragen
    GLint activeUniforms = 0;
    GLint maxNameLength = 0;
    glGetProgramiv(programID, GL_ACTIVE_UNIFORMS, &activeUniforms);
    glGetProgramiv(programID, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxNameLength);

    if (activeUniforms <= 0 || maxNameLength <= 0)
    {
        return uniforms;
    }

    // Buffer für den Namen bereitstellen
    std::vector<GLchar> nameBuffer(maxNameLength);

    // 2. Jede einzelne Uniform via Index abfragen
    for (GLint i = 0; i < activeUniforms; ++i)
    {
        // Basis-Metadaten holen (Name, Typ, Array-Größe)
        GLsizei length = 0;
        GLint size = 0;
        GLenum type = 0;
        glGetActiveUniform(programID, i, maxNameLength, &length, &size, &type, nameBuffer.data());

        std::string uniformName(nameBuffer.data(), length);

        // Die tatsächliche Location im Shader abfragen
        // Hinweis: Uniform-Blöcke (UBOs) liefern hier -1, was korrekt ist.
        GLint location = glGetUniformLocation(programID, uniformName.c_str());

        // Optional: Prüfen, ob die Uniform zu einem Uniform-Block (UBO) gehört
        GLint blockIndex = -1;
        glGetActiveUniformsiv(programID, 1, reinterpret_cast<GLuint*>(&i), GL_UNIFORM_BLOCK_INDEX, &blockIndex);

        // In die Map einfügen
        Uniform u;
        u.name = uniformName;
        u.location = location;
        u.type = type;
        u.size = size;
        u.blockIndex = blockIndex;

        uniforms[uniformName] = u;
    }

    return uniforms;
}

} // end namespace gpu.
} // end namespace de.
