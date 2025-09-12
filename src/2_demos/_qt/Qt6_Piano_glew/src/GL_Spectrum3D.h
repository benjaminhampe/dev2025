#pragma once
#include <AudioEngine.h>
#include <de/image/Image.h>
#include <GL_Utils.h>
#include <de_kissfft.hpp>
#include <de/ShiftMatrix.h>
#include <de/ColorGradient.h>

// ===========================================================================
class GL_Spectrum3D // : protected QOpenGLFunctions_4_5_Core
// ===========================================================================
{
    DspSampleCollector* m_sampleSource = nullptr;

    LineShader m_lineShader;

    // glm::mat4 m_modelMat = glm::mat4(1.0f);
    // glm::mat4 m_viewMat = glm::mat4(1.0f);
    // glm::mat4 m_projMat = glm::mat4(1.0f);

    SMeshBuffer m_lines;

    SMeshBuffer m_lines2;

    SMeshBuffer m_triangles;

    de::KissFFT m_fft;

    std::vector< float > m_decibels;

    SMeshBuffer m_spectrum;

    ShiftMatrix< float > m_shiftMatrixL;

    SMeshBuffer m_spectrumMatrix;

    //   de::gpu::SMeshBuffer m_wavtrisFrontL;
    //   de::gpu::SMeshBuffer m_wavtrisFrontR;
    de::LinearColorGradient m_cgWaveform;
    de::LinearColorGradient m_cgSpektrum;

public:
    GL_Spectrum3D();
    ~GL_Spectrum3D(); // override;
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL( de::gpu::Camera* pCamera );
    void setSampleSource( DspSampleCollector* sampleSource );
    void createLines2();
    void createTriangles();
    void createSpectrum();
    void createSpectrumMatrix();
};
