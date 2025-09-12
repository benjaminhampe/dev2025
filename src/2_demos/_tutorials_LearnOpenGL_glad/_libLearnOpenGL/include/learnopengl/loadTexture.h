#pragma once
#include <cstdint>
#include <learnopengl/so.h>

namespace learnopengl {

uint32_t loadCubemap(std::vector<std::string> faces);

uint32_t loadTexture( std::string const & uri, SO const & so = SO(), bool gammaCorrection = false );

uint32_t TextureFromFile( std::string const& uri, std::string const &directory, bool gamma = false);


//float getMaxAnisotropicFilter();


/*
// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const * path, bool gammaCorrection)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum internalFormat;
        GLenum dataFormat;
        if (nrComponents == 1)
        {
            internalFormat = dataFormat = GL_RED;
        }
        else if (nrComponents == 3)
        {
            internalFormat = gammaCorrection ? GL_SRGB : GL_RGB;
            dataFormat = GL_RGB;
        }
        else if (nrComponents == 4)
        {
            internalFormat = gammaCorrection ? GL_SRGB_ALPHA : GL_RGBA;
            dataFormat = GL_RGBA;
        }

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
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

struct TexFormat
{
   enum EFormat : uint8_t
   {
      R8 = 0,
      R8G8B8,
      R8G8B8A8,
      R16,
      R32F,
      D24S8,
      D32F,
      D32FS8,
      INVALID
   };

   static std::string getString( EFormat const texFormat )
   {
      switch ( texFormat )
      {
         case R8 : return "R8";
         case R8G8B8 : return "R8G8B8";
         case R8G8B8A8 : return "R8G8B8A8";
         case R16 : return "R16";
         case R32F : return "R32F";
         case D24S8 : return "D24S8";
         case D32F : return "D32F";
         case D32FS8 : return "D32FS8";
         default: return "INVALID";
      }
   }

   EFormat m_format;
   TexFormat() : m_format(INVALID) {}
   TexFormat( EFormat format ) : m_format(format) {}
   TexFormat( TexFormat const & other ) : m_format(other.m_format) {}
   operator EFormat() const { return m_format; }
};

struct TexSemantic
{
   enum ESemantic : uint8_t
   {
      DiffuseMap,
      NormalMap,
      BumpMap,
      CombiNormalBumpMap,
      DetailMap,
      SpecularMap,
      INVALID
   };

   static std::string getString( ESemantic const texSemantic )
   {
      switch ( texSemantic )
      {
         case DiffuseMap : return "DiffuseMap";
         case NormalMap : return "NormalMap";
         case BumpMap : return "BumpMap";
         case CombiNormalBumpMap : return "CombiNormalBumpMap";
         case DetailMap : return "DetailMap";
         case SpecularMap : return "SpecularMap";
         default: return "INVALID";
      }
   }
   ESemantic m_semantic;
   TexSemantic() : m_semantic(INVALID) {}
   TexSemantic( ESemantic semantic ) : m_semantic(semantic) {}
   TexSemantic( TexSemantic const & other ) : m_semantic(other.m_semantic) {}
   operator ESemantic() const { return m_semantic; }
};

*/

} // end namespace learnopengl.
