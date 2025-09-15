#include <de/gpu/mtl/PMaterialRenderer.h>
#include <de/gpu/VideoDriver.h>
#include <de/gpu/smesh/SMeshLibrary.h>
#include <de_opengl.h>
#include <de/gpu/GL_debug_layer.h>

namespace de {
namespace gpu {

// ===========================================================================
// struct PLight
// ===========================================================================

std::string PLight::str() const
{
    std::ostringstream o; o <<
    "typ(" << typ << "), "
    "siz(" << siz << "), "
    "pos(" << pos << "), "
    "color(" << color << "), "
    "dir(" << dir << "), "
    "maxdist(" << maxdist << "), "
    "power(" << power << ")";
    return o.str();
}

std::wstring PLight::wstr() const
{
    return de_wstr(str());
}

// ===========================================================================
PMaterialRenderer::PMaterialRenderer()
// ===========================================================================
    : m_driver(nullptr)
    //, m_screenGamma( 1.25f )
{}

void
PMaterialRenderer::addLight( int typ,
                                V3 pos,
                                V3 siz,
                                T maxdist,
                                V3 dir,
                                V3 color )
{
    m_lights.emplace_back();
    auto & light = m_lights.back();
    light.typ = typ;
    light.siz = siz;
    light.pos = pos;
    light.maxdist = maxdist;
    light.dir = glm::normalize(dir);
    light.color = color;
}

void 
PMaterialRenderer::init( VideoDriver* driver )
{ 
    m_driver = driver;

    addLight(0, // typ: 0 = Point, 1 = Directional
             V3(0,320,0), // pos
             V3(100,100,100), // siz
             500, // maxdist
             V3(0,-1,0), // dir
             dbRGBAf( 255,255,200 )); // color

    addLight(0, // typ: 0 = Point, 1 = Directional
             V3( 0, 500, -500 ), // pos
             V3(100,100,100), // size
             1000.0f,
             V3(0,-1,0), // down
             dbRGBAf( 255,100,100 )); // Rot

    addLight(0, // typ: 0 = Point, 1 = Directional
             V3( 2000, 2000, -2000 ),
             V3(100,100,100),
             5000.0f,
             V3(-1,0,1),
             dbRGBAf( 105,220,105 )); // Gruen

    addLight( 0, // typ: 0 = Point, 1 = Directional
             V3( -2000, 1000, 1000 ),
             V3(100,100,100),
             5000.0f,
             V3(1,-1,1),
             dbRGBAf( 105,105,220 )); // Blau

    //m_AnimTime.start = dbTimeInSeconds();
    //m_AnimTime.last = m_AnimTime.start;
    //m_AnimTime.curr = m_AnimTime.start;
}

void PMaterialRenderer::drawDebugLights()
{
    SMeshBuffer sphere( PrimitiveType::Triangles );
    SMeshSphere::add( sphere, V3(1,1,1), 0xFFFFFFFF, 33,33);

    auto smr = m_driver->getSMaterialRenderer();

    for (PLight & light : m_lights)
    {
        SMeshBuffer sphere( PrimitiveType::Triangles );
        auto color = dbRGB(light.color);
        SMeshSphere::add( sphere, light.siz, color, 33,33, light.pos);
        sphere.setDirty();
        sphere.upload();

        smr->setMaterial(sphere.getMaterial());
        sphere.draw();
        sphere.destroy();
    }
}

void PMaterialRenderer::setLights(Shader* shader)
{
    if (!shader) return;
    shader->setInt("u_numLights", m_lights.size());

    for (int i = 0; i < m_lights.size(); ++i)
    {
        const auto& light = m_lights[ i ];
        shader->setVec3f(dbStr("u_lights[",i,"].pos"), light.pos);
        shader->setVec3f(dbStr("u_lights[",i,"].dir"), light.dir);
        shader->setVec3f(dbStr("u_lights[",i,"].color"), light.color);
        shader->setFloat(dbStr("u_lights[",i,"].intensity"), light.power);
        shader->setInt(  dbStr("u_lights[",i,"].type"), light.typ); // 0 = Point, 1 = Directional, 2 = Spotlight
    }
}

bool PMaterialRenderer::setMaterial( PMaterial& mtl )
{
    // [GetProgram]
    if ( !m_driver ) { DE_ERROR("No driver.") return false; }

    // [GetProgram]
    const auto shaderName = getShaderName( mtl );

    // [GetProgram]
    auto shader = m_driver->getShader( shaderName );
    if ( !shader )
    {
        shader = m_driver->createShader( shaderName, getVS( mtl ), getFS( mtl ) );
        if ( !shader )
        {
            DE_ERROR("No shader built(",shaderName,")")
            return false;
        }
    }

    // [UseProgram]
    if ( !m_driver->useShader( shader ) )
    {
        DE_ERROR("No active shader(",shaderName,")")
        return false;
    }

    //DE_DEBUG("Use shader ",shaderName)

    // [Lights]
    setLights( shader );

    // [M] New: not a uniform anymore. See a_instanceMat as vertexattribute.
    //, const glm::mat4& modelMat
    //shader->setMat4f( "u_modelMat", modelMat );

    // [VP]
    auto viewMat = glm::dmat4( 1.0 );
    auto projMat = glm::dmat4( 1.0 );
    auto cameraPos = glm::dvec3( 0, 0, 0 );
    auto camera = m_driver->getCamera();
    if ( camera )
    {
        camera->update();
        viewMat = camera->getViewMatrix();
        projMat = camera->getProjectionMatrix();
        cameraPos = camera->getPos();
    }
    shader->setMat4f( "u_viewMat", viewMat );
    shader->setMat4f( "u_projMat", projMat );
    shader->setVec3f( "u_cameraPos", cameraPos );
    /*
    // [Gamma] + [Fog] Common to all shaders
    shader->setVec1f( "u_screenGamma", m_ScreenGamma );
    */

    shader->setInt( "u_illum", mtl.illum );
    shader->setFloat( "u_d", mtl.d );
    shader->setFloat( "u_Ni", mtl.Ni );
    shader->setFloat( "u_Ns", mtl.Ns );
    shader->setVec3f( "u_Kd", mtl.Kd );
    shader->setVec3f( "u_Ks", mtl.Ks );
    shader->setVec3f( "u_Ka", mtl.Ka );

    uint32_t texStage = 0;

    // [DiffuseMap] Combi <RGB> colorMap + <A> opacityMap
    if (mtl.map_Kd.tex)
    {
        m_driver->useTexture(mtl.map_Kd.tex,texStage);
        shader->setInt("u_diffuseMap", texStage);
        shader->setVec4f("u_diffuseMapCoords", mtl.map_Kd.coords());
        shader->setVec2f("u_diffuseMapRepeat", mtl.map_Kd.repeat());
        texStage++;
    }

    // [BumpMap] Combi <RGB> normalMap + <A> bumpMap
    if (mtl.map_Bump.tex)
    {
        m_driver->useTexture(mtl.map_Bump.tex,texStage);
        shader->setInt("u_bumpMap", texStage);
        shader->setVec4f("u_bumpMapCoords", mtl.map_Bump.coords());
        shader->setVec2f("u_bumpMapRepeat", mtl.map_Bump.repeat());
        shader->setFloat("u_bumpMapScale", mtl.map_Bump.bumpScale);
        texStage++;
    }

    GL_VALIDATE

    // [State] Cull, Depth, Stencil, Blend, etc...
    State state;
    state.depth = Depth();
    state.culling = Culling::disabled();
    state.blend = Blend::alphaBlend();
    m_driver->setState( state );

    m_material = mtl;
    return true;
}

void PMaterialRenderer::unsetMaterial( PMaterial& mtl )
{
}


// static
std::string
PMaterialRenderer::getShaderName(const PMaterial& mtl)
{
    std::ostringstream o;
    o << "Ptl";
    //o << "+illum" << mtl.illum;
    //o << "+fog" << mtl.fog;
    //o << "+gamma" << mtl.gamma;
    // o << "+d" << mtl.d.value_or(1.0);
    // o << "+Ni" << mtl.d.value_or(1.0);
    // o << "+Ns" << mtl.d.value_or(1.0);
    // if ( mtl.Kd ) { o << "+Kd"; }
    // if ( mtl.Ks ) { o << "+Ks"; }
    // if ( mtl.Ka ) { o << "+Ka"; }
    if ( mtl.map_Kd.tex ) { o << "+map_Kd"; }
    if ( mtl.map_Bump.tex ) { o << "+map_Bump"; }
    // if ( mtl.map_Ks.tex ) { o << "+map_Ks"; }
    // if ( mtl.map_Ka.tex ) { o << "+map_Ka"; }
    // if ( mtl.map_decal.tex ) { o << "+map_decal"; }
    // if ( mtl.map_disp.tex ) { o << "+map_disp"; }
    // if ( mtl.map_refl.tex ) { o << "+map_refl"; }
    return o.str();
}

// static
std::string
PMaterialRenderer::getVS( const PMaterial& mtl )
{
    std::ostringstream o;
    o << R"(
    // ParallaxMtl_Vertex.fvf = POS_3D + NORMAL_3D + TANGENT_3D + DIFFUSECOLOR_4D + TEX_2D + TEX_2D
    layout(location = 0) in vec3 a_pos;			// 12 bytes (12)
    layout(location = 1) in vec3 a_normal;		// 12 bytes (24)
    layout(location = 2) in vec3 a_tangent;		// 12 bytes (36)
    layout(location = 3) in lowp vec4 a_color;	//  4 bytes (40)
    layout(location = 4) in vec3 a_texCoords;	//  8 bytes (52)
	layout(location = 5) in vec3 a_texCoords2;	//  8 bytes (64) = 64 Byte per vertex
    layout(location = 6) in mat4 a_instanceMat; // 64 bytes (spans 4! attribute locations for a single mat4)
    // 128 Bytes per instanced vertex!

    uniform mat4 u_viewMat;
    uniform mat4 u_projMat;
    uniform vec3 u_cameraPos;

	out vec3 v_fragPos;
    out mat3 v_TBN;		// TBN matrix for tangent space <parallax|normal> mapping.
    out vec3 v_normal;
    out vec4 v_color;
	out vec2 v_texCoords;
	
    void main()
    {
        // Construct TBN matrix for tangent space normal mapping
        vec3 T = normalize(vec3(a_instanceMat * vec4(a_tangent, 0.0)));
        vec3 N = normalize(vec3(a_instanceMat * vec4(a_normal, 0.0)));
        vec3 B = cross(N, T);
        v_TBN = mat3(T, B, N);
        v_fragPos = vec3(a_instanceMat * vec4(a_pos, 1.0));
        v_normal = vec3(a_instanceMat * vec4(a_normal, 1.0));
        //v_color = unpackUnorm4x8(a_instanceColor);
        v_color = clamp( vec4( a_color ) * (1.0 / 255.0), vec4( 0,0,0,0 ), vec4( 1,1,1,1 ) );
        v_texCoords = a_texCoords.xy;
        //v_texCoords2 = a_texCoords2;
        gl_Position = u_projMat * u_viewMat * vec4(v_fragPos, 1.0);
    }
    )";
    return o.str();
}

// static
std::string
PMaterialRenderer::getFS( const PMaterial& mtl )
{
    std::ostringstream fs;
    fs << R"(

    out vec4 fragColor;

    in vec3 v_fragPos; // worldModelPos
    in mat3 v_TBN;
    in vec3 v_normal;
    in vec4 v_color;
    in vec2 v_texCoords;

    struct PLight
    {
        vec3 pos;   // Point light position
        vec3 dir;  	// Spotlight / directional light direction
        vec3 color;      // Light color (RGB)
        float intensity; // Overall brightness
        int type;        // 0 = Point, 1 = Directional, 2 = Spotlight
    };

    #define MAX_LIGHTS 8
    uniform PLight u_lights[MAX_LIGHTS];
    uniform int u_numLights;

    uniform vec3 u_cameraPos;

    uniform int u_illum;        // Lichtberechnung
    uniform float u_d;          // Opacity
    uniform float u_Ni;			// Brechungsindex
    uniform float u_Ns;			// shininess
    uniform vec3 u_Kd;			// diffuseColor
    uniform vec3 u_Ka;			// ambientColor
    uniform vec3 u_Ks;			// specularColor

    )";
    if (mtl.map_Kd.tex)
    {
        fs << R"(
    uniform sampler2D u_diffuseMap; 	// diffuseMap parameter
    uniform vec4 u_diffuseMapCoords;	// texture patch offset(x,y) + scale(w,h) as (zw)
    uniform vec2 u_diffuseMapRepeat;    // texture patch repeat(x,y)
        )";
    }
    if (mtl.map_Bump.tex)
    {
        fs << R"(
    uniform sampler2D u_bumpMap;     // bumpMap parameter
    uniform vec4 u_bumpMapCoords;   // texture patch offset(x,y) + scale(w,h) as (zw)
    uniform vec2 u_bumpMapRepeat;   // texture patch repeat(x,y)
    uniform float u_bumpMapScale; 	// height/parallax-depth scale factor
        )";
    };
