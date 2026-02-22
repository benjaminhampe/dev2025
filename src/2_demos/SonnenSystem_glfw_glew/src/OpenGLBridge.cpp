#include "OpenGLBridge.h"

#include <de_opengl.h>
#include <de/gpu/GL_debug_layer.h>
#include <de/Core.h>

#ifdef _WIN32
    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #endif
    #include <windows.h>
#else
    #define APIENTRY
#endif



namespace de {
namespace gl {

static void APIENTRY
BridgeMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
    std::ostringstream o;
    o << "GL CALLBACK: " << (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "") <<
        " type = 0x" << std::hex << type <<
        ", severity = 0x" << severity <<
        ", message = " << message << std::endl;
    DE_ERROR(o.str())
}

// static
uint32_t
Bridge::convertTexTargetToBindingEnum(uint32_t target)
{
    switch (target)
    {
    case GL_TEXTURE_1D: return GL_TEXTURE_BINDING_1D;
    case GL_TEXTURE_2D: return GL_TEXTURE_BINDING_2D;
    case GL_TEXTURE_3D: return GL_TEXTURE_BINDING_3D;
    case GL_TEXTURE_CUBE_MAP: return GL_TEXTURE_BINDING_CUBE_MAP;
    case GL_TEXTURE_1D_ARRAY: return GL_TEXTURE_BINDING_1D_ARRAY;
    case GL_TEXTURE_2D_ARRAY: return GL_TEXTURE_BINDING_2D_ARRAY;
    case GL_TEXTURE_RECTANGLE: return GL_TEXTURE_BINDING_RECTANGLE;
    case GL_TEXTURE_BUFFER: return GL_TEXTURE_BINDING_BUFFER;
    case GL_TEXTURE_CUBE_MAP_ARRAY: return GL_TEXTURE_BINDING_CUBE_MAP_ARRAY;
    case GL_TEXTURE_2D_MULTISAMPLE: return GL_TEXTURE_BINDING_2D_MULTISAMPLE;
    case GL_TEXTURE_2D_MULTISAMPLE_ARRAY: return GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY;
    default: return 0;
    }
}

// Push state
void
Bridge::init()
{
    ensureDesktopOpenGL(); // initGlew()

    // <OpenGL debug output>
    de_glEnable(GL_DEBUG_OUTPUT);
    de_glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(BridgeMessageCallback, 0);

    // <OpenGL shaderVersion>
    glGetIntegerv( GL_MAJOR_VERSION, &m_shaderVersionMajor );
    glGetIntegerv( GL_MINOR_VERSION, &m_shaderVersionMinor );

    // <OpenGL maxTessGenLevel>
    GLint maxTessGenLevel = 0;
    glGetIntegerv(GL_MAX_TESS_GEN_LEVEL, &maxTessGenLevel);
    m_maxTessGenLevel = static_cast<uint32_t>(maxTessGenLevel);

    // <OpenGL programId>
    GLint id = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &id);
    m_initialProgramId = static_cast<uint32_t>(id);
    m_currentProgramId = m_initialProgramId;

    // <OpenGL texparameters>
    glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
    glPixelStorei( GL_PACK_ALIGNMENT, 1 );

