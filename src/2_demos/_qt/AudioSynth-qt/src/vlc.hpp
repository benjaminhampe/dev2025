#pragma once
#include <de/vlc/ffAudioFile.hpp>
#include <de/vlc/ffVideoFile.hpp>

struct VLC
{
   std::string m_uri;
   de::vlc::AudioFile m_audioFile;
   de::vlc::VideoFile m_videoFile;
};

extern VLC g_vlc;
