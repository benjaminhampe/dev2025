#include <de_gpu/de_renderer_Skybox.h>
#include <de_gpu/de_GL_debug_layer.h>
#include <de_image/de_ImageUtils.h>
#include <de_image_formats/DarkImage.h>

namespace de {

// static
uint32_t
SkyboxRenderer::loadCubemap( std::vector<std::string> const & faces )
{
   uint32_t textureID = 0;
   glGenTextures(1, &textureID);
   glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

   de::Image img;
   for (size_t i = 0; i < faces.size(); i++)
   {
      if (dbLoadImage(img,faces[i]))
      {
         de::removeAlphaChannel( img );
         int w = img.getWidth();
         int h = img.getHeight();
         glTexImage2D( GLenum(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i),
                       0,
                       GL_RGB,
                       w,
                       h,
                       0,
                       GL_RGB,
                       GL_UNSIGNED_BYTE, img.data() );
      }
      else
      {
         std::cout << "Cubemap["<<i<<"] failed to load, " << faces[i] <<
         std::endl;
      }
   }
   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
   glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
   return textureID;
}


SkyboxRenderer::SkyboxRenderer()
   : m_driver( nullptr )
   , m_shader( nullptr )
   //, m_so( 0,  TexOptions::Minify::Linear,
   //            TexOptions::Magnify::Linear,
   //            TexOptions::Wrap::ClampToEdge,
   //            TexOptions::Wrap::ClampToEdge )
   , m_vao(0)
   , m_vbo(0)
   , m_cubeMap(0)
{}

SkyboxRenderer::~SkyboxRenderer()
{
   if ( m_cubeMap )
   {
      std::cout << "Forgot to call SkyboxRenderer.destroy()" << std::endl;
   }
}

void
SkyboxRenderer::destroy()
{
   if ( m_vao )
   {
      glDeleteVertexArrays(1, &m_vao);
      m_vao = 0;
   }
   if ( m_vbo )
   {
      glDeleteBuffers(1, &m_vbo);
      m_vbo = 0;
   }
   if ( m_cubeMap )
   {
      glDeleteTextures(1, &m_cubeMap);
      m_cubeMap = 0;
   }
}

void
SkyboxRenderer::init( VideoDriver* driver,
                      std::string const & px, std::string const & nx,
                      std::string const & py, std::string const & ny,
                      std::string const & pz, std::string const & nz )
{
   m_driver = driver;

   if ( px.empty() )
   {
      std::string dir = "../../media/skybox/Miramar/";
      m_textures = std::vector<std::string>{
         dir + "px.png", //"right.jpg",
         dir + "nx.png", //"left.jpg",
         dir + "py.png", //"top.jpg",
         dir + "ny.png", //"bottom.jpg",
         dir + "pz.png", //"front.jpg",
         dir + "nz.png"  //"back.jpg"
      };
   }
   else
   {
      m_textures = std::vector<std::string>{ px,nx,py,ny,pz,nz } ;
   }

   if (!m_cubeMap)
   {
      m_cubeMap = loadCubemap( m_textures );
   }

   // back face
   if ( !m_vao )
   {
      auto addQuad = [&]( glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d )
      {
         m_vertices.emplace_back( a );
         m_vertices.emplace_back( b );
         m_vertices.emplace_back( c );
         m_vertices.emplace_back( a );
         m_vertices.emplace_back( c );
         m_vertices.emplace_back( d );
      };

      // nx = left face
      addQuad( glm::vec3(-1.0f, -1.0f, -1.0f),  // top-right
               glm::vec3(-1.0f,  1.0f, -1.0f),  // top-left
               glm::vec3(-1.0f,  1.0f,  1.0f),  // bottom-left
               glm::vec3(-1.0f, -1.0f,  1.0f)); // bottom-right

      // nz = back face
      addQuad( glm::vec3( 1.0f, -1.0f, -1.0f),  // bottom-left
               glm::vec3( 1.0f,  1.0f, -1.0f),  // top-right
               glm::vec3(-1.0f,  1.0f, -1.0f),  // bottom-right
               glm::vec3(-1.0f, -1.0f, -1.0f)); // top-left

      // pz = front face
      addQuad( glm::vec3(-1.0f, -1.0f,  1.0f),  // bottom-left
               glm::vec3(-1.0f,  1.0f,  1.0f),  // bottom-right
               glm::vec3( 1.0f,  1.0f,  1.0f),  // top-right
               glm::vec3( 1.0f, -1.0f,  1.0f)); // top-left

      // ny = bottom face
      addQuad( glm::vec3(-1.0f, -1.0f, -1.0f),  // top-right
               glm::vec3( 1.0f, -1.0f, -1.0f),  // top-left
               glm::vec3( 1.0f, -1.0f,  1.0f),  // bottom-left
               glm::vec3(-1.0f, -1.0f,  1.0f)); // bottom-right

      // py = top face
      addQuad( glm::vec3(-1.0f,  1.0f, -1.0f),  // top-left
               glm::vec3( 1.0f,  1.0f, -1.0f),  // bottom-right
               glm::vec3( 1.0f,  1.0f,  1.0f),  // top-right
               glm::vec3(-1.0f,  1.0f,  1.0f)); // bottom-left

      // px = right face
      addQuad( glm::vec3(1.0f, -1.0f,  1.0f),   // top-left
               glm::vec3(1.0f,  1.0f,  1.0f),   // bottom-right
               glm::vec3(1.0f,  1.0f, -1.0f),   // top-right
               glm::vec3(1.0f, -1.0f, -1.0f));  // bottom-left

      glGenBuffers(1, &m_vbo);
      glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
      glBufferData(GL_ARRAY_BUFFER,
                   GLint(m_vertices.size() * sizeof(glm::vec3)),
                   m_vertices.data(),
                   GL_STATIC_DRAW);
      glBindBuffer(GL_ARRAY_BUFFER, 0);

      glGenVertexArrays(1, &m_vao);
      glBindVertexArray(m_vao);
      glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                               sizeof(glm::vec3), reinterpret_cast< void* >(0));

      glBindVertexArray(0);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
   }
}

