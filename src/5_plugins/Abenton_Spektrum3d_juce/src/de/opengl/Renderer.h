#pragma once
#include <de/opengl/Mesh16.h>
#include <de/audio/SampleCollector.h>
#include <de/opengl/details/TestRenderer.h>

// ==========================================================
class Renderer
// ==========================================================
{
public:
    typedef float T;
    typedef glm::vec3 V3;
    typedef glm::mat4 M4;

    de::gpu::VideoDriver* m_driver;
    de::gpu::Texture* m_texBenniIcon;
    de::gpu::Texture* m_texJuceIcon;

    bool m_bFirstMouse;
    bool m_bCameraFreeLook;
    bool m_bMouseLeftPressed;
    bool m_bMouseRightPressed;
    bool m_bMouseMiddlePressed;
    bool m_bReserved1;
    bool m_bRenderingEnabled;
    bool m_bShowPerfOverlay;
    bool m_bRenderFftMatrix3D;

    int m_mouseX;
    int m_mouseY;
    int m_lastMouseX;
    int m_lastMouseY;
    int m_mouseMoveX;
    int m_mouseMoveY;
    int m_fpsTimerId;
    uint32_t m_matrix_fft_cols;
    uint32_t m_matrix_fft_rows;
    uint32_t m_matrix_fft_xmode; // 0 = linear, 1 = log10
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
