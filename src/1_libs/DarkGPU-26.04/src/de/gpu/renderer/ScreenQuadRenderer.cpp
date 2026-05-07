#include <de/gpu/renderer/ScreenQuadRenderer.h>
#include <de/gpu/VideoDriver.h>
#include <de_opengl.h>

namespace de {
namespace gpu {

//========================================================
ScreenQuadRenderer::ScreenQuadRenderer()
//========================================================
	: m_driver(nullptr)
	, m_shader(nullptr)
	, m_vao(0)
    , m_u_tex(-1)
    , m_u_posTransform(-1)
    , m_u_screenSize(-1)
{
}

ScreenQuadRenderer::~ScreenQuadRenderer()
{
}

void 
ScreenQuadRenderer::init(VideoDriver* driver)
{
	m_driver = driver;

	const char* vs = R"(

        uniform vec2 u_screenSize;
        uniform vec4 u_posTransform;

        out vec2 v_tex;

		void main()
		{
            const vec2 c_vertices[6] = vec2[6](
				vec2(0.0, 1.0),
				vec2(1.0, 1.0),
				vec2(1.0, 0.0),
				vec2(0.0, 1.0),
				vec2(1.0, 0.0),
				vec2(0.0, 0.0)
			);

            v_tex = c_vertices[gl_VertexID];

            vec2 pos = u_posTransform.xy + v_tex * u_posTransform.zw;
            vec2 ndc = (pos / u_screenSize) * 2.0 - 1.0;
			ndc.y = -ndc.y;

			gl_Position = vec4(ndc, 0.0, 1.0);
		}
	)";

	const char* fs = R"(

        out vec4 fragColor;

        uniform sampler2D u_tex;

        in vec2 v_tex;

		void main()
		{
            fragColor = texture(u_tex, v_tex);
		}
	)";
	
	m_shader = m_driver->createShader("sq2d", vs, fs );

    m_u_tex = glGetUniformLocation(m_shader->id, "u_tex");
    m_u_posTransform = glGetUniformLocation(m_shader->id, "u_posTransform");
    m_u_screenSize = glGetUniformLocation(m_shader->id, "u_screenSize");

	// Dummy VAO (required in core profile)
	glGenVertexArrays(1, &m_vao);
    //glBindVertexArray(m_vao);
}

void 
ScreenQuadRenderer::draw2D(const de::Rectf& pos, Texture* tex, bool blend)
{
    if (!tex) { DE_ERROR("No tex") return; }
    if (!m_driver) { DE_ERROR("No driver") return; }
    if (!m_shader) { DE_ERROR("No shader") return; }

	m_driver->useShader(m_shader);
    glBindVertexArray(m_vao);

    glBindTextureUnit(0, tex->id());
    glUniform1i(m_u_tex, 0);

    int w = m_driver->getRenderWidth();
    int h = m_driver->getRenderHeight();
    glUniform2f(m_u_screenSize, (float)w, (float)h);

    glUniform4f(m_u_posTransform, pos.x(), pos.y(), pos.w(), pos.h());

    m_driver->setBlend(Blend::alphaBlend());

	glDrawArrays(GL_TRIANGLES, 0, 6);
}

} // end namespace gpu.
} // end namespace de.