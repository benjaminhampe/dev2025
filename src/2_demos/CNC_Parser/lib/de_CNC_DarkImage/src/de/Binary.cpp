#include <de/Binary.hpp>

namespace de {

// ============================================================================
// static
// ============================================================================

Binary
Binary::fromRAM( uint8_t const * ptr, uint64_t count, std::string uri )
{
   Binary b;
   b.openMemory( ptr, count, uri );
   return b;
}

Binary
Binary::fromHDD( std::string uri )
{
   Binary b;
   b.load( uri );
   return b;
}

// ============================================================================
Binary::Binary()
// ============================================================================
   : m_Ptr( nullptr )
   , m_Count( 0 )
   , m_Index( 0 )
   , m_Avail( 0 )
{

}

Binary::Binary( std::string uri )
   : Binary()
{
   load( uri );
}

//~Binary() { close(); }

uint64_t             Binary::tell() const { return m_Index; }
uint64_t             Binary::size() const { return m_Count; }
uint8_t const*       Binary::data() const { return m_Ptr; }
uint8_t*             Binary::data() { return m_Ptr; }
std::string const&   Binary::getUri() const { return m_Uri; }
bool                 Binary::is_open() const { return m_Ptr != nullptr; }
void                 Binary::close() { m_Ptr = nullptr; m_Blob.clear(); }
void                 Binary::clear() { m_Ptr = nullptr; m_Blob.clear(); }

bool
Binary::save( std::string uri ) const
{
   FILE* file = ::fopen( uri.c_str(), "wb" );
   if ( !file )
   {
      DE_ERROR("Cant open to write ", uri )
      return false;
   }

   if ( m_Ptr )
   {
      size_t bytesWritten = ::fwrite( m_Ptr, 1, m_Count, file );
      if ( bytesWritten < m_Blob.size() )
      {
         DE_ERROR("bytesWritten(",bytesWritten,") < m_Count(",m_Count,")")
      }
   }
   else
   {
      DE_ERROR("No data to write ", uri )
   }

   ::fclose( file );
   return true;
}

bool
Binary::openMemory( uint8_t const * ptr, uint64_t count, std::string uri )
{
   m_Blob.clear();
   m_Ptr = const_cast< uint8_t* >( ptr );
   m_Count = count;
   m_Avail = count;
   m_Index = 0;
   m_Uri = uri;
   return true;
}

bool
Binary::load( std::string uri )
{
   m_Uri = uri;
   //DE_DEBUG("Load binary file ",uri)

   FILE* file = ::fopen( uri.c_str(), "rb" );
   if ( !file )
   {
      DE_ERROR("Cant open ", uri )
      return false;
   }

   ::fseeko64( file, 0, SEEK_END );
   size_t byteCount = size_t( ::ftello64( file ) );
   ::fseeko64( file, 0, SEEK_SET );

   if ( byteCount < 1 )
   {
      DE_ERROR("Empty file content ", uri )
      return false;
   }

   m_Blob.clear();
   try
   {
      m_Blob.resize( byteCount, 0x00 );
      size_t bytesRead = ::fread( m_Blob.data(), 1, m_Blob.size(), file );
      if ( bytesRead < byteCount )
      {
         DE_ERROR("bytesRead(",bytesRead,") < byteCount(",byteCount,")")
         return false;
      }
   }
   catch(...)
   {
      DE_ERROR("OOM")
      return false;
   }

   ::fclose( file );
   m_Ptr = m_Blob.data();
   m_Count = m_Blob.size();
   m_Avail = m_Blob.size();
   m_Index = 0;
   return true;
}

void
Binary::rewind()
{
   m_Index = 0;
   m_Avail = m_Count;
}

uint8_t
Binary::readU8()
{
   if ( !m_Ptr || m_Avail < 1 )
   {
      DE_DEBUG("EOF")
      return 0;
   }

   uint8_t byte = *(m_Ptr + m_Index);
   m_Index += 1;
   m_Avail -= 1;
   return byte;
}

uint16_t
Binary::readU16_lsb()
{
   if ( !m_Ptr || m_Avail < 2 )
   {
      DE_DEBUG("EOF")
      return 0;
   }

   uint8_t const * p = m_Ptr + m_Index;
   uint8_t a = *p;
   uint8_t b = *(p + 1);
   m_Index += 2;
   m_Avail -= 2;
   return size_t( a ) | ( size_t( b ) << 8 );
}

uint32_t
Binary::readU32_lsb()
{
   if ( !m_Ptr || m_Avail < 4 )
   {
      DE_DEBUG("EOF")
      return 0;
   }

   uint8_t const * p = m_Ptr + m_Index;
   uint8_t a = *p;
   uint8_t b = *(p + 1);
   uint8_t c = *(p + 2);
   uint8_t d = *(p + 3);
   m_Index += 4;
   m_Avail -= 4;
   return size_t( a ) | ( size_t( b ) << 8 ) | ( size_t( c ) << 16 ) | ( size_t( d ) << 24 );

//      uint8_t a = readU8();
//      uint8_t b = readU8();
//      uint8_t c = readU8();
//      uint8_t d = readU8();
//      return size_t( a ) | ( size_t( b ) << 8 ) | ( size_t( c ) << 16 ) | ( size_t( d ) << 24 );
}

uint16_t
Binary::readU16_msb()
{
   if ( !m_Ptr || m_Avail < 2 )
   {
      DE_DEBUG("EOF")
      return 0;
   }

   uint8_t const * p = m_Ptr + m_Index;
   uint8_t a = *p;
   uint8_t b = *(p + 1);
   m_Index += 2;
   m_Avail -= 2;
   return size_t( b ) | ( size_t( a ) << 8 );

//      uint8_t a = readU8();
//      uint8_t b = readU8();
//      return size_t( b ) | ( size_t( a ) << 8 );
}

uint32_t
Binary::readU32_msb()
{
   if ( !m_Ptr || m_Avail < 4 )
   {
      DE_DEBUG("EOF")
      return 0;
   }

   uint8_t const * p = m_Ptr + m_Index;
   uint8_t a = *p;
   uint8_t b = *(p + 1);
   uint8_t c = *(p + 2);
   uint8_t d = *(p + 3);
   m_Index += 4;
   m_Avail -= 4;
   return size_t( d ) | ( size_t( c ) << 8 ) | ( size_t( b ) << 16 ) | ( size_t( a ) << 24 );

//      uint8_t a = readU8();
//      uint8_t b = readU8();
//      uint8_t c = readU8();
//      uint8_t d = readU8();
//      return size_t( d ) | ( size_t( c ) << 8 ) | ( size_t( b ) << 16 ) | ( size_t( a ) << 24 );
}

uint64_t
Binary::read( void* dst, uint64_t byteCount )
{
   if ( !m_Ptr )
   {
      DE_DEBUG("EOF")
      return 0;
   }

   if ( byteCount > m_Avail )
   {
      byteCount = m_Avail;
   }

   if ( byteCount > 0 )
   {
      ::memcpy( dst, m_Ptr + m_Index, byteCount );
      m_Index += byteCount;
      m_Avail -= byteCount;
      //DE_DEBUG("CHUNK(",byteCount,"), m_Index(",m_Index,"), m_Avail(",m_Avail,")")
   }

   return byteCount;
}

bool
Binary::seek( uint64_t byteOffset, int dir )
{
   if ( !m_Ptr || m_Count == 0 || m_Avail == 0 )
   {
      //DE_DEBUG("Not loaded")
      return false;
   }

   if ( byteOffset >= m_Count ) // byteOffset must be in range[0,m_Count-1]
   {
      DE_DEBUG("Invalid offset(",byteOffset,"), size(",m_Count,")")
      return false;
   }

   if ( dir == SEEK_SET )
   {
      m_Index = byteOffset;
      m_Avail = m_Count - m_Index;
      return true;
   }
   else if ( dir == SEEK_END )
   {
      m_Index = m_Count - byteOffset;
      m_Avail = byteOffset;
      return true;
   }
   else if ( dir == SEEK_CUR )
   {
      m_Index += byteOffset;
      m_Avail -= byteOffset;
      return true;
   }
   else
   {
      DE_DEBUG("Invalid SEEK MODE ", dir )
      return false;
   }
}

} // end namespace de.


/*
   // Skip
   if ( relativeMovement )
   {
      if ( dir == SEEK_SET )
      {
         if ( byteOffset > m_Avail )
         {
            DE_DEBUG("Invalid rel offset")
            return false;
         }
         else
         {
            m_Index += byteOffset;
            m_Avail -= byteOffset;
            return true;
         }

      }
      else if ( dir == SEEK_END )
      {
         if ( byteOffset > m_Count )
         {
            DE_DEBUG("Invalid end offset")
            return false;
         }
         else
         {
            m_Index -= byteOffset;
            m_Avail += byteOffset;
         }
      }

   }
   // Seek
   else
   {


   }
*/
