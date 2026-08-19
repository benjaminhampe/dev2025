#pragma once
#include <DarkImage.h>
#include <DarkAudio.h>
#include <GL_Utils.h>
#include <GL_Mesh16.h>
#include <de/smesh/SMeshIO.h>

struct Plot3D
{
    uint32_t typ; // 0 = None, 1 = Wav, 2 = Wav01, 3 = Fft
    std::string name;
    glm::vec3 pos; // T V3(1000,0,1000);
    glm::vec3 size; // S V3(1000,100,4000);
    glm::mat4 modelMat;
    de::BBox3f bbox;
    uint32_t rows;
    uint32_t cols;
    BenMeshBuffer mesh; // _triangles matrix
    GL_Mesh16 mesh16; // _triangles matrix
};

struct GL_Tex2D
{
    uint32_t m_id;

    GL_Tex2D();
    ~GL_Tex2D();
    void close();
    void open();
    uint32_t id() const;
};

// ===========================================================================
class GL_Spectrum3D // : protected QOpenGLFunctions_4_5_Core
// ===========================================================================
{
    typedef float T;
    typedef glm::vec3 V3;
    typedef glm::mat4 M4;

    //de::gpu::TexManager* m_texManager;
    de::gpu::VideoDriver* m_driver;

    BenShader m_lineShader;
    GL_Mesh16_Shader m_meshShader;
    GL_Mesh16_Material m_meshMaterial;

    de::LinearColorGradient m_wav_colorGradient;
    de::LinearColorGradient m_fft_colorGradient;

    std::vector<Plot3D> m_plots;

    DE_AlignedFloatVector m_front; // work buffers
    DE_AlignedFloatVector m_left;
    DE_AlignedFloatVector m_right;
    DE_AlignedFloatVector m_back;

    // Drawing

    glm::vec3 m_d; // V3(1000,100,4000);
    //glm::vec3 m_e; // V3(4000,100,4000);
    //glm::vec3 m_f; // V3(1000,100,8000);

    BenMeshBuffer bks_lines;
    BenMeshBuffer bks_x_lines;
    BenMeshBuffer bks_y_lines;
    BenMeshBuffer bks_z_lines;

    Plot3D raw;
    Plot3D raw_fft;
    Plot3D vol;
    Plot3D vol_fft;
    Plot3D accum;
    Plot3D accum_fft;

/*
    Plot3D L_raw;
    Plot3D R_raw;
    Plot3D L_raw_fft;
    Plot3D R_raw_fft;

    Plot3D L_vol;
    Plot3D R_vol;
    Plot3D L_vol_fft;
    Plot3D R_vol_fft;

    Plot3D L_accum;
    Plot3D R_accum;
    Plot3D L_accum_fft;
    Plot3D R_accum_fft;

    Plot3D L_blackman;
    Plot3D R_blackman;
    Plot3D L_hamm;
    Plot3D R_hamm;
    Plot3D L_hann;
    Plot3D R_hann;

    Plot3D L_blackman_fft;
    Plot3D R_blackman_fft;
    Plot3D L_hamm_fft;
    Plot3D R_hamm_fft;
    Plot3D L_hann_fft;
    Plot3D R_hann_fft;
*/
public:
    GL_Spectrum3D();
    ~GL_Spectrum3D(); // override;
    //void initializeGL(de::gpu::TexManager* texManager);
    void initializeGL(de::gpu::VideoDriver* driver);
    void paintGL();
    //void drawGL(BenMeshBuffer & mesh, glm::vec3 const & pos);

    void createLineBox(BenMeshBuffer & m, de::BBox3f const & bbox );
    void createLineBox(BenMeshBuffer & m, de::BBox3f const & bbox, uint32_t color );
    void createBksLines(BenMeshBuffer & m, float d );
    void createBksLinesX(BenMeshBuffer & m, float d );
    void createBksLinesY(BenMeshBuffer & m, float d );
    void createBksLinesZ(BenMeshBuffer & m, float d );
    void createVLines(BenMeshBuffer & m, glm::vec3 const & d, DE_AlignedFloatVector const & pcm);
    void createVLines(BenMeshBuffer & m, glm::vec3 const & d, DE_AlignedFloatShiftMatrix const & pcm);
    void createWavLines(BenMeshBuffer & m, glm::vec3 const & d, DE_AlignedFloatVector const & pcm);
    void createWavMatrix(BenMeshBuffer & m, glm::vec3 const & d, DE_AlignedFloatShiftMatrix const & pcm);
    void createWavMatrix01(BenMeshBuffer & m, glm::vec3 const & d, DE_AlignedFloatShiftMatrix const & table);
    void createFftMatrix(BenMeshBuffer & m, glm::vec3 const & d, DE_AlignedFloatShiftMatrix const & table);

    void initPlot3D(int typ, Plot3D & plot, V3 const & d, DE_AlignedFloatShiftMatrix const & table, V3 const & pos);
    void drawPlot3D(Plot3D & plot, DE_AlignedFloatShiftMatrix const & table);
    static de::BBox3f recomputeBoundingBox(V3 const & d, DE_AlignedFloatShiftMatrix const & table);
    /*
    void createWavTris(SMeshBuffer & m,
                    glm::vec3 const & d,
                    DE_AlignedFloatVector const & pcm,
                    de::LinearColorGradient const & color_gradient);
    void createWavTris(SMeshBuffer & m,
                    glm::vec3 const & d,
                    DE_AlignedFloatShiftMatrix const & pcm,
                    de::LinearColorGradient const & color_gradient);
*/

    void addQuad(BenMeshBuffer & m,
                 BenVertex const & a,
                 BenVertex const & b,
                 BenVertex const & c,
                 BenVertex const & d, bool bIndexed = false)
    {
        auto v0 = m.Vertices.size();
        auto i0 = m.Indices.size();

        if (bIndexed || i0 > 0)
        {
            m.Vertices.push_back( a );
            m.Vertices.push_back( b );
            m.Vertices.push_back( c );
            m.Vertices.push_back( d );

            uint32_t A = v0 + 0;
            uint32_t B = v0 + 1;
            uint32_t C = v0 + 2;
            uint32_t D = v0 + 3;
            m.Indices.push_back( A );
            m.Indices.push_back( B );
            m.Indices.push_back( C );

            m.Indices.push_back( A );
            m.Indices.push_back( C );
            m.Indices.push_back( D );
        }
        else
        {
            m.Vertices.push_back( a );
            m.Vertices.push_back( b );
            m.Vertices.push_back( c );

            m.Vertices.push_back( a );
            m.Vertices.push_back( c );
            m.Vertices.push_back( d );
        }
    }
};
