#pragma once
#include "Matrix3D.h"

// ==========================================================
class GL_Spectrum3D // : protected QOpenGLFunctions_4_5_Core
// ==========================================================
{
    typedef float T;
    typedef glm::vec3 V3;
    typedef glm::mat4 M4;

    //de::gpu::TexManager* m_texManager;
    de::gpu::VideoDriver* m_driver;

    //bool m_bRenderingEnabled;
    bool m_showFftMatrix3D;

    GL_Mesh16_Shader m_mesh16Shader;
    GL_Mesh16_Material m_mesh16Material;
    Matrix3D m_matrix_fft;

    // de::LinearColorGradient m_wav_colorGradient;
    // de::LinearColorGradient m_fft_colorGradient;


    // DE_AlignedFloatVector m_front; // work buffers
    // DE_AlignedFloatVector m_left;
    // DE_AlignedFloatVector m_right;
    // DE_AlignedFloatVector m_back;

public:
    GL_Spectrum3D();
    ~GL_Spectrum3D(); // override;

    // void setRenderingEnabled( bool bEnabled )
    // {
    //     m_bRenderingEnabled = bEnabled;
    // }

    void setVisibleFftMatrix( bool bVisible )
    {
        m_showFftMatrix3D = bVisible;
    }

    void initializeGL(de::gpu::VideoDriver* driver);
    void paintGL();

};