    // <OpenGL maximalAnisotropy>
    GLfloat maxAnisotropy = 0.0f;
    de_glGetFloatv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy );
    m_maxAnisotropy = maxAnisotropy;

    // <OpenGL maximalTex2DSize>
    GLint maxTex2DSize = 0;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTex2DSize);
    m_maxTex2DSize = static_cast<uint32_t>(maxTex2DSize);

    // <OpenGL texunits>
    GLint nTexUnits = 0;
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &nTexUnits);
    m_initialTexUnits.resize(nTexUnits);

    for (int unit = 0; unit < nTexUnits; ++unit)
    {
        for (GLenum target : {  GL_TEXTURE_1D,
                                GL_TEXTURE_2D,
                                GL_TEXTURE_3D,
                                GL_TEXTURE_CUBE_MAP,
                                GL_TEXTURE_1D_ARRAY,
                                GL_TEXTURE_2D_ARRAY,
                                GL_TEXTURE_RECTANGLE,
                                GL_TEXTURE_BUFFER,
                                GL_TEXTURE_CUBE_MAP_ARRAY,
                                GL_TEXTURE_2D_MULTISAMPLE,
                                GL_TEXTURE_2D_MULTISAMPLE_ARRAY })
        {
            GLint textureId = 0;
            glGetIntegeri_v(convertTexTargetToBindingEnum(target), unit, &textureId);
            if (textureId != 0)
            {
                // We are not prepared for multiple textures bound at same tex-unit!
                if (m_initialTexUnits[ unit ].textureId > 0
                 && m_initialTexUnits[ unit ].textureId != textureId)
                {
                    DE_ERROR("TexUnit[",unit,"] had multiple target bound to a texture")
                    DE_ERROR("We are not prepared for multiple textures bound at same tex-unit!")
                    DE_ERROR("Restoring tex-units will fail, information loss!")
                }

                // Store (unit, target, texID)
                m_initialTexUnits[ unit ].targetTyp = target;
                m_initialTexUnits[ unit ].textureId = textureId;

                GLint samplerId = 0;
                glGetIntegeri_v(GL_SAMPLER_BINDING, unit, &samplerId);
                m_initialTexUnits[ unit ].samplerId = samplerId;
            }
        }
    }

    m_currentTexUnits = m_initialTexUnits;

    // DE_DEBUG( "NumHardwareTexUnits = ", m_numTexUnits )
    // DE_DEBUG( "Max.Texture2D.Size = ", m_maxTex2DSize )
    // DE_DEBUG( "Max.Anisotropy = ", m_maxAnisotropy )
    //m_texUnits.clear();
    //m_texUnits.resize( nTexUnits, TexRef() );

    // <OpenGL DepthTest>
    /*
    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // <OpenGL Culling>
    // Enable face culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    // <OpenGL Blending>
    // Enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // <OpenGL DepthTest>
    // Smooth shading (optional for older OpenGL versions)
    //glShadeModel(GL_SMOOTH);
    */

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Wireframe mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Solid mode

    // <OpenGL CullMode>
    m_initialCullMode = gpu::Culling::query();
    m_currentCullMode = gpu::Culling( gpu::Culling::Back, gpu::Culling::CCW );
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    // <OpenGL DepthTest>
    m_initialDepthTest = gpu::Depth::query();
    m_currentDepthTest = gpu::Depth();
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // <OpenGL BlendFunc>
    m_initialBlendMode = gpu::Blend::query();
    m_currentBlendMode = gpu::Blend::disabled();
    glDisable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    DE_INFO("GL_MAJOR_VERSION = ", m_shaderVersionMajor)
    DE_INFO("GL_MINOR_VERSION = ", m_shaderVersionMinor)
    DE_INFO("GL_VERSION = ", glGetString(GL_VERSION))
    DE_INFO("GL_VENDOR = ", glGetString(GL_VENDOR))
    DE_INFO("GL_RENDERER = ", glGetString(GL_RENDERER))
    DE_INFO("GL_SHADING_LANGUAGE_VERSION = ", glGetString(GL_SHADING_LANGUAGE_VERSION))
    DE_INFO("GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT = ", m_maxAnisotropy)
    DE_INFO("GL_MAX_TEXTURE_IMAGE_UNITS = ", m_initialTexUnits.size())
    DE_INFO("GL_MAX_TEXTURE_SIZE = ", m_maxTex2DSize)
    DE_INFO("GL_MAX_TESS_GEN_LEVEL = ", m_maxTessGenLevel)

    //const std::string extensionList = (const char*)glGetString(GL_EXTENSIONS);
    //const auto extensions = de::StringUtil::split(extensionList,' ');
    //DE_INFO("GL_EXTENSIONS = ", extensions.size())
    // for (size_t i = 0; i < extensions.size(); ++i)
    // {
    //     DE_INFO("GL_EXTENSION[",i,"] ", extensions[i])
    // }

    //dumpVideoModes();


    // ===================
    // === init camera ===
    // ===================
    m_camera.setScreenSize( 1920, 1080 );
    m_camera.setFOV( 90.0f );
    m_camera.setNearValue( 1.0f );
    m_camera.setFarValue( 38000.0f );
    m_camera.lookAt( glm::vec3(10,100,-100), glm::vec3(0,0,0) );
    m_camera.setMoveSpeed(1.0);
    m_camera.setStrafeSpeed(1.0);
    m_camera.setUpSpeed(1.0);

    // ===================
    // === init lights ===
    // ===================
    // de::gpu::Light light0; // (sun)
    // light0.pos = glm::vec3(0,300,0);
    // light0.color = glm::vec3(1,1,0.9);
    // m_lights.push_back( light0 );
}

