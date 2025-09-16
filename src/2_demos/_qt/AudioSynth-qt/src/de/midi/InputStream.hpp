#pragma once
#include <DarkImage.h>

namespace de {
namespace midi {

inline char
hexStrLowNibble( uint8_t byte )
{
   size_t lowbyteNibble = byte & 0x0F;
   if ( lowbyteNibble < 10 )
   {
      return '0' + lowbyteNibble;
   }
   else
   {
      return 'A' + (lowbyteNibble-10);
   }
}

inline char
hexStrHighNibble( uint8_t byte )
{
   return hexStrLowNibble( byte >> 4 );
}

inline std::string
hexStr( uint8_t byte )
{
   std::ostringstream s;
   s << hexStrHighNibble( byte ) << hexStrLowNibble( byte );
   return s.str();
}

inline std::string
hexStr( uint16_t color )
{
   uint8_t r = color & 0xFF;
   uint8_t g = ( color >> 8 ) & 0xFF;
   std::stringstream s;
   s << hexStr( g ) << hexStr( r );
   return s.str();
}

inline std::string
hexStr( uint32_t color )
{
   uint8_t r = color & 0xFF;
   uint8_t g = ( color >> 8 ) & 0xFF;
   uint8_t b = ( color >> 16 ) & 0xFF;
   uint8_t a = ( color >> 24 ) & 0xFF;
   std::stringstream s;
   s << hexStr( a ) << hexStr( b ) << hexStr( g ) << hexStr( r );
   return s.str();
}

inline std::string
hexStr( uint64_t color )
{
   uint8_t r = color & 0xFF;
   uint8_t g = ( color >> 8 ) & 0xFF;
   uint8_t b = ( color >> 16 ) & 0xFF;
   uint8_t a = ( color >> 24 ) & 0xFF;
   uint8_t x = ( color >> 32 ) & 0xFF;
   uint8_t y = ( color >> 40 ) & 0xFF;
   uint8_t z = ( color >> 48 ) & 0xFF;
   uint8_t w = ( color >> 56 ) & 0xFF;
   std::stringstream s;
   s << hexStr( w ) << hexStr( z ) << hexStr( y ) << hexStr( x );
   s << hexStr( a ) << hexStr( b ) << hexStr( g ) << hexStr( r );
   return s.str();
}

inline std::string
hexStr( uint8_t const* beg, uint8_t const* end )
{
   std::stringstream s;
   auto src = beg;
   while ( src < end )
   {
      if ( src > beg ) s << ", ";
      s << "0x" << hexStr( *src );
      src++;
   }
   return s.str();
}

inline std::string
hexStr( uint8_t const* beg, uint8_t const* end, size_t nBytesPerRow )
{
   size_t i = 0;
   std::stringstream s;
   auto src = beg;
   while ( src < end )
   {
      i++;
      s << "0x" << hexStr( *src++ );
      if ( src < end ) s << ", ";
      if ( i >= nBytesPerRow )
      {
         i -= nBytesPerRow;
         s << "\n";
      }
   }
   return s.str();
}


// =================================================================
struct ByteOrder
// =================================================================
{
   constexpr inline static uint16_t
   makeU16( uint8_t a, uint8_t b ) noexcept
   {
       return uint16_t( a ) | (uint16_t( b ) << 8);
   }

   constexpr inline static uint32_t
   makeU32( uint8_t r, uint8_t g, uint8_t b, uint8_t a ) noexcept
   {
      return uint32_t( r ) |
           ( uint32_t( g ) << 8  ) |
           ( uint32_t( b ) << 16 ) |
           ( uint32_t( a ) << 24 );
   }

   constexpr inline static uint64_t
   makeU64( uint8_t r, uint8_t g, uint8_t b, uint8_t a,
            uint8_t x, uint8_t y, uint8_t z, uint8_t w ) noexcept
   {
      return uint64_t( r ) |
           ( uint64_t( g ) << 8  ) |
           ( uint64_t( b ) << 16 ) |
           ( uint64_t( a ) << 24 ) |
           ( uint64_t( x ) << 32 ) |
           ( uint64_t( y ) << 40 ) |
           ( uint64_t( z ) << 48 ) |
           ( uint64_t( w ) << 56 );
   }

   constexpr inline static uint16_t
   bigEndianU16( void const* bytes ) noexcept
   {
      return makeU16(
               static_cast< uint8_t const* >( bytes )[ 1 ],
               static_cast< uint8_t const* >( bytes )[ 0 ] );
   }

   constexpr inline static uint32_t
   bigEndianU32( void const* bytes ) noexcept
   {
      return makeU32(
               static_cast< uint8_t const* >( bytes )[ 3 ],
               static_cast< uint8_t const* >( bytes )[ 2 ],
               static_cast< uint8_t const* >( bytes )[ 1 ],
               static_cast< uint8_t const* >( bytes )[ 0 ] );
   }

   constexpr inline static uint64_t
   bigEndianU64( void const* bytes ) noexcept
   {
      return makeU64(
               static_cast< uint8_t const* >( bytes )[ 7 ],
               static_cast< uint8_t const* >( bytes )[ 6 ],
               static_cast< uint8_t const* >( bytes )[ 5 ],
               static_cast< uint8_t const* >( bytes )[ 4 ],
               static_cast< uint8_t const* >( bytes )[ 3 ],
               static_cast< uint8_t const* >( bytes )[ 2 ],
               static_cast< uint8_t const* >( bytes )[ 1 ],
               static_cast< uint8_t const* >( bytes )[ 0 ] );
   }

};

// =======================================================================
struct InputStream
// =======================================================================
{
   DE_CREATE_LOGGER("de.InputStream")
   std::vector< uint8_t > m_data;
   size_t m_readIndex;
   std::string m_uri;

   inline InputStream() noexcept : m_readIndex( 0 )
   {
   }


   inline bool
   loadFromData( uint8_t const* src, uint64_t byteCount, uint64_t byteLimit = (uint64_t(1) << 30) ) // noexcept
   {
      if ( !src ) { DE_ERROR("No src") return false; }

      if ( byteCount > byteLimit ) // reached RAM limit.
      {
         DE_ERROR("Reached byteLimit(",byteLimit,") with byteCount(",byteCount,"). ",m_uri)
         return false;
      }

      if ( byteCount < 0 )
      {
         reset();
      }
      else
      {
         m_data.resize( byteCount );
         memcpy( m_data.data(), src, byteCount );
      }

      DE_DEBUG("Data has ",byteCount," byte(s).")
      m_readIndex = 0;
      m_uri = "setData" + std::to_string(byteCount);
      return true;
   }

   // 8GB RAM limit
   inline bool
   loadFromFile( std::string const & uri, uint64_t byteLimit = (uint64_t(1) << 30) ) // noexcept
   {
      FILE* file = ::fopen64( uri.c_str(), "rb" );
      if ( !file )
      {
         DE_ERROR("Cant open file ",uri)
         return false;
      }

      ::fseeko64( file, 0, SEEK_END );    // Jump to end
      auto siz = ::ftello64( file );      // Read file size
      ::fseeko64( file, 0, SEEK_SET );    // Jump to begin

      if ( siz < 1 ) // No data
      {
         DE_ERROR("Empty file ",uri)
         return false;
      }

      if ( siz > byteLimit ) // Bad, reached RAM limit.
      {
         DE_ERROR("File reached RAM limit(",byteLimit,"), abort. ",uri)
         return false;
      }

      // Copy entire file to RAM
      m_data.resize( siz );
      ::fread( m_data.data(), 1, m_data.size(), file );

      // Close file and use m_data in RAM now ( aka very fast ) ...
      ::fclose( file );

      DE_DEBUG("File has uri = ",uri)
      DE_DEBUG("File has byteCount = ",m_data.size())

      m_readIndex = 0;
      m_uri = uri;
      return true;
   }

   ~InputStream()
   {
   }

   inline void
   reset() noexcept
   {
      m_data.clear();
      m_readIndex = 0;
   }

   inline uint8_t*
   beg() noexcept { return m_data.data(); }

   inline uint8_t const*
   beg() const noexcept { return m_data.data(); }

   inline uint8_t*
   end() noexcept { return m_data.data() + m_data.size(); }

   inline uint8_t const*
   end() const noexcept { return m_data.data() + m_data.size(); }

   inline std::string const &
   uri() const noexcept { return m_uri; }

   inline uint8_t const*
   data() const noexcept { return m_data.data(); }

   inline uint8_t*
   data() noexcept { return m_data.data(); }

   inline uint8_t const*
   read() const noexcept { return m_data.data() + m_readIndex; }

   inline size_t
   avail() const noexcept
   {
      if ( tell() > size() ) { DE_ERROR("invalid index(",tell(),"), size(",size(),")") return 0; }
      return size() - tell();
   }

   inline size_t
   size() const noexcept { return m_data.size(); }

   inline size_t
   tell() const noexcept { return m_readIndex; }

   inline uint8_t
   getc() noexcept
   {
      if ( tell() > size() - 1 ) { return 0; }
      uint8_t byte = m_data[ m_readIndex ];
      m_readIndex++;
      return byte;
   }

   inline size_t
   seek( size_t i ) noexcept
   {
      if ( i == tell() ) { DE_ERROR("invalid index(",i,"), size(",size(),")") return false; }
      if ( i > size() - 1 ) { DE_ERROR("invalid index(",i,"), size(",size(),")") return false; }
      m_readIndex = i;
      //DE_DEBUG("seek(",i, ") from pos(",tell(), ")")
      return tell();
   }

   inline size_t
   skip( size_t bytesCount ) noexcept
   {
      if ( bytesCount < 1 ) return true;
      return seek( tell() + bytesCount );
   }

   inline size_t
   putback( size_t relativeBytes ) noexcept
   {
      if ( relativeBytes < 1 ) return true;
      if ( relativeBytes > m_readIndex ) { m_readIndex = 0; return false; }
      //DE_DEBUG("putback(",relativeBytes, ") from pos(",tell(), ")")
      m_readIndex -= relativeBytes;
      return true;
   }

   inline size_t
   readU8( void* dst ) noexcept
   {
      if ( tell() > size() - 1 ) { return 0; }
      auto pDst = static_cast< uint8_t* >( dst );
      *pDst = m_data[ m_readIndex ];
      m_readIndex += sizeof( uint8_t );
      return 1; // 1 Byte consumed
   }

   inline size_t
   readU16( void* dst ) noexcept
   {
      if ( tell() > size() - 2 ) { return 0; }
      auto pDst = static_cast< uint16_t* >( dst );
      auto pSrc = reinterpret_cast< uint16_t* >( &m_data[ m_readIndex ]  );
      *pDst = *pSrc;
      m_readIndex += sizeof( uint16_t );
      return 2; // 2 Bytes consumed
   }

   inline size_t
   readU32( void* dst ) noexcept
   {
      if ( tell() > size() - 4 ) { return 0; }
      auto pDst = static_cast< uint32_t* >( dst );
      auto pSrc = reinterpret_cast< uint32_t* >( &m_data[ m_readIndex ]  );
      *pDst = *pSrc;
      m_readIndex += sizeof( uint32_t );
      return 4; // 4 Bytes consumed
   }

   inline size_t
   readU8_be( void* dst ) noexcept { return readU8( dst ); }

   inline size_t
   readU16_be( void* dst ) noexcept
   {
      if ( tell() > size() - 2 ) { return 0; }
      auto pDst = static_cast< uint16_t* >( dst );
      auto pSrc = reinterpret_cast< uint16_t* >( &m_data[ m_readIndex ] );
      *pDst = ByteOrder::bigEndianU16( pSrc );
      m_readIndex += sizeof( uint16_t );
      return 2; // 2 Bytes consumed
   }

   inline size_t
   readU32_be( void* dst ) noexcept
   {
      if ( tell() > size() - 4 ) { return 0; }
      auto pDst = static_cast< uint32_t* >( dst );
      auto pSrc = reinterpret_cast< uint32_t* >( &m_data[ m_readIndex ] );
      *pDst = ByteOrder::bigEndianU32( pSrc );
      m_readIndex += sizeof( uint32_t );
      return 4; // 4 Bytes consumed
   }



};

} // end namespace midi
} // end namespace de
