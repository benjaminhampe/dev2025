#include <de/gpu/Texture.h>
#include <de_opengl.h>
#include <de/gpu/GL_debug_layer.h>

namespace de {
namespace gpu {

std::string TextureFormat_str(uint32_t internalFormat)
{
    switch (internalFormat)
    {
        // --- Basis-Formate (Legacy / Transfer-Formate) ---
        case GL_RED:                           return "GL_RED";
        case GL_RG:                            return "GL_RG";
        case GL_RGB:                           return "GL_RGB";
        case GL_RGBA:                          return "GL_RGBA";

        // --- 8-Bit Formate ---
        case GL_R8:                            return "GL_R8";
        case GL_RG8:                           return "GL_RG8";
        case GL_RGB8:                          return "GL_RGB8";
        case GL_RGBA8:                         return "GL_RGBA8";

        case GL_R8_SNORM:                      return "GL_R8_SNORM";
        case GL_RG8_SNORM:                     return "GL_RG8_SNORM";
        case GL_RGB8_SNORM:                    return "GL_RGB8_SNORM";
        case GL_RGBA8_SNORM:                   return "GL_RGBA8_SNORM";

        // --- 16-Bit Formate ---
        case GL_R16:                           return "GL_R16";
        case GL_RG16:                          return "GL_RG16";
        case GL_RGB16:                         return "GL_RGB16";
        case GL_RGBA16:                        return "GL_RGBA16";

        case GL_R16_SNORM:                     return "GL_R16_SNORM";
        case GL_RG16_SNORM:                    return "GL_RG16_SNORM";
        case GL_RGB16_SNORM:                   return "GL_RGB16_SNORM";
        case GL_RGBA16_SNORM:                  return "GL_RGBA16_SNORM";

        // --- Gleitkomma (Floating-Point) Formate ---
        case GL_R16F:                          return "GL_R16F";
        case GL_RG16F:                         return "GL_RG16F";
        case GL_RGB16F:                        return "GL_RGB16F";
        case GL_RGBA16F:                       return "GL_RGBA16F";

        case GL_R32F:                          return "GL_R32F";
        case GL_RG32F:                         return "GL_RG32F";
        case GL_RGB32F:                        return "GL_RGB32F";
        case GL_RGBA32F:                       return "GL_RGBA32F";

        // --- Ganzzahlige Formate (Signed Integers) ---
        case GL_R8I:                           return "GL_R8I";
        case GL_RG8I:                          return "GL_RG8I";
        case GL_RGB8I:                         return "GL_RGB8I";
        case GL_RGBA8I:                        return "GL_RGBA8I";

        case GL_R16I:                          return "GL_R16I";
        case GL_RG16I:                         return "GL_RG16I";
        case GL_RGB16I:                        return "GL_RGB16I";
        case GL_RGBA16I:                       return "GL_RGBA16I";

        case GL_R32I:                          return "GL_R32I";
        case GL_RG32I:                         return "GL_RG32I";
        case GL_RGB32I:                        return "GL_RGB32I";
        case GL_RGBA32I:                       return "GL_RGBA32I";

        // --- Vorzeichenlose Ganzzahlige Formate (Unsigned Integers) ---
        case GL_R8UI:                          return "GL_R8UI";
        case GL_RG8UI:                         return "GL_RG8UI";
        case GL_RGB8UI:                        return "GL_RGB8UI";
        case GL_RGBA8UI:                       return "GL_RGBA8UI";

        case GL_R16UI:                         return "GL_R16UI";
        case GL_RG16UI:                        return "GL_RG16UI";
        case GL_RGB16UI:                       return "GL_RGB16UI";
        case GL_RGBA16UI:                      return "GL_RGBA16UI";

        case GL_R32UI:                         return "GL_R32UI";
        case GL_RG32UI:                        return "GL_RG32UI";
        case GL_RGB32UI:                       return "GL_RGB32UI";
        case GL_RGBA32UI:                      return "GL_RGBA32UI";

        // --- sRGB Formate ---
        case GL_SRGB8:                         return "GL_SRGB8";
        case GL_SRGB8_ALPHA8:                  return "GL_SRGB8_ALPHA8";

        // --- Spezielle gepackte Farbformate ---
        case GL_R11F_G11F_B10F:                return "GL_R11F_G11F_B10F";
        case GL_RGB9_E5:                       return "GL_RGB9_E5";
        case GL_RGB10_A2:                      return "GL_RGB10_A2";
        case GL_RGB10_A2UI:                    return "GL_RGB10_A2UI";

        // --- Tiefen- und Stencil-Formate ---
        case GL_DEPTH_COMPONENT16:             return "GL_DEPTH_COMPONENT16";
        case GL_DEPTH_COMPONENT24:             return "GL_DEPTH_COMPONENT24";
        case GL_DEPTH_COMPONENT32F:            return "GL_DEPTH_COMPONENT32F";
        case GL_DEPTH24_STENCIL8:              return "GL_DEPTH24_STENCIL8";
        case GL_DEPTH32F_STENCIL8:             return "GL_DEPTH32F_STENCIL8";
        case GL_STENCIL_INDEX8:                return "GL_STENCIL_INDEX8";

        default:                               return "UNKNOWN_FORMAT";
    }
}

// ===========================================================================
Texture::Texture()
// ===========================================================================
    : m_id(0)
    , m_target(0)
    , m_unit(-1)
    , m_mipLevels(0)
    , m_w(0)
    , m_h(0)
    , m_layers(0)
    , m_gpuFormat(0)
    , m_cpuFormat(0)
    , m_dataType(0)
    , m_so()
    , m_fmt(PixelFormat::Unknown)
{}

void Texture::destroy()
{
    if (m_id != 0)
    {
        glDeleteTextures(1, &m_id);
        m_id = 0;
    }
}

std::string Texture::str() const
{
    std::ostringstream o; o <<
    "name:" << m_name << "|"
    "id:" << m_id << "|"
    "target:" << m_target << "|"
    "unit:" << m_unit << "|"
    "w:" << m_w << "|"
    "h:" << m_h << "|"
    "layers:" << m_layers << "|"
    "mipLevels:" << m_mipLevels << "|"
    "gpuFormat:" << TextureFormat_str(m_gpuFormat) << "|"
    "cpuFormat:" << TextureFormat_str(m_cpuFormat) << "|"
    "dataType:" << TextureFormat_str(m_dataType) << "|"
    "pixFormat:" << pixelFormatStr() << "|"
    "so:" << m_so.str();
    return o.str();
}

/*
std::string Tex2DArray::str() const
{
    std::ostringstream o; o <<
    "id(" << m_id << "), "
    "w(" << m_w << "), "
    "h(" << m_h << "), "
    "layers(" << layers << "), "
    "mipLevels(" << mipLevels << "), "
    "gpuFormat(" << TextureFormat_str(gpuFormat) << ")";
    return o.str();
}
*/

// ===========================================================================
Tex2DArray::Tex2DArray()
// ===========================================================================
    : Texture()
{

}

/*
Tex2DArray::~Tex2DArray()
{
    if (m_id != 0)
    {
        DE_ERROR("Forgot to call destroy(), ")
        //destroy();
    }
}

// 1. Der Move-Konstruktor (Kein self-check nötig!)
Tex2DArray::Tex2DArray(Tex2DArray&& other) noexcept
    : id(other.id)
    , w(other.w)
    , h(other.h)
    , layers(other.layers)
    , mipLevels(other.mipLevels)
    , gpuFormat(other.gpuFormat)
    , so(other.so)
{
    // Ganz wichtig: Dem alten Objekt die ID wegnehmen,
    // damit dessen Destruktor die Textur nicht auf der GPU löscht!
    other.id = 0;
}

// 2. Der Move-Zuweisungsoperator (HIER ist der Check Pflicht!)
Tex2DArray&
Tex2DArray::operator=(Tex2DArray&& other) noexcept
{
    if (this != &other)
    {
        // Clean our house before absorbing new furnitures.
        destroy();

        // New furnitures:
        id = other.id;
        w = other.w;
        h = other.h;
        layers = other.layers;
        mipLevels = other.mipLevels;
        gpuFormat = other.gpuFormat;
        so = other.so;

        // Leave an eviction notice.
        other.id = 0;
    }
    return *this;
}
*/

// Tex2DArray::Tex2DArray(const Tex2DArray&) = delete;
// Tex2DArray::Tex2DArray& operator=(const Tex2DArray&) = delete;


/**
 * Erstellt das Textur-Array. Berechnet automatisch die maximal möglichen Mipmap-Stufen.
 */
bool
Tex2DArray::create(const std::string& name, const std::vector<const Image*>& images, const SamplerOptions& so)
{
    destroy();

    if (images.empty())
    {
        DE_ERROR("Empty image vector, ",name)
        return false;
    }

    // DE_DEBUG("Image vector, ",images.size())

    // Bilder in Level 0 hochladen
    int32_t img_w = images[0]->w();
    int32_t img_h = images[0]->h();
    auto img_fmt = images[0]->pixelFormat();

    for (size_t i = 1; i < images.size(); ++i)
    {
        const Image* img = images[i];
        if (!img)
        {
            DE_ERROR("Null img[",i,"]")
            continue;
        }
        const int32_t w2 = img->w();
        const int32_t h2 = img->h();
        const auto fmt2 = img->pixelFormat();

        if (w2 != img_w)
        {
            DE_ERROR("Img[",i,"] w2(",w2,") != w(",img_w,")")
            return false;
        }
        if (h2 != img_h)
        {
            DE_ERROR("Img[",i,"] h2(",h2,") != h(",img_h,")")
            return false;
        }
        if (fmt2 != img_fmt)
        {
            DE_ERROR("Img[",i,"] fmt2(",fmt2.str(),") != fmt(",img_fmt.str(),")")
            return false;
        }
    }

    if ((img_fmt != PixelFormat::R8G8B8) &&
        (img_fmt != PixelFormat::R8G8B8A8))
    {
        DE_ERROR("Unsupported pixelFormat ",img_fmt.str())
        return false;
    }

    m_name = name;
    m_w = img_w;   // Setzt die Ziel-Breite fest
    m_h = img_h;  // Setzt die Ziel-Höhe fest
    m_layers = static_cast<GLsizei>(images.size());
    m_mipLevels = static_cast<GLsizei>(std::floor(std::log2(std::max(m_w, m_h)))) + 1;
    m_gpuFormat = (img_fmt == PixelFormat::R8G8B8) ? GL_RGB8 : GL_RGBA8;
    m_cpuFormat = (img_fmt == PixelFormat::R8G8B8) ? GL_RGB : GL_RGBA;
    m_dataType = GL_UNSIGNED_BYTE;

    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_2D_ARRAY, m_id);
    glTexStorage3D(
        GL_TEXTURE_2D_ARRAY,
        m_mipLevels,
        m_gpuFormat, // internalFormat
        m_w,
        m_h,
        m_layers);

