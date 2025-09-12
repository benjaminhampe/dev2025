#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <de_glm.hpp>

// #include "imgui.h"
// #include "imgui_impl_glfw.h"
// #include "imgui_impl_opengl3.h"

// standard
// #include <iostream>
// #include <fstream>
#include <string>
#include <sstream> 
#include <vector>
//#include <stdio.h>
#include <cmath>
#include <algorithm>

// libDeutschland
#include <DarkImage.h>
#include <de/gpu/Camera.h>
#include <de/gpu/GPU.h>
#include <de/gpu/smesh/SMeshLibrary.h>
#include <de/gpu/GL_debug_layer.h>

// // core
// #include "core/Renderer.h" 
// #include "core/gl_util/OpenGLdebugFuncs.h"
// #include "core/camera/CameraHandler.hpp"

// // app
// #include "GUI/CameraComponentGUI.h"
// #include "GUI/DockspaceGUI.h"
// #include "GUI/PerformanceCounterGUI.h"
// #include "GUI/InspectorGUI.h"
// #include "GUI/SkyBoxGUI.h"
// #include "GUI/BVHsettingsGUI.h"

// #include "delta_lib/DeltaTime.h"
// #include "scenes/Scene1.hpp"


/*


struct Bridge
{
    GLuint m_initialProgramId = 0;
    GLuint m_currentProgramId = 0;

    struct TexState
    {
        GLuint textureId = 0;
        GLuint targetTyp = 0;
        GLuint samplerId = 0;
    };

    std::vector<TexState> m_initialTexUnits;
    std::vector<TexState> m_currentTexUnits;

    static GLenum
    convertTexTargetToBindingEnum(GLenum target)
    {
        switch (target)
        {
        case GL_TEXTURE_1D: return GL_TEXTURE_BINDING_1D;
        case GL_TEXTURE_2D: return GL_TEXTURE_BINDING_2D;
        case GL_TEXTURE_3D: return GL_TEXTURE_BINDING_3D;
        case GL_TEXTURE_CUBE_MAP: return GL_TEXTURE_BINDING_CUBE_MAP;
        case GL_TEXTURE_1D_ARRAY: return GL_TEXTURE_BINDING_1D_ARRAY;
        case GL_TEXTURE_2D_ARRAY: return GL_TEXTURE_BINDING_2D_ARRAY;
        case GL_TEXTURE_RECTANGLE: return GL_TEXTURE_BINDING_RECTANGLE;
        case GL_TEXTURE_BUFFER: return GL_TEXTURE_BINDING_BUFFER;
        case GL_TEXTURE_CUBE_MAP_ARRAY: return GL_TEXTURE_BINDING_CUBE_MAP_ARRAY;
        case GL_TEXTURE_2D_MULTISAMPLE: return GL_TEXTURE_BINDING_2D_MULTISAMPLE;
        case GL_TEXTURE_2D_MULTISAMPLE_ARRAY: return GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY;
        default: return 0;
        }
    }

    void init() // Push state
    {
        GLint id = 0;
        glGetIntegerv(GL_CURRENT_PROGRAM, &id);
        m_initialProgramId = static_cast<GLuint>(id);
        m_currentProgramId = m_initialProgramId;

        GLint nTexUnits = 0;
        glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &nTexUnits);
        m_initialTexUnits.resize(nTexUnits);

        for (int unit = 0; unit < nTexUnits; ++unit)
        {
            for (GLenum target : {  GL_TEXTURE_1D,
                                    GL_TEXTURE_2D,
                                    GL_TEXTURE_3D,
                                    GL_TEXTURE_CUBE_MAP,
                                    GL_TEXTURE_1D_ARRAY,
                                    GL_TEXTURE_2D_ARRAY,
                                    GL_TEXTURE_RECTANGLE,
                                    GL_TEXTURE_BUFFER,
                                    GL_TEXTURE_CUBE_MAP_ARRAY,
                                    GL_TEXTURE_2D_MULTISAMPLE,
                                    GL_TEXTURE_2D_MULTISAMPLE_ARRAY })
            {
                GLint textureId = 0;
                glGetIntegeri_v(convertTexTargetToBindingEnum(target), unit, &textureId);
                if (textureId != 0)
                {
                    // We are not prepared for multiple textures bound at same tex-unit!
                    if (m_initialTexUnits[ unit ].textureId > 0
                     && m_initialTexUnits[ unit ].textureId != textureId)
                    {
                        DE_ERROR("TexUnit[",unit,"] had multiple target bound to a texture")
                        DE_ERROR("We are not prepared for multiple textures bound at same tex-unit!")
                        DE_ERROR("Restoring tex-units will fail, information loss!")
                    }

                    // Store (unit, target, texID)
                    m_initialTexUnits[ unit ].targetTyp = target;
                    m_initialTexUnits[ unit ].textureId = textureId;

                    GLint samplerId = 0;
                    glGetIntegeri_v(GL_SAMPLER_BINDING, unit, &samplerId);
                    m_initialTexUnits[ unit ].samplerId = samplerId;
                }
            }
        }

        m_currentTexUnits = m_initialTexUnits;
    }

    void restore() // Pop state.
    {
        // Rebuild initial state.
    }

    void useProgram( GLuint programId )
    {
        if (programId == m_currentProgramId)
        {
            return;
        }

        glUseProgram( programId );
        m_currentProgramId = programId;
    }

    void useTexture( GLuint textureId, GLuint samplerId, GLuint stage )
    {
        if (stage >= m_currentTexUnits.size())
        {
            DE_WARN("Invalid texUnit ", stage, " of ", m_currentTexUnits.size())
            stage = m_currentTexUnits.size() - 1;
        }

        auto & unit = m_currentTexUnits[ stage ];
        if (unit.textureId != textureId
         || unit.samplerId != samplerId)
        {
            glBindTextureUnit( stage, textureId );
            glBindSampler( stage, samplerId );
            unit.textureId = textureId;
            unit.samplerId = samplerId;
            DE_DEBUG("Bound texture(", textureId, ") and sampler(", samplerId, ")")
        }
    }

};

*/
