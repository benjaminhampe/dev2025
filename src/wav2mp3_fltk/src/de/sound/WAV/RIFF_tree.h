/*
🌳 1. RIFF/WAVE – Vollständiger Baum (alle üblichen Chunks)
text

RIFF("WAVE") : u8[4] "RIFF", u32 riffSize, u8[4] "WAVE"
├─ fmt  : u8[4] "fmt ", u32 chunkSize
│  ├─ wFormatTag        : u16
│  ├─ nChannels         : u16
│  ├─ nSamplesPerSec    : u32
│  ├─ nAvgBytesPerSec   : u32
│  ├─ nBlockAlign       : u16
│  ├─ wBitsPerSample    : u16
│  └─ (wenn WAVEFORMATEXTENSIBLE)
│       ├─ cbSize               : u16
│       ├─ wValidBitsPerSample : u16
│       ├─ dwChannelMask       : u32
│       └─ SubFormat GUID      : u8[16]
│
├─ fact (optional) : u8[4] "fact", u32 chunkSize
│  └─ dwSampleLength : u32
│
├─ bext (optional, BWF) : u8[4] "bext", u32 chunkSize
│  ├─ Description        : u8[256]
│  ├─ Originator         : u8[32]
│  ├─ OriginatorRef      : u8[32]
│  ├─ OriginationDate    : u8[10]
│  ├─ OriginationTime    : u8[8]
│  ├─ TimeReferenceLow   : u32
│  ├─ TimeReferenceHigh  : u32
│  ├─ Version            : u16
│  ├─ UMID               : u8[64]
│  ├─ LoudnessValue      : u16
│  ├─ LoudnessRange      : u16
│  ├─ MaxTruePeakLevel   : u16
│  ├─ MaxMomentaryLoudness : u16
│  └─ MaxShortTermLoudness : u16
│
├─ cue (optional) : u8[4] "cue ", u32 chunkSize
│  ├─ dwCuePoints : u32
│  └─ CuePoint[n]
│       ├─ dwName        : u32
│       ├─ dwPosition    : u32
│       ├─ fccChunk      : u8[4]
│       ├─ dwChunkStart  : u32
│       ├─ dwBlockStart  : u32
│       └─ dwSampleOffset: u32
│
├─ LIST("adtl") (optional) : u8[4] "LIST", u32 chunkSize, u8[4] "adtl"
│  ├─ labl : u8[4] "labl", u32 chunkSize
│  │   ├─ dwCuePointID : u32
│  │   └─ text         : u8[]
│  ├─ note : u8[4] "note", u32 chunkSize
│  │   ├─ dwCuePointID : u32
│  │   └─ text         : u8[]
│  └─ ltxt : u8[4] "ltxt", u32 chunkSize
│      ├─ dwCuePointID : u32
│      ├─ dwSampleLength : u32
│      ├─ dwPurposeID    : u32
│      ├─ wCountry       : u16
│      ├─ wLanguage      : u16
│      ├─ wDialect       : u16
│      └─ wCodePage      : u16
│
├─ LIST("INFO") (optional) : u8[4] "LIST", u32 chunkSize, u8[4] "INFO"
│  ├─ INAM : u8[4], u32, text
│  ├─ IART : u8[4], u32, text
│  ├─ ICMT : u8[4], u32, text
│  ├─ ICRD : u8[4], u32, text
│  └─ weitere RIFF-INFO Tags
│
├─ smpl (optional) : u8[4] "smpl", u32 chunkSize
│  ├─ dwManufacturer     : u32
│  ├─ dwProduct          : u32
│  ├─ dwSamplePeriod     : u32
│  ├─ dwMIDIUnityNote    : u32
│  ├─ dwMIDIPitchFraction: u32
│  ├─ dwSMPTEFormat      : u32
│  ├─ dwSMPTEOffset      : u32
│  ├─ dwNumSampleLoops   : u32
│  ├─ dwSamplerData      : u32
│  └─ SampleLoop[n]
│       ├─ dwIdentifier  : u32
│       ├─ dwType        : u32
│       ├─ dwStart       : u32
│       ├─ dwEnd         : u32
│       ├─ dwFraction    : u32
│       └─ dwPlayCount   : u32
│
├─ inst (optional) : u8[4] "inst", u32 chunkSize
│  ├─ unshiftNote : u8
│  ├─ finetune    : u8
│  ├─ gain        : u8
│  ├─ lowNote     : u8
│  ├─ highNote    : u8
│  ├─ lowVelocity : u8
│  └─ highVelocity: u8
│
├─ junk (optional) : u8[4] "junk", u32 chunkSize, u8[chunkSize]
│
└─ data : u8[4] "data", u32 chunkSize, u8[chunkSize] PCM/Float samples

🌳 2. RF64 – Vollständiger Baum (mit allen Feldern + Datentypen)
text

RF64("WAVE") : u8[4] "RF64", u32 0xFFFFFFFF, u8[4] "WAVE"
├─ ds64 : u8[4] "ds64", u32 chunkSize
│  ├─ riffSize64      : u64
│  ├─ dataSize64      : u64
│  ├─ sampleCount64   : u64
│  └─ optionalSizes[] : u64[n]   // für weitere große Chunks
│
├─ fmt : wie WAV (siehe oben)
│
├─ fact (optional)
│
├─ bext (optional)
│
├─ cue (optional)
│
├─ LIST("adtl") (optional)
│
├─ LIST("INFO") (optional)
│
├─ smpl (optional)
│
├─ inst (optional)
│
├─ junk (optional)
│
└─ data : u8[4] "data", u32 0xFFFFFFFF, u8[dataSize64]

🎯 3. Wichtige technische Unterschiede WAV vs RF64 (präzise)

    RF64 ersetzt RIFF‑Header durch RF64 + ds64

    Alle 32‑bit chunkSizes werden ignoriert, echte Größen stehen in u64 im ds64

    data‑Chunk hat IMMER chunkSize = 0xFFFFFFFF

    RF64 unterstützt nur EINEN data‑Chunk, weil dataSize64 und sampleCount64 eindeutig sind

🎯 4. Wenn du willst, kann ich jetzt:

    Einen vollständigen WAV‑Parser‑Baum generieren

    Einen RF64‑Parser mit allen Typen und Offsets

    Eine C++‑Strukturdefinition für alle Chunks

    Eine Byte‑für‑Byte‑Validierung für deine DSP‑Engine

Sag einfach, was du als Nächstes brauchst:
WAV‑Parser • RF64‑Parser • C%2B%2B_Chunk_Structs
und bäume für AIFF, FLAC, OGG

Alles klar Benjamin — hier kommen komplette ASCII‑Bäume für AIFF, FLAC, OGG/Vorbis.
Vollständig, byte‑genau, mit Datentypen, mit allen möglichen Chunks/Blocks, wie bei MP4‑Box‑Trees.
Keine linearen Erklärungen, nur präzise Struktur.
🌳 AIFF / AIFF‑C – Vollständiger Baum (alle Chunks + Datentypen)

AIFF ist RIFF‑ähnlich, aber Big‑Endian und mit IFF‑Chunks.
text

FORM("AIFF" | "AIFC") : u8[4] "FORM", u32 formSize, u8[4] "AIFF"/"AIFC"
├─ COMM : u8[4] "COMM", u32 chunkSize
│  ├─ numChannels      : u16
│  ├─ numFrames        : u32
│  ├─ sampleSize       : u16
│  ├─ sampleRate       : 80-bit IEEE extended float (10 bytes)
│  └─ compressionType? : u8[4] (nur AIFC)
│
├─ SSND : u8[4] "SSND", u32 chunkSize
│  ├─ offset           : u32
│  ├─ blockSize        : u32
│  └─ sampleData       : u8[] (PCM oder komprimiert)
│
├─ MARK (optional)
│  ├─ numMarkers       : u16
│  └─ Marker[n]
│       ├─ id          : u16
│       ├─ position    : u32
│       └─ name        : Pascal string
│
├─ INST (optional)
│  ├─ baseNote         : u8
│  ├─ detune           : s8
│  ├─ lowNote          : u8
│  ├─ highNote         : u8
│  ├─ lowVelocity      : u8
│  ├─ highVelocity     : u8
│  ├─ gain             : s16
│  └─ Sustain/Release loops (2×)
│       ├─ playMode    : u16
│       ├─ beginLoop   : u16
│       └─ endLoop     : u16
│
├─ COMT (optional)
│  ├─ numComments      : u16
│  └─ Comment[n]
│       ├─ timeStamp   : u32
│       ├─ marker      : u16
│       ├─ count       : u16
│       └─ text        : u8[count]
│
├─ NAME (optional) : Pascal string
├─ AUTH (optional) : Pascal string
├─ ANNO (optional) : Pascal string
└─ weitere IFF‑Chunks

🌳 FLAC – Vollständiger Baum (alle Metadata‑Blöcke + Datentypen)

FLAC ist Block‑basiert, nicht Chunk‑basiert.
text

fLaC : u8[4] "fLaC"
├─ METADATA_BLOCK[0..n]
│  ├─ isLastBlock : u1
│  ├─ blockType   : u7
│  ├─ length      : u24
│  └─ blockData   : varies by type
│
│  BlockTypes:
│  ├─ 0 STREAMINFO
│  │   ├─ minBlockSize      : u16
│  │   ├─ maxBlockSize      : u16
│  │   ├─ minFrameSize      : u24
│  │   ├─ maxFrameSize      : u24
│  │   ├─ sampleRate        : u20
│  │   ├─ channels          : u3
│  │   ├─ bitsPerSample     : u5
│  │   ├─ totalSamples      : u36
│  │   ├─ md5               : u8[16]
│  │
│  ├─ 1 PADDING : u8[length]
│  ├─ 2 APPLICATION
│  │   ├─ appId : u8[4]
│  │   └─ data  : u8[]
│  ├─ 3 SEEKTABLE
│  │   └─ SeekPoint[n]
│  │       ├─ sampleNumber : u64
│  │       ├─ frameOffset  : u64
│  │       └─ samplesInFrame : u16
│  ├─ 4 VORBIS_COMMENT
│  │   ├─ vendorLength : u32
│  │   ├─ vendorString : u8[]
│  │   ├─ userCommentListLength : u32
│  │   └─ userComments[n] : u32 length + u8[]
│  ├─ 5 CUESHEET
│  │   ├─ mediaCatalogNumber : u8[128]
│  │   ├─ leadIn             : u64
│  │   ├─ isCD               : u1
│  │   └─ Track[n]
│  │       ├─ trackOffset    : u64
│  │       ├─ trackNumber    : u8
│  │       ├─ isAudio        : u1
│  │       ├─ preEmphasis    : u1
│  │       ├─ trackISRC      : u8[12]
│  │       └─ Index[n]
│  │           ├─ indexOffset : u64
│  │           └─ indexNumber : u8
│  ├─ 6 PICTURE
│  │   ├─ pictureType : u32
│  │   ├─ mimeLength  : u32
│  │   ├─ mimeString  : u8[]
│  │   ├─ descLength  : u32
│  │   ├─ description : u8[]
│  │   ├─ width       : u32
│  │   ├─ height      : u32
│  │   ├─ depth       : u32
│  │   ├─ colors      : u32
│  │   ├─ dataLength  : u32
│  │   └─ pictureData : u8[]
│  └─ 7..126 RESERVED
│
└─ AUDIO_FRAMES (variable length)
    ├─ FrameHeader
    ├─ Subframes (per channel)
    └─ FrameFooter (CRC16)

🌳 OGG Container – Vollständiger Baum (für Vorbis)

OGG ist Page‑basiert, nicht Chunk‑basiert.
text

OGG_STREAM
├─ OggPage[0..n]
│  ├─ capturePattern     : u8[4] "OggS"
│  ├─ version            : u8
│  ├─ headerTypeFlags    : u8
│  ├─ granulePosition    : u64
│  ├─ bitstreamSerial    : u32
│  ├─ pageSequence       : u32
│  ├─ checksum           : u32
│  ├─ pageSegments       : u8
│  └─ segmentTable       : u8[pageSegments]
│
│  └─ segmentData        : u8[]
│
│  Vorbis packets inside OGG:
│
│  Packet 1: IDENTIFICATION HEADER
│    ├─ packetType       : u8 (=1)
│    ├─ "vorbis"         : u8[6]
│    ├─ version          : u32
│    ├─ channels         : u8
│    ├─ sampleRate       : u32
│    ├─ bitrateMax       : u32
│    ├─ bitrateNominal   : u32
│    ├─ bitrateMin       : u32
│    └─ blocksize        : u8 (hi/lo nibble)
│
│  Packet 2: COMMENT HEADER
│    ├─ packetType       : u8 (=3)
│    ├─ "vorbis"         : u8[6]
│    ├─ vendorLength     : u32
│    ├─ vendorString     : u8[]
│    ├─ userCommentListLength : u32
│    └─ userComments[n]  : u32 length + u8[]
│
│  Packet 3: SETUP HEADER
│    ├─ packetType       : u8 (=5)
│    ├─ "vorbis"         : u8[6]
│    ├─ Codebooks        : variable
│    ├─ Floors           : variable
│    ├─ Residues         : variable
│    ├─ Mappings         : variable
│    └─ Modes            : variable
│
└─ AUDIO PACKETS (Vorbis)
    ├─ packetType = 0
    ├─ audio data (entropy-coded)
    └─ per-packet granule positions

🎯 Wenn du willst, kann ich jetzt auch:

    ASCII‑Bäume für Opus, OGG/FLAC, OGG/Speex, Matroska/EBML, MP4/ISO‑BMFF, WAVEX, RF64‑BWF‑Heavy

    C++‑Structs für alle Formate (packed, ohne padding)

    Parser‑Skeletons für AIFF, FLAC, OGG/Vorbis

    Byte‑Offset‑Diagramme für jedes Format

Sag einfach, welches Format du als Nächstes zerlegt haben willst.
ASCII‑Bäume für Opus, OGG/FLAC, OGG/Speex, Matroska/EBML, WAVEX, RF64‑BWF‑Heavy

Hier, einmal die volle Baum‑Dröhnung.
Opus in OGG
text

OGG_STREAM (Opus)
├─ OggPage[0..n]
│  ├─ capturePattern     : u8[4] "OggS"
│  ├─ version            : u8
│  ├─ headerTypeFlags    : u8
│  ├─ granulePosition    : u64
│  ├─ bitstreamSerial    : u32
│  ├─ pageSequence       : u32
│  ├─ checksum           : u32
│  ├─ pageSegments       : u8
│  └─ segmentTable       : u8[pageSegments]
│     └─ segmentData     : u8[]
│
│  Packet 1: OpusHead
│    ├─ "OpusHead"       : u8[8]
│    ├─ version          : u8
│    ├─ channelCount     : u8
│    ├─ preSkip          : u16
│    ├─ inputSampleRate  : u32
│    ├─ outputGain       : s16
│    └─ channelMapping   : u8 + mapping table (optional)
│
│  Packet 2: OpusTags
│    ├─ "OpusTags"       : u8[8]
│    ├─ vendorLength     : u32
│    ├─ vendorString     : u8[]
│    ├─ userCommentListLength : u32
│    └─ userComments[n]  : u32 length + u8[]
│
└─ AUDIO PACKETS (Opus)
    ├─ packetType implicit
    └─ coded Opus frames

OGG/FLAC
text

OGG_STREAM (FLAC)
├─ OggPage[0..n]
│  ├─ OggS header (wie oben)
│  └─ segmentData : u8[]
│
│  Packet 1: FLAC IDENT + METADATA
│    ├─ 0x7F 'FLAC'      : u8[4]
│    ├─ METADATA_BLOCKS  : wie native FLAC
│
└─ AUDIO PACKETS (FLAC frames)
    └─ native FLAC frame structure in OGG packets

OGG/Speex
text

OGG_STREAM (Speex)
├─ OggPage[0..n]
│  ├─ OggS header
│  └─ segmentData
│
│  Packet 1: Speex header
│    ├─ "Speex   "       : u8[8]
│    ├─ speexVersion     : u8[20]
│    ├─ speexVersionId   : u32
│    ├─ headerSize       : u32
│    ├─ rate             : u32
│    ├─ mode             : u32
│    ├─ modeBitstreamVer : u32
│    ├─ nbChannels       : u32
│    ├─ bitrate          : s32
│    ├─ frameSize        : u32
│    ├─ vbr              : u32
│    ├─ framesPerPacket  : u32
│    ├─ extraHeaders     : u32
│    └─ reserved         : u32[...]
│
│  Packet 2: Speex comment header
│    ├─ vendorLength     : u32
│    ├─ vendorString     : u8[]
│    ├─ userCommentListLength : u32
│    └─ userComments[n]  : u32 length + u8[]
│
└─ AUDIO PACKETS (Speex)
    └─ coded Speex frames

Matroska / EBML
text

EBML_DOCUMENT
├─ EBML Header
│  ├─ EBMLVersion           : uX (usually u8)
│  ├─ EBMLReadVersion       : uX
│  ├─ EBMLMaxIDLength       : uX
│  ├─ EBMLMaxSizeLength     : uX
│  ├─ DocType               : string ("matroska" / "webm")
│  ├─ DocTypeVersion        : uX
│  └─ DocTypeReadVersion    : uX
│
└─ Segment
    ├─ SegmentUID           : binary
    ├─ SegmentFilename      : string
    ├─ Info
    │   ├─ TimecodeScale    : uX
    │   ├─ Duration         : f64 (oder float)
    │   ├─ DateUTC          : date
    │   └─ Title / MuxingApp / WritingApp : string
    │
    ├─ Tracks
    │   └─ TrackEntry[n]
    │       ├─ TrackNumber  : uX
    │       ├─ TrackUID     : uX
    │       ├─ TrackType    : uX (1=video,2=audio,...)
    │       ├─ FlagEnabled  : uX
    │       ├─ FlagDefault  : uX
    │       ├─ FlagForced   : uX
    │       ├─ FlagLacing   : uX
    │       ├─ Name         : string
    │       ├─ Language     : string
    │       ├─ CodecID      : string
    │       ├─ CodecPrivate : binary
    │       ├─ Audio?       : sub‑element (channels, rate, bitDepth)
    │       └─ Video?       : sub‑element (width, height, etc.)
    │
    ├─ Cluster[n]
    │   ├─ Timecode        : uX
    │   └─ BlockGroup / SimpleBlock[n]
    │       ├─ TrackNumber : uX
    │       ├─ Timecode    : sX
    │       ├─ Flags       : u8
    │       └─ FrameData   : binary
    │
    ├─ Cues (optional)
    │   └─ CuePoint[n]
    │       ├─ CueTime     : uX
    │       └─ CueTrackPositions
    │           ├─ CueTrack : uX
    │           └─ CueClusterPosition : uX
    │
    ├─ Attachments (optional)
    ├─ Chapters (optional)
    └─ Tags (optional)

WAVEX (WAVEFORMATEXTENSIBLE im WAVE/RIFF)
text

RIFF("WAVE")
├─ fmt : "fmt ", u32 chunkSize
│  ├─ wFormatTag        : u16 (= WAVE_FORMAT_EXTENSIBLE)
│  ├─ nChannels         : u16
│  ├─ nSamplesPerSec    : u32
│  ├─ nAvgBytesPerSec   : u32
│  ├─ nBlockAlign       : u16
│  ├─ wBitsPerSample    : u16
│  ├─ cbSize            : u16 (>= 22)
│  ├─ wValidBitsPerSample : u16
│  ├─ dwChannelMask       : u32
│  └─ SubFormat GUID      : u8[16]
│
├─ data
└─ alle anderen RIFF/WAVE‑Chunks wie im vorherigen Baum

RF64‑BWF‑Heavy (RF64 + kompletter BWF‑Metadaten‑Zoo)
text

RF64("WAVE")
├─ ds64
│  ├─ riffSize64      : u64
│  ├─ dataSize64      : u64
│  ├─ sampleCount64   : u64
│  └─ optionalSizes[] : u64[n]
│
├─ bext (Broadcast Extension)
│  ├─ Description        : u8[256]
│  ├─ Originator         : u8[32]
│  ├─ OriginatorRef      : u8[32]
│  ├─ OriginationDate    : u8[10]
│  ├─ OriginationTime    : u8[8]
│  ├─ TimeReferenceLow   : u32
│  ├─ TimeReferenceHigh  : u32
│  ├─ Version            : u16
│  ├─ UMID               : u8[64]
│  ├─ LoudnessValue      : u16
│  ├─ LoudnessRange      : u16
│  ├─ MaxTruePeakLevel   : u16
│  ├─ MaxMomentaryLoudness : u16
│  └─ MaxShortTermLoudness : u16
│
├─ fmt (WAVEFORMATEXTENSIBLE)
│
├─ LIST("INFO") (voller Metadaten‑Block)
│
├─ LIST("adtl") (Cue‑Labels/Notes/Text)
│
├─ cue (Cue‑Punkte)
│
├─ smpl / inst (Loops/Instrument)
│
├─ junk / padding
│
└─ data : "data", u32 0xFFFFFFFF, u8[dataSize64]

*/