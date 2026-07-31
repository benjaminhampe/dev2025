#pragma once
#include <de/sound/Sound.h>

namespace de {
namespace sound {

/*
🎧 Audio

    .ogg — generischer Ogg‑Container (Vorbis, Opus, FLAC, Speex, …)
    .oga — Ogg Audio (🎧 Vorbis, Opus, FLAC)
    .opus — Ogg/Opus (🎧 Opus im Ogg‑Container)
    .spx — Ogg/Speex (🎧 Speex im Ogg‑Container)
    .ogv — Ogg/Theora (🎥 Video)
    .ogx — Ogg Multiplex (🎥+🎧 beliebige Kombination aus Audio/Video)
    .vorbis - Ogg/Vorbis

    .drc -> Dirac (⭐ raw)
    .speex -> Speex	Audio (⭐ raw)
    .flac ist reiner FLAC, kein Ogg

    vorbisfile
    opusfile
    speexdec
    theora

    ogg_packet op;
    ogg_stream_packetout(&os, &op);

    OggCodec codec = map_ogg_codec(op);
    printf("Codec: %s\n", codec_name(codec));
*/

bool
load_sound_ogg(
    Sound & sound,
    const std::string & uri,
    const SoundLoadOptions& options );

} // end namespace sound.
} // end namespace de.
