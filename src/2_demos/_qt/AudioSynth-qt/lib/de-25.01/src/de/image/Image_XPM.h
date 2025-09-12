#pragma once
#include <de/image/ImageConfig.h>
// #include <de_image_formats/de_ImageReader.h>
// #include <de_image_formats/de_ImageWriter.h>
// #include <de_core/de_StringUtil.h>
// #include <unordered_map>


namespace de {
namespace image {

#if defined(DE_IMAGE_READER_XPM_ENABLED) || defined(DE_IMAGE_WRITER_XPM_ENABLED)

namespace xpm {

struct Color
{
    std::string m_Key;
    uint32_t m_Color;
    Color( std::string key, uint32_t color ) : m_Key( key ), m_Color( color ) {}
};

struct Utils
{
    static std::string
    toRGBA( uint32_t color )
    {
        std::string s;
        if ( color == 0 )
        {
            return "none"; // transparent
        }
        else
        {
            uint8_t r = dbRGBA_R(color);
            uint8_t g = dbRGBA_G(color);
            uint8_t b = dbRGBA_B(color);
            uint8_t a = dbRGBA_A(color);
            s += '#';
            s += StringUtil::hex( r );
            s += StringUtil::hex( g );
            s += StringUtil::hex( b );
            s += StringUtil::hex( a );
        }
        return s;
    }

    static std::string
    toRGB( uint32_t color )
    {
        std::string s;
        if ( color == 0 )
        {
            return "none"; // transparent
        }
        else
        {
            uint8_t r = color & 0xFF;
            uint8_t g = ( color >> 8 ) & 0xFF;
            uint8_t b = ( color >> 16 ) & 0xFF;
            s += '#';
            s += StringUtil::hexHighNibble( r );
            s += StringUtil::hexLowNibble( r );
            s += StringUtil::hexHighNibble( g );
            s += StringUtil::hexLowNibble( g );
            s += StringUtil::hexHighNibble( b );
            s += StringUtil::hexLowNibble( b );
        }
        return s;
    }

//   static std::string
//   getValidChars()
//   {
//      return " .+@#$%&*=-;>,')!~{]^/(_:<[}|1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ`";
//   }

    static uint32_t
    pow_u( uint32_t val, uint32_t exponent = 0 )
    {
        uint32_t potenz = 1;
        for ( size_t i = 0; i < size_t( exponent ); ++i )
        {
            potenz *= val;
        }
        return potenz;
    }

    static std::string
    computeColorName( uint64_t index, std::string const & valid_chars, int digitCount )
    {
        std::string tmp;
        uint32_t v = uint32_t( valid_chars.size() );
        uint64_t rest = index; // 50000
        for ( int i = digitCount-1; i >= 0; --i ) // n=2 -> [0,1]
        {
            uint32_t n = pow_u( v, uint32_t(i) ); // 39^2
            double f = double( rest ) / double( n ); // 50000/39^2 = 32.8731
            uint32_t digit = uint32_t( f ); // digit = 32;
            char ch = valid_chars[ digit ];
            tmp += ch;
            rest -= digit * n;
        }
        return tmp;
    }
};


} // end namespace xpm

#endif // DE_IMAGE_CODEC_XPM_ENABLED


#ifdef DE_IMAGE_READER_XPM_ENABLED

class ImageReaderXPM : public IImageReader
{
public:
    std::vector< std::string > getSupportedReadExtensions() const override { return { "xpm" }; }

    bool isSupportedReadExtension( std::string const & ext ) const override { return ext == "xpm"; }

    bool load( Image & img, const uint8_t* p, size_t n, const std::string& uri = "" ) override;
};

#endif // DE_IMAGE_READER_XPM_ENABLED

#ifdef DE_IMAGE_WRITER_XPM_ENABLED

class ImageWriterXPM : public IImageWriter
{
public:
    std::vector< std::string > getSupportedWriteExtensions() const override { return { "xpm", "h", "hpp" }; }

    bool isSupportedWriteExtension( std::string const & ext ) const override { return ext == "xpm" || ext == "h" || ext == "hpp"; }

    bool save( Image const & img, std::string const & uri, uint32_t quality = 0 ) override;
};

#endif // DE_IMAGE_WRITER_XPM_ENABLED


} // end namespace image.
} // end namespace de.

