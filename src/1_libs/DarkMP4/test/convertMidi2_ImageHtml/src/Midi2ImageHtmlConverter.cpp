#include "Midi2ImageHtmlConverter.hpp"

namespace de {
namespace midi {

using namespace ::de::midi::file;

// static
bool
ImageHtml::convert( std::string saveUri, file::File const & file )
{
    /*
    std::vector< std::string > imageFiles;
    for ( size_t i = 0; i < file.m_tracks.size(); ++i )
    {
    Image img = createTrackImage( file, int(i), w, h );
    std::string imageUri = saveUri + std::to_string(i) + ".png";
    if ( dbSaveImage( img, imageUri ) )
    {
    imageFiles.emplace_back( imageUri );
    }
    }
    */
    //FileSystem::saveText( saveUri, sheetParser.m_sheet.toString() );

    std::ostringstream o;

    o << "<html>\n"
    "<title>Midi2Html: " << saveUri << "</title>\n"
    "<style type=\"text/css\">\n"
    "*    { margin:0; border:0; padding:0; }\n"
    "body { width:100%; height:100%; color:rgb(0,0,0); background-color:rgb(255,255,255); font-family:monospace; font-size:16px; }\n"
    //"caption { font-weight:bold; font-size:24px; line-height:50px; vertical-align:middle;}\n"
    "content { background-color:white; color:black; position:absolute; top:50px; padding-left:20px; padding-top:20px; white-space:nowrap; z-index:1;}\n"
    // "img { border:5px; border-color:black; padding:5px; margin:5px;}\n"
    // "div12 { width:100%; height:12px; line-height:12px; display-style:inline; }\n"
    // "h1 { color:white; background-color:rgb(0   ,0   ,0  ); font-size:64px; vertical-align:middle; font-weight:900; }\n"
    // "h2 { color:white; background-color:rgb(127 ,192 ,255); font-size:32px; vertical-align:middle; font-weight:750; }\n"
    // "h3 { color:white; background-color:rgb(192 ,200 ,255); font-size:24px; vertical-align:middle; font-weight:600; }\n"
    // "h4 { color:white; background-color:rgb(200 ,200 ,255); font-size:24px; vertical-align:middle; font-weight:600; }\n"
    // "h5 { color:black; background-color:rgb(255 ,255 ,255); font-size:16px; vertical-align:middle; font-weight:400; }\n"
    // "h6 { color:white; background-color:rgb(64  ,64  ,192); font-size:12px; vertical-align:middle; font-weight:400; }\n"
    // "hr { color:black; background-color:rgb(255 ,255 ,0); height:10px; line-height:5px; vertical-align:middle; }\n"
    // "date { color:#FF00FF; font-weight:bold; margin-left:20px;}\n"
    // "line { color:#207F20; font-weight:bold; margin-left:20px;}\n"
    // "time { color:#4040DF; font-weight:bold; margin-left:20px;}\n"
    // "trace {color:white; background-color:#00AF00; margin-left:20px;}\n"
    // "debug {color:white; background-color:#FF00FF; margin-left:20px;}\n"
    // "info { margin-left:20px; }\n"
    // "warn {color:black; background-color:yellow; margin-left:20px;}\n"
    // "error {color:white; background-color:red; margin-left:20px;}\n"
    //"pre { font-family:Courier New; font-size:12pt; }"
    "</style>\n</head>\n"
    "<body>\n"

    // ---------------------- title (=loadUri) ----------------------------------
    "<panel style=\"background-color:blue; position:fixed; top:0px; left:0px; width:100%; height:50px; z-index:2;\">\n"
    "<table width=\"100%\" height=\"100%\">\n"
    "<tr valign=\"middle\">"
    "<td align=\"center\">"
    "<caption style=\"color:white; font-weight:bold; font-size:16px; line-height:50px; vertical-align:middle;\">"
    << FileSystem::fileName(file.m_fileName)
    << ", Duration(" << StringUtil::seconds( file.getDurationInSec() ) << ")"
    << "</caption></td>\n"
    "</tr>\n"
    "</table>\n"
    "</panel>\n";

    // ---------------------- content start ----------------------------------
    o << "<content>\n";

    // ---------------------- print file Header ----------------------------------

    o << "<pre>"
    "FileName = " << file.m_fileName << "\n"
    "FileType = " << file.m_fileType << "\n"
    "TrackCount = " << file.m_trackCount << "\n"
    "TicksPerQuarterNote = " << file.m_ticksPerQuarterNote << "\n"
    "ParsedTracks = " << file.m_tracks.size() << "\n"
    "Duration = " << StringUtil::seconds( file.getDurationInSec() ) << "\n"
    "MemoryConsumption = " << StringUtil::bytes( file.computeMemoryConsumption() ) << "\n";

    for ( size_t i = 0; i < file.m_tracks.size(); ++i )
    {
        Track const & track = file.m_tracks[ i ];
        std::string trackName = track.name();
        if ( trackName.empty() )
        {
            trackName = "UnnamedMasterTrack";
        }

        o << "Track[" << i << "] Name(" << trackName << ")\n";

        if ( track.m_setTempoEvents.size() > 0 )
        {
            o << "\tSetTempoEvents = " << track.m_setTempoEvents.size() << "\n";
        }
        if ( track.m_timeSignatureEvents.size() > 0 )
        {
            o << "\tTimeSignatureEvents = " << track.m_timeSignatureEvents.size() << "\n";
        }
        if ( track.m_keySignatureEvents.size() > 0 )
        {
            o << "\tKeySignatureEvents = " << track.m_keySignatureEvents.size() << "\n";
        }
        if ( track.m_polyphonicAftertouch.m_events.size() > 0 )
        {
            o << "\tPolyphonicAftertouchEvents = " << track.m_polyphonicAftertouch.m_events.size() << "\n";
        }

        //      for ( size_t c = 0; c < track.m_setTempoEvents.size(); ++c )
        //      {
        //         SetTempoEvent const & e = track.m_setTempoEvents[ c ];
        //         o <<
        //         "\tSetTempo[" << c << "] " << e.toString() << "\n";
        //      }

        for ( size_t c = 0; c < track.m_channels.size(); ++c )
        {
            Channel const & ch = track.m_channels[ c ];
            o << "\t"
            "Channel[" << ch.m_channelIndex << "] has " << ch.m_notes.size() << " notes" << "\n";
        }

    }
    o << "</pre>\n";

    // ---------------------- Loop tracks ----------------------------------

    for ( size_t t = 0; t < file.m_tracks.size(); ++t )
    {
        o << "<br>";
        Track const & track = file.m_tracks[ t ];
        /*
        o << "<panel style=\"background-color:rgb(100,100,255); left:0px; width:100%; height:50px; z-index:2;\">\n"
        "<table width=\"100%\" height=\"100%\">\n"
        "<tr valign=\"middle\">"
        "<td align=\"center\">"
        "<caption style=\"color:white; font-weight:bold; font-size:16px; line-height:50px; vertical-align:middle;\">"
        << "Track[" << track.m_trackIndex << "/" << file.m_trackCount << "] "<< track.m_trackName << "</caption></td>\n"
        "</tr>\n"
        "</table>\n"
        "</panel>\n";
        */
        o << "<h3>Track[" << track.m_trackIndex+1 << "/" << file.m_trackCount << "] Name("<< track.m_trackName << ")</h3>\n";

        // Write SetTempo events:
        //      if ( track.m_setTempoEvents.size() )
        //      {
        //         o << "<h4>SetTempo events = " << track.m_setTempoEvents.size() << "</h4>\n";
        //         o << "<pre>";
        //         for ( size_t i = 0; i < track.m_setTempoEvents.size(); ++i )
        //         {
        //            if ( i > 0 ) o << "\n";
        //            o << track.m_setTempoEvents[i].toString();
        //         }
        //         o << "</pre><br>\n";
        //      }

        //      // Write TimeSignature events:
        //      if ( track.m_timeSignatureEvents.size() )
        //      {
        //         o << "<h4>TimeSignature events = " << track.m_timeSignatureEvents.size() << "</h4>\n";
        //         o << "<pre>";
        //         for ( size_t i = 0; i < track.m_timeSignatureEvents.size(); ++i )
        //         {
        //            if ( i > 0 ) o << "\n";
        //            o << track.m_timeSignatureEvents[i].toString();
        //         }
        //         o << "</pre><br>\n";
        //      }

        //      // Write KeySignature events:
        //      if ( track.m_keySignatureEvents.size() )
        //      {
        //         o << "<h4>KeySignature events = " << track.m_keySignatureEvents.size() << "</h4>\n";
        //         o << "<pre>";
        //         for ( size_t i = 0; i < track.m_keySignatureEvents.size(); ++i )
        //         {
        //            if ( i > 0 ) o << "\n";
        //            o << track.m_keySignatureEvents[i].toString();
        //         }
        //         o << "</pre><br>\n";
        //      }

        // Write MetaText events:
        if ( track.m_textEvents.size() )
        {
            o << "<h4>MetaText events = " << track.m_textEvents.size() << "</h4>\n";
            o << "<pre>";
            for ( size_t i = 0; i < track.m_textEvents.size(); ++i )
            {
                if ( i > 0 ) o << "\n";
                o << track.m_textEvents[i].toString();
            }
            o << "</pre><br>\n";
        }

        // Write Lyric events:
        if ( track.m_lyricEvents.size() )
        {
            o << "<h4>MetaLyric events = " << track.m_lyricEvents.size() << "</h4>\n";
            o << "<pre>";
            for ( size_t i = 0; i < track.m_lyricEvents.size(); ++i )
            {
                //if ( i > 0 ) o << " ";
                o << track.m_lyricEvents[i].m_text;
            }
            o << "</pre><br>\n";
        }

        //      // Write PolyphonicAftertouch events:
        //      if ( track.m_polyphonicAftertouch.m_events.size() )
        //      {
        //         ControlChangeEventMap const & cc = track.m_polyphonicAftertouch;
        //         o << "<h4>PolyphonicAftertouch events = " << cc.m_events.size() << "</h4>\n";
        //         Image img = createCCImage( file, cc );
        //         std::string imageUri = StringUtil::join(saveUri,"_track",t,"_pat.png");
        //         dbSaveImage( img, imageUri );
        //         o << "<img src=\"./"<< FileSystem::fileName( imageUri ) << "\" "
        //            << "width=\""<< img.w() << "\" "<< "height=\""<< img.h() << "\" "
        //            "/>\n";
        //      }

        // Write channels:
        if ( track.m_channels.size() )
        {
            for ( size_t c = 0; c < track.m_channels.size(); ++c )
            {
                Channel const & channel = track.m_channels[ c ];
                o << "<h3>"
                "Track[" << track.m_trackIndex+1 << "/" << file.m_trackCount << "] "
                "Channel[" << channel.m_channelIndex << "] " << GM_toString(channel.m_instrument) << ", "
                "Notes(" << channel.m_notes.size() << "), "
                "Controller(" << channel.m_controller.size() << ")"
                "</h3>\n";

                // Write simple editor notes: ( kinda wasteful in space, but easy to read )
                if ( channel.m_notes.size() )
                {
                    Image img = ImageHtml::createNoteImage( file, int(t), int(c) );
                    std::string imageUri = StringUtil::join(saveUri,"_track",t,"_ch",c,".png");
                    dbSaveImage( img, imageUri );
                    o << "<img src=\"./"<< FileSystem::fileName( imageUri ) << "\" "
                    << "width=\""<< img.w() << "\" "<< "height=\""<< img.h() << "\" "
                    "/>\n";
                }

                // Write ChannelAftertouch events:
                if ( channel.m_channelAftertouch.m_events.size() )
                {
                    ControlChangeEventMap const & cc = channel.m_channelAftertouch;
                    o << "<h4>ChannelAftertouch events = " << cc.m_events.size() << "</h4>\n";

                    if ( cc.m_events.size() > 4 )
                    {
                        Image img = ImageHtml::createCCImage( file, cc );
                        std::string imageUri = StringUtil::join(saveUri,"_track",t,"_ch",c,"_cat.png");
                        dbSaveImage( img, imageUri );
                        o << "<img src=\"./"<< FileSystem::fileName( imageUri ) << "\" "
                        << "width=\""<< img.w() << "\" "<< "height=\""<< img.h() << "\" "
                        "/>\n";
                    }
                    else
                    {
                        o << "<pre>";
                        for ( size_t i = 0; i < cc.m_events.size(); ++i )
                        {
                            if ( i > 0 ) o << "\n";
                            o << cc.m_events[i].toString();
                        }
                        o << "</pre>\n";
                    }
                }

                // Write PitchBend events:
                if ( channel.m_pitchBend.m_events.size() )
                {
                    ControlChangeEventMap const & cc = channel.m_pitchBend;
                    o << "<h4>PitchBend events = " << cc.m_events.size() << "</h4>\n";

                    if ( cc.m_events.size() > 2 )
                    {
                        Image img = ImageHtml::createPitchBendImage( file, cc );
                        std::string imageUri = StringUtil::join(saveUri,"_track",t,"_ch",c,"_pitchbend.png");
                        dbSaveImage( img, imageUri );
                        o << "<img src=\"./"<< FileSystem::fileName( imageUri ) << "\" "
                        << "width=\""<< img.w() << "\" "<< "height=\""<< img.h() << "\" />\n";
                    }
                    else
                    {
                        o << "<pre>";
                        for ( size_t i = 0; i < cc.m_events.size(); ++i )
                        {
                            if ( i > 0 ) o << "\n";
                            o << cc.m_events[i].toString();
                        }
                        o << "</pre>\n";
                    }
                }

                // Write CC events:
                if ( channel.m_controller.size() )
                {
                    for ( size_t j = 0; j < channel.m_controller.size(); ++j )
                    {
                        ControlChangeEventMap const & cc = channel.m_controller[ j ];

                        o << "<h4>"
                        "Track[" << track.m_trackIndex+1 << "/" << file.m_trackCount << "] "
                        "Channel[" << channel.m_channelIndex << "] "
                        "Controller["<<j+1<<"/"<<channel.m_controller.size()<<"] "
                        << CC_toString(cc.m_cc)<<", events("<<cc.m_events.size()<<")</h4>\n";

                        if ( cc.m_events.size() > 4 )
                        {
                            Image img = ImageHtml::createCCImage( file, int(t), int(c), int(j) );
                            std::string imageUri = StringUtil::join(saveUri,"_track",t,"_ch",c,"_cc",j,".png");
                            dbSaveImage( img, imageUri );
                            o << "<img src=\"./"<< FileSystem::fileName( imageUri ) << "\" "
                            "width=\""<< img.w() << "\" "<< "height=\""<< img.h() << "\" />\n";
                        }
                        else if ( cc.m_events.size() > 0 )
                        {
                            o << "<pre>";
                            for ( size_t i = 0; i < cc.m_events.size(); ++i )
                            {
                                if ( i > 0 ) o << "\n";
                                o << cc.m_events[0].toString();
                            }
                            o << "</pre>\n";
                        }
                    }
                }
            }
        //o << "<br>";
        }
    //o << "<br>";
    }

    //o << "<br>";


    // ---------------------- image table ----------------------------------
    /*
    o <<
    "<table cellspacing=\"5\" cellpadding=\"5\">\n";

    for ( size_t i = 0; i < imageFiles.size(); ++i )
    {
    std::string uri = imageFiles[i];

    o << "<tr><td><img src=\"./"<< FileSystem::fileName( uri ) << "\" /></td></tr>\n";
    }

    o <<
    "</table>\n"
    */


    // ---------------------- content end ----------------------------------

    o << "<br><br><br><br><br></content>\n"

    // ---------------------------- footer ----------------------------------

    "<footer style=\"background-color:blue; color:white; position:fixed; bottom:0px; left:0px; width:100%; height:50px; line-height:50px; z-index:3;\">\n"
    "<table width=\"100%\" height=\"100%\">\n"
    "<tr valign=\"middle\"><td align=\"center\"><h3 style=\"color:white;\">File produced by class Midi2ImageHtml</h3></td></tr>\n"
    "<tr valign=\"middle\"><td align=\"center\"><h6 style=\"color:white;\">(c) 2023 by BenjaminHampe@gmx.de</h6></td></tr>\n"
    "</table>\n"
    "</footer>\n"
    "</body>\n"
    "</html>\n";

    std::string const htmlStr = o.str();

    if ( FileSystem::saveStr( saveUri, htmlStr ) )
    {
        DE_DEBUG("Saved HTML ", saveUri, " with ", htmlStr.size(), " bytes.")
    }
    else
    {
        DE_ERROR("Cant save HTML ", saveUri, " with ", htmlStr.size(), " bytes.")
    }

    return true;
}


// static
Image
ImageHtml::createPitchBendImage( file::File const & file, ControlChangeEventMap const & cc )
{
    //auto const valueRange = cc.getValueRange();
    auto const tickRange = cc.getTickRange();

    Font5x8 fontSmall(1,1,0,0,1,1);
    Font5x8 fontMedium(2,2,0,0,1,1);
    Font5x8 fontBig(4,4,1,1,2,2);

    double nQuarterNotes = double(tickRange.max) / double(file.m_ticksPerQuarterNote);

    int w_start = 32;
    int w_per_quarter = 32; // 8th = 16px, 16th = 8px, 32th = 4px;
    int w_content = int(w_per_quarter * nQuarterNotes);

    Image img(w_start + w_content + 10, 128 + 20);
    img.fill(0xFFDDFFFF);

    // Draw content rect
    Recti r_content( w_start, 10, img.w() - w_start - 10, img.h() - 20 );
    ImagePainter::drawRectBorder( img, r_content, 0xFF808080 );

    // Draw bars
    for ( int i = w_start; i < img.w()-10; i += w_per_quarter * 4 )
    {
        ImagePainter::drawLine( img, i, 10, i, img.h() - 1 - 10, 0xFF808080 );
    }

    // Draw controller events
    ControlChangeEvent def;
    def.m_tick = 0;
    def.m_value = (16384 / 2) - 1;
    for ( size_t i = 0; i < cc.m_events.size(); i++ )
    {
        ControlChangeEvent const & e1 = (i==0) ? def : cc.m_events[ i-1 ];
        ControlChangeEvent const & e2 = cc.m_events[ i ];
        int x1 = w_start + int(double(r_content.w) * double(e1.m_tick) / double(tickRange.max) );
        int x2 = w_start + int(double(r_content.w) * double(e2.m_tick) / double(tickRange.max) );
        int y1 = img.h() - 1 - 10 - int(double(e1.m_value) * 127.0/16383.0);
        int y2 = img.h() - 1 - 10 - int(double(e2.m_value) * 127.0/16383.0);
        ImagePainter::drawLine( img, x1, y1, x2, y2, 0xFF4040FF );
    }

    return img;
}

// static
Image
ImageHtml::createCCImage( file::File const & file, ControlChangeEventMap const & cc )
{
    //auto const valueRange = cc.getValueRange();
    auto const tickRange = cc.getTickRange();

    Font5x8 fontSmall(1,1,0,0,1,1);
    Font5x8 fontMedium(2,2,0,0,1,1);
    Font5x8 fontBig(4,4,1,1,2,2);

    double nQuarterNotes = double(tickRange.max) / double(file.m_ticksPerQuarterNote);

    int w_start = 32;
    int w_per_quarter = 32; // 8th = 16px, 16th = 8px, 32th = 4px;
    int w_content = int(w_per_quarter * nQuarterNotes);

    Image img(w_start + w_content + 10, 128 + 20);
    img.fill(0xFFDDFFFF);

    // Draw content rect
    Recti r_content( w_start, 10, img.w() - w_start - 10, img.h() - 20 );
    ImagePainter::drawRectBorder( img, r_content, 0xFF808080 );

    // Draw bars
    for ( int i = w_start; i < img.w()-10; i += w_per_quarter * 4 )
    {
        ImagePainter::drawLine( img, i, 10, i, img.h() - 1 - 10, 0xFF808080 );
    }

    // Draw controller events
    ControlChangeEvent def;
    def.m_tick = 0;
    def.m_value = 0;
    if ( cc.m_cc == CC_7_Volume )
    {
        def.m_value = 127;
    }

    for ( size_t i = 0; i < cc.m_events.size(); i++ )
    {
        ControlChangeEvent const & e1 = (i==0) ? def : cc.m_events[ i-1 ];
        ControlChangeEvent const & e2 = cc.m_events[ i ];
        int x1 = w_start + int(double(r_content.w) * double(e1.m_tick) / double(tickRange.max) );
        int x2 = w_start + int(double(r_content.w) * double(e2.m_tick) / double(tickRange.max) );
        int y1 = img.h() - 1 - 10 - e1.m_value;
        int y2 = img.h() - 1 - 10 - e2.m_value;
        ImagePainter::drawLine( img, x1, y1, x2, y2, 0xFF4040FF );
    }

return img;
}

// static
Image
ImageHtml::createCCImage( file::File const & file, int trackIndex, int channelIndex, int ccIndex )
{
    Track const & track = file.m_tracks[ trackIndex ];
    Channel const & channel = track.m_channels[ channelIndex ];
    ControlChangeEventMap const & cc = channel.m_controller[ ccIndex ];
    return createCCImage( file, cc );
}

// static
Image
ImageHtml::createNoteImage( file::File const & file, int trackIndex, int channelIndex )
{
    Track const & track = file.m_tracks[ trackIndex ];
    Channel const & channel = track.m_channels[ channelIndex ];
    auto const noteRange = channel.getNoteRange(); // y
    auto const tickRange = channel.getTickRange(); // x=t

    Font5x8 fontSmall(1,1,0,0,1,1);
    Font5x8 fontMedium(2,2,0,0,1,1);
    Font5x8 fontBig(4,4,1,1,2,2);

    int lineHeight = 11;

    double nQuarterNotes = double(tickRange.max) / double(file.m_ticksPerQuarterNote);

    int w_keyboard = 32;
    int w_per_quarter = 32; // 8th = 16px, 16th = 8px, 32th = 4px;
    int w_notes = int(w_per_quarter * nQuarterNotes);
    int h_notes = lineHeight * int(noteRange.getRange() + 3);

    Image img(w_notes + w_keyboard,h_notes);
    img.fill(0xFFDDFFFF);

    // Draw keyboard background
    for ( int i = 0; i <= noteRange.getRange(); i++ )
    {
        int midiNote = noteRange.min + i;
        bool isBlackKey = MidiUtil::isBlackPianoKey(midiNote);
        int y = img.h() - 1 - 2*lineHeight - (midiNote - noteRange.min) * lineHeight;
        Recti r_note(0,y,img.w(),lineHeight);
        ImagePainter::drawRect( img, r_note, isBlackKey ? 0xFFAAAAAA : 0xFFEEEEEE );
        ImagePainter::drawRectBorder( img, r_note, 0xFF888888 );
    }

    // Draw keyboard foreground
    for ( int i = 0; i <= noteRange.getRange(); i++ )
    {
        int midiNote = noteRange.min + i;
        int y = img.h() - 1 - 2*lineHeight - (midiNote - noteRange.min) * lineHeight;
        Recti r_note(0,y,32,lineHeight);

        ImagePainter::drawText5x8( img, r_note.centerX() + 1, r_note.centerY() + 1,
        std::to_string(midiNote), 0xFF000000, fontSmall, Align::Centered );
    }

    // Draw bars
    for ( int i = w_keyboard; i < img.w(); i += w_per_quarter * 4 )
    {
        ImagePainter::drawLine( img, i, lineHeight, i, img.h() - 1 - lineHeight, 0xFF808080 );
    }

    // Draw notes
    for ( size_t i = 0; i < channel.m_notes.size(); ++i )
    {
        NoteEvent const & note = channel.m_notes[ i ];
        std::string noteName = MidiUtil::getNoteStr(note.m_midiNote, false);
        uint32_t noteColor = MidiUtil::getNoteColor(note.m_midiNote);
        bool isBlack = MidiUtil::isBlackPianoKey(note.m_midiNote);

        int x = w_keyboard + int(double(w_notes) * double(note.m_attackMs) / double(tickRange.max) );
        int w = int(double(w_notes) * double(note.m_releaseMs - note.m_attackMs) / double(tickRange.max) );

        int y = img.h() - 1 - 2*lineHeight - (note.m_midiNote - noteRange.min) * lineHeight;
        int h = lineHeight;

        Recti r_note(x,y,w,h);
        if ( r_note.w < 8 )
        {
            r_note.setWidth(8);
        }
        ImagePainter::drawRect( img, r_note, noteColor );
        ImagePainter::drawRectBorder( img, r_note, isBlack ? 0xFFFFFFFF : 0xFF000000 );

        int cx = r_note.centerX() + 1;
        int cy = r_note.centerY() + 1;

        ImagePainter::drawText5x8( img, cx,cy, noteName, 0xFF000000, fontSmall, Align::Centered );
    }

    return img;
}

} // end namespace midi
} // end namespace de
