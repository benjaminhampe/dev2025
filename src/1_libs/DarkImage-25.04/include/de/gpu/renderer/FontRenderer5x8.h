#pragma once
#include <de/Color.h>
#include <de/Font5x8.h>
#include <de/gpu/GPU.h>

namespace de {
namespace gpu {


#pragma pack( push )
#pragma pack( 1 )

// ===========================================================================
struct FontRenderer5x8_Vertex
// ===========================================================================
{
    float x;
    float y;
    float z;
    uint32_t color;
    FontRenderer5x8_Vertex()
        : x(.0f), y(.0f), z(.0f), color(0xFFFFFFFF) {}
    FontRenderer5x8_Vertex( float X, float Y, float Z, uint32_t C = 0xFFFFFFFF )
        : x(X), y(Y), z(Z), color(C) {}
};

#pragma pack( pop )

struct VideoDriver;
struct Shader;

// ===========================================================================
struct FontRenderer5x8
// ===========================================================================
{
    VideoDriver* m_driver;
    Shader* m_shader;
    uint32_t m_vao = 0;
    uint32_t m_vbo = 0;
    bool m_shouldUpload = true;
    int32_t m_screenWidthLoc = -1;
    int32_t m_screenHeightLoc = -1;
    std::vector< FontRenderer5x8_Vertex > m_vertices;

    FontRenderer5x8();
    //~Font5x8Renderer();

    void init( VideoDriver* driver );
    void destroy();
    void draw2DText( int x, int y, std::string const & msg, uint32_t color = 0xFFFFFFFF, Align align = Align::Default,
                    Font5x8 const & font = Font5x8(), uint32_t bgColor = 0, int padding = 0);

    void draw2DText(int x, int y, std::wstring const & msg, uint32_t color = 0xFFFFFFFF, Align align = Align::Default,
                    Font5x8 const & font = Font5x8(), uint32_t bgColor = 0, int padding = 0 )
    {
        draw2DText(x,y,StringUtil::to_str(msg),color, align, font, bgColor, padding);
    }

};

} // end namespace gpu.
} // end namespace de.
