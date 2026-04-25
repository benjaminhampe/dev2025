#pragma once
#include <DarkImage.h>
#include <de/smesh/SMeshIO.h>
#include <gui/viz/GL_Mesh16.h>

// ==========================================================
struct Matrix3D
// ==========================================================
{
    typedef float T;
    typedef glm::vec3 V3;
    typedef glm::mat4 M4;

    uint32_t m_rows;
    uint32_t m_cols;
    uint32_t m_scaleModeX; // 0 = linear, 1 = log10
    V3 m_pos; // T V3(1000,0,1000);
    V3 m_size; // S V3(1000,100,4000);
    M4 m_modelMat;
    de::BBox3f m_bbox;
    GL_Mesh16 m_mesh16;
    std::string m_name;

    std::vector< float > m_XMap; // Log10

    Matrix3D();
    ~Matrix3D();

    void
    init(V3 size, V3 pos, std::string name = "FftMatrix");

    void
    draw(GL_Mesh16_Shader3D& shader,
        const GL_Mesh16_Material& material);

    static de::BBox3f
    recomputeBoundingBox(const V3& d, const DE_AlignedFloatShiftMatrix& table);

// protected:
//     void updateVertices(DE_AlignedFloatShiftMatrix const & table);
//     void updateIndices(DE_AlignedFloatShiftMatrix const & table);
};
