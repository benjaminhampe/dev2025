#include "SMeshIO_OBJ.hpp"
#include "VideoDriver.hpp"
namespace de {
namespace gpu {

// static
std::string
SMeshOBJ::toString( glm::vec3 const & pos )
{
   std::ostringstream s;
   s << -pos.x << " " << pos.y << " " << pos.z << "\n";
   return s.str();
}

// static
std::string
SMeshOBJ::toString( glm::vec2 const & pos )
{
   std::ostringstream s;
   s << pos.x << " " << -pos.y << "\n";
   return s.str();
}

// static
std::string
SMeshOBJ::toString( uint32_t color, std::string prefix )
{
   std::ostringstream s; s << prefix << " "
   << float( RGBA_R(color) )/255.f << " "
   << float( RGBA_G(color) )/255.f << " "
   << float( RGBA_B(color) )/255.f << "\n";
   return s.str();
}

// static
bool
SMeshOBJ::save( SMeshBuffer const & buf, std::string const & objUri, VideoDriver* driver )
{
   std::string mtlUri = dbGetFileBaseName( objUri ) + ".mtl";
   DE_DEBUG("Write SMeshBuffer to MTL = ", mtlUri )
   DE_DEBUG("Write SMeshBuffer to OBJ = ", objUri )

   std::vector< Material > matList;
   matList.reserve( 1 );
   matList.emplace_back();
   Material & mat = matList.back();
   mat = buf.getMaterial();

   if ( mat.Name.empty() )
   {
      mat.Name = "Material";
   }

   std::string bufName = buf.Name;
   if ( bufName.empty() )
   {
      bufName = "SMeshBuffer";
   }

   MTL::save( matList, mtlUri );

   std::ostringstream s;
   s <<
   "# exported by libDarkGPU (c) 2021 Benjamin Hampe <benjaminhampe@gmx.de>\n"
   "mtllib " << mtlUri << "\n\n";

   //   o Cube
   //   v 1.000000 -1.000000 -1.000000
   //   v ...
   //   vt 0.000000 1.000000
   //   vt ...
   //   vn 0.000000 0.000000 -1.000000
   //   vn ...
   //   g Cube_Cube_Material
   //   usemtl Material
   //   s off
   //   f 2//1 3//1 4//1
   //   f ...

   s << "# " << buf.getPrimitiveTypeStr() << "\n";
   s << "o " << bufName << "\n";

   // Vertices
   for ( uint32_t i = 0; i < buf.getVertexCount(); ++i )
   {
      auto const & pos = buf.getVertex( i ).getPos();
      s << "v " << pos.x << " " << pos.y << " " << pos.z << "\n";
   }

   // TexCoords
   for ( uint32_t i = 0; i < buf.getVertexCount(); ++i )
   {
      auto const & tex = buf.getVertex( i ).getTexCoord();
      s << "vt " << tex.x << " " << tex.y << "\n";
   }

   // Normals
   for ( uint32_t i = 0; i < buf.getVertexCount(); ++i )
   {
      auto const & nrm = buf.getVertex( i ).getNormal();
      s << "vn " << nrm.x << " " << nrm.y << " " << nrm.z << "\n";
   }

   s << "g " << bufName << "\n"; // 12993_Long_Fin_White__Cloud
   s << "usemtl " << mat.Name << "\n";
   s << "s off\n";

   // Faces ( Indices )
   if ( buf.getPrimitiveType() == PrimitiveType::Points )
   {
      if ( buf.getIndexCount() >= buf.getVertexCount() )
      {
         for ( uint32_t i = 0; i < buf.getIndexCount(); ++i )
         {
            auto A = buf.getIndex( i ) + 1;
            s << "f " << A << "/" << A << "/" << A << "\n";
         }
      }
      else
      {
         for ( uint32_t i = 0; i < buf.getVertexCount(); ++i )
         {
            auto A = i + 1;
            s << "f " << A << "/" << A << "/" << A << "\n";
         }
      }

   }
   else if ( buf.getPrimitiveType() == PrimitiveType::Lines )
   {
      if ( buf.getIndexCount() >= buf.getVertexCount() )
      {
         for ( uint32_t i = 0; i < buf.getIndexCount(); i += 2 )
         {
            auto A = 1 + buf.getIndex( i );
            auto B = 1 + buf.getIndex( i+1 );
            s << "f "
              << A << "/" << A << "/" << A << " "
              << B << "/" << B << "/" << B << "\n";
         }
      }
      else
      {
         for ( uint32_t i = 0; i < buf.getVertexCount(); i += 2 )
         {
            uint32_t A = 1 + i, B = i+2;
            s << "f "
              << A << "/" << A << "/" << A << " "
              << B << "/" << B << "/" << B << "\n";
         }
      }
   }
   else if ( buf.getPrimitiveType() == PrimitiveType::Triangles )
   {
      if ( buf.getIndexCount() >= buf.getVertexCount() )
      {
         for ( uint32_t i = 0; i < buf.getIndexCount()/3; ++i)
         {
            auto A = 1 + buf.getIndex( 3*i );
            auto B = 1 + buf.getIndex( 3*i+1 );
            auto C = 1 + buf.getIndex( 3*i+2 );
            s << "f "
              << A << "/" << A << "/" << A << " "
              << B << "/" << B << "/" << B << " "
              << C << "/" << C << "/" << C << "\n";
         }
      }
      else
      {
         for ( uint32_t i = 0; i < buf.getVertexCount()/3; ++i )
         {
            uint32_t A = 3*i+1, B = A+1, C = A+2;
            s << "f "
              << A << "/" << A << "/" << A << " "
              << B << "/" << B << "/" << B << " "
              << C << "/" << C << "/" << C << "\n";
         }
      }
   }
   else if ( buf.getPrimitiveType() == PrimitiveType::TriangleStrip )
   {
      if ( buf.getIndexCount() >= buf.getVertexCount() )
      {
         for ( uint32_t i = 2; i < buf.getIndexCount(); ++i )
         {
            auto A = 1 + buf.getIndex( i-2 );
            auto B = 1 + buf.getIndex( i-1 );
            auto C = 1 + buf.getIndex( i );
            s << "f "
              << A << "/" << A << "/" << A << " "
              << B << "/" << B << "/" << B << " "
              << C << "/" << C << "/" << C << "\n";
         }
      }
      else
      {
         for ( uint32_t i = 2; i < buf.getVertexCount(); ++i )
         {
            uint32_t A = i-1, B = i, C = i+1;
            s << "f "
              << A << "/" << A << "/" << A << " "
              << B << "/" << B << "/" << B << " "
              << C << "/" << C << "/" << C << "\n";
         }
      }
   }
   else // if ( buf.getPrimitiveType() == PrimitiveType::TriangleStrip )
   {
      throw std::runtime_error("Unsupported primType");
//         if ( buf.getIndexCount() >= buf.getVertexCount() )
//         {
//            for ( uint32_t i = 0; i < buf.getIndexCount(); i += 4 )
//            {
//               auto A = buf.getIndex( i );
//               auto B = buf.getIndex( i+1 );
//               auto C = buf.getIndex( i+2 );
//               auto D = buf.getIndex( i+3 );
//               s << "f "
//                 << A << "/" << A << "/" << A << " "
//                 << B << "/" << B << "/" << B << " "
//                 << C << "/" << C << "/" << C << " "
//                 << D << "/" << D << "/" << D << "\n";
//            }
//         }
//         else
//         {
//            for ( uint32_t i = 0; i < buf.getVertexCount(); i += 4 )
//            {
//               uint32_t A = i, B = i+1, C = i+2, D = i+3;
//               s << "f "
//                 << A << "/" << A << "/" << A << " "
//                 << B << "/" << B << "/" << B << " "
//                 << C << "/" << C << "/" << C << " "
//                 << D << "/" << D << "/" << D << "\n";
//            }
//         }
   }
   s << "\n";

   if ( dbSaveText( objUri, s.str() ) )
   {
      DE_DEBUG("Saved mesh (",bufName,") to OBJ file to disk, uri = ", objUri )
      return true;
   }
   else
   {
      DE_DEBUG("Cant save OBJ mesh file to disk, uri = ", objUri )
      return false;
   }

}

// static
bool
SMeshOBJ::save( SMesh const & mesh, std::string const & objUri, VideoDriver* driver )
{
   std::string mtlUri = dbGetFileBaseName( objUri ) + ".mtl";
   DE_DEBUG("Writing MTL = ", mtlUri )
   DE_DEBUG("Writing OBJ = ", objUri )

   std::vector< Material > matList;
   matList.reserve( mesh.getMeshBufferCount() );

   for ( int i = 0; i < mesh.getMeshBufferCount(); ++i )
   {
      matList.emplace_back();
      Material & mat = matList.back();
      mat = mesh.getMeshBuffer( i ).getMaterial();

      if ( mat.Name.empty() )
      {
         mat.Name = "Material";
         mat.Name += std::to_string( i );
      }


   }

   MTL::save( matList, mtlUri );

   std::ostringstream s;
   s <<
   "# exported by libDarkGPU (c) 2021 Benjamin Hampe <benjaminhampe@gmx.de>\n"
   "mtllib " << mtlUri << "\n\n";

   //   o Cube
   //   v 1.000000 -1.000000 -1.000000
   //   v ...
   //   vt 0.000000 1.000000
   //   vt ...
   //   vn 0.000000 0.000000 -1.000000
   //   vn ...
   //   g Cube_Cube_Material
   //   usemtl Material
   //   s off
   //   f 2//1 3//1 4//1
   //   f ...

   for ( int b = 0; b < mesh.getMeshBufferCount(); ++b )
   {
      SMeshBuffer const & src = mesh.getMeshBuffer( b );

      std::string bufName = src.Name;
      if ( bufName.empty() )
      {
         bufName = "SMeshBuffer";
         bufName += std::to_string( b );
      }

      std::string matName = src.getMaterial().Name;
      if ( matName.empty() )
      {
         matName = "Material";
         matName += std::to_string( b );
      }

      s << "# " << getPrimitiveTypeString( src.PrimType ) << "\n";
      s << "o " << bufName << "\n";

      // Vertices
      for ( uint32_t i = 0; i < src.getVertexCount(); ++i )
      {
         auto const & pos = src.getVertex( i ).getPos();
         s << "v " << pos.x << " " << pos.y << " " << pos.z << "\n";
      }

      // TexCoords
      for ( uint32_t i = 0; i < src.getVertexCount(); ++i )
      {
         auto const & tex = src.getVertex( i ).getTexCoord();
         s << "vt " << tex.x << " " << tex.y << "\n";
      }

      // Normals
      for ( uint32_t i = 0; i < src.getVertexCount(); ++i )
      {
         auto const & nrm = src.getVertex( i ).getNormal();
         s << "vn " << nrm.x << " " << nrm.y << " " << nrm.z << "\n";
      }

      s << "g " << bufName << "\n"; // 12993_Long_Fin_White__Cloud
      s << "usemtl " << matName << "\n";
      s << "s off\n";

      // Faces ( Indices )
      if ( src.getPrimitiveType() == PrimitiveType::Points )
      {
         if ( src.getIndexCount() >= src.getVertexCount() )
         {
            for ( uint32_t i = 0; i < src.getIndexCount(); ++i )
            {
               auto A = 1+src.getIndex( i );
               s << "f " << A << "/" << A << "/" << A << "\n";
            }
         }
         else
         {
            for ( uint32_t i = 0; i < src.getVertexCount(); ++i )
            {
               auto A = 1+i;
               s << "f " << A << "/" << A << "/" << A << "\n";
            }
         }

      }
      else if ( src.getPrimitiveType() == PrimitiveType::Lines )
      {
         if ( src.getIndexCount() >= src.getVertexCount() )
         {
            for ( uint32_t i = 0; i < src.getIndexCount(); i += 2 )
            {
               auto A = 1 + src.getIndex( i );
               auto B = 1 + src.getIndex( i+1 );
               s << "f "
                 << A << "/" << A << "/" << A << " "
                 << B << "/" << B << "/" << B << "\n";
            }
         }
         else
         {
            for ( uint32_t i = 0; i < src.getVertexCount(); i += 2 )
            {
               uint32_t A = i+1, B = i+2;
               s << "f "
                 << A << "/" << A << "/" << A << " "
                 << B << "/" << B << "/" << B << "\n";
            }
         }
      }
      else if ( src.getPrimitiveType() == PrimitiveType::Triangles )
      {
         if ( src.getIndexCount() >= src.getVertexCount() )
         {
            for ( uint32_t i = 0; i < src.getIndexCount(); i += 3 )
            {
               auto A = 1 + src.getIndex( i );
               auto B = 1 + src.getIndex( i+1 );
               auto C = 1 + src.getIndex( i+2 );
               s << "f "
                 << A << "/" << A << "/" << A << " "
                 << B << "/" << B << "/" << B << " "
                 << C << "/" << C << "/" << C << "\n";
            }
         }
         else
         {
            for ( uint32_t i = 0; i < src.getVertexCount(); i += 3 )
            {
               uint32_t A = i+1, B = i+2, C = i+3;
               s << "f "
                 << A << "/" << A << "/" << A << " "
                 << B << "/" << B << "/" << B << " "
                 << C << "/" << C << "/" << C << "\n";
            }
         }
      }
      else if ( src.getPrimitiveType() == PrimitiveType::TriangleStrip )
      {
         if ( src.getIndexCount() >= src.getVertexCount() )
         {
            for ( uint32_t i = 2; i < src.getIndexCount(); ++i )
            {
               auto A = 1 + src.getIndex( i-2 );
               auto B = 1 + src.getIndex( i-1 );
               auto C = 1 + src.getIndex( i );
               s << "f "
                 << A << "/" << A << "/" << A << " "
                 << B << "/" << B << "/" << B << " "
                 << C << "/" << C << "/" << C << "\n";
            }
         }
         else
         {
            for ( uint32_t i = 2; i < src.getVertexCount(); ++i )
            {
               uint32_t A = i-1, B = i, C = i+1;
               s << "f "
                 << A << "/" << A << "/" << A << " "
                 << B << "/" << B << "/" << B << " "
                 << C << "/" << C << "/" << C << "\n";
            }
         }
      }
      else // if ( buf.getPrimitiveType() == PrimitiveType::Quads )
      {
         throw std::runtime_error("Unsupported primType");

//            if ( src.getIndexCount() >= src.getVertexCount() )
//            {
//               for ( uint32_t i = 0; i < src.getIndexCount(); i += 4 )
//               {
//                  auto A = src.getIndex( i );
//                  auto B = src.getIndex( i+1 );
//                  auto C = src.getIndex( i+2 );
//                  auto D = src.getIndex( i+3 );
//                  s << "f "
//                    << A << "/" << A << "/" << A << " "
//                    << B << "/" << B << "/" << B << " "
//                    << C << "/" << C << "/" << C << " "
//                    << D << "/" << D << "/" << D << "\n";
//               }
//            }
//            else
//            {
//               for ( uint32_t i = 0; i < src.getVertexCount(); i += 4 )
//               {
//                  uint32_t A = i, B = i+1, C = i+2, D = i+3;
//                  s << "f "
//                    << A << "/" << A << "/" << A << " "
//                    << B << "/" << B << "/" << B << " "
//                    << C << "/" << C << "/" << C << " "
//                    << D << "/" << D << "/" << D << "\n";
//               }
//            }
      }
      s << "\n";
   }

   if ( dbSaveText( objUri, s.str() ) )
   {
      DE_DEBUG("Saved mesh (",mesh.Name,") to OBJ file to disk, uri = ", objUri )
      return true;
   }
   else
   {
      DE_DEBUG("Cant save OBJ mesh file to disk, uri = ", objUri )
      return false;
   }

}


} // end namespace
} // end namespace