/*
    fs << R"(
    vec3 calculateLightEffect(PLight light, vec3 normal, vec3 viewDir)
    {
        vec3 lightDir;

        if (light.type == 0) // Point Light
        {
            lightDir = normalize(light.pos - v_fragPos);
        }
        else if (light.type == 1) // Directional Light
        {
            lightDir = normalize(-light.dir);
        }
        else // Spotlight
        {
            lightDir = normalize(light.pos - v_fragPos);
        }

        // Ambient color component
        vec3 ambientColor = u_Ka * light.color;

        // Diffuse color component
        float diffuseFactor = max(dot(normal, lightDir), 0.0);
        vec3 diffuseColor = u_Kd * light.color * diffuseFactor;

        // Specular component
        vec3 reflectDir = reflect(-lightDir, normal);
        float specularFactor = pow(max(dot(viewDir, reflectDir), 0.0), u_Ns);
        vec3 specularColor = u_Ks * light.color * specularFactor;

        return ambientColor + diffuseColor + specularColor;
    }
    )";
*/
    fs << R"(
    void main()
    {
    )";

    if (mtl.map_Kd.tex)
    {
        fs << R"(
        // Apply diffuse textures
        vec2 Kd_offset = u_diffuseMapCoords.xy;
        vec2 Kd_scale = u_diffuseMapCoords.zw;
        vec2 Kd_texCoords = (fract(v_texCoords * u_diffuseMapRepeat) * Kd_scale ) + Kd_offset;
        vec4 diffuseColor = texture(u_diffuseMap, Kd_texCoords) * v_color;
        )";
    }
    else
    {
        fs << R"(
        vec4 diffuseColor = v_color;
        )";
    }

    fs << R"(
        /*
        if (u_illum == 1)
        {
            vec3 viewDir = normalize(u_cameraPos - v_fragPos); // View direction for parallax
            vec3 normal = v_normal; // applyNormalMapping(); // Uses normal map or bump mapping

            vec3 resultColorRGB = vec3(0.0);
            for (int i = 0; i < u_numLights; ++i)
            {
                resultColorRGB += calculateLightEffect(u_lights[i], normal, viewDir);
            }

            fragColor = vec4(resultColorRGB * diffuseColor.rgb, diffuseColor.a);
        }
        else
        {
        */
            fragColor = vec4(diffuseColor.rgb, diffuseColor.a);
        // }
    }
    )";


