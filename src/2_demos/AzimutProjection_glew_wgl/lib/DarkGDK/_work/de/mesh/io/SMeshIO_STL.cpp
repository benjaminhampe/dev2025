#include "SMeshIO_STL.hpp"
#include <fstream>

namespace de {
namespace gpu {

// static
bool
SMeshBufferSTL::load( SMeshBuffer const & mesh, std::string uri, int flags )
{
   DE_DEBUG("Loading STL file Not impl ", uri )
   // if (flags > 0) // EMWF_WRITE_COMPRESSED
      // return writeMeshBinary( mesh, uri, flags);
   // else
   //return writeMeshASCII( mesh, uri, flags);
   return false;
}

// static
std::string
SMeshBufferSTL::writeVec3f( glm::vec3 const & pos )
{
   std::ostringstream s;
   s << pos.x << " " << pos.y << " " << pos.z;
   return s.str();
}

// static
std::string
SMeshBufferSTL::writeTriangle3f( glm::vec3 const & a, glm::vec3 const & b, glm::vec3 const & c )
{
   glm::vec3 n = Math::getNormal3D( a, b, c );
   std::ostringstream s;
   s << "facet normal " << writeVec3f( n ) << "\n"
   "  outer loop\n"
   "    vertex " << writeVec3f( a ) << "\n"
   "    vertex " << writeVec3f( b ) << "\n"
   "    vertex " << writeVec3f( c ) << "\n"
   "  endloop\n"
   "endfacet\n";
   return s.str();
}

// static
bool
SMeshBufferSTL::writeMeshASCII( SMeshBuffer const & mesh, std::string uri, int flags )
{
   std::ostringstream s;
   // header
   s << "solid " << mesh.getName() << "\n\n";

   // buffers
//      for ( size_t i = 0; i < mesh.getMeshBufferCount(); ++i )
//      {
//         auto p = mesh.getMeshBuffer( i );
//         if ( p )
//         {
         auto p = &mesh;
         auto indexCount = p->getIndexCount();
         for ( size_t j = 0; j < indexCount; j += 3 )
         {
            auto a = p->getVertex( p->getIndex( j ) ).getPos();
            auto b = p->getVertex( p->getIndex( j+1 ) ).getPos();
            auto c = p->getVertex( p->getIndex( j+2 ) ).getPos();
            s << writeTriangle3f( a,b,c );
         }
         s << "\n";
//         }
//      }

   s << "endsolid " << mesh.getName();

   //dbSaveText( uri, s.str() );
   std::ofstream fout( uri.c_str() );
   if ( fout.is_open() )
   {
      fout << s.rdbuf();
      fout.close();
      DE_DEBUG("Saved STL file ", uri)
      return true;
   }
   else
   {
      DE_ERROR("Did not save STL file ", uri)
      return false;
   }
}

// static
bool
SMeshBufferSTL::save( SMeshBuffer const & mesh, std::string uri, int flags )
{
   DE_DEBUG("Writing STL file ", uri )
   // if (flags > 0) // EMWF_WRITE_COMPRESSED
      // return writeMeshBinary( mesh, uri, flags);
   // else
      return writeMeshASCII( mesh, uri, flags);
}


/*
   bool writeMeshBinary( SMesh const & mesh, std::string uri, int flags )
   {

      // write STL MESH header

      file->write("binary ",7);
      const core::stringc name(SceneManager->getMeshCache()->getMeshName(mesh));
      const s32 sizeleft = 73-name.size(); // 80 byte header
      if (sizeleft<0)
         file->write(name.c_str(),73);
      else
      {
         char* buf = new char[80];
         memset(buf, 0, 80);
         file->write(name.c_str(),name.size());
         file->write(buf,sizeleft);
         delete [] buf;
      }
      u32 facenum = 0;
      for (u32 j=0; j<mesh->getMeshBufferCount(); ++j)
         facenum += mesh->getMeshBuffer(j)->getIndexCount()/3;
      file->write(&facenum,4);

      // write mesh buffers

      for (u32 i=0; i<mesh->getMeshBufferCount(); ++i)
      {
         IMeshBuffer* buffer = mesh->getMeshBuffer(i);
         if (buffer)
         {
            const u32 indexCount = buffer->getIndexCount();
            const u16 attributes = 0;
            for (u32 j=0; j<indexCount; j+=3)
            {
               const core::vector3df& v1 = buffer->getPosition(buffer->getIndices()[j]);
               const core::vector3df& v2 = buffer->getPosition(buffer->getIndices()[j+1]);
               const core::vector3df& v3 = buffer->getPosition(buffer->getIndices()[j+2]);
               const core::plane3df tmpplane(v1,v2,v3);
               file->write(&tmpplane.Normal, 12);
               file->write(&v1, 12);
               file->write(&v2, 12);
               file->write(&v3, 12);
               file->write(&attributes, 2);
            }
         }
      }
      return true;
   }

   // EType getType() const { return EMWT_STL; }

   static std::string
   Vec3f_toString( glm::vec3 const & pos )
   {
      std::ostringstream s;
      s << pos.x << " " << pos.y << " " << pos.z;
      return s.str();
   }

   static std::string
   Triangle3f_toString( glm::vec3 const & a, glm::vec3 const & b, glm::vec3 const & c )
   {
      glm::vec3 n = Math::getNormal3D( a, b, c );
      std::ostringstream s;
      s << "facet normal " << Vec3f_toString( n ) << "\n"
      "  outer loop\n"
      "    vertex " << Vec3f_toString( a ) << "\n"
      "    vertex " << Vec3f_toString( b ) << "\n"
      "    vertex " << Vec3f_toString( c ) << "\n"
      "  endloop\n"
      "endfacet\n";
      return s.str();
   }

   static bool
   writeMeshASCII( SMeshBuffer const & mesh, std::string uri, int flags )
   {
      std::ostringstream s;
      // header
      s << "solid " << mesh.getName() << "\n\n";

      // buffers
//      for ( size_t i = 0; i < mesh.getMeshBufferCount(); ++i )
//      {
//         auto p = mesh.getMeshBuffer( i );
//         if ( p )
//         {
            auto p = &mesh;
            auto indexCount = p->getIndexCount();
            for ( size_t j = 0; j < indexCount; j += 3 )
            {
               auto a = p->getVertex( p->getIndex( j ) ).getPos();
               auto b = p->getVertex( p->getIndex( j+1 ) ).getPos();
               auto c = p->getVertex( p->getIndex( j+2 ) ).getPos();
               s << Triangle3f_toString( a,b,c );
            }
            s << "\n";
//         }
//      }

      s << "endsolid " << mesh.getName();

      //dbSaveText( uri, s.str() );
      std::ofstream fout( uri.c_str() );
      if ( fout.is_open() )
      {
         fout << s.rdbuf();
         fout.close();
         DE_DEBUG("Saved STL file ", uri)
         return true;
      }
      else
      {
         DE_ERROR("Did not save STL file ", uri)
         return false;
      }
   }

   bool writeMeshBinary( SMesh const & mesh, std::string uri, int flags )
   {

      // write STL MESH header

      file->write("binary ",7);
      const core::stringc name(SceneManager->getMeshCache()->getMeshName(mesh));
      const s32 sizeleft = 73-name.size(); // 80 byte header
      if (sizeleft<0)
         file->write(name.c_str(),73);
      else
      {
         char* buf = new char[80];
         memset(buf, 0, 80);
         file->write(name.c_str(),name.size());
         file->write(buf,sizeleft);
         delete [] buf;
      }
      u32 facenum = 0;
      for (u32 j=0; j<mesh->getMeshBufferCount(); ++j)
         facenum += mesh->getMeshBuffer(j)->getIndexCount()/3;
      file->write(&facenum,4);

      // write mesh buffers

      for (u32 i=0; i<mesh->getMeshBufferCount(); ++i)
      {
         IMeshBuffer* buffer = mesh->getMeshBuffer(i);
         if (buffer)
         {
            const u32 indexCount = buffer->getIndexCount();
            const u16 attributes = 0;
            for (u32 j=0; j<indexCount; j+=3)
            {
               const core::vector3df& v1 = buffer->getPosition(buffer->getIndices()[j]);
               const core::vector3df& v2 = buffer->getPosition(buffer->getIndices()[j+1]);
               const core::vector3df& v3 = buffer->getPosition(buffer->getIndices()[j+2]);
               const core::plane3df tmpplane(v1,v2,v3);
               file->write(&tmpplane.Normal, 12);
               file->write(&v1, 12);
               file->write(&v2, 12);
               file->write(&v3, 12);
               file->write(&attributes, 2);
            }
         }
      }
      return true;
   }
*/

} // end namespace gpu
} // end namespace de


