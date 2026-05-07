#pragma once
#include <de/gpu/GPU.h>

namespace de {
namespace gpu {

struct Shader;
struct VideoDriver;

// Uses gl_VertexID.
// Does not use VBO and IBO
// ------------------------------------------------------------
class ScreenQuadRenderer
// ------------------------------------------------------------
{
	VideoDriver* m_driver;
	Shader* m_shader;
	uint32_t m_vao;
    int32_t m_u_tex;
    int32_t m_u_posTransform;
    int32_t m_u_screenSize;
public:
	ScreenQuadRenderer();
	~ScreenQuadRenderer();
	
	void init(VideoDriver* driver);
    void draw2D( const de::Rectf& pos, Texture* tex, bool blend);
};

} // end namespace gpu.
} // end namespace de.
