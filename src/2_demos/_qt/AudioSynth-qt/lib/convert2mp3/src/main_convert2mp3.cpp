#include <de/audio/decoder/DecoderFFMPEG.hpp>
#include <de/audio/encoder/EncoderLAME.hpp>
#include <de/Core.h>
#include <de/os/CommonDialogs.h>
#include <de/audio/buffer/BufferIO.hpp>

struct convert2mp3
{
    static int
    runW( int argc, char** argv )
    {
        std::wstring loadUri;
        //      if ( argc > 1 )
        //      {
        //         loadUri = argv[1];
        //      }

        if ( !de::FileSystem::existFile(loadUri) )
        {
            de::OpenFileParamsW params;
            params.caption = L"Load/open any file that contains an audio stream (mp3,wav,mkv,avi,mp4,etc..) that is supported by libAVCodec ( vlc )";
            loadUri = dbOpenFileDlg( params );
        }

        DE_DEBUG("Got loadUri = ", de::StringUtil::to_str(loadUri))

        if ( !de::FileSystem::existFile(loadUri) )
        {
            DE_ERROR("Empty loadUri or not a file, program exits now... Bye bye.")
            return 0;
        }

        std::wstring saveUri = loadUri + L".convert2mp3.mp3";
        //      if ( argc > 2 )
        //      {
        //         saveUri = argv[ 2 ];
        //      }
        //      saveUri = loadUri + ".convert2mp3.mp3";

        DE_DEBUG("Got saveUri = ", de::StringUtil::to_str(saveUri))

        //DE_DEBUG("[LoadMP3] loadUri = ", loadUri )
        //Buffer workBuffer;

        de::PerformanceTimer perf;
        perf.start();

        de::audio::decoder::DecoderCtx ds;
        de::audio::decoder::DecoderTools::initDecoder( ds );
        if ( !de::audio::decoder::DecoderTools::openDecoder( ds, de::StringUtil::to_str(loadUri), -1, true ) )
        {
            DE_ERROR("No decoder open, ", de::StringUtil::to_str(loadUri) )
            return false;
        }

        de::audio::encoder::EncoderLAME mp3;
        if ( !mp3.open( de::StringUtil::to_str(saveUri), ds.channelCount, ds.sampleRate, 192 ) )
        {
            DE_ERROR("No encoder open, saveUri", de::StringUtil::to_str(saveUri) )
            return false;
        }


        size_t totalBytes = 0;
        size_t printBytes = 0;
        size_t totalChunks = 0;
        double totalDuration = 0.0;

        auto onPayload = [&]( de::audio::decoder::AudioPayload const & payload )
        {
            size_t n = payload.frameCount; // * sizeof( float );
            if ( n > 0 )
            {
                mp3.writeSamples( payload.samples.data(), n );
                totalBytes += payload.byteCount();
                printBytes += payload.byteCount();
                totalDuration += payload.duration();
            }
            totalChunks++;
        };

        while ( AVERROR_EOF != de::audio::decoder::DecoderTools::readFrame( ds, onPayload ) )
        {
            // while loop body
            if ( printBytes >= 256 * 1024 * 1024 )
            {
                printBytes -= 256 * 1024 * 1024;

                DE_DEBUG("[FFMPEG] "
                "Bytes(", dbStrBytes( totalBytes ), "), "
                "Duration(", dbStrSeconds( totalDuration ), "), "
                "Chunks(", totalChunks, ")")
            }
        }

        mp3.close();

        perf.stop();
        DE_DEBUG("[JoinMP3] needed ", perf.ms(), " ms, uri = ", de::StringUtil::to_str(loadUri) )
        return true;
    }

    static int
    run( int argc, char** argv )
    {
        std::string loadUri;
        if ( argc > 1 )
        {
            loadUri = argv[1];
        }

        if ( !de::FileSystem::existFile(loadUri) )
        {
            de::OpenFileParamsA params;
            params.caption = "Load/open any file that contains an audio stream (mp3,wav,mkv,avi,mp4,etc..) that is supported by libAVCodec ( vlc )";
            loadUri = dbOpenFileDlg( params );
        }

        DE_DEBUG("Got loadUri = ", loadUri)

        if ( !de::FileSystem::existFile(loadUri) )
        {
            DE_ERROR("Empty loadUri or not a file, program exits now... Bye bye.")
            return 0;
        }

        std::string saveUri;
        if ( argc > 2 )
        {
            saveUri = argv[ 2 ];
        }
        saveUri = loadUri + ".convert2mp3.mp3";

        //DE_DEBUG("[LoadMP3] loadUri = ", loadUri )
        //Buffer workBuffer;

        de::PerformanceTimer perf;
        perf.start();

        de::audio::decoder::DecoderCtx ds;
        de::audio::decoder::DecoderTools::initDecoder( ds );
        if ( !de::audio::decoder::DecoderTools::openDecoder( ds, loadUri, -1, true ) )
        {
            DE_ERROR("No decoder open, ",loadUri)
            return false;
        }

        de::audio::encoder::EncoderLAME mp3;
        if ( !mp3.open( saveUri, ds.channelCount, ds.sampleRate, 192 ) )
        {
            DE_ERROR("No encoder open, ",loadUri)
            return false;
        }


        size_t totalBytes = 0;
        size_t printBytes = 0;
        size_t totalChunks = 0;
        double totalDuration = 0.0;

        auto onPayload = [&]( de::audio::decoder::AudioPayload const & payload )
        {
            size_t n = payload.frameCount; // * sizeof( float );
            if ( n > 0 )
            {
                mp3.writeSamples( payload.samples.data(), n );
                totalBytes += payload.byteCount();
                printBytes += payload.byteCount();
                totalDuration += payload.duration();
            }
            totalChunks++;
        };

        while ( AVERROR_EOF != de::audio::decoder::DecoderTools::readFrame( ds, onPayload ) )
        {
            // while loop body
            if ( printBytes >= 256 * 1024 * 1024 )
            {
                printBytes -= 256 * 1024 * 1024;

                DE_DEBUG("[FFMPEG] "
                "Bytes(", dbStrBytes( totalBytes ), "), "
                "Duration(", dbStrSeconds( totalDuration ), "), "
                "Chunks(", totalChunks, ")")
            }
        }

        mp3.close();

        perf.stop();
        DE_DEBUG("[JoinMP3] needed ", perf.ms(), " ms, uri = ", loadUri )
        return true;
    }

};

//========================================================================
int main(int argc, char** argv)
//========================================================================
{
    bool ok = convert2mp3::runW( argc, argv );
    return 0;
}