/*
    if (mtl.map_Bump.tex)
    {
        fs << R"(
        vec3 normal = applyNormalMapping(); // Uses normal map or bump mapping
        )";
    }
    else
    {
        fs << R"(
        vec3 normal = v_normal; // Uses normal from polygon
        )";
    }

        // (Needs) u_bumpMap:
        // Normal mapping: Use RGB data or generate pseudo-normals from grayscale
        vec3 applyNormalMapping( vec2 texCoords )
        {
            vec3 my_normal = (texture(u_bumpMap, texCoords).rgb * 2.0) - 1.0; // Convert from [0,1] to [-1,1]
            return normalize(v_TBN * (my_normal * u_mtl.bumpScale));

            float height = texture(u_bumpMap, texCoords).a; // Grayscale bump map
            // vec3 pseudoNormal = vec3(dFdx(height), dFdy(height), 1.0);
            // return normalize(v_TBN * (pseudoNormal * mtl.bumpScale));
        }

        // (Needs) u_bumpMap:
        // Parallax mapping: Shift texture coordinates based on Alpha height map
        vec2 applyParallaxMapping( vec2 texCoords, vec3 viewDir )
        {
            float height = texture(u_bumpMap, texCoords).a; // Alpha = height map
            float depth = (height - 0.5) * u_bumpMapScale; // Center height at 0.5
            return texCoords + (depth * viewDir.xy); // Offset UVs dynamically
        }


        fs << R"(
        // Detect bump type: grayscale or RGB normal + Alpha height
        bool isNormalMap()
        {
            vec3 sampleColor = texture(mtl.bumpMap, v_texCoords).rgb;
            return dot(sampleColor, vec3(1.0)) > 0.5; // If RGB intensity is high, assume normal map
        }

        // main1: Uses always AlphaBlend
        void main()
        {
            vec3 viewDir = normalize(u_cameraPos - v_fragPos);

            vec3 normal = applyNormalMapping(); // Uses normal map or bump mapping

            vec3 resultColorRGB = vec3(0.0);
            for (int i = 0; i < numLights; ++i)
            {
                resultColorRGB += calculateLightEffect(lights[i], normal, viewDir);
            }

            // Apply textures
            vec4 diffuseColor = texture(mtl.map_Kd, v_texCoords);
            FragColor = vec4(resultColorRGB * diffuseColor.rgb, diffuseColor.a);
        }
    )";
    }

    fs << R"(
    // main1: Uses always AlphaBlend
    void main()
    {
        vec3 viewDir = normalize(u_cameraPos - v_fragPos);
    )";

    fs << R"(
        vec3 resultColorRGB = vec3(0.0);
        for (int i = 0; i < numLights; ++i)
        {
            resultColorRGB += calculateLightEffect(lights[i], normal, viewDir);
        }
    )";

*/

    return fs.str();
}


