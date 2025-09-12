#pragma once
#include <de/gpu/GPU.h>

namespace de {
namespace gpu {

class VideoDriver;
class Shader;

//==========================================================
struct SkyboxRenderer
//==========================================================
{
    SkyboxRenderer();
    ~SkyboxRenderer();

    void init(VideoDriver* driver);

    void load( const Image* nx, const Image* px,
               const Image* ny, const Image* py,
               const Image* nz, const Image* pz);

    void load(
        std::string const & mediaDir = "./",
        std::string const & px = "",
        std::string const & nx = "",
        std::string const & py = "",
        std::string const & ny = "",
        std::string const & pz = "",
        std::string const & nz = "" );

    void loadGalaxy()
    {
        load("media/H3/gfx/skybox/galaxy/",
             "px.png", "nx.png",
             "py.png", "ny.png",
             "pz.png", "nz.png");
    }

    void loadMiramar()
    {
        load("media/H3/gfx/skybox/Miramar/",
             "px.png", "nx.png",
             "py.png", "ny.png",
             "pz.png", "nz.png");
    }

    void loadPowerlines()
    {
        load("media/H3/gfx/skybox/Powerlines/",
             "px.jpg", "nx.jpg",
             "py.jpg", "ny.jpg",
             "pz.jpg", "nz.jpg");
    }

    void initShader();
    void initGeometry();

    void destroy();

    void render();

    //   GL_TEXTURE_CUBE_MAP_POSITIVE_X Right
    //   GL_TEXTURE_CUBE_MAP_NEGATIVE_X Left
    //   GL_TEXTURE_CUBE_MAP_POSITIVE_Y Top
    //   GL_TEXTURE_CUBE_MAP_NEGATIVE_Y Bottom
    //   GL_TEXTURE_CUBE_MAP_POSITIVE_Z Back
    //   GL_TEXTURE_CUBE_MAP_NEGATIVE_Z Front
    static uint32_t
    loadCubemap( std::vector<std::string> const & faces );

    static uint32_t
    loadCubemap( std::array<const Image*,6> const & images );
   /*
   static void
   debugSkybox( std::vector<std::string> const & faces )
   {
      Image px; dbLoadImage(px,faces[0]); // right
      Image nx; dbLoadImage(nx,faces[1]); // left
      Image py; dbLoadImage(py,faces[2]); // top
      Image ny; dbLoadImage(ny,faces[3]); // bottom
      Image nz; dbLoadImage(nz,faces[4]); // back
      Image pz; dbLoadImage(pz,faces[5]); // front

      int w = px.getWidth();
      int h = px.getHeight();
      Image big( 4*w, 3*h );
      ImagePainter::drawImage( big, nx, 0*w,h );
      ImagePainter::drawImage( big, pz, 1*w,h ); // swapped by gl, because z goes other way
      ImagePainter::drawImage( big, px, 2*w,h );
      ImagePainter::drawImage( big, nz, 3*w,h ); // swapped by gl, because z goes other way
      ImagePainter::drawImage( big, py, w,0 );
      ImagePainter::drawImage( big, ny, w,2*h );

      Font5x8 font(4,4,1,1,1,1);
      int cx = w/2;
      int cy = h/2;
      uint32_t color = 0xFF0000FF; // green
      Align align = Align::Centered; // align center
      ImagePainter::drawText5x8( big, cx+0*w,cy+1*h, "NX", color, font, align );
      ImagePainter::drawText5x8( big, cx+1*w,cy+1*h, "PZ", color, font, align );
      ImagePainter::drawText5x8( big, cx+2*w,cy+1*h, "PX", color, font, align );
      ImagePainter::drawText5x8( big, cx+3*w,cy+1*h, "NZ", color, font, align );
      ImagePainter::drawText5x8( big, cx+1*w,cy+0*h, "PY", color, font, align );
      ImagePainter::drawText5x8( big, cx+1*w,cy+2*h, "NY", color, font, align );

      dbSaveImage( big, "debugSkybox.png" );
      //return big
   }
   */

   VideoDriver* m_driver;
   Shader* m_shader;
   int m_locMVP;
   int m_locTex;

   uint32_t m_vao;
   uint32_t m_vbo;
   uint32_t m_cubeMap;
   std::vector< glm::vec3 > m_vertices;
   //std::vector<std::string> m_textures;
   // TexOptions m_so;

};

} // end namespace gpu.
} // end namespace de.
