#ifndef UNICODE

#include <de/audio/GetCoverArt.h>

#include <de/Core.h>
#include <de/os/CommonDialogs.h>
// #include <de/audio/buffer/BufferIO.hpp>

namespace de {
namespace audio {

std::string
repairString( std::wstring loadUri )
{
   //std::wstring w = dbStrW( loadUri );
   //std::wcout << "LoadUri = " << w << L'\n';

   std::stringstream s;
   for ( size_t i = 0; i < loadUri.size(); ++i )
   {
      uint32_t c = loadUri[ i ];
      if ( c <= 30 ) c = '_';
      if ( c > 'z' ) c = '_';
      s << char( c );
   }

   std::string r = s.str();
   DE_OK("RepairedUri = ",r)
   return r;

}

struct CoverArtExtractor
{
    static std::wstring getLoadFileName()
    {
        de::OpenFileParamsW loadP;
        loadP.caption = L"Load file (mp3,wav,mkv,avi,mp4,etc..) that is supported by libAVCodec ( vlc )";
        loadP.x = 0;
        loadP.y = 0;
        loadP.w = 800;
        loadP.h = 600;
        loadP.newui = true;
        loadP.initDir = L"../../";
        loadP.initFileName = L"";

        return dbOpenFileDlg(loadP);
    }

    static std::wstring getSaveFileName()
    {
        de::SaveFileParamsW saveP;
        saveP.caption = L"Save file (png,webp,jpg,bmp,xpm,exr,tga,tif,gif,dds,etc..) that is supported by libAVCodec ( vlc )";
        saveP.x = 0;
        saveP.y = 0;
        saveP.w = 800;
        saveP.h = 600;
        saveP.newui = true;
        saveP.initDir = L"../../";
        saveP.initFileName = L"";

        return dbSaveFileDlg(saveP);
    }

    static bool extractCoverArt()
    {
        std::wstring loadUri = getLoadFileName();

        CoverArt coverArt;
        CoverArt :: getCoverArt( coverArt, loadUri );

        if (coverArt.img.empty())
        {
            DE_ERROR("No CoverArt in ",de_mbstr(loadUri))
            return false;
        }

        std::wstring saveUri = getSaveFileName();
        if (saveUri.empty())
        {
            DE_WARN("Empty save uri, abort program! ",de_mbstr(loadUri) )
            return false;
        }

        DE_DEBUG("Extract: ",de_mbstr(loadUri) )
        DE_DEBUG("CoverArt: ",de_mbstr(saveUri),", img:", coverArt.img.str() )
        dbSaveImage( coverArt.img, de_mbstr(saveUri) );
        return true;
   }

};

} // end namespace audio
} // end namespace de


//========================================================================
int main(int argc, char** argv)
//========================================================================
{
   de::audio::CoverArtExtractor::extractCoverArt();
   return 0;
}

#endif
