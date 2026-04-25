#include "SMeshIO_MTL.hpp"
#include "VideoDriver.hpp"

namespace de {
namespace gpu {

// Only makes sense for .obj model files.
// A .mtl stores multiple materials in one file.
// Each Material will result in one meshbuffer?
// Dont know thats why the loader returns a list of SMaterials
// ordered by first encounter in mtl file and by name.

// A list of Material
//   # 3ds
//   # File Created:
//   newmtl 12993_bla_diffuse
//      Ns 30.0000
//      Ni 1.5000
//      d 1.0000
//      Tr 0.0000
//      Tf 1.0000 1.0000 1.0000
//      illum 2
//      Ka 1.0000 1.0000 1.0000
//	     Kd 1.0000 1.0000 1.0000
//	     Ks 0.3600 0.3600 0.3600
//	     Ke 0.0000 0.0000 0.0000
//	     map_Ka fish.jpg
//	     map_Kd fish.jpg

// static
bool
MTL::save( std::vector< Material > const & liste, std::string uri )
{
   DE_DEBUG("Write ",liste.size()," materials as MTL" )
   std::ostringstream s;

   auto write_int = [&] ( std::string name, int value )
   {
      s << "   " << name << " " << value << "\n";
   };

   auto write_float = [&] ( std::string name, float value )
   {
      std::setprecision( 4 );
      s << "   " << name << " " << value << "\n";
   };

   auto write_float_rgb = [&] ( std::string name, glm::vec3 const & color )
   {
      std::setprecision( 4 );
      s << "   " << name << " "
        << color.r << " "
        << color.g << " "
        << color.b << "\n";
   };

   auto write_tex = [&] ( std::string name, std::string tex )
   {
      if ( tex.empty() ) return;

      std::string uri = tex;
      dbStrLowerCase( uri );

      if ( dbStrEndsWith( tex, "") )
      {

      }
      s << "   " << name << " " << tex << "\n";
   };

   //s << "# exported by libDarkGPU (c) Benjamin Hampe 2021\n"
   for ( size_t i = 0; i < liste.size(); ++i )
   {
      Material const & mat = liste[ i ];
      s << "newmtl " << mat.Name << "\n";
      write_float( "Ns", mat.Shininess ); // shininess / 0.128f;
      write_float( "Ni", mat.Shininess_strength );
      write_float( "d", mat.Opacity );    // float ?
      write_float( "Tr", mat.Reflectivity );   // float ?
      write_float_rgb( "Tf", glm::vec3(1,1,1) );   // vec3 ?
      write_int( "illum", mat.Lighting );

      write_float_rgb( "Ka", mat.Ka );
      write_float_rgb( "Kd", mat.Kd );
      write_float_rgb( "Ks", mat.Ks );
      write_float_rgb( "Ke", mat.Ke );
      write_float_rgb( "Kr", mat.Kr );
      write_float_rgb( "Kt", mat.Kt );

      if ( mat.getDiffuseMap().m_tex )
      {
         write_tex( "map_Kd", mat.getDiffuseMap().m_tex->getName() );
      }
      if ( mat.getBumpMap().m_tex )
      {
         write_tex( "map_bump", mat.getBumpMap().m_tex->getName() );
      }
      if ( mat.getNormalMap().m_tex )
      {
         write_tex( "map_Kn", mat.getNormalMap().m_tex->getName() );
      }
      if ( mat.getSpecularMap().m_tex )
      {
         write_tex( "map_Ks", mat.getSpecularMap().m_tex->getName() );
      }
      s << "\n";
   }

   return dbSaveText( uri, s.str() );
}

// static
bool
MTL::load( std::vector< Material > & liste, std::string uri )
{
   return true;
}

} // end namespace
} // end namespace
