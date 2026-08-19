#pragma once
#include <cstdint>
#include <string>
#if defined(__APPLE__)
#  include <OpenGL/gl3.h> // defines OpenGL 3.0+ functions
#else
// Note: GLEW_STATIC is defined by CMake if the static lib is linked
#  include <GL/glew.h>
#endif

extern void add_output(const char *format, ...);

struct GL_Util
{
    static std::string
    getShaderTypeStr( uint32_t shaderType )
    {
        switch (shaderType)
        {
            case GL_VERTEX_SHADER: return "VS";
            case GL_FRAGMENT_SHADER: return "FS";
            case GL_GEOMETRY_SHADER: return "GS";
            case GL_TESS_CONTROL_SHADER: return "TCS";
            case GL_TESS_EVALUATION_SHADER: return "TES";
            case GL_COMPUTE_SHADER: return "CS";
            default: return "unknown";
        }
    }

    static uint32_t
    compileShader(const std::string& shaderName, uint32_t shaderType, const std::string& srcText)
    {
        auto shaderId = glCreateShader(shaderType);
        const char* pSrc = srcText.c_str();
        const GLint nSrc = srcText.size();
        glShaderSource(shaderId, 1, &pSrc, &nSrc);
        glCompileShader(shaderId);
        int ok = 0;
        char infoLog[512];
        glGetShaderiv(shaderId, GL_COMPILE_STATUS, &ok);
        if (!ok)
        {
            glGetShaderInfoLog(shaderId, 512, NULL, infoLog);

            const auto shaderTypeStr = getShaderTypeStr(shaderType);
            add_output("%s-SHADER[%s]::COMPILATION_FAILED:\n%s",
                        shaderTypeStr.c_str(),
                        shaderName.c_str(),
                        infoLog);
            // auto txt = de::StringUtil::prefixLineNumbers(srcText);
            // DE_DEBUG("\n", txt)
        }

        return shaderId;
    }

    static uint32_t
    createShader(const std::string& name,
                const std::string& vsText,
                const std::string& fsText )
    {
        uint32_t vsShaderId = compileShader(name, GL_VERTEX_SHADER, vsText);
        uint32_t fsShaderId = compileShader(name, GL_FRAGMENT_SHADER, fsText);
        uint32_t programId = glCreateProgram();
        glAttachShader(programId, vsShaderId);
        glAttachShader(programId, fsShaderId);
        glLinkProgram(programId);
        int ok = 0;
        char infoLog[512];
        glGetProgramiv(programId, GL_LINK_STATUS, &ok);
        if (!ok)
        {
            glGetProgramInfoLog(programId, 512, NULL, infoLog);
            add_output("SHADER[%s]::LINKING_FAILED:\n%s",
                        name.c_str(),
                        infoLog);
        }

        glDeleteShader(vsShaderId);
        glDeleteShader(fsShaderId);
        return programId;
    }
};

struct GL_Driver
{
    int m_glsl_version_major = 3;
    int m_glsl_version_minor = 3;

    void init()
    {
        auto text = (char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
        sscanf(text, "%d.%d", &m_glsl_version_major, &m_glsl_version_minor);
        add_output("GLSL Version = %d.%d\n",m_glsl_version_major, m_glsl_version_minor);

        auto t2 = glGetString(GL_VERSION);
        add_output("GL_VERSION = %s\n", t2);

        // sscanf((const char *)glv, "%d", &gl_version_major);
        // if (gl_version_major < 3)
        // {
        //     add_output("\nThis platform does not support OpenGL V3 :\n"
        //     "FLTK widgets will appear but the programmed "
        //     "rendering pipeline will not run.\n");
        //     mode(mode() & ~FL_OPENGL3);
        // }
    }
};
