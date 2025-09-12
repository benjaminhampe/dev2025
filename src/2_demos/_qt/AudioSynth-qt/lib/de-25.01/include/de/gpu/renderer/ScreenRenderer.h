#pragma once
#include <de/gpu/smesh/SMesh.h>

namespace de {
namespace gpu {

struct Shader;
struct VideoDriver;

// ===========================================================================
struct ScreenRenderer
// ===========================================================================
{
    ScreenRenderer();
    //~ScreenRenderer() {}

    void init( VideoDriver* driver ) { m_driver = driver; }

    int getScreenWidth() const;
    int getScreenHeight() const;

    bool setMaterial( const SMaterial& material, const Recti& pos = Recti(0,0,1,1) );
    void unsetMaterial( const SMaterial& material );
    void animate( double pts ) {}

    void draw2D( const SMeshBuffer& m );

    void draw2DHexagon( const Recti& pos, const uint32_t color = 0xFFFFFFFF, const TexRef& tex = TexRef() );

    void draw2DCircle( const Recti& pos, const uint32_t color = 0xFFFFFFFF, const TexRef& tex = TexRef(), int tess = 23 );
    void draw2DLine( int x1, int y1,
                     int x2, int y2,
                     uint32_t c1,
                     uint32_t c2 );
    //void draw2DLine( int x1, int y1, int x2, int y2, const uint32_t color = 0xFFFFFFFF, const TexRef& tex = TexRef() );
    void draw2DRect( const Recti& pos, const uint32_t color = 0xFFFFFFFF, const TexRef& tex = TexRef() );
    void draw2DRectLine( const Recti& pos, const uint32_t color = 0xFFFFFFFF, const TexRef& tex = TexRef() );
    void draw2DRoundRect( const Recti& pos, const glm::ivec2& r, const uint32_t color = 0xFFFFFFFF, const TexRef& tex = TexRef(), int tess = 23 );
    void draw2DRoundRectLine( const Recti& pos, const glm::ivec2& r, const uint32_t color = 0xFFFFFFFF, const TexRef& tex = TexRef(), int tess = 23 );

protected:
    static std::string createShaderName( SMaterial const& material );
    static std::string createVS( SMaterial const& material );
    static std::string createFS( SMaterial const& material );

    VideoDriver* m_driver;
    SMaterial m_material;
    float m_zIndex;
    bool m_isDirty;
    AnimTimer m_animTime;

};

} // end namespace gpu.
} // end namespace de.
