#include "H3_Chip_Tex2DArray.h"

#include <H3/H3_Game.h>
#include <de/image/Utils.h>
#include <de_opengl.h>
#include <de/gpu/GL_debug_layer.h>

bool
H3_ChipTexture_create( H3_Game & game, H3_ChipTexture & ct )
{
    std::vector<de::Image> images;
    images.reserve( 18 );

    int finalSize = 128;

    auto addImage = [&]( H3_Img::eID id )
    {
        images.emplace_back();
        de::Image & img = images.back();

        img = *H3_getImg( game, id );

        // Replace transparent color with white color.
        if (img.a() > 0)
        {
            de::ImagePainter::replaceColor(img, 0, 0xFFFFFFFF);
        }

        if (img.pixelFormat() != de::PixelFormat::RGB_24)
        {
            img = img.convert(de::PixelFormat::RGB_24);
        }

        int w = img.w();
        int h = img.h();
        if (w < h)
        {
            int x = 0;
            int y = (h-w)/2;
            img = de::ImageCutter::copyRegionRect(img,de::Recti(x,y,w,w));
        }
        else if (h < w)
        {
            int x = (w-h)/2;
            int y = 0;
            img = de::ImageCutter::copyRegionRect(img,de::Recti(x,y,h,h));
        }

        if ( finalSize != img.w() )
        {
            img = de::ImageScaler::resizeBicubic(img, finalSize, finalSize);
        }

        //auto uri = dbStr("H3_Chip_Tex2DArray", finalSize, "_",int(id),".webp");
        //dbSaveImage(img, uri);
    };

    addImage( H3_Img::Chip2);    // 0
    addImage( H3_Img::Chip3);    // 1
    addImage( H3_Img::Chip4);    // 2
    addImage( H3_Img::Chip5);    // 3
    addImage( H3_Img::Chip6);    // 4
    addImage( H3_Img::Chip8);    // 5
    addImage( H3_Img::Chip9);    // 6
    addImage( H3_Img::Chip10);   // 7
    addImage( H3_Img::Chip11);   // 8
    addImage( H3_Img::Chip12);   // 9
    addImage( H3_Img::ChipS);    // 10
    addImage( H3_Img::ChipW);    // 11
    addImage( H3_Img::Chip3v1);  // 12
    addImage( H3_Img::ChipA);    // 13
    addImage( H3_Img::ChipB);    // 14
    addImage( H3_Img::ChipC);    // 15
    addImage( H3_Img::ChipD);    // 16
    addImage( H3_Img::ChipE);    // 17

    std::vector<const de::Image*> pointers;
    pointers.reserve( images.size() );

    for (const de::Image& img : images)
    {
        pointers.emplace_back( &img );
    }

    de::gpu::SO so( 1.0f,
        de::gpu::SO::Minify::LinearMipmapLinear,
        de::gpu::SO::Magnify::Linear,
        de::gpu::SO::Wrap::ClampToEdge,
        de::gpu::SO::Wrap::ClampToEdge);
    bool ok = ct.m_tex.create("chips_tex2darr", pointers, so );
    if (!ok)
    {
        DE_ERROR("Failed.")
    }
    return ok;
}

void H3_ChipTexture_destroy( H3_ChipTexture & out )
{
    out.m_tex.destroy();
}

