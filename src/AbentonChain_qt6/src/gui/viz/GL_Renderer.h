#pragma once
#include <gui/viz/create2DWav.h>

// This class exists to split QtOpenGL from our custom GLEW rendering.
// Qt hates our GLEW headers and pollutes the console with warnings.

// ==========================================================
class GL_Renderer
// ==========================================================
{
    typedef float T;
    typedef glm::vec3 V3;
    typedef glm::mat4 M4;

    //de::gpu::TexManager* m_texManager;
    de::gpu::VideoDriver* m_driver;

    //bool m_bRenderingEnabled;
    bool m_showFftMatrix3D;

    GL_Mesh16_Shader2D m_mesh16Shader2D;
    GL_Mesh16_Shader3D m_mesh16Shader3D;
    GL_Mesh16_Material m_mesh16Material;

    // Draw: Matrix3D + Front3D
    uint32_t m_matrix_fft_cols;
    uint32_t m_matrix_fft_rows;
    uint32_t m_matrix_fft_xmode;
    std::vector< float > m_matrix_fft_xmap; // Log10
    GL_Mesh16 m_matrix_fft;
    GL_Mesh16 m_matrix_fft_front;
    GL_Mesh16 m_matrix_fft_axis_x;
    GL_Mesh16 m_lineStripL;
    GL_Mesh16 m_lineStripR;

    // Draw: Matrix3D + Front3D
    GL_Mesh16 m_lineStripFft;
    GL_Mesh16 m_triStripFft;
    // de::LinearColorGradient m_wav_colorGradient;
    // de::LinearColorGradient m_fft_colorGradient;


    // DE_AlignedFloatVector m_front; // work buffers
    // DE_AlignedFloatVector m_left;
    // DE_AlignedFloatVector m_right;
    // DE_AlignedFloatVector m_back;

public:
    GL_Renderer();
    ~GL_Renderer(); // override;

    void setVisibleFftMatrix( bool bVisible )
    {
        m_showFftMatrix3D = bVisible;
    }

    void initializeGL(de::gpu::VideoDriver* driver);
    void paintGL();

    void draw2DLineStripL();
    void draw2DLineStripR();

    void draw3DAccumFftMatrix();

    void draw2DLineStripFft();


// Boring:
    void draw3DLineStripL();
    void draw3DLineStripR();

};
