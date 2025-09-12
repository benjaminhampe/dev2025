#include <fn_copyImageRect.hpp>
#include <fn_genImageNormalMap.hpp>
#include <de/os/CommonDialogs.h>
#include <de/os/VideoModes.h>

using namespace de;

int main(int argc, char** argv )
{
    int x = 0;
    int y = 0;
    int w = 0;
    int h = 0;
    std::string loadUri;
    std::string saveUri;

    if ( argc < 3 )
    {
        DE_DEBUG("[cutImage] Need atleast 2 user params: x y <w> <h> <in-file> <out-file>")
        return 0;
    }

    x = atol( argv[ 1 ] );
    y = atol( argv[ 2 ] );

    if ( argc > 3 ) { w = atol( argv[ 3 ] ); }
    if ( argc > 4 ) { h = atol( argv[ 4 ] ); }

    DE_DEBUG("[cutImage] Got x = ",x)
    DE_DEBUG("[cutImage] Got y = ",y)
    DE_DEBUG("[cutImage] Got w = ",w)
    DE_DEBUG("[cutImage] Got h = ",h)

    if ( argc > 5 ) { loadUri = argv[ 5 ]; }

    if ( !de::FileSystem::existFile( loadUri ) )
    {
        OpenFileParamsA params;
        params.caption = "Load image file (bmp,xpm,png,tga,jpg,tif,gif,dds,etc..)";
        loadUri = dbOpenFileDlg( params );
    }

    if ( !de::FileSystem::existFile( loadUri ) )
    {
        DE_DEBUG("[cutImage] No loadUri ", loadUri )
        return 0;
    }

    DE_DEBUG("[cutImage] Got loadUri = ", loadUri )

    if ( argc > 6 ) { saveUri = argv[ 6 ]; }

    if ( saveUri.empty() )
    {
        SaveFileParamsA params;
        params.caption = "Save image (bmp,png,jpg,gif,tga,dds,tif)";
        params.x = 200;
        params.y = 200;
        params.w = dbDesktopWidth() - 400;
        params.h = dbDesktopHeight() - 400;
        saveUri = dbSaveFileDlg( params );
    }

    DE_DEBUG("[cutImage] Got saveUri = ",saveUri)

    de::PerformanceTimer perf;

    perf.start();
    de::Image img;
    bool ok = dbLoadImage( img, loadUri );
    perf.stop();

    DE_DEBUG("[cutImage] LoadImage needed ",perf.ms()," ms, loadUri = ",loadUri)

    if ( !ok )
    {
        DE_ERROR("[cutImage] Cant load image. Nothing todo.")
        return 0;
    }

    perf.start();
    Image subImage = copyImageRect( img, Recti(x,y,w,h) );
    perf.stop();

    DE_DEBUG("[cutImage] CopyImage needed ",perf.us()," us.")

    perf.start();
    ok = dbSaveImage( subImage, saveUri );
    perf.stop();

    DE_DEBUG("[cutImage] SaveImage needed ",perf.ms()," ms, saveUri = ",saveUri)

    if ( !ok )
    {
        DE_ERROR("[cutImage] Cant save image.")
    }
    return 0;
}