void
Bridge::restore() // Pop state.
{
    // Rebuild initial state.
}

void
Bridge::useProgram( GLuint programId )
{
    if (programId == m_currentProgramId)
    {
        return;
    }

    glUseProgram( programId );
    m_currentProgramId = programId;
}

void
Bridge::useTexture( const Tex& gpuHandle, int stage )
{
    if (stage < 0 || stage >= m_currentTexUnits.size())
    {
        DE_WARN("Invalid texUnit ", stage, " of ", m_currentTexUnits.size())
        stage = m_currentTexUnits.size() - 1;
    }

    auto & unit = m_currentTexUnits[ stage ];
    if (unit.textureId != gpuHandle.textureId
     || unit.samplerId != gpuHandle.samplerId)
    {
        glBindTextureUnit( stage, gpuHandle.textureId );
        glBindSampler( stage, gpuHandle.samplerId );
        unit.textureId = gpuHandle.textureId;
        unit.samplerId = gpuHandle.samplerId;
        DE_DEBUG("Bound texture(", gpuHandle.textureId, ") and sampler(", gpuHandle.samplerId, ")")
    }
}

Tex
Bridge::createTex2D( const std::string& name, int unit, const Image& img, const SO& so ) const
{
    Tex tex;
    tex.name = name;

    int w = img.w();
    int h = img.h();
    void const* const pixels = img.data();
    PixelFormat fmt = img.pixelFormat();
    if ( !uploadTex2D( unit, tex, w, h, pixels, fmt, so ) )
    {
        DE_ERROR("Upload failed, tex-name(",name,")")
    }

    return tex;
}

bool Bridge::uploadTex2D( int unit, Tex& tex, const Image& img, const SO& so ) const
{
    int w = img.w();
    int h = img.h();
    void const* const pixels = img.data();
    PixelFormat fmt = img.pixelFormat();
    return uploadTex2D( unit, tex, w, h, pixels, fmt, so );
}

bool Bridge::uploadTex2D( int unit, Tex& tex, int w, int h, void const* const pixels, PixelFormat fmt, const SO& so ) const
{
    // Create texture handle:
    if (!tex.textureId)
    {
        GLuint textureId = 0;
        glGenTextures(1, &textureId);

        if ( !textureId )
        {
            DE_ERROR("No textureId. ", tex.name)
            return false;
        }

        tex.textureId = textureId;
    }

    tex.targetTyp = GL_TEXTURE_2D;
    tex.boundUnit = -1;
    tex.samplerId = 0;

    if ( w < 1 || h < 1 )
    {
        DE_ERROR( "Invalid texture size(",w,",",h,") ", tex.name)
        return false;
    }

    // tex->m_w = w;
    // tex->m_h = h;
    // tex->m_fmt = fmt;
    // tex->m_so = so;
    // tex->m_layer = 0;

    uint32_t m_preset = 0;
    uint32_t m_format = 0;
    uint32_t m_dataType = 0;

    auto set = [&]( uint32_t preset, uint32_t format, uint32_t dataType)
    {
        m_preset = preset; m_format = format; m_dataType = dataType;
        // DE_BENNI("m_preset(",StringUtil::hex(m_preset),"), "
        //          "m_format(",StringUtil::hex(m_format),"), "
        //          "m_dataType(",StringUtil::hex(m_dataType),")")
    };

         if (fmt == PixelFormat::R8)        { set( GL_RED, GL_RED, GL_UNSIGNED_BYTE ); }
    else if (fmt == PixelFormat::R8G8B8)    { set( GL_RGB, GL_RGB, GL_UNSIGNED_BYTE); } // GL_SRGB
    else if (fmt == PixelFormat::R8G8B8A8)  { set( GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE ); } // gammaCorrection ? GL_SRGB_ALPHA
    else if (fmt == PixelFormat::R32F)      { set( GL_R32F, GL_RED, GL_FLOAT); }
    else if (fmt == PixelFormat::RGB16F)    { set( GL_RGB16F, GL_RGB, GL_FLOAT ); }
    else if (fmt == PixelFormat::RGBA16F)   { set( GL_RGBA16F, GL_RGBA, GL_FLOAT ); }
    else if (fmt == PixelFormat::RGB32F)    { set( GL_RGB32F, GL_RGB, GL_FLOAT ); }
    else if (fmt == PixelFormat::RGBA32F)   { set( GL_RGBA32F, GL_RGBA, GL_FLOAT ); }
    else if (fmt == PixelFormat::R16)       { set( GL_R16I, GL_R16I, GL_SHORT ); }
    else if (fmt == PixelFormat::D24S8)     { set( GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8); }
    else if (fmt == PixelFormat::D32FS8)    { set( GL_DEPTH32F_STENCIL8, GL_DEPTH_STENCIL, GL_FLOAT_32_UNSIGNED_INT_24_8_REV); }
    else if (fmt == PixelFormat::D32F)      { set( GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_FLOAT); }
    else if (fmt == PixelFormat::S8)        { set( GL_STENCIL_INDEX8, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE); }
    else                                    { DE_ERROR( "Unsupported tex format ", fmt.str()) return false; }

    glActiveTexture( GL_TEXTURE0 + unit );
    glBindTexture( tex.targetTyp, tex.textureId );
    // glBindTextureUnit(tex.boundUnit, tex.textureId);

    applyTexParameters( so );

    int level = 0;
    glTexImage2D(tex.targetTyp, // GLenum target
                 level,         // GLint level
                 m_preset,      // GLenum internalFormat
                 w,             // GLsizei width
                 h,             // GLsizei height
                 0,             // GLint border
                 m_format,      // GLenum internalDataFormat
                 m_dataType,    // GLenum internalDataType
                 pixels );      // void* pixel_data, if any, can be nullptr.

    if (so.hasMipmaps())
    {
        glGenerateMipmap( tex.targetTyp );
    }

    // CleanUp -> Unbind texture after creation and upload
    glBindTexture( tex.targetTyp, 0 );

    // Create sampler handle:
    if (!tex.samplerId)
    {
        GLuint samplerId = 0;
        glGenSamplers(1, &samplerId);

        if ( !samplerId )
        {
            DE_ERROR("No samplerId.")
            return false;
        }

        tex.samplerId = samplerId;
    }

    // Apply settings, sampler does not to be bound for configuration
    applySamplerOptions(tex.samplerId, so);

    return true;
}