    // tex.mipLevels
    // 1. Calculate the CPU-side data format based on your PixelFormat enum


    // Bilder in Level 0 hochladen
    for (GLsizei i = 0; i < m_layers; ++i)
    {
        glTexSubImage3D(
            GL_TEXTURE_2D_ARRAY,// GLenum target
            0,                  // GLint level
            0,                  // GLint xoffset
            0,                  // GLint yoffset
            i,                  // GLint zoffset
            m_w,                // GLsizei width
            m_h,                // GLsizei height
            1,                  // GLsizei depth
            m_cpuFormat,        // GLenum format
            m_dataType,         // GLenum type
            images[i]->data()   // const void *pixels
        );
    }

    // Mipmaps nur generieren, wenn der Sampler-Filter sie anfordert
    if (so.hasMipmaps()) { glGenerateMipmap(GL_TEXTURE_2D_ARRAY); }

    applySamplerOptions(so);

    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

    // DE_DEBUG("Created Tex2DArray ",str())
    return true;
}

// ===========================================================================
TexRef::TexRef()
// ===========================================================================
    : tex(nullptr)
    , layer(0)
    , miplayer(0)
    , dummy(0)
    , fmt(PixelFormat::Unknown)
    , rect()
    , coords(0,0,1,1)
    , repeat(1,1)
    , intensity(1)
    , gamma(1)
{}

TexRef::TexRef(Texture* tex_)
    : TexRef()
{
    reset(tex_);
}

TexRef::TexRef(Texture* tex_, const Recti& pos_)
    : TexRef()
{
    reset(tex_);
    coords = computeCoords(tex, pos_);
}

TexRef::TexRef(Texture* tex_, const Recti& pos_, const glm::vec2& repeat_) //  = glm::vec2(1,1)
    : TexRef()
{
    reset(tex_);
    coords = computeCoords(tex, pos_);
    repeat = repeat_;
}

void
TexRef::reset(Texture* tex_, uint32_t layer_ )
{
    layer = layer_;
    coords = glm::vec4(0,0,1,1);
    repeat = glm::vec2(1,1);
    intensity = 1.0f;
    gamma = 1.0f;
    tex = nullptr;
    fmt = PixelFormat::Unknown;
    so = SO( 0, SO::Minify::Nearest, SO::Magnify::Nearest,
                SO::Wrap::Repeat, SO::Wrap::Repeat, SO::Wrap::Repeat );
    rect = Recti(0,0,0,0);

    if (tex_)
    {
        tex = tex_;
        fmt = tex_->pixelFormat();
        so = tex_->so();
        rect = Recti(0,0,tex_->w(), tex_->h());

    }
}

uint32_t
TexRef::w() const { return rect.w; }

uint32_t
TexRef::h() const { return rect.h; }

std::string
TexRef::str() const
{
    std::ostringstream o;
    o << "TexRef["<< sizeof(TexRef) << "](";
    if (tex)
    {
        o << "name(" << name << "), tex(" << tex->str() << "), "
        "coords(" <<coords.x<<","<<coords.y << ","<<coords.z<<","<<coords.w << "), "
        "repeat(" << repeat.x << "," << repeat.y << ")";
    }
    else
    {
        o << "empty)";
    }

    return o.str();
}

bool
TexRef::empty() const
{
    constexpr float e = std::numeric_limits<float>::epsilon();
    return !tex || !tex->id() || (coords.z <= e) || (coords.w <= e);
}

glm::vec4
TexRef::computeCoords( Texture* tex, Recti _pos)
{
    glm::vec4 retVal = glm::vec4(0,0,1,1);
    if (tex)
    {
        int w = tex->w();
        int h = tex->h();
        if (w > 0 && h > 0)
        {
            retVal = glm::vec4(_pos.x, _pos.y, _pos.w, _pos.h)
                   / glm::vec4(w,h,w,h);

            // retVal = glm::vec4( float(_pos.x) / float(w),
            //                    float(_pos.y) / float(h),
            //                    float(_pos.w) / float(w),
            //                    float(_pos.h) / float(h) );
        }
    }
    return retVal;
};

void
TexRef::setTransform( float x, float y, float w, float h )
{
    coords = glm::vec4( x,y,w,h );
}

const glm::vec4&
TexRef::getTransform() const
{
    return coords;
}

// ===========================================================================
TexManager::TexManager()
// ===========================================================================
    : m_numTexUnits(4)
    , m_maxTex2DSize(1024)
    , m_maxAnisotropy(0)
{
}

TexManager::~TexManager()
{
    //close();
}

void TexManager::init()
{
    //glGetIntegerv( GL_MAJOR_VERSION, &m_shaderVersionMajor );
    //glGetIntegerv( GL_MINOR_VERSION, &m_shaderVersionMinor );
    glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
GL_VALIDATE
    glPixelStorei( GL_PACK_ALIGNMENT, 1 );
GL_VALIDATE
    m_numTexUnits = uint32_t( glGetInteger( GL_MAX_TEXTURE_IMAGE_UNITS ) );
GL_VALIDATE
    m_maxTex2DSize = uint32_t( glGetInteger( GL_MAX_TEXTURE_SIZE ) );
GL_VALIDATE
    GLfloat maxAF = 0.0f;
    glGetFloatv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAF );
GL_VALIDATE
    m_maxAnisotropy = maxAF;
GL_VALIDATE
    // DE_DEBUG( "NumHardwareTexUnits = ", m_numTexUnits )
    // DE_DEBUG( "Max.Texture2D.Size = ", m_maxTex2DSize )
    // DE_DEBUG( "Max.Anisotropy = ", m_maxAnisotropy )
    //m_texUnits.clear();
    //m_texUnits.resize( nTexUnits, TexRef() );
    //dumpTextures();
}

