#include <de/terrain/BumpShader.h>
#include <de_opengl.h>

namespace de {
namespace gpu {
	
void BumpShader::init( VideoDriver * driver )
{
    m_driver = driver;
    initShader();
}

void BumpShader::setMaterial( const BumpMaterial & material )
{
    if (!m_driver) { DE_ERROR("No driver") return; }

    if (!m_shader) { DE_ERROR("No shader") return; }

    // ==== Shader ====
    m_driver->useShader(m_shader);

    // ==== Shader: projViewMat ====
    glm::mat4 projViewMat = glm::mat4(1.0f);
    auto camera = m_driver->getCamera();
    if (camera)
    {
        projViewMat = camera->getViewProjectionMatrix();
    }
    glUniformMatrix4fv(m_locProjViewMat, 1, GL_FALSE, glm::value_ptr( projViewMat ));

    // ==== Shader: DiffuseMap ====
    int stage = 0;
    
	m_driver->useTexture(material.diffuseMap.tex, stage);
	
    glUniform1i(m_locDiffuseMap, stage);
	
	stage++;
	
    // ==== Shader: BumpMap ====    
    m_driver->useTexture(material.bumpMap.tex, stage);
	
    glUniform1i(m_locBumpMap, stage);
	
	glUniform1f(m_locBumpScale, material.bumpScale);
	
    //glActiveTexture(GL_TEXTURE0 + 0);
    //glBindTexture(GL_TEXTURE_2D, m_texWood->id);
    //GT_applyTex2DOptions(m_texWood->so);
	
    State state;
    state.culling = Culling::disabled();
    state.blend = Blend::disabled();
    m_driver->setState( state );	
}

void BumpShader::initShader()
{
    if (!m_driver) { DE_ERROR("No driver") return; }

    if (m_shader) return;

    std::string const & shaderName = "BumpShader";

    std::string vs = R"(

    layout(location = 0) in vec3 a_pos;
    layout(location = 1) in vec2 a_tex;
    layout(location = 2) in mat4 a_instanceMat; // (mat4 spans 4 attribute locations)

    uniform mat4 u_projViewMat;
	
	uniform sampler2D u_bumpMap;
	
	uniform float u_bumpScale;

	out vec3 v_pos;
    out vec2 v_tex;
	//out vec3 v_normal;
	//out vec3 v_tangent;
    
    void main()
    {
        v_tex = a_tex;
		
		float height = u_bumpScale * texture( u_bumpMap, a_tex ).r;
		
		v_pos = vec3(a_pos.x, a_pos.y + height, a_pos.z);
		
        gl_Position = u_projViewMat * a_instanceMat * vec4(v_pos, 1.0);
    }
    )";

    std::string fs = R"(

    out vec4 fragColor;

    in vec3 v_pos;
	in vec2 v_tex;

    uniform sampler2D u_diffuseMap;

    void main()
    {
        vec3 diffuseColor = texture(u_diffuseMap, v_tex).rgb;

        fragColor = vec4(diffuseColor, 1.0);
    }
    )";

    m_shader = m_driver->createShader( shaderName, vs, fs );

    m_locDiffuseMap = glGetUniformLocation(m_shader->id, "u_diffuseMap");
	
	m_locBumpMap = glGetUniformLocation(m_shader->id, "u_bumpMap");
	
	m_locBumpScale = glGetUniformLocation(m_shader->id, "u_bumpScale");

    m_locProjViewMat = glGetUniformLocation(m_shader->id, "u_projViewMat");
}

} // end namespace gpu.
} // end namespace de.