/*
struct SMeshBufferOBJ
{
   DE_CREATE_LOGGER("de.gpu.SMeshBufferReaderOBJ")
   //EType getType() const {	return EMWT_OBJ; }

   static std::string
   toString( glm::vec3 const & pos )
   {
      std::ostringstream s;
      s << -pos.x << " " << pos.y << " " << pos.z << "\n";
      return s.str();
   }

   static std::string
   toString( glm::vec2 const & pos )
   {
      std::ostringstream s;
      s << pos.x << " " << -pos.y << "\n";
      return s.str();
   }

   static std::string
   toString( uint32_t color, std::string prefix = "" )
   {
      std::ostringstream s; s << prefix << " "
      << float( RGBA_R(color) )/255.f << " "
      << float( RGBA_G(color) )/255.f << " "
      << float( RGBA_B(color) )/255.f << "\n";
      return s.str();
   }

   static bool
   load( SMeshBuffer const & src, std::string objUri )
   {
      return false;
   }

   static bool
   save( SMeshBuffer const & src, std::string const & objUri )
   {
      std::string mtlUri = dbGetFileBaseName( objUri ) + ".mtl";
      DE_DEBUG("Writing MTL = ", mtlUri )
      src.getMaterial().saveMTL( mtlUri );

      DE_DEBUG("Writing OBJ = ", objUri )
      //   # exported by libDarkGPU (c) 2021 <benjaminhampe@gmx.de>
      //   # www.blender.org
      //   mtllib Cube.mtl
      //   o Cube
      //   v 1.000000 -1.000000 -1.000000
      //   v 1.000000 -1.000000 1.000000
      //   v -1.000000 -1.000000 1.000000
      //   v -1.000000 -1.000000 -1.000000
      //   v 1.000000 1.000000 -0.999999
      //   v 0.999999 1.000000 1.000001
      //   v -1.000000 1.000000 1.000000
      //   v -1.000000 1.000000 -1.000000
      //   vn 0.000000 -1.000000 0.000000
      //   vn 0.000000 1.000000 0.000000
      //   vn 1.000000 -0.000000 0.000000
      //   vn -0.000000 -0.000000 1.000000
      //   vn -1.000000 -0.000000 -0.000000
      //   vn 0.000000 0.000000 -1.000000
      //   g Cube_Cube_Material
      //   usemtl Material
      //   s off
      //   f 2//1 3//1 4//1
      //   f 8//2 7//2 6//2
      //   f 1//3 5//3 6//3
      //   f 2//4 6//4 7//4
      //   f 7//5 8//5 4//5
      //   f 1//6 4//6 8//6
      //   f 1//1 2//1 4//1
      //   f 5//2 8//2 6//2
      //   f 2//3 1//3 6//3
      //   f 3//4 2//4 7//4
      //   f 3//5 7//5 4//5
      //   f 5//6 1//6 8//6

//      uint32_t faceCount = src.getFaceCount();
//      uint32_t edgeCount = src.getEdgeCount();
      uint32_t mbv = src.getVertexCount();
      uint32_t mbi = src.getIndexCount();

      std::ostringstream s;
      s <<
      "# exported by libDarkGPU (c) 2021 Benjamin Hampe <benjaminhampe@gmx.de>\n"
      "# SMeshBuffer.Name = " << src.getName() << "\n"
      "# SMeshBuffer.Uri = " << objUri << "\n"
      "# SMeshBuffer.Material.Name = " << src.getMaterial().Name << "\n"
      "# SMeshBuffer.Material.Uri = " << mtlUri << "\n"
      "# SMeshBuffer.BBox.Min = " << src.getBoundingBox().getMin() << "\n"
      "# SMeshBuffer.BBox.Max = " << src.getBoundingBox().getMax() << "\n"
      "# SMeshBuffer.FVF = " << src.getFVF().getByteSize() << "\n"
      "# SMeshBuffer.PrimitiveType = " << src.getPrimitiveTypeStr() << "\n"
      "# SMeshBuffer.VertexCount = " << src.getVertexCount() << "\n"
      "# SMeshBuffer.IndexCount = " << src.getIndexCount() << "\n"
      "# SMeshBuffer.IndexType = " << src.getIndexTypeStr() << "\n"

      "mtllib " << mtlUri << "\n\n"
      "o " << src.getName() << "\n"

      //"" << mbv << " " << faceCount << " " << edgeCount << "\n"
      //"g grp" << prefix << "\n"

      "# SMeshBuffer.Vertices = " << mbv << "\n";
      for ( uint32_t i = 0; i < mbv; ++i )
      {
         auto const & pos = src.getVertex( i ).getPos();
         s << "v " << pos.x << " " << pos.y << " " << pos.z << "\n";
      }

      s << "# SMeshBuffer.TexCoords = " << mbv << "\n";
      for ( uint32_t i = 0; i < mbv; ++i )
      {
         auto const & tex = src.getVertex( i ).getTexCoord();
         s << "vt " << tex.x << " " << tex.y << "\n";
      }
      s << "# SMeshBuffer.Normals = " << mbv << "\n";
      for ( uint32_t i = 0; i < mbv; ++i )
      {
         auto const & nrm = src.getVertex( i ).getNormal();
         s << "vn " << nrm.x << " " << nrm.y << " " << nrm.z << "\n";
      }

      s << "usemtl " << src.getMaterial().Name << "\n";
      s << "s off\n";

      if ( mbi == 0 )
      {
         s << "# No indices and faces, vertex only";
      }

      if ( src.getPrimitiveType() == PrimitiveType::Points )
      {
         for ( uint32_t i = 0; i < mbi; i++ )
         {
            auto A = src.getIndex( i );
            s << "f " << A << "/" << A << "/" << A << "\n";
         }
      }
      else if ( src.getPrimitiveType() == PrimitiveType::Lines )
      {
         for ( uint32_t i = 0; i < mbi; i += 2 )
         {
            auto A = src.getIndex( i );
            auto B = src.getIndex( i+1 );
            s << "f "
              << A << "/" << A << "/" << A << " "
              << B << "/" << B << "/" << B << "\n";
         }
      }
      if ( src.getPrimitiveType() == PrimitiveType::Triangles )
      {
         for ( uint32_t i = 0; i < mbi; i += 3 )
         {
            auto A = src.getIndex( i );
            auto B = src.getIndex( i+1 );
            auto C = src.getIndex( i+2 );
            s << "f "
              << A << "/" << A << "/" << A << " "
              << B << "/" << B << "/" << B << " "
              << C << "/" << C << "/" << C << "\n";
         }
      }
      if ( src.getPrimitiveType() == PrimitiveType::Quads )
      {
         for ( uint32_t i = 0; i < mbi; i += 4 )
         {
            auto A = src.getIndex( i );
            auto B = src.getIndex( i+1 );
            auto C = src.getIndex( i+2 );
            auto D = src.getIndex( i+3 );
            s << "f "
              << A << "/" << A << "/" << A << " "
              << B << "/" << B << "/" << B << " "
              << C << "/" << C << "/" << C << " "
              << D << "/" << D << "/" << D << "\n";
         }
      }

      if ( dbSaveText( objUri, s.str() ) )
      {
         DE_DEBUG("Saved OBJ mesh file to disk, uri = ", objUri )
         return true;
      }
      else
      {
         DE_DEBUG("Cant save OBJ mesh file to disk, uri = ", objUri )
         return false;
      }
   }
};
*/
