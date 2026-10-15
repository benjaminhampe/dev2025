#include <de/gpu/Tex2DArray.h>
#include <de_opengl.h>

// #include <vector>
// #include <string>
// #include <stdexcept>
// #include <algorithm>
// #include <cmath>

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


Tex2DArray::Tex2DArray()
{

}

Tex2DArray::~Tex2DArray()
{
    if (id != 0)
    {
        DE_ERROR("Forgot to call destroy(), ")
        destroy();
    }
}

// Verschiebe-Semantik (Move)
Tex2DArray::Tex2DArray(Tex2DArray&& other) noexcept
    : id(other.id)
    , w(other.w)
    , h(other.h)
    , layers(other.layers)
    , mipLevels(other.mipLevels)
    , gpuFormat(other.gpuFormat)
{
    other.id = 0;
}

Tex2DArray&
Tex2DArray::operator=(Tex2DArray&& other) noexcept
{
    if (this != &other)
    {
        destroy();
        id = other.id;
        w = other.w;
        h = other.h;
        layers = other.layers;
        mipLevels = other.mipLevels;
        gpuFormat = other.gpuFormat;
        other.id = 0;
    }
    return *this;
}

// Tex2DArray::Tex2DArray(const Tex2DArray&) = delete;
// Tex2DArray::Tex2DArray& operator=(const Tex2DArray&) = delete;

void Tex2DArray::destroy()
{
    if (id != 0)
    {
        glDeleteTextures(1, &id);
        id = 0;
    }
}

std::string Tex2DArray::str() const
{
    std::ostringstream o; o <<
    "id(" << id << "), "
    "w(" << w << "), "
    "h(" << h << "), "
    "layers(" << layers << "), "
    "mipLevels(" << mipLevels << "), "
    "gpuFormat(" << TextureFormat_str(gpuFormat) << ")";
    return o.str();
}

/**
 * Erstellt das Textur-Array. Berechnet automatisch die maximal möglichen Mipmap-Stufen.
 */
//static
Tex2DArray
Tex2DArray::create(const std::string& name, const std::vector<const Image*>& images, const SamplerOptions& so)
{
    if (images.empty())
    {
        DE_ERROR("Empty image vector, ",name)
        return {};
    }

    DE_ERROR("Image vector, ",images.size())

    // Bilder in Level 0 hochladen
    int32_t w = images[0]->w();
    int32_t h = images[0]->h();
    auto fmt = images[0]->pixelFormat();

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

        if (w2 != w) { DE_ERROR("Img[",i,"] w2(",w2,") != w(",w,")") return {}; }
        if (h2 != h) { DE_ERROR("Img[",i,"] h2(",h2,") != h(",h,")") return {}; }
        if (fmt2 != fmt) { DE_ERROR("Img[",i,"] fmt2(",fmt2.str(),") != fmt(",fmt.str(),")") return {}; }
    }

    Tex2DArray tex;
    tex.w = w;   // Setzt die Ziel-Breite fest
    tex.h = h;  // Setzt die Ziel-Höhe fest
    tex.layers = static_cast<GLsizei>(images.size());
    tex.mipLevels = static_cast<GLsizei>(std::floor(std::log2(std::max(tex.w, tex.h)))) + 1;
    tex.gpuFormat = (fmt == PixelFormat::R8G8B8) ? GL_RGB8 : GL_RGBA8; // internalFormat.

    glGenTextures(1, &tex.id);
    glBindTexture(GL_TEXTURE_2D_ARRAY, tex.id);
    glTexStorage3D(GL_TEXTURE_2D_ARRAY, tex.mipLevels, tex.gpuFormat, tex.w, tex.h, tex.layers);

    // 1. Calculate the CPU-side data format based on your PixelFormat enum
    GLenum cpuFormat = (fmt == PixelFormat::R8G8B8) ? GL_RGB : GL_RGBA;

    // Bilder in Level 0 hochladen
    for (GLsizei i = 0; i < tex.layers; ++i)
    {
        glTexSubImage3D(
            GL_TEXTURE_2D_ARRAY,//GLenum target
            0,                  // GLint level
            0,                  // GLint xoffset
            0,                  // GLint yoffset
            i,                  // GLint zoffset
            w,                  // GLsizei width
            h,                  // GLsizei height
            1,                  // GLsizei depth
            cpuFormat,          // GLenum format
            GL_UNSIGNED_BYTE,   // GLenum type
            images[i]->data()   // const void *pixels
        );
    }

    // Mipmaps nur generieren, wenn der Sampler-Filter sie anfordert
    if (so.hasMipmaps()) { glGenerateMipmap(GL_TEXTURE_2D_ARRAY); }

    applySamplerOptions(so);

    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

    DE_DEBUG("Created Tex2DArray ",tex.str())
    return tex;
}

#if 0
/**
 * Diese Funktion wird kurz vor dem Rendern aufgerufen.
 * Sie setzt die Filteroptionen und generiert die Mipmaps NUR DANN, wenn sie gebraucht werden.
 */
void Tex2DArray::applySamplerOptions(const SamplerOptions& so, GLuint slot)
{
    // Textur aktivieren und binden
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D_ARRAY, id);

    // Mipmaps nur generieren, wenn der Sampler-Filter sie anfordert
    if (so.hasMipmaps())
    {
        glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
    }

    // Sampler-Parameter an die GPU übergeben
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, so.minFilter);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, options.magFilter);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, options.wrapS);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, options.wrapT);
}
#endif

} // end namespace gpu.
} // end namespace de.
