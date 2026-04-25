#include <DarkImage.h>
// #include <de/Math.h>
// #include <de/ColorGradient.h>
// #include <de/image/Image.h>
// #include <de/image/ImagePainter.h>
// #include <de/image/Bresenham.h>
// #include <de/os/CommonDialogs.h>
// #include <de/Font5x8.h>

// #ifdef _WIN32
    // #include <de/os/win32/ComInit.h>
// #endif

de::Image
dbImageFromLinearColorGradient(const de::LinearColorGradient & cg,
                         int w, int h, bool bVertical)
{
    if ( w < 1 || h < 1 )
    {
        return {};
    }

    de::Image img( w, h );

    // vertical filling
    if ( bVertical )
    {
        float const fy = 1.0f / float( h );

        for ( int32_t y = 0; y < h; ++y )
        {
            uint32_t color = cg.getColor32( fy*y );

            for ( int32_t x = 0; x < w; ++x )
            {
                img.setPixel( x, y, color );
            }
        }
    }
    // horizontal filling
    else
    {
        float const fx = 1.0f / float( w );

        for ( int32_t x = 0; x < w; ++x )
        {
            uint32_t color = cg.getColor32( fx*x );

            for ( int32_t y = 0; y < h; ++y )
            {
                img.setPixel( x, y, color );
            }
        }
    }

    return img;
}

bool
dbImageToLinearColorGradient( de::LinearColorGradient & cg,
                        const de::Image &img, bool bVertical)
{
    int w = img.w();
    int h = img.h();
    if ( w < 1 || h < 1 )
    {
        return false;
    }

    cg.m_stops.clear();

    // [v]ertical extraction:
    if ( bVertical )
    {
        cg.m_stops.reserve(h);

        float const fy = 1.0f / float( h-1 );

        for ( int32_t y = 0; y < h; y++ )
        {
            float t = fy * y;
            cg.m_stops.emplace_back( t, img.getPixel(0,y) );
        }
    }
    // horizontal filling
    else
    {
        cg.m_stops.reserve(w);

        float const fx = 1.0f / float( w-1 );

        for ( int32_t x = 0; x < w; ++x )
        {
            float t = fx * x;
            cg.m_stops.emplace_back( t, img.getPixel(x,0) );
        }
    }

    return true;
}
