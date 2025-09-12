#pragma once
#include <de/image/Image.h>
#include <de/Font5x8.h>

namespace de {
    
struct ImageSkyboxRepair
{
    static u32
    getEdgeColor()
    {
        constexpr std::array< u32, 12 > edgeColors{
            dbRGBA(255,0,0), dbRGBA(0,255,0), dbRGBA(0,0,255), dbRGBA(255,255,0),
            dbRGBA(255,0,255), dbRGBA(0,255,255), dbRGBA(255,128,0), dbRGBA(55,200,55),
            dbRGBA(155,100,55), dbRGBA(155,0,0), dbRGBA(0,155,255), dbRGBA(0,255,155)
        };

        static u32 counter = 0;

        const u32 color = edgeColors[ counter ];

        counter++;

        if (counter >= edgeColors.size())
        {
            counter = 0;
        }

        return color;
    }


    static void drawEdge( Image & img, int edge, u32 color )
    {
        if (edge == 0)
        {
            for (int y = 0; y < 5; ++y)
            {
                for (int x = 0; x < img.w(); ++x)
                {
                    img.setPixel(x,y, color);
                }
            }
        }
        else if (edge == 1)
        {
            for (int y = 0; y < img.h(); ++y)
            {
                for (int x = 0; x < 5; ++x)
                {
                    img.setPixel(x, y, color);
                }
            }
        }
        else if (edge == 2)
        {
            for (int y = 0; y < 5; ++y)
            {
                for (int x = 0; x < img.w(); ++x)
                {
                    img.setPixel(x, img.h() - 1 - y, color);
                }
            }
        }
        else if (edge == 3)
        {
            for (int y = 0; y < img.h(); ++y)
            {
                for (int x = 0; x < 5; ++x)
                {
                    img.setPixel( img.w() - 1 - x, y, color);
                }
            }
        }
    }

    static std::vector<u32> getEdgePixels( const Image & img, int edge )
    {
        std::vector<u32> pixels;

        if (edge == 0)
        {
            pixels.reserve(img.w());
            for (int x = 0; x < img.w(); ++x)
            {
                pixels.emplace_back(img.getPixel(x,0));
            }
        }
        else if (edge == 1)
        {
            pixels.reserve(img.h());
            for (int y = 0; y < img.h(); ++y)
            {
                pixels.emplace_back(img.getPixel(0,y));
            }
        }
        else if (edge == 2)
        {
            pixels.reserve(img.w());
            for (int x = 0; x < img.w(); ++x)
            {
                pixels.emplace_back(img.getPixel(x,img.h()-1));
            }
        }
        else if (edge == 3)
        {
            pixels.reserve(img.h());
            for (int y = 0; y < img.h(); ++y)
            {
                pixels.emplace_back(img.getPixel(img.w()-1,y));
            }
        }

        return pixels;
    }

    static u32 compareEdges( const std::vector<u32>& e1, const std::vector<u32>& e2 )
    {
        if (e1.size() != e2.size())
        {
            DE_ERROR("Edge sizes differ")
            return 0xFFFFFFFF;
        }

        // Foreward compare edges:
        u32 delta1 = 0;
        for (size_t i = 0; i < e1.size(); ++i )
        {
            int r1 = dbRGBA_R(e1[i]);
            int g1 = dbRGBA_G(e1[i]);
            int b1 = dbRGBA_B(e1[i]);

            int r2 = dbRGBA_R(e2[i]);
            int g2 = dbRGBA_G(e2[i]);
            int b2 = dbRGBA_B(e2[i]);

            delta1 += std::abs(r1-r2);
            delta1 += std::abs(g1-g2);
            delta1 += std::abs(b1-b2);
        }

        // Backward compare edges:
        u32 delta2 = 0;
        for (size_t i = 0; i < e1.size(); ++i )
        {
            int r1 = dbRGBA_R(e1[i]);
            int g1 = dbRGBA_G(e1[i]);
            int b1 = dbRGBA_B(e1[i]);

            int r2 = dbRGBA_R(e2[e2.size() - 1 - i]);
            int g2 = dbRGBA_G(e2[e2.size() - 1 - i]);
            int b2 = dbRGBA_B(e2[e2.size() - 1 - i]);

            delta2 += std::abs(r1-r2);
            delta2 += std::abs(g1-g2);
            delta2 += std::abs(b1-b2);
        }

        return std::min(delta1,delta2);
    }

    static void
    drawDebugText( Image & img, std::string uri )
    {
        Font5x8 font(4,4,1,1,1,1);
        std::string msg = FileSystem::fileName(uri);
        font.drawText( img, img.w()/2, img.h()/2, msg, 0xFFFFFFFF, Align::Centered);
    }

    static void
    repair( std::string saveDir,
            std::string nxUri, std::string pxUri,  
            std::string nyUri, std::string pyUri,  
            std::string nzUri, std::string pzUri )
    {
        Image nx;
        Image ny;
        Image nz;
        Image px;
        Image py;
        Image pz;
        
        ImageLoadOptions opts;
        opts.outputFormat = PixelFormat::R8G8B8;
        
        dbLoadImage(nx, nxUri, opts);
        dbLoadImage(ny, nyUri, opts);
        dbLoadImage(nz, nzUri, opts);
        dbLoadImage(px, pxUri, opts);
        dbLoadImage(py, pyUri, opts);
        dbLoadImage(pz, pzUri, opts);
        
        drawDebugText( nx, nxUri );
        drawDebugText( ny, nyUri );
        drawDebugText( nz, nzUri );
        drawDebugText( px, pxUri );
        drawDebugText( py, pyUri );
        drawDebugText( pz, pzUri );

        std::array< Image*, 6 > imgs { &nx, &px, &ny, &py, &nz, &pz };
        
        // Loop all images
        for (int i : std::vector<int>{0,3})
        {
            Image & img1 = *imgs[i];

            // Loop all images
            for (int e = 0; e < 4; ++e)
            {
                const std::vector<u32> e1 = getEdgePixels( img1, e );

                u32 delta_min = 0xFFFFFFFF;
                int found_img_index = -1;
                int found_edge_index = -1;

                for (int k = 0; k < 6; ++k)
                {
                    if ( k == i ) { continue; } // Don't compare with itself

                    Image & img2 = *imgs[k];
                    for (int f = 0; f < 4; ++f)
                    {
                        const std::vector<u32> e2 = getEdgePixels( img2, f );

                        u32 delta = compareEdges(e1,e2);
                        if (delta_min > delta)
                        {
                            delta_min = delta;
                            found_img_index = k;
                            found_edge_index = f;
                        }
                    }
                }

                if ( found_img_index > -1 )
                {
                    u32 edgeColor = getEdgeColor();
                    drawEdge( *imgs[found_img_index], found_edge_index, edgeColor);
                    drawEdge( *imgs[i], e, edgeColor);
                }
            }
        }

        dbSaveImage(nx,dbStr(saveDir,"/", FileSystem::fileBase(nxUri),".webp"));
        dbSaveImage(px,dbStr(saveDir,"/", FileSystem::fileBase(pxUri),".webp"));
        dbSaveImage(ny,dbStr(saveDir,"/", FileSystem::fileBase(nyUri),".webp"));
        dbSaveImage(py,dbStr(saveDir,"/", FileSystem::fileBase(pyUri),".webp"));
        dbSaveImage(nz,dbStr(saveDir,"/", FileSystem::fileBase(nzUri),".webp"));
        dbSaveImage(pz,dbStr(saveDir,"/", FileSystem::fileBase(pzUri),".webp"));
    }
                
};

} // end namespace de
