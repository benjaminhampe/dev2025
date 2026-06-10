#pragma once
#include <de/opengl/Mesh16.h>
#include <de/audio/SampleCollector.h>
#include <de/opengl/details/TestRenderer.h>

// ==========================================================
class Renderer
// ==========================================================
{
    typedef float T;
    typedef glm::vec3 V3;
    typedef glm::mat4 M4;

    //de::gpu::TexManager* m_texManager;
    de::gpu::VideoDriver* m_driver{ nullptr };

    bool m_bFirstMouse{ false };
    bool m_bCameraFreeLook{ false };
    bool m_bMouseLeftPressed{ false };
    bool m_bMouseRightPressed{ false };
    bool m_bMouseMiddlePressed{ false };
    bool m_bReserved1{ false };
    bool m_bRenderingEnabled{ true };
    bool m_bShowPerfOverlay{ true };
    bool m_bRenderFftMatrix3D{ true };

    int m_mouseX{ 0 };
    int m_mouseY{ 0 };
    int m_lastMouseX{ 0 };
    int m_lastMouseY{ 0 };
    int m_mouseMoveX{ 0 };
    int m_mouseMoveY{ 0 };
    int m_fpsTimerId{ 0 };
    uint32_t m_matrix_fft_cols{ 0 };
    uint32_t m_matrix_fft_rows{ 0 };
    uint32_t m_matrix_fft_xmode{ 1 }; // 0 = linear, 1 = log10
    de::TAlignedVector< float > m_matrix_fft_xmap; // Log10

    de::audio::SampleCollector m_collector;

    TestRenderer m_test;

    Mesh16_Shader2D m_mesh16Shader2D;
    Mesh16_Shader3D m_mesh16Shader3D;
    Mesh16_Material m_mesh16Material;

    Mesh16 m_matrix_fft;
    Mesh16 m_matrix_fft_front;
    Mesh16 m_matrix_fft_axis_x;
    Mesh16 m_lineStripL;
    Mesh16 m_lineStripR;

    // Draw: Matrix3D + Front3D
    Mesh16 m_lineStripFft;
    Mesh16 m_triStripFft;
    // de::LinearColorGradient m_wav_colorGradient;
    // de::LinearColorGradient m_fft_colorGradient;


    // DE_AlignedFloatVector m_front; // work buffers
    // DE_AlignedFloatVector m_left;
    // DE_AlignedFloatVector m_right;
    // DE_AlignedFloatVector m_back;

public:
    Renderer();
    ~Renderer(); // override;

    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    void uninitGL();
    void draw2DFftOverlay();

    void dsp_push(const de::TAlignedVector<float>& sum);



    void draw3DAccumFftMatrix();

    void draw2DLineStripFft();

    // void showFftMatrix3D( bool bVisible )
    // {
    //     m_bRenderFftMatrix3D = bVisible;
    // }

// Boring:
    // void draw2DLineStripL();
    // void draw2DLineStripR();

    // void draw3DLineStripL();
    // void draw3DLineStripR();

};
