#pragma once
#include <de_core/de_FileSystem.h>

namespace de {

enum class SeekMode
{
   SET = 0,
   CUR = 1,
   END = 2,
   Count = 3,
   Default = SeekMode::SET
};

//
// Load any file fully to RAM, then fast seek and rewind on it.
//
// ============================================================================
struct Binary
// ============================================================================
{
   DE_CREATE_LOGGER("de.Binary")

   std::vector< uint8_t > m_Blob;
   uint8_t* m_Ptr;
   uint64_t m_Count;
   uint64_t m_Index;
   uint64_t m_Avail;

   std::string m_Uri;

   Binary();
   Binary( std::string uri );
   //~Binary();

   static Binary        fromRAM( uint8_t const * ptr, uint64_t count, std::string uri );
   static Binary        fromHDD( std::string uri );

   // File and memory stuff (RAM)
   std::string const &  getUri() const;
   bool                 is_open() const;
   bool                 openMemory( uint8_t const * ptr, uint64_t count, std::string uri );
   void                 close();
   bool                 load( std::string uri );
   bool                 save( std::string uri ) const;
   void                 clear();
   uint64_t             size() const;
   uint8_t const *      data() const;
   uint8_t*             data();

   // FileStream stuff
   void                 rewind();
   uint64_t             tell() const;
   uint64_t             read( void* dst, uint64_t byteCount );
   uint8_t              readU8();
   uint16_t             readU16_lsb();
   uint32_t             readU32_lsb();
   uint16_t             readU16_msb();
   uint32_t             readU32_msb();
   bool                 seek( uint64_t byteOffset, int dir = SEEK_SET );
};

} // end namespace de.