void TexManager::deinit()
{
    removeTextures();
    updateTextures();
}

// ######################
// ###   TexManager   ###
// ######################

void TexManager::dumpTextures() const
{
    DE_DEBUG( "TexUnit.Count = ", m_numTexUnits )

/*
    DE_DEBUG( "TexUnit.Count = ", m_texUnits.size() )
    for ( size_t i = 0; i < m_texUnits.size(); ++i )
    {
        DE_DEBUG( "TexUnit[",i,"] = ", m_texUnits[ i ].toString() )
    }
*/

    DE_DEBUG( "TexCache.Count = ", m_texCache.size() )
    for ( size_t i = 0; i < m_texCache.size(); ++i )
    {
        Texture* tex = m_texCache[ i ];
        DE_DEBUG( "[",i,"] ", tex->str() )
    }
}

void TexManager::updateTextures()
{
    for ( size_t i = 0; i < m_texTrash.size(); ++i )
    {
        Texture* markedAsTrash = m_texTrash[ i ];
        if (!markedAsTrash)
        {
            DE_ERROR("Got nullptr at index ",i)
            continue;
        }
        markedAsTrash->destroy();
    }
    m_texTrash.clear();
}

void TexManager::removeTextures()
{
    for ( size_t i = 0; i < m_texCache.size(); ++i )
    {
        m_texTrash.emplace_back( m_texCache[ i ] );
    }
    m_texTrash.clear();
}

