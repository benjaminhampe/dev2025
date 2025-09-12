#include <learnopengl/loadTexture.h>

// ImageLib headers
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace learnopengl {

uint32_t TextureFromFile(std::string const & uri, std::string const &directory, bool gamma)
{
    std::string filename = directory + '/' + uri;

    uint32_t textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load, uri = " << uri << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}


float getMaxAnisotropicFilter()
{
   float maxAF = 0.0f;
   glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAF);
   return maxAF;
}

uint32_t loadTexture( std::string const & uri, SO const & so, bool gammaCorrection )
{
    uint32_t textureID;
    glGenTextures(1, &textureID);

    int w = 0;
    int h = 0;
    int nrComponents = 0;
    u8* data = stbi_load(uri.c_str(), &w, &h, &nrComponents, 0);
    if (data)
    {
        GLenum format = GL_RGBA;
             if (nrComponents == 1) format = GL_RED;
        else if (nrComponents == 3) format = GL_RGB;
        else if (nrComponents == 4) format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GLint(format), w, h, 0, format, GL_UNSIGNED_BYTE, data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, SO::toGL(so.min) );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, SO::toGL(so.mag) );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, SO::toGL(so.wrapS) );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, SO::toGL(so.wrapT) );
        //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, SO::toGL(so.wrapR) );
        if ( so.hasMipmaps() )
        {
           glGenerateMipmap( GL_TEXTURE_2D );
        }

        // [Anisotropic Filtering] AF | needs GL_EXT_texture_filter_anisotropic ( >= 3.0 ) so its there.
        if ( so.anisotropicFilter > 0.5f )
        {
           GLfloat maxAF = 0.0f;
           glGetFloatv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAF );
           glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAF );
        }

//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load: " << uri << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}


// loads a cubemap texture from 6 individual texture faces
// order:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front) 
// -Z (back)
// -------------------------------------------------------
uint32_t loadCubemap(std::vector<std::string> faces)
{
    uint32_t textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (size_t i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

/*
uint32_t loadTexture( std::string const & uri, SO const & so, bool gammaCorrection )
{
   de::Image m_img;
   if (!dbLoadImage( m_img, uri ))
   {
      std::cout << "Cant load tex image " << uri << std::endl;
      return 0;
   }

   int w = m_img.w();
   int h = m_img.h();
   auto colorFormat = m_img.getFormat();

   // GLenum m_format = 0;
   // if (m_nrComponents == 1)      m_format = GL_RED;
   // else if (m_nrComponents == 3) m_format = GL_RGB;
   // else if (m_nrComponents == 4) m_format = GL_RGBA;

   GLenum m_glInternalFormat = GL_RED;
   GLenum m_glDataFormat = GL_RED;
   GLenum m_glDataType = GL_UNSIGNED_BYTE;

   if (colorFormat == de::ColorFormat::R8)
   {
      //m_format = Texture::TF_R8;
      m_glInternalFormat = GL_RED;
      m_glDataFormat = GL_RED;
      m_glDataType = GL_UNSIGNED_BYTE;
   }
   else if (colorFormat == de::ColorFormat::RGB888 )
   {
      //m_format = Texture::TF_R8G8B8;
      m_glInternalFormat = gammaCorrection ? GL_SRGB : GL_RGB;
      m_glDataFormat = GL_RGB;
      m_glDataType = GL_UNSIGNED_BYTE;
   }
   else if (colorFormat == de::ColorFormat::RGBA8888)
   {
      //m_format = Texture::TF_R8G8B8A8;
      m_glInternalFormat = gammaCorrection ? GL_SRGB_ALPHA : GL_RGBA;
      m_glDataFormat = GL_RGBA;
      m_glDataType = GL_UNSIGNED_BYTE;
   }
   else if (colorFormat == de::ColorFormat::R32F)
   {
      //m_format = Texture::TF_R32F;
      m_glInternalFormat = GL_R32F;
      m_glDataFormat = GL_R32F;
      m_glDataType = GL_FLOAT;
   }
   else if (colorFormat == de::ColorFormat::R16 )
   {
      //m_format = Texture::TF_R16;
      m_glInternalFormat = GL_R16I;
      m_glDataFormat = GL_R16I;
      m_glDataType = GL_SHORT;
   }
   //   else if ( texFormat == TexFormat::D24S8 )
   //   {
   //      tif.internal = GL_DEPTH24_STENCIL8;
   //      tif.format = GL_DEPTH_STENCIL;
   //      tif.type = GL_UNSIGNED_INT_24_8;
   //   }
   else
   {
      std::cout << "Texture.upload() :: unsupported colorFormat(" <<
         de::ColorUtil::getString(colorFormat) << ")" << std::endl;
      return false;
   }

   uint32_t m_id = 0;
   de_glGenTextures(1, &m_id);

   //glActiveTexture(GL_TEXTURE0);
   de_glBindTexture(GL_TEXTURE_2D, m_id);

   de_glTexImage2D(  GL_TEXTURE_2D,       // GLenum target
                  0,                   // GLint level
                  m_glInternalFormat,  // GLenum internalFormat
                  w,                 // GLsizei width
                  h,                 // GLsizei height
                  0,                   // GLint border
                  m_glDataFormat,      // GLenum format
                  m_glDataType,        // GLenum type
                  m_img.data()      ); // void* pixel_data


   if ( so.hasMipmaps() )
   {
      de_glGenerateMipmap(GL_TEXTURE_2D);
   }

   so.apply();


   return m_id;
}
*/

/*
// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

// loads a cubemap texture from 6 individual texture faces
// order:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front) 
// -Z (back)
// -------------------------------------------------------
unsigned int loadCubemap(std::vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

*/

} // end namespace learnopengl.
