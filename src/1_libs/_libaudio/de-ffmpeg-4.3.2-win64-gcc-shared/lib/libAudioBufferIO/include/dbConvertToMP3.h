#pragma once

bool dbConvertToMP3_ffmpeg_lame();


struct Converter_AnyFile_To_MP3
{
    static bool
    convert_using_ffmpeg(std::wstring loadUri = L"")
    {
        de::PerformanceTimer perf;
        perf.start();

        if ( !de::FileSystem::existFile(loadUri) )
        {
            de::OpenFileParamsW params;
            params.caption = L"Load/open any file that contains an audio stream (mp3,wav,mkv,avi,mp4,etc..) that is supported by libAVCodec ( vlc )";
            loadUri = dbOpenFileDlg( params );
        }

        DE_DEBUG("LoadUri = ", de::StringUtil::to_str(loadUri))

        if ( !de::FileSystem::existFile(loadUri) )
        {
            DE_ERROR("Empty loadUri or not a file, program exits now... Bye bye.")
            return false;
        }

        std::wstring saveUri = loadUri + L".fcmp3.mp3";
        DE_DEBUG("SaveUri = ", de::StringUtil::to_str(saveUri))


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

};