void TexManager::removeTexture( const std::string& name )
{
    Texture* tex = getTexture( name );
    if ( !tex )
    {
        DE_DEBUG("No texture with name ", name)
        return;
    }

    removeTexture( tex );
}

void TexManager::removeTexture( Texture* &tex )
{
    if ( !tex )
    {
        DE_WARN("Got nullptr")
        return;
    }

    if ( tex->empty() )
    {
        DE_WARN("Empty texture ", tex->str())
        return;
    }

    // Delete cache entry
    auto it = std::find_if( m_texCache.begin(), m_texCache.end(),
        [tex] ( Texture const * const cached )
        {
            return cached->id() == tex->id();
        });
    if ( it == m_texCache.end() )
    {
        DE_WARN("No cached texture with id ", tex->id())
        return;
    }

    DE_TRACE("[TexCache] MarkObsolete ", tex->str() )

    //unbindTexture( ref );
    //m_refs.erase( cached->name() );
    m_texTrash.emplace_back( *it );
    m_texCache.erase( it );
}

Texture* TexManager::getTexture( const std::string& name ) const
{
    // We abort on empty string, nothing todo, return invalid texref.
    if ( name.empty() ) { return nullptr; }

    // Delete cache entry
    auto it = std::find_if( m_texCache.begin(), m_texCache.end(),
        [name] ( Texture const * const cached )
        {
            return cached->name() == name;
        });
    if ( it == m_texCache.end() )
    {
        // (cachemiss)...
        // If cache has no string 'name' then it always tries to load
        // the given filename 'name'. This 'always loading' will be bad
        // behaviour if the ressource file does not exist.
        //DE_ERROR("The texture does not exist, call create/loadTexture() before using it. ", name)
        return nullptr; // Cache Miss
    }
    return *it; // Cache hit
}

