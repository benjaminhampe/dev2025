//////////////////////////////////////////////////////////////////////////////
/// @file main_MidiParser.cpp
/// @author Benjamin Hampe <benjamin.hampe@gmx.de>
//////////////////////////////////////////////////////////////////////////////

#include "DebugHtml.h"

// #include <de/midi/DurationComputer.h>
// #include <de/midi/MidiFileListener.h>


#ifdef _WIN32
    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #endif
    #include <windows.h>
    #include <shellapi.h>

    void open_in_browser(const std::string& path)
    {
        // Add Quotation marks for uris with spaces.
        // std::wostringstream o;
        // o << "\"" << saveUri << "\"";
        // _wsystem( o.str().c_str() );
        std::wstring w = de_wstr(path);
        ShellExecuteW(nullptr, L"open", w.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
    }
#else
    #include <cstdlib>
    #include <string>

    void open_in_browser(const std::string& path)
    {
        std::string cmd = "xdg-open \"" + path + "\"";
        std::system(cmd.c_str());
    }
#endif

//========================================================================
int main( int argc, char** argv )
//========================================================================
{
    DE_DEBUG( argv[ 0 ], " by (c) 2022 by <benjaminhampe@gmx.de>" )

    std::wstring loadUri;
    // if ( argc > 1 )
    // {
    //     loadUri = de_argv[1];
    // }

    // (c) 2022 by <benjaminhampe@gmx.de>
    // Choose midi file...
    if ( !de::FileSystem::existFile( loadUri ) )
    {
        de::OpenFileParamsW params;
        params.caption = L"MidiParser expects a midi file (.mid, .midi, .smf ) | (c) 2022 by <benjaminhampe@gmx.de>";
        loadUri = dbOpenFileDlg( params );
    }

    if ( !de::FileSystem::existFile( loadUri ) )
    {
        DE_ERROR("No loadUri")
        return 0;
    }

    // (c) 2022 by <benjaminhampe@gmx.de>
    // If no command 3rd command line arg for saveUri was found it
    // chooses to save mid.html along the .mid file in same directory.
    std::wstring saveUri = loadUri + L".html";
    // if ( argc > 2 )
    // {
    //     saveUri = argv[ 2 ];
    // }

    // (c) 2022 by <benjaminhampe@gmx.de>
    // Validate dialog result.
    // if ( !de::midi::convertMidi2Html::convert( loadUri, saveUri ) )
    // {
    // std::cout << "Cant convert midi("<<loadUri<<") to html("<<saveUri<<"), program exits now."<<std::endl;
    // return 0;
    // }

    // if ( saveUri.empty() )
    // {
    //     DE_ERROR("No saveUri")
    //     saveUri = loadUri + ".html";
    //     // return false;
    // }

    DE_DEBUG("MidiParser loadUri = ", de_mbstr(loadUri))
    DE_DEBUG("HtmlWriter saveUri = ", de_mbstr(saveUri))

    // Parser:
    de::file::mp4::MP4 mp4File;
    if ( !de::file::mp4::Parser::parse( de_mbstr(loadUri), mp4File ) )
    {
        DE_ERROR("Parser had error, ", de_mbstr(loadUri))
    }

    // Add listener 0: DurationComputer
    //de::DurationComputer durationComputer;
    //parser.addListener( &durationComputer );

    // Add listener 1: Html writer
    // DebugHtml html;
    // parser.addListener( &html );

    // Add listener 2: Standard Midi file listener
    // de::midi::file::File file;
    // de::midi::file::Listener fileListener;
    // fileListener.setFile( &file );
    // parser.addListener( &fileListener );

    // if ( !parser.parse( de_mbstr(loadUri) ) )
    // {
    //     DE_ERROR("Parser had error, ", de_mbstr(loadUri))
    // }

    // Write html ( most time intensive part )
    //html.save( de_mbstr(saveUri) );

    //DE_DEBUG("Open html file ",de_mbstr(saveUri))
    //open_in_browser(de_mbstr(saveUri));

    return 0;
}