/*
    struct Light {
        glm::vec3 position;
        glm::vec3 direction;
        glm::vec3 color;
        float intensity;
        int type;
    };
    void setLights(GLuint shaderID, Light* lights, int numLights)
    {
        glUseProgram(shaderID);
        glUniform1i(glGetUniformLocation(shaderID, "numLights"), numLights);

        for (int i = 0; i < numLights; ++i) {
            std::string index = "lights[" + std::to_string(i) + "]";
            glUniform3fv(glGetUniformLocation(shaderID, (index + ".position").c_str()), 1, &lights[i].position[0]);
            glUniform3fv(glGetUniformLocation(shaderID, (index + ".direction").c_str()), 1, &lights[i].direction[0]);
            glUniform3fv(glGetUniformLocation(shaderID, (index + ".color").c_str()), 1, &lights[i].color[0]);
            glUniform1f(glGetUniformLocation(shaderID, (index + ".intensity").c_str()), lights[i].intensity);
            glUniform1i(glGetUniformLocation(shaderID, (index + ".type").c_str()), lights[i].type);
        }
    }

    // [Lighting]
    if ( mtl.Lighting > 0 )
    {
        //      if ( mtl.Lighting > 1 )
        //      {
        //         shader->setUniformf( "u_shininess", mtl.Shininess );
        //      }

        if ( mtl.Lighting == 3 )
        {
            for ( size_t i = 0; i < m_Lights.size(); ++i )
            {
                std::string l1 = std::string( "u_light_pos" ) + std::to_string( i );
                std::string l2 = std::string( "u_light_color" ) + std::to_string( i );
                std::string l3 = std::string( "u_light_power" ) + std::to_string( i );

                // TODO: repair mvp with modelMat
                //glm::vec4 clipRaw = glm::vec4( vp * glm::dvec4( glm::dvec3( m_Lights[i].pos ), 1.0 ) );
                glm::vec4 clipRaw = glm::vec4( projViewMat * glm::dvec4( glm::dvec3( m_Lights[i].pos ), 1.0 ) );
                glm::vec3 clipPos = glm::vec3( clipRaw ) / clipRaw.w;
                m_Lights[i].clipPos = clipPos;

                shader->setVec3f( l1, m_Lights[i].clipPos );
                shader->setVec3f( l2, m_Lights[i].color );
                shader->setVec1f( l3, m_Lights[i].maxdist );
            }
        }
    }
    */


