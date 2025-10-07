/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2022 - Raw Material Software Limited

   JUCE is an open source library subject to commercial or open-source
   licensing.

   By using JUCE, you agree to the terms of both the JUCE 7 End-User License
   Agreement and JUCE Privacy Policy.

   End User License Agreement: www.juce.com/juce-7-licence
   Privacy Policy: www.juce.com/juce-privacy-policy

   Or: You may also use this code under the terms of the GPL v3 (see
   www.gnu.org/licenses).

   JUCE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
   DISCLAIMED.

  ==============================================================================
*/
#include <juce_opengl/opengl/juce_OpenGLShaderProgram.h>
#include <juce_opengl/opengl/juce_OpenGLContext.h>
#include <juce_opengl/opengl/juce_checkGLError.h>
#include <juce_opengl/opengl/juce_OpenGLHelpers.h>

#include <de_opengl.h>
// #if _WIN32
// #include <GL/wglew.h>
// #endif

namespace juce
{

OpenGLShaderProgram::OpenGLShaderProgram (const OpenGLContext& c) noexcept  : context (c)
{
}

OpenGLShaderProgram::~OpenGLShaderProgram() noexcept
{
    release();
}

GLuint OpenGLShaderProgram::getProgramID() const noexcept
{
    if (programID == 0)
    {
        // This method should only be called when the current thread has an active OpenGL context.
        jassert (OpenGLHelpers::isContextActive());

        glCreateProgram();
    }

    return programID;
}

void OpenGLShaderProgram::release() noexcept
{
    if (programID != 0)
    {
        glDeleteProgram (programID);
        programID = 0;
    }
}

double OpenGLShaderProgram::getLanguageVersion()
{
    return String::fromUTF8 ((const char*) glGetString (GL_SHADING_LANGUAGE_VERSION))
            .retainCharacters("1234567890.").getDoubleValue();
}

bool OpenGLShaderProgram::addShader (const String& code, GLenum type)
{
    GLuint shaderID = glCreateShader (type);

    const GLchar* c = code.toRawUTF8();
    glShaderSource (shaderID, 1, &c, nullptr);

    glCompileShader (shaderID);

    GLint status = GL_FALSE;
    glGetShaderiv (shaderID, GL_COMPILE_STATUS, &status);

    if (status == (GLint) GL_FALSE)
    {
        std::vector<GLchar> infoLog (16384);
        GLsizei infoLogLength = 0;
        glGetShaderInfoLog (shaderID, (GLsizei) infoLog.size(), &infoLogLength, infoLog.data());
        errorLog = String (infoLog.data(), (size_t) infoLogLength);

       #if JUCE_DEBUG && ! JUCE_DONT_ASSERT_ON_GLSL_COMPILE_ERROR
        // Your GLSL code contained compile errors!
        // Hopefully this compile log should help to explain what went wrong.
        DBG (errorLog);
        jassertfalse;
       #endif

        return false;
    }

    glAttachShader (getProgramID(), shaderID);
    glDeleteShader (shaderID);
    JUCE_CHECK_OPENGL_ERROR
    return true;
}

bool OpenGLShaderProgram::addVertexShader (const String& code)    { return addShader (code, GL_VERTEX_SHADER); }
bool OpenGLShaderProgram::addFragmentShader (const String& code)  { return addShader (code, GL_FRAGMENT_SHADER); }

bool OpenGLShaderProgram::link() noexcept
{
    // This method can only be used when the current thread has an active OpenGL context.
    jassert (OpenGLHelpers::isContextActive());

    GLuint progID = getProgramID();

    glLinkProgram (progID);

    GLint status = GL_FALSE;
    glGetProgramiv (progID, GL_LINK_STATUS, &status);

    if (status == (GLint) GL_FALSE)
    {
        std::vector<GLchar> infoLog (16384);
        GLsizei infoLogLength = 0;
        glGetProgramInfoLog (progID, (GLsizei) infoLog.size(), &infoLogLength, infoLog.data());
        errorLog = String (infoLog.data(), (size_t) infoLogLength);

       #if JUCE_DEBUG && ! JUCE_DONT_ASSERT_ON_GLSL_COMPILE_ERROR
        // Your GLSL code contained link errors!
        // Hopefully this compile log should help to explain what went wrong.
        DBG (errorLog);
        jassertfalse;
       #endif
    }

    JUCE_CHECK_OPENGL_ERROR
    return status != (GLint) GL_FALSE;
}

void OpenGLShaderProgram::use() const noexcept
{
    // The shader program must have been successfully linked when this method is called!
    jassert (programID != 0);

    glUseProgram (programID);
}

GLint OpenGLShaderProgram::getUniformIDFromName (const char* uniformName) const noexcept
{
    // The shader program must be active when this method is called!
    jassert (programID != 0);

    return (GLint) glGetUniformLocation (programID, uniformName);
}

void OpenGLShaderProgram::setUniform (const char* name, GLfloat n1) noexcept                                       { glUniform1f  (getUniformIDFromName (name), n1); }
void OpenGLShaderProgram::setUniform (const char* name, GLint n1) noexcept                                         { glUniform1i  (getUniformIDFromName (name), n1); }
void OpenGLShaderProgram::setUniform (const char* name, GLfloat n1, GLfloat n2) noexcept                           { glUniform2f  (getUniformIDFromName (name), n1, n2); }
void OpenGLShaderProgram::setUniform (const char* name, GLfloat n1, GLfloat n2, GLfloat n3) noexcept               { glUniform3f  (getUniformIDFromName (name), n1, n2, n3); }
void OpenGLShaderProgram::setUniform (const char* name, GLfloat n1, GLfloat n2, GLfloat n3, GLfloat n4) noexcept   { glUniform4f  (getUniformIDFromName (name), n1, n2, n3, n4); }
void OpenGLShaderProgram::setUniform (const char* name, GLint n1, GLint n2, GLint n3, GLint n4) noexcept           { glUniform4i  (getUniformIDFromName (name), n1, n2, n3, n4); }
void OpenGLShaderProgram::setUniform (const char* name, const GLfloat* values, GLsizei numValues) noexcept         { glUniform1fv (getUniformIDFromName (name), numValues, values); }
void OpenGLShaderProgram::setUniformMat2 (const char* name, const GLfloat* v, GLint num, GLboolean trns) noexcept  { glUniformMatrix2fv (getUniformIDFromName (name), num, trns, v); }
void OpenGLShaderProgram::setUniformMat3 (const char* name, const GLfloat* v, GLint num, GLboolean trns) noexcept  { glUniformMatrix3fv (getUniformIDFromName (name), num, trns, v); }
void OpenGLShaderProgram::setUniformMat4 (const char* name, const GLfloat* v, GLint num, GLboolean trns) noexcept  { glUniformMatrix4fv (getUniformIDFromName (name), num, trns, v); }

//==============================================================================
OpenGLShaderProgram::Attribute::Attribute (const OpenGLShaderProgram& program, const char* name)
    : attributeID ((GLuint)glGetAttribLocation (program.getProgramID(), name))
{
   #if JUCE_DEBUG && ! JUCE_DONT_ASSERT_ON_GLSL_COMPILE_ERROR
    jassert ((GLint) attributeID >= 0);
   #endif
}

//==============================================================================
OpenGLShaderProgram::Uniform::Uniform (const OpenGLShaderProgram& program, const char* const name)
    : uniformID (glGetUniformLocation (program.getProgramID(), name))
    , context (program.context)
{
   #if JUCE_DEBUG && ! JUCE_DONT_ASSERT_ON_GLSL_COMPILE_ERROR
    jassert (uniformID >= 0);
   #endif
}

void OpenGLShaderProgram::Uniform::set (GLfloat n1) const noexcept                                    { glUniform1f (uniformID, n1); }
void OpenGLShaderProgram::Uniform::set (GLint n1) const noexcept                                      { glUniform1i (uniformID, n1); }
void OpenGLShaderProgram::Uniform::set (GLfloat n1, GLfloat n2) const noexcept                        { glUniform2f (uniformID, n1, n2); }
void OpenGLShaderProgram::Uniform::set (GLfloat n1, GLfloat n2, GLfloat n3) const noexcept            { glUniform3f (uniformID, n1, n2, n3); }
void OpenGLShaderProgram::Uniform::set (GLfloat n1, GLfloat n2, GLfloat n3, GLfloat n4) const noexcept  { glUniform4f (uniformID, n1, n2, n3, n4); }
void OpenGLShaderProgram::Uniform::set (GLint n1, GLint n2, GLint n3, GLint n4) const noexcept        { glUniform4i (uniformID, n1, n2, n3, n4); }
void OpenGLShaderProgram::Uniform::set (const GLfloat* values, GLsizei numValues) const noexcept      { glUniform1fv (uniformID, numValues, values); }

void OpenGLShaderProgram::Uniform::setMatrix2 (const GLfloat* v, GLint num, GLboolean trns) const noexcept { glUniformMatrix2fv (uniformID, num, trns, v); }
void OpenGLShaderProgram::Uniform::setMatrix3 (const GLfloat* v, GLint num, GLboolean trns) const noexcept { glUniformMatrix3fv (uniformID, num, trns, v); }
void OpenGLShaderProgram::Uniform::setMatrix4 (const GLfloat* v, GLint num, GLboolean trns) const noexcept { glUniformMatrix4fv (uniformID, num, trns, v); }

} // namespace juce
