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
    , m_u_screenSize(-1)
    , m_u_posTransform(-1)
    , m_u_tex(-1)
    , m_u_texTransform(-1)
{
}

ScreenQuadRenderer::~ScreenQuadRenderer()
{
}


void
ScreenQuadRenderer::draw2DRect(const de::Recti& pos, const TexRef& ref, bool blend)
{
    if (ref.empty()) { DE_ERROR("No tex") return; }
    //if (!ref.tex->id()) { DE_ERROR("No texId ",ref.str()) return; }
    if (!m_driver) { DE_ERROR("No driver") return; }
    if (!m_shader) { DE_ERROR("No shader") return; }
    if (!m_vao) { DE_ERROR("No vao") return; }

    State state;
    state.culling = Culling::disabled();
    state.depth = Depth::disabled();
    if (blend) state.blend = Blend::alphaBlend();
    m_driver->setState(state);
    m_driver->useShader(m_shader);

    const int w = m_driver->getScreenWidth();
    const int h = m_driver->getScreenHeight();
    // m_shader->setVec2f("u_screenSize", glm::vec2(w,h));
    // m_shader->setVec4f("u_posTransform", glm::vec4(pos.x, pos.y, pos.w, pos.h));
    // m_driver->useTexture(ref.tex,0);
    // m_shader->setInt("u_tex",0);
    // m_shader->setVec4f("u_texTransform", ref.coords);

    glUniform2f(m_u_screenSize, (float)w, (float)h);
    glUniform4f(m_u_posTransform, pos.x, pos.y, pos.w, pos.h);
    glBindTextureUnit(0, ref.tex->id());
    glUniform1i(m_u_tex, 0);
    glUniform4fv(m_u_texTransform, 1, glm::value_ptr(ref.coords));

    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void
ScreenQuadRenderer::init(VideoDriver* driver)
{
    m_driver = driver;

    if (!m_shader)
    {
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

                vec2 a_pos = c_vertices[gl_VertexID];

                vec2 vertPos = u_posTransform.xy;
                vec2 vertDim = u_posTransform.zw;
                vec2 pos = vertPos + (vertDim * a_pos);
                vec2 ndc = (pos / u_screenSize) * 2.0 - 1.0;
                ndc.y = -ndc.y;

                gl_Position = vec4(ndc, 0.0, 1.0);

                v_tex = a_pos;
            }
        )";

        const char* fs = R"(

            out vec4 fragColor;

            uniform sampler2D u_tex;
            uniform vec4 u_texTransform;

            in vec2 v_tex;

            void main()
            {
                vec2 texPos = u_texTransform.xy;
                vec2 texDim = u_texTransform.zw;
                vec2 texCoords = texPos + (texDim * v_tex);
                fragColor = texture(u_tex, texCoords);
            }
        )";

        m_shader = m_driver->createShader("sq2d", vs, fs );
        if (!m_shader)
        {
            DE_ERROR("No shader")
        }
        else
        {
            m_u_screenSize = glGetUniformLocation(m_shader->id, "u_screenSize");
            m_u_posTransform = glGetUniformLocation(m_shader->id, "u_posTransform");
            m_u_tex = glGetUniformLocation(m_shader->id, "u_tex");
            m_u_texTransform = glGetUniformLocation(m_shader->id, "u_texTransform");
        }
    }

    if (!m_vao)
    {
        // Dummy VAO (required in core profile)
        glGenVertexArrays(1, &m_vao);
    }
}


} // end namespace gpu.
} // end namespace de.