/*
static setMaterial( Mtl & mtl, const glm::mat4& modelMat = glm::mat4(1.0f) ) )
{
    glUseProgram(shaderID);

    // Set color properties
    glUniform3fv(glGetUniformLocation(shaderID, "mtl.ambient"), 1, &mtl.ambient[0]);
    glUniform3fv(glGetUniformLocation(shaderID, "mtl.diffuse"), 1, &material.diffuse[0]);
    glUniform3fv(glGetUniformLocation(shaderID, "material.specular"), 1, &material.specular[0]);
    glUniform1f(glGetUniformLocation(shaderID, "material.shininess"), material.shininess);
    glUniform1f(glGetUniformLocation(shaderID, "material.bumpScale"), material.bumpScale);
    glUniform1f(glGetUniformLocation(shaderID, "material.parallaxDepth"), material.parallaxDepth);

    // Activate and bind textures dynamically
    int textureUnit = 0;
    if (material.hasDiffuse) {
        glActiveTexture(GL_TEXTURE0 + textureUnit);
        glBindTexture(GL_TEXTURE_2D, material.diffuseMap);
        glUniform1i(glGetUniformLocation(shaderID, "material.diffuseMap"), textureUnit++);
    }
    if (material.hasSpecular) {
        glActiveTexture(GL_TEXTURE0 + textureUnit);
        glBindTexture(GL_TEXTURE_2D, material.specularMap);
        glUniform1i(glGetUniformLocation(shaderID, "material.specularMap"), textureUnit++);
    }
    if (material.hasNormalMap) {
        glActiveTexture(GL_TEXTURE0 + textureUnit);
        glBindTexture(GL_TEXTURE_2D, material.normalMap);
        glUniform1i(glGetUniformLocation(shaderID, "material.normalMap"), textureUnit++);
    }
}
*/

// // === Set/Get Shader Uniform Gamma
// float getGamma() const { return m_ScreenGamma; }
// void setGamma( float gamma ) { m_ScreenGamma = gamma; }
// // === Lights ===, could move to driver

// uint32_t getLightCount() const { return uint32_t(m_Lights.size()); }
// SMaterialLight & getLight( uint32_t light ) { return m_Lights[ light ]; }
// SMaterialLight const & getLight( uint32_t light ) const { return m_Lights[ light ]; }

// // New: Mostly for render stats, counts vertices, indices and primitives
// void beginFrame();
// void endFrame();
// void drawLights();
// void animate( double pts ); // called once each frame, in Driver::beginFrame()

} // namespace gpu.
} // namespace de.


