#pragma once
#include <de/smesh/SMeshIO.h>

namespace de {
namespace smesh {

struct SMeshReader3DS : public IReader
{
    SMeshReader3DS() {}
    ~SMeshReader3DS() override {}

    std::vector< std::string >
    getSupportedExtensions() const override { return { "3ds" }; }

    bool
    load(gpu::VideoDriver* driver,
        SMesh & m, std::string const & uri, const uint8_t* p, size_t n,
         LoadOptions const & opt = LoadOptions() ) override;


/*
   // inline
   static bool load( SMesh & mesh, std::string uri, VideoDriver* driver )
   {
      Binary file( uri );
      if ( !file.is_open() )
      {
         DE_ERROR("Cant open ",uri)
         return false;
      }
      return load( mesh, file, driver );
   }

   // impl in .cpp
   static bool load( SMesh & mesh, Binary & file, VideoDriver* driver );
*/
};


} // end namespace smesh.
} // end namespace de.
