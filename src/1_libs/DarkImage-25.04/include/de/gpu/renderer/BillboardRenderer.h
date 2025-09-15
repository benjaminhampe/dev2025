#pragma once
#include <de/gpu/smesh/SMesh.h>

namespace de {
namespace gpu {

struct VideoDriver;
struct Shader;

// ===========================================================================
struct Billboard
// ===========================================================================
{
   enum EType
   {
      Rect = 0,
      RoundRect,
      Circle,
      ETypeCount
   };

   int typ = Rect;
   int tess = 33;
   glm::vec2 size;
   glm::vec2 radius; // relevant for RoundRect, not Circle
   //glm::vec3 pos;
   glm::mat4 transform = glm::mat4(1.0f);
   float zoffset; // for multiple billboards at same pos, order of overlap zoffset.
   uint32_t color;
   TexRef tex;
   SMeshBuffer mesh;
};

// ===========================================================================
struct BillboardRenderer
// ===========================================================================
{
    VideoDriver* m_driver;
    Shader* m_shader;
    SMeshBuffer meshRect;
    SMeshBuffer meshRoundRect;
    SMeshBuffer meshCircle;
    std::vector< Billboard > m_billboards;

    BillboardRenderer();
    ~BillboardRenderer();
    void init( VideoDriver* driver );
    void clear();
    void render();
    void draw3DBillboard( SMeshBuffer & vao, const glm::mat4& modelMat = glm::mat4(1.0f) );
    void add( int typ, glm::vec2 size, glm::vec3 pos, glm::vec2 radius, uint32_t color = 0xFFFFFFFF, TexRef ref = TexRef() );

    void addRect( glm::vec2 size, glm::vec3 pos, uint32_t color = 0xFFFFFFFF, TexRef ref = TexRef() )
    {
        add( Billboard::Rect, size, pos, glm::vec2(0,0), color, ref );
    }
    void addRoundRect( glm::vec2 size, glm::vec3 pos, glm::vec2 radius, uint32_t color = 0xFFFFFFFF, TexRef ref = TexRef() )
    {
        add( Billboard::RoundRect, size, pos, radius, color, ref );
    }
    void addCircle( glm::vec2 size, glm::vec3 pos, uint32_t color = 0xFFFFFFFF, TexRef ref = TexRef() )
    {
        add( Billboard::Circle, size, pos, glm::vec2(0,0), color, ref );
    }

private:
   std::string createShaderName( SMaterial const & mtl ) const;
};

} // end namespace gpu.
} // end namespace de.