void TexManager::applySamplerOptions(SamplerOptions &so)
{
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAF );

    GLint magMode = GL_NEAREST; // Low quality is default.
    switch ( so.mag )
    {
        case SamplerOptions::Magnify::Linear: magMode = GL_LINEAR; break;// High quality on demand.
        default: break;
    }

    GLint minMode = GL_NEAREST;   // Low quality is default.
    switch ( so.min )
    {
        case SamplerOptions::Minify::Linear: minMode = GL_LINEAR; break;
        case SamplerOptions::Minify::NearestMipmapNearest: minMode = GL_NEAREST_MIPMAP_NEAREST; break;
        case SamplerOptions::Minify::NearestMipmapLinear: minMode = GL_NEAREST_MIPMAP_LINEAR; break;
        case SamplerOptions::Minify::LinearMipmapNearest: minMode = GL_LINEAR_MIPMAP_NEAREST; break;
        case SamplerOptions::Minify::LinearMipmapLinear: minMode = GL_LINEAR_MIPMAP_LINEAR; break;
        default: break;
    }

    GLint wrapS = GL_REPEAT;
    switch ( so.wrapS )
    {
        case SamplerOptions::Wrap::RepeatMirrored: wrapS = GL_MIRRORED_REPEAT; break;
        case SamplerOptions::Wrap::ClampToEdge: wrapS = GL_CLAMP_TO_EDGE; break;
        case SamplerOptions::Wrap::ClampToBorder: wrapS = GL_CLAMP_TO_BORDER; break;
        default: break;
    }
    GLint wrapT = GL_REPEAT;
    switch ( so.wrapT )
    {
        case SamplerOptions::Wrap::RepeatMirrored: wrapT = GL_MIRRORED_REPEAT; break;
        case SamplerOptions::Wrap::ClampToEdge: wrapT = GL_CLAMP_TO_EDGE; break;
        case SamplerOptions::Wrap::ClampToBorder: wrapT = GL_CLAMP_TO_BORDER; break;
        default: break;
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minMode);
GL_VALIDATE
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magMode);
GL_VALIDATE
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
GL_VALIDATE
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
GL_VALIDATE

    // anisotropicFilter
    if ( so.af > 0 )
    {
        if (so.af == 1)
        {
            GLfloat maxAF = 0.0f;
            glGetFloatv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAF );
GL_VALIDATE
            so.af = static_cast<uint8_t>(maxAF);
        }
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, so.af );
GL_VALIDATE
    }
}

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
    if (name.empty())
    {
        DE_ERROR("Empty name")
        return nullptr;
    }

    auto cached = getTexture( name );
    if (cached)
    {
        DE_ERROR("Texture name already exist, ", name)
        return nullptr;
    }

    Texture* tex = new Texture();
    tex->set_name(name);
    tex->set_target(GL_TEXTURE_2D);
    tex->set_unit(-1);
    tex->set_so(so);

    if (!uploadTexture2D( tex, w, h, pixels, fmt ))
    {
        DE_ERROR("Upload failed, tex(",tex->str(), ")")
        tex->destroy();
        delete tex;
        return nullptr;
    }

    m_texCache.emplace_back( tex );
    return m_texCache.back();
    // return tex;
}

bool TexManager::uploadTexture2D( Texture* &tex, int w, int h, void const* const pixels, PixelFormat fmt )
{
    if (!tex)
    {
        DE_ERROR("Got nullptr texture")
        return false;
    }

    if (tex->empty())
    {
        uint32_t texId = 0;
        glGenTextures(1, &texId);
GL_VALIDATE
        if ( !texId )
        {
            DE_ERROR("No tex created ", tex->str())
            return false;
        }

        tex->set_id(texId);
    }

    if (tex->empty())
    {
        DE_ERROR( "No tex id")
        return false;
    }

    if ( w < 1 || h < 1 )
    {
        DE_ERROR( "No tex size(",w,",",h,")")
        return false;
    }

    tex->set_target(GL_TEXTURE_2D);
    tex->set_w(w);
    tex->set_h(h);
    tex->set_pixelFormat(fmt);
    tex->set_layers(0);

         if (fmt == PixelFormat::R8)        { tex->set( GL_RED, GL_RED, GL_UNSIGNED_BYTE ); }
    else if (fmt == PixelFormat::R8G8B8)    { tex->set( GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE); } // GL_SRGB
    else if (fmt == PixelFormat::R8G8B8A8)  { tex->set( GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE ); } // gammaCorrection ? GL_SRGB_ALPHA
    else if (fmt == PixelFormat::R32F)      { tex->set( GL_R32F, GL_RED, GL_FLOAT); }
    else if (fmt == PixelFormat::RGB16F)    { tex->set( GL_RGB16F, GL_RGB, GL_FLOAT ); }
    else if (fmt == PixelFormat::RGBA16F)   { tex->set( GL_RGBA16F, GL_RGBA, GL_FLOAT ); }
    else if (fmt == PixelFormat::RGB32F)    { tex->set( GL_RGB32F, GL_RGB, GL_FLOAT ); }
    else if (fmt == PixelFormat::RGBA32F)   { tex->set( GL_RGBA32F, GL_RGBA, GL_FLOAT ); }
    else if (fmt == PixelFormat::R16)       { tex->set( GL_R16I, GL_R16I, GL_SHORT ); }
    else if (fmt == PixelFormat::D24S8)     { tex->set( GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8); }
    else if (fmt == PixelFormat::D32FS8)    { tex->set( GL_DEPTH32F_STENCIL8, GL_DEPTH_STENCIL, GL_FLOAT_32_UNSIGNED_INT_24_8_REV); }
    else if (fmt == PixelFormat::D32F)      { tex->set( GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_FLOAT); }
    else if (fmt == PixelFormat::S8)        { tex->set( GL_STENCIL_INDEX8, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE); }
    else                                    { DE_ERROR( "Unsupported tex format ", fmt.str()) return false; }

    tex->set_unit(-1);
    glBindTexture( GL_TEXTURE_2D, tex->id() );
GL_VALIDATE

    applySamplerOptions( tex->so() );

    glTexImage2D(tex->target(),     // GLenum target
                 0,                 // GLint level
                 tex->gpuFormat(),  // GLenum internalFormat
                 w,                 // GLsizei width
                 h,                 // GLsizei height
                 0,                 // GLint border
                 tex->cpuFormat(),  // GLenum internalDataFormat
                 tex->dataType(),   // GLenum internalDataType
                 pixels );          // void* pixel_data, if any, can be nullptr.

    if (tex->so().hasMipmaps())
    {
        glGenerateMipmap( tex->target() );
    }

    glBindTexture( tex->target(), 0 );
    return true;
}