void
SkyboxRenderer::render( glm::mat4 const & viewProjMat )
{
   std::string const & shaderName = "SkyboxCubeMap";

   if ( !m_shader )
   {
      m_shader = m_driver->getShader( shaderName );
   }

   if ( !m_shader )
   {
      std::string const vs = R"(
      layout (location = 0) in vec3 a_pos;

      out vec3 v_texCoords;

      uniform mat4 u_mvp;

      void main()
      {
         v_texCoords = a_pos; // must be in range [-1,1].
         gl_Position = u_mvp * vec4(a_pos, 1.0);
      })";

      std::string const fs = R"(

         in vec3 v_texCoords; // direction vector

         uniform samplerCube u_cubeMap; // cubemap texture

         out vec4 color;

         void main()
         {
            color = texture(u_cubeMap, v_texCoords);
         })";

      m_shader = m_driver->createShader( shaderName, vs, fs );
   }

   if ( !m_shader )
   {
      return;
   }

   m_driver->useShader( m_shader );

   //glDepthMask(GL_FALSE);
   glEnable(GL_DEPTH_TEST);
   glDisable(GL_CULL_FACE);

   float size = 3800.0f;
   glm::mat4 modelMat = glm::mat4(1.0f);
   modelMat = glm::scale(modelMat, glm::vec3(size,size,size));
   m_shader->setMat4f("u_mvp", viewProjMat * modelMat);

   glBindVertexArray(m_vao);
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubeMap);

   m_shader->setVec1i("u_cubeMap", 0);

   glDrawArrays(GL_TRIANGLES, 0, 36);
   glBindVertexArray(0);
   glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
   //glDepthMask(GL_TRUE);
}

} // end namespace GL.