void
Bridge::applySamplerOptions( uint32_t & samplerId, const SO& so ) const
{
    if (samplerId < 1)
    {
        DE_ERROR("Invalid samplerId.")
        return;
    }

    // glSamplerParameteri(samplerId, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    // glSamplerParameteri(samplerId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // glSamplerParameteri(samplerId, GL_TEXTURE_WRAP_S, GL_REPEAT);
    // glSamplerParameteri(samplerId, GL_TEXTURE_WRAP_T, GL_REPEAT);

    GLint magMode = GL_NEAREST; // Low quality is default.
    switch ( so.mag )
    {
        case SO::Magnify::Linear: magMode = GL_LINEAR; break;// High quality on demand.
        default: break;
    }

    GLint minMode = GL_NEAREST;   // Low quality is default.
    switch ( so.min )
    {
        case SO::Minify::Linear: minMode = GL_LINEAR; break;
        case SO::Minify::NearestMipmapNearest: minMode = GL_NEAREST_MIPMAP_NEAREST; break;
        case SO::Minify::NearestMipmapLinear: minMode = GL_NEAREST_MIPMAP_LINEAR; break;
        case SO::Minify::LinearMipmapNearest: minMode = GL_LINEAR_MIPMAP_NEAREST; break;
        case SO::Minify::LinearMipmapLinear: minMode = GL_LINEAR_MIPMAP_LINEAR; break;
        default: break;
    }

    GLint wrapS = GL_REPEAT;
    switch ( so.wrapS )
    {
        case SO::Wrap::RepeatMirrored: wrapS = GL_MIRRORED_REPEAT; break;
        case SO::Wrap::ClampToEdge: wrapS = GL_CLAMP_TO_EDGE; break;
        case SO::Wrap::ClampToBorder: wrapS = GL_CLAMP_TO_BORDER; break;
        default: break;
    }
    GLint wrapT = GL_REPEAT;
    switch ( so.wrapT )
    {
        case SO::Wrap::RepeatMirrored: wrapT = GL_MIRRORED_REPEAT; break;
        case SO::Wrap::ClampToEdge: wrapT = GL_CLAMP_TO_EDGE; break;
        case SO::Wrap::ClampToBorder: wrapT = GL_CLAMP_TO_BORDER; break;
        default: break;
    }

    //GLfloat borderColor[] = {1.0f, 0.0f, 0.0f, 1.0f}; // Red border
    //glSamplerParameterfv(samplerId, GL_TEXTURE_BORDER_COLOR, borderColor);

    glSamplerParameteri(samplerId, GL_TEXTURE_MIN_FILTER, minMode);
    glSamplerParameteri(samplerId, GL_TEXTURE_MAG_FILTER, magMode);
    glSamplerParameteri(samplerId, GL_TEXTURE_WRAP_S, wrapS);
    glSamplerParameteri(samplerId, GL_TEXTURE_WRAP_T, wrapT);

    // Anisotropic Filtering is only really meaningful with mipmaps:
    if (so.hasMipmaps())
    {
        GLfloat anisotropicFilter = std::clamp( float(so.af), 1.0f, getMaxAnisotropy() );
        glSamplerParameterf(samplerId, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropicFilter );
    }

    // glBindSampler(0, samplerId);
    // return samplerId;
}