Texture* TexManager::loadTexture2D( const std::string& name, const SamplerOptions& so, ImageLoadOptions const & ilo )
{
    Image img;
    if ( !dbLoadImage( img, name, ilo ) )
    {
        DE_ERROR(name, "Cant load texture ",name)
        //DE_ERROR("No image loaded for texture ", name )
        return nullptr;
    }

    return createTexture2D( name, img, so );
}

Tex2DArray*
TexManager::createTex2DArray(
        const std::string& name,
        const std::vector<const Image*>& images,
        const SamplerOptions& so)
{
    if (name.empty())
    {
        DE_ERROR("Empty name")
        return nullptr;
    }

    auto cached = getTexture( name );
    if (cached)
    {
        DE_ERROR("Texture name already exist, ", name)
        return nullptr;
    }

    Tex2DArray* tex = new Tex2DArray();

    if (!tex->create(name,images,so))
    {
        DE_ERROR("Cannot create Tex2DArray ", tex->str())
        tex->destroy();
        delete tex;
        return nullptr;
    }

    m_texCache.emplace_back( tex );
    return tex;
}

} // end namespace gpu.
} // end namespace de.



/*
uint32_t GT_createTexHandle()
{
    uint32_t texId = 0;
    glGenTextures(1, &texId);
    if ( !texId )
    {
        DE_ERROR("No tex created")
    }
    return texId;
}

void GT_deleteTexHandle( uint32_t & texId )
{
    if ( !texId ) { return; }
    glDeleteTextures( 1, &texId );
    texId = 0;
}

// returns valid stage (texunit) of bound texture on success.
// returns -2 on fail.
// if stage is -1, then no glActiveTexture is called
int GT_bindTex2D( uint32_t texId, int stage )
{
    if ( !texId )
    {
        DE_ERROR( "No texture id ", texId )
        return -1;
    }

    if ( stage < 0 )
    {
        DE_ERROR( "Fix invalid stage ", stage )
        stage = 0;
    }

    glActiveTexture( GL_TEXTURE0 + stage );
    glBindTexture( GL_TEXTURE_2D, texId );
    return stage;
}

void GT_unbindTex2D( int stage )
{
    if ( stage < 0 )
    {
        DE_ERROR( "Fix invalid stage ", stage )
        stage = 0;
    }

    glActiveTexture( GL_TEXTURE0 + stage );
    glBindTexture( GL_TEXTURE_2D, 0 );
}

void GT_applyTex2DOptions( SamplerOptions const & so )
{
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAF );

    GLint magMode = GL_NEAREST;   // Low quality is default.
    if ( so.mag == SamplerOptions::Magnify::Linear ) { magMode = GL_LINEAR; }// High quality on demand.

    GLint minMode = GL_NEAREST;   // Low quality is default.
    switch ( so.min )
    {
        case SamplerOptions::Minify::Linear: minMode = GL_LINEAR; break;
        case SamplerOptions::Minify::NearestMipmapNearest: minMode = GL_NEAREST_MIPMAP_NEAREST; break;
        case SamplerOptions::Minify::NearestMipmapLinear: minMode = GL_NEAREST_MIPMAP_LINEAR; break;
        case SamplerOptions::Minify::LinearMipmapNearest: minMode = GL_LINEAR_MIPMAP_NEAREST; break;
        case SamplerOptions::Minify::LinearMipmapLinear: minMode = GL_LINEAR_MIPMAP_LINEAR; break;
        default: break;
    }

    GLint wrapS = GL_REPEAT;
    switch ( so.wrapS )
    {
        case SamplerOptions::Wrap::RepeatMirrored: wrapS = GL_MIRRORED_REPEAT; break;
        case SamplerOptions::Wrap::ClampToEdge: wrapS = GL_CLAMP_TO_EDGE; break;
        case SamplerOptions::Wrap::ClampToBorder: wrapS = GL_CLAMP_TO_BORDER; break;
        default: break;
    }
    GLint wrapT = GL_REPEAT;
    switch ( so.wrapT )
    {
        case SamplerOptions::Wrap::RepeatMirrored: wrapT = GL_MIRRORED_REPEAT; break;
        case SamplerOptions::Wrap::ClampToEdge: wrapT = GL_CLAMP_TO_EDGE; break;
        case SamplerOptions::Wrap::ClampToBorder: wrapT = GL_CLAMP_TO_BORDER; break;
        default: break;
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);

    if ( so.af > 0 ) // anisotropicFilter
    {
        GLfloat maxAF = 0.0f;
        glGetFloatv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAF );
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAF );
        //so.af = static_cast<uint8_t>(maxAF);
    }
}

bool
GT_uploadTex2D( TexGpuHandle & handle, const Image& img, const SamplerOptions& so )
{
    handle.target = GL_TEXTURE_2D;
    const int w = img.w();
    const int h = img.h();
    const void * const pixels = img.data();
    const PixelFormat fmt = img.pixelFormat();
    return GT_uploadTex2D( handle, w, h, pixels, fmt, so );
}

bool
GT_uploadTex2D( TexGpuHandle & handle, int w, int h, void const* const pixels, PixelFormat fmt, SamplerOptions const & so )
{
    if ( w < 1 || h < 1 )
    {
        DE_ERROR( "No tex size(",w,",",h,")")
        return false;
    }

    handle.target = GL_TEXTURE_2D;
    handle.fmt = fmt;
    handle.w = w;
    handle.h = h;
    handle.so = so;
    handle.layer = 0;

    // GenTexture
    if ( !handle.id )
    {
        handle.id = GT_createTexHandle();
        if ( !handle.id )
        {
            DE_ERROR("No tex.id created")
            return false;
        }
    }

    if (fmt == PixelFormat::R8) { handle.set( GL_RED, GL_RED, GL_UNSIGNED_BYTE ); }
    else if (fmt == PixelFormat::R8G8B8) { handle.set( GL_RGB, GL_RGB, GL_UNSIGNED_BYTE); } // GL_SRGB
    else if (fmt == PixelFormat::R8G8B8A8) { handle.set( GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE ); } // gammaCorrection ? GL_SRGB_ALPHA
    else if (fmt == PixelFormat::R32F) { handle.set( GL_R32F, GL_R32F, GL_FLOAT); }
    else if (fmt == PixelFormat::RGB16F) { handle.set( GL_RGB16F, GL_RGB, GL_FLOAT ); }
    else if (fmt == PixelFormat::RGBA16F) { handle.set( GL_RGBA16F, GL_RGBA, GL_FLOAT ); }
    else if (fmt == PixelFormat::RGB32F) { handle.set( GL_RGB32F, GL_RGB, GL_FLOAT ); }
    else if (fmt == PixelFormat::RGBA32F) { handle.set( GL_RGBA32F, GL_RGBA, GL_FLOAT ); }
    else if (fmt == PixelFormat::R16) { handle.set( GL_R16I, GL_R16I, GL_SHORT ); }
    else if (fmt == PixelFormat::D24S8) { handle.set( GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8); }
    else
    {
        DE_ERROR( "Unsupported tex format")
        return false;
    }

    glActiveTexture( GL_TEXTURE0 + 0 );
    glBindTexture( handle.target, handle.id );

    GT_applyTex2DOptions( so );

    glTexImage2D(handle.target,       // GLenum target
                 handle.layer,      // GLint level
                 handle.preset,    // GLenum internalFormat
                 w,                 // GLsizei width
                 h,                 // GLsizei height
                 0,                 // GLint border
                 handle.format,    // GLenum internalDataFormat
                 handle.dataType,  // GLenum internalDataType
                 pixels );          // void* pixel_data, if any, can be nullptr.

    GL_VALIDATE

    GT_unbindTex2D( 0 );
    return true;
}

TexGpuHandle
GT_createTexture2D( Image const & img, SamplerOptions const & so )
{
    TexGpuHandle handle;

    if (!GT_uploadTex2D( handle, img, so ))
    {
        DE_ERROR("Got upload error")
    }

    return handle;
}

TexGpuHandle
GT_loadTexture2D( std::string const & uri, SamplerOptions const & so, ImageLoadOptions const & ilo )
{
    TexGpuHandle handle;

    // Load image
    Image img;
    if ( !dbLoadImage( img, uri, ilo ) )
    {
        DE_ERROR( "Cant load image ", uri)
        return handle;
    }

    return GT_createTexture2D( img, so );
}

Texture* TexManager::createTexture2D( const std::string& name )
{
    if (name.empty())
    {
        DE_ERROR("Invalid empty texture name.")
        return nullptr;
    }

    if (getTexture(name))
    {
        DE_ERROR( "Texture already exists, ", name)
        return nullptr;
    }

    const uint32_t texId = GT_createTexHandle();
    if ( !texId )
    {
        DE_ERROR("Cannot create texture handle, ", name)
        return nullptr;
    }

    auto tex = new Texture();
    tex->id = texId;
    tex->name = name;
    //tex->type = 0; // GL_TEXTURE_2D;
    //tex->unit = -1;
    m_texCache.emplace_back( tex );
    return m_texCache.back();
}

bool VideoDriverGL::uploadTexture( ITexture* tex, Image const & img )
{
    if ( !tex )
    {
        return false;
    }

    return tex->upload( img.w(), img.h(), img.data(), img.getFormat() );
}

int32_t VideoDriverGL::findUnit( uint32_t texId ) const
{
    if ( texId < 1 ) return -1; // Not an id.
    for ( size_t i = 0; i < m_TexUnits.size(); ++i )
    {
        if ( m_TexUnits[ i ] == texId )
        {
            return int( i );
        }
    }
    return -1;
}

bool VideoDriverGL::bindTextureId( int stage, uint32_t texId )
{
    if ( texId < 1 )
    {
        DE_WARN("More an unbind with texId = ", texId )
    }

    if ( stage < 0 || stage >= int(m_TexUnits.size()) )
    {
        DE_ERROR("No valid stage ",stage,"/", int(m_TexUnits.size()) )
        return false;
    }

    if ( m_TexUnits[ stage ] == texId )
    {
        return true; // Cache hit, nothing todo.
    }

    m_TexUnits[ stage ] = texId; // Update unit
    glActiveTexture( GL_TEXTURE0 + stage ); GL_VALIDATE
    glBindTexture( GL_TEXTURE_2D, texId ); GL_VALIDATE
    //   bool ok = GL_VALIDATE;
    //   if ( !ok )
    //   {
    //      throw std::runtime_error( dbStrJoin( "No stage(",stage,") and texId(",texId,")" ) );
    //   }
    // ShaderUtil::setSamplerOptions2D( tex->getSamplerOptions() );
    return true;
}

bool VideoDriverGL::unbindTextureId( uint32_t texId )
{
    if ( texId < 1 ) return true;  // Nothing to unbind.

    // Search unit with this tex...
    int found = findUnit( texId );
    if ( found < 0 )
    {
        return true; // Nothing to unbind.
    }

    m_TexUnits[ found ] = 0;
    glActiveTexture( GL_TEXTURE0 + found ); GL_VALIDATE
    glBindTexture( GL_TEXTURE_2D, 0 ); GL_VALIDATE
    // DE_DEBUG( "Unbound texture ", texId, " from stage ", found, " and target=", target )
    return true;
}

// More intelligent GL: stage/unit is auto select.
// AutoSelect a free tex unit and return its index for use in setSamplerUniform().
int VideoDriverGL::bindTextureId( uint32_t texId )
{
    if ( texId < 1 ) { return -1; }

    int unit = findUnit( texId );
    if ( unit > -1 )
    {
        return unit; // Cache hit, activated and bound.
    }

    // Find free unit, if any, and activate it
    for ( size_t i = 0; i < m_TexUnits.size(); ++i )
    {
        if ( !m_TexUnits[ i ] )
        {
            if ( bindTextureId( i, texId ) )
            {
                //DE_DEBUG("Bound tex ", texId, " at unit ", i)
                return int( i );
            }
            else
            {
                DE_ERROR("Not bound unit(", i, ") with texId(",texId,")")
            }
        }
    }

    static int unitCounter = 0;
    unitCounter = (unitCounter + 1) % int(m_TexUnits.size());
    unit = unitCounter;
    if ( !bindTextureId( unit, texId ) )
    {
        DE_ERROR("No force bound unit(", unit, ") with texId(",texId,")")
        return -1;
    }

    return unit;
}

// Benni's HighLevelCalls (class ITexture) to native bindTexture2D

bool VideoDriverGL::bindTexture( int stage, ITexture* tex )
{
    if ( !tex ) { DE_ERROR("No tex") return false; }
    if ( tex->type() == TexType::Tex2D )
    {
        if ( bindTextureId( stage, tex->id() ) )
        {
            //tex->setUnit( stage );
            return true;
        }
    }
    //   else if ( tex->isRenderBuffer() )
    //   {
    //      ::glBindRenderbuffer( GL_RENDERBUFFER, tex->getTextureId() ); GL_VALIDATE
    ////      ::glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h ); GL_VALIDATE
    ////      ::glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
    ////            GL_RENDERBUFFER, m_depthTex ); GL_VALIDATE
    //   }
    return false;
}

// AutoSelect a free tex unit and return its index for use in setSamplerUniform().
int VideoDriverGL::bindTexture( ITexture* tex )
{
    if ( !tex ) { DE_ERROR("No tex") return -1; }
    if ( tex->type() == TexType::Tex2D )
    {
        return bindTextureId( tex->id() );
    }
    else
    {
        // throw std::runtime_error("Invalid texture type");
    }
    return -1;
}

bool VideoDriverGL::unbindTexture( ITexture* tex )
{
    if ( !tex ) { return true; }
    if ( tex->type() == TexType::Tex2D )
    {
        if ( !unbindTextureId( tex->id() ) )
        {
            DE_ERROR("Cant unbind tex ", tex->str())
            return false;
        }
        else
        {
            return true;
        }
    }
    else
    {
        DE_ERROR("Unsupported tex type ", tex->str())
        return false;
    }
}

// int32_t findTex( ITexture* tex ) const;
// int32_t findTex( std::string const & name ) const;
// bool hasTex( std::string const & name ) const;
// TexRef getTex( std::string const & name ) const;
// TexRef load2D( std::string const & name, std::string uri, bool keepImage = true, TexOptions so = TexOptions() );
// TexRef add2D( std::string const & name, Image const & img, bool keepImage = true, TexOptions so = TexOptions() );



bool VideoDriverGL::hasTexture( std::string const & name ) const
{
    auto it = m_lutTexNames.find( name ); // Find 'name' in cache
    if (it == m_lutTexNames.end()) // Cache miss => try load ( every time, yet )
    {
        return false;
    }
    else
    {
        return true;
    }
}

*/
