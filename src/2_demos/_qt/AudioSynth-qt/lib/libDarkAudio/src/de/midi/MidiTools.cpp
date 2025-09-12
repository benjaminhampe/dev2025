#include <de/midi/MidiTools.hpp>

namespace de {
namespace midi {

// static
std::string
// =======================================================================
MidiTools::noteStr( int key )
// =======================================================================
{
   std::stringstream s;

   int semi = key % 12;
   int octave = key / 12 - 1;
   switch ( semi )
   {
      case  0: s << "C";  break;
      case  1: s << "C#"; break;
      case  2: s << "D";  break;
      case  3: s << "D#"; break;
      case  4: s << "E";  break;
      case  5: s << "F";  break;
      case  6: s << "F#"; break;
      case  7: s << "G";  break;
      case  8: s << "G#"; break;
      case  9: s << "A";  break;
      case 10: s << "A#"; break;
      case 11: s << "B";  break;
   }
   s << octave;

   return s.str();
}

size_t
MidiTools::readU16( uint8_t const* beg, uint8_t const* end, uint16_t & value ) noexcept
{
   if ( end - beg < 2 ) { return 0; }
   auto src = beg;
   uint_fast16_t r = *src++;
   uint_fast16_t g = *src++;
   value = (g << 8) | r;
   return src - beg;
}

size_t
MidiTools::readU16_be( uint8_t const* beg, uint8_t const* end, uint16_t & value ) noexcept
{
   if ( end - beg < 2 ) { return 0; }
   auto src = beg;
   uint_fast16_t r = *src++;
   uint_fast16_t g = *src++;
   value = (r << 8) | g;
   return src - beg;
}

size_t
MidiTools::readU32( uint8_t const* beg, uint8_t const* end, uint32_t & value ) noexcept
{
   if ( end - beg < 2 ) { return 0; }
   auto src = beg;
   uint_fast32_t r = *src++;
   uint_fast32_t g = *src++;
   uint_fast32_t b = *src++;
   uint_fast32_t a = *src++;
   value = (a << 24) | (b << 8) | (g << 8) | r;
   return src - beg;
}

size_t
MidiTools::readU32_be( uint8_t const* beg, uint8_t const* end, uint32_t & value ) noexcept
{
   if ( end - beg < 2 ) { return 0; }
   auto src = beg;
   uint_fast32_t r = *src++;
   uint_fast32_t g = *src++;
   uint_fast32_t b = *src++;
   uint_fast32_t a = *src++;
   value = (r << 24) | (g << 8) | (b << 8) | a;
   return src - beg;
}

//
// VLQ - Variable-Length Quantity
//

uint32_t
MidiTools::parseVLQ( uint8_t const* beg, uint8_t const* end, uint32_t & value )
{
   if ( beg == end ) return 0;
   auto it = beg;     // Store original pointer to compute delta.
   uint8_t c = *it++; // Read 1st byte, hopefully the last.

   uint32_t r = c & 0x7F;
   while ( c >= 0x80 && ( it != end ) )
   {
      c = *it++;      // Read next byte, hopefully the last.
      r = ( r << 7 ) | ( 0x7F & c );
   }

   //DE_DEBUG("[",hexStr(uint64_t(src)),"] Parsed VLQ = ",retVal," after ",n," bytes")
   value = r;
   return it - beg;
}

void
MidiTools::testVLQ( int k, std::vector< uint8_t > dat, uint32_t n, uint32_t expectedBytes, uint32_t expectedValue )
{
   uint8_t const* beg = dat.data();
   uint8_t const* end = dat.data() + dat.size();
   uint32_t returnedValue = 0;
   uint32_t returnedBytes = parseVLQ( beg, end, returnedValue );

   std::stringstream s;
   s << "Test[" << k << "] :: Data(" << dat.size() << ")";
   s << "[";
   for ( size_t i = 0; i < dat.size(); ++i )
   {
      s << "0x" << hexStr( dat[i] );
      if ( i < dat.size() - 1 )
      {
         s << ", ";
      }
   }
   s << "] ";
   s << "expect( " << expectedBytes << "B, " << hexStr(expectedValue) << " = " << expectedValue << " ), ";
   s << "return( " << returnedBytes << "B, " << hexStr(returnedValue) << " = " << returnedValue << " ), ";

   bool ok = true;
   ok &= (expectedBytes == returnedBytes);
   ok &= (expectedValue == returnedValue);

   if ( ok )
   {
      s << " OK";
      DE_DEBUG( s.str() )
   }
   else
   {
      s << " FAIL";
      DE_ERROR( s.str() )
      throw std::runtime_error( s.str() );
   }
}

// NUMBER   VARIABLE LENGTH QUANTITY
// 00000000 00
// 00000040 40
// 0000007F 7F
// 00000080 81 00
// 00002000 C0 00
// 00003FFF FF 7F
// 00004000 81 80 00
// 00100000 C0 80 00
// 001FFFFF FF FF 7F
// 00200000 81 80 80 00
// 08000000 C0 80 80 00
// 0FFFFFFF FF FF FF 7F

void
MidiTools::test()
{
   testVLQ( 1, { 0x00 }, 1, 1, 0 );
   testVLQ( 2, { 0x40 }, 1, 1, 64 );
   testVLQ( 3, { 0x7F }, 1, 1, 127 );

   testVLQ( 4, { 0x81, 0x00 }, 2, 2, 0x80 ); // 128
   testVLQ( 5, { 0xC0, 0x00 }, 2, 2, 0x2000 ); // 8*1024
   testVLQ( 6, { 0xFF, 0x7F }, 2, 2, 0x3FFF );

   testVLQ( 7, { 0x81, 0x80, 0x00 }, 3, 3, 0x00004000 );
   testVLQ( 8, { 0xC0, 0x80, 0x00 }, 3, 3, 0x00100000 );
   testVLQ( 9, { 0xFF, 0xFF, 0x7F }, 3, 3, 0x001FFFFF );

   testVLQ( 10, { 0x81, 0x80, 0x80, 0x00 }, 4, 4, 0x00200000 );
   testVLQ( 11, { 0xC0, 0x80, 0x80, 0x00 }, 4, 4, 0x08000000 );
   testVLQ( 12, { 0xFF, 0xFF, 0xFF, 0x7F }, 4, 4, 0x0FFFFFFF );

   // Test with one more byte
   testVLQ( 13, { 0x00, 0x00 }, 2, 1, 0 );
   testVLQ( 14, { 0x40, 0x00 }, 2, 1, 64 );
   testVLQ( 15, { 0x7F, 0x00 }, 2, 1, 127 );

   testVLQ( 16, { 0x81, 0x00, 0x00 }, 3, 2, 0x80 ); // 128
   testVLQ( 17, { 0xC0, 0x00, 0x00 }, 3, 2, 0x2000 ); // 8*1024
   testVLQ( 18, { 0xFF, 0x7F, 0x00 }, 3, 2, 0x3FFF );

   testVLQ( 19, { 0x81, 0x80, 0x00, 0x00 }, 4, 3, 0x00004000 );
   testVLQ( 20, { 0xC0, 0x80, 0x00, 0x00 }, 4, 3, 0x00100000 );
   testVLQ( 21, { 0xFF, 0xFF, 0x7F, 0x00 }, 4, 3, 0x001FFFFF );

   testVLQ( 22, { 0x81, 0x80, 0x80, 0x00, 0x00 }, 5, 4, 0x00200000 );
   testVLQ( 23, { 0xC0, 0x80, 0x80, 0x00, 0x00 }, 5, 4, 0x08000000 );
   testVLQ( 24, { 0xFF, 0xFF, 0xFF, 0x7F, 0x00 }, 5, 4, 0x0FFFFFFF );
}

uint32_t
MidiTools::getChannelColor( int channel )
{
   switch ( channel )
   {
      case 0: return 0xFF0000FF;    // red
      case 1: return 0xFF0080FF;    // orange
      case 2: return 0xFF00AF00;
      case 3: return 0xFFFFAF00;
      case 4: return 0xFFAF0000;
      case 5: return 0xFFFF0040;
      case 6: return 0xFF00FFFF;
      case 7: return 0xFFFFFFFF;
      case 8: return 0xFFFFC08F;
      case 9: return 0xFF000000;    // Drums = black
      case 10: return 0xFFFF00FF;
      case 11: return 0xFF00FF00;
      case 12: return 0xFF8F008F;
      case 13: return 0xFF3FF03F;
      case 14: return 0xFF7F40DF;
      case 15: return 0xFF3F808F;
      default: return 0xFFFFFFFF;
   }
}

} // end namespace midi
} // end namespace de

