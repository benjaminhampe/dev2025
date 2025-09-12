#include <DarkImage.h>
#include <de/os/CommonDialogs.h>
#include <de/os/VideoModes.h>

namespace de {

struct convertImage
{
    static int
    run( int argc, char** argv )
    {
        std::wstring loadUri;

        if ( argc > 1 )
        {
            loadUri = de::StringUtil::to_wstr( std::string( argv[ 1 ] ) );
        }

        if ( !de::FileSystem::existFile( loadUri ) )
        {
            OpenFileParamsW params;
            params.caption = L"Load/open any image file (bmp,xpm,png,tga,jpg,tif,gif,dds,etc..)";
            loadUri = dbOpenFileDlg( params );
        }

        if ( !de::FileSystem::existFile( loadUri ) )
        {
            DE_ERROR("Nothing todo")
            return 0;
        }

        std::wstring saveUri;

        if ( argc > 2 )
        {
            saveUri = de::StringUtil::to_wstr( std::string( argv[ 2 ] ) );
        }

        if ( saveUri.empty() )
        {
            SaveFileParamsW params;
            params.caption = L"Save image (bmp,png,jpg,gif,tga,dds,tif)";
            params.x = 200;
            params.y = 200;
            params.w = dbDesktopWidth() - 400;
            params.h = dbDesktopHeight() - 400;
            saveUri = dbSaveFileDlg( params );
        }

        DE_DEBUG("Got saveUri = ", de::StringUtil::to_str(saveUri))

        //DE_DEBUG("[LoadMP3] loadUri = ", loadUri )
        //Buffer workBuffer;

        de::PerformanceTimer perf;
        perf.start();

        de::Image img;
        bool ok = dbLoadImage( img, de::StringUtil::to_str(loadUri) );
        perf.stop();
        DE_DEBUG("[LoadImage] needed ", perf.ms(), " ms, uri = ", de::StringUtil::to_str(loadUri) )

        if ( ok )
        {
            perf.start();
            ok = dbSaveImage( img, de::StringUtil::to_str( saveUri ) );
            perf.stop();
            DE_DEBUG("[SaveImage] needed ", perf.ms(), " ms, uri = ", de::StringUtil::to_str(saveUri) )

            if ( !ok )
            {
                DE_ERROR("Cant save image ",de::StringUtil::to_str(saveUri))
            }
        }
        else
        {
            DE_ERROR("Cant load image ",de::StringUtil::to_str(loadUri))
        }

        DE_DEBUG("[ConvertImage] program end." )
        return ok;
    }
};

} // end namespace de

//========================================================================
int main(int argc, char** argv)
//========================================================================
{
   //bool ok =
   de::convertImage::run( argc, argv );
   return 0;
}