// static
void
Bridge::applyTexParameters( const SO& so ) const
{
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAF );

    GLint magMode = GL_NEAREST; // Low quality is default.
    switch ( so.mag )
    {
        case SO::Magnify::Linear: magMode = GL_LINEAR; break;// High quality on demand.
        default: break;
    }

    GLint minMode = GL_NEAREST;   // Low quality is default.
    switch ( so.min )
    {
        case SO::Minify::Linear: minMode = GL_LINEAR; break;
        case SO::Minify::NearestMipmapNearest: minMode = GL_NEAREST_MIPMAP_NEAREST; break;
        case SO::Minify::NearestMipmapLinear: minMode = GL_NEAREST_MIPMAP_LINEAR; break;
        case SO::Minify::LinearMipmapNearest: minMode = GL_LINEAR_MIPMAP_NEAREST; break;
        case SO::Minify::LinearMipmapLinear: minMode = GL_LINEAR_MIPMAP_LINEAR; break;
        default: break;
    }

    GLint wrapS = GL_REPEAT;
    switch ( so.wrapS )
    {
        case SO::Wrap::RepeatMirrored: wrapS = GL_MIRRORED_REPEAT; break;
        case SO::Wrap::ClampToEdge: wrapS = GL_CLAMP_TO_EDGE; break;
        case SO::Wrap::ClampToBorder: wrapS = GL_CLAMP_TO_BORDER; break;
        default: break;
    }
    GLint wrapT = GL_REPEAT;
    switch ( so.wrapT )
    {
        case SO::Wrap::RepeatMirrored: wrapT = GL_MIRRORED_REPEAT; break;
        case SO::Wrap::ClampToEdge: wrapT = GL_CLAMP_TO_EDGE; break;
        case SO::Wrap::ClampToBorder: wrapT = GL_CLAMP_TO_BORDER; break;
        default: break;
    }

    // GLfloat borderColor[] = {1.0f, 0.0f, 0.0f, 1.0f}; // Red border
    // glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);

    // Anisotropic Filtering is only really meaningful with mipmaps:
    if (so.hasMipmaps())
    {
        GLfloat anisotropicFilter = std::clamp( float(so.af), 1.0f, getMaxAnisotropy() );
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropicFilter );
    }
}


/*
Texture* TexManager::createTexture2D( const std::string& name, const Image& img, const SamplerOptions& so )
{
    if (img.empty())
    {
        DE_ERROR("Empty image ",name)
        return nullptr;
    }

    return createTexture2D( name, img.w(), img.h(), img.data(), img.pixelFormat(), so );
}

Texture* TexManager::createTexture2D( const std::string& name, int w, int h, void const* const pixels, PixelFormat fmt, const SamplerOptions& so )
{
    auto cached = getTexture( name );
    if (cached)
    {
        DE_ERROR("Texture name already exist, ", name)
        return nullptr;
    }

    Texture* tex = new Texture();
    tex->m_name = name;
    tex->m_target = GL_TEXTURE_2D;
    tex->m_unit = -1;

    if (!uploadTexture2D( tex, w, h, pixels, fmt, so ))
    {
        DE_ERROR("Upload failed, tex(",tex->str(), ")")
    }

    m_texCache.emplace_back( tex );
    return m_texCache.back();
    return tex;
}
*/

} // namespace gl
} // namespace de
