#pragma once

/*
MP4-FILE
    ├─ ftyp  (File Type Box)
    │   ├─ major_brand
    │   ├─ minor_version
    │   └─ compatible_brands[]
    │      → tells players what features to expect
    │
    ├─ free / skip / wide (padding)
    │
    ├─ moov  (Movie Box — ALL metadata)
    │   ├─ mvhd (Movie Header)
    │   │   ├─ creation_time
    │   │   ├─ modification_time
    │   │   ├─ timescale
    │   │   ├─ duration
    │   │   ├─ rate
    │   │   ├─ volume
    │   │   └─ matrix (video transform)
    │   │
    │   ├─ trak (Track Box — one per stream)
    │   │   ├─ tkhd (Track Header)
    │   │   │   ├─ track_id
    │   │   │   ├─ duration
    │   │   │   ├─ layer
    │   │   │   ├─ alternate_group
    │   │   │   ├─ volume (audio)
    │   │   │   └─ matrix (video)
    │   │   │
    │   │   ├─ tref (Track References)
    │   │   │   ├─ dpnd (depends-on)
    │   │   │   ├─ ipir (IPMP)
    │   │   │   ├─ mpod (OD)
    │   │   │   └─ tmcd (timecode)
    │   │   │
    │   │   ├─ edts (Edit Box)
    │   │   │   └─ elst (Edit List)
    │   │   │       ├─ segment_duration
    │   │   │       ├─ media_time
    │   │   │       └─ media_rate
    │   │   │       → used for trimming, delay, gaps
    │   │   │
    │   │   └─ mdia (Media Box)
    │   │       ├─ mdhd (Media Header)
    │   │       │   ├─ timescale
    │   │       │   └─ duration
    │   │       │
    │   │       ├─ hdlr (Handler)
    │   │       │   └─ handler_type = "vide" / "soun" / "text"
    │   │       │      → tells you if this is audio or video
    │   │       │
    │   │       └─ minf (Media Information)
    │   │           ├─ vmhd (video) / smhd (audio) / hmhd (hint)
    │   │           │   └─ audio balance / video graphics mode
    │   │           │
    │   │           ├─ dinf
    │   │           │   └─ dref (Data Reference)
    │   │           │       └─ url / urn
    │   │           │
    │   │           └─ stbl (Sample Table — THE HEART)
    │   │               ├─ stsd (Sample Descriptions)
    │   │               │   ├─ mp4a (AAC)
    │   │               │   │   ├─ data_reference_index
    │   │               │   │   ├─ channelcount
    │   │               │   │   ├─ samplerate
    │   │               │   │   └─ esds
    │   │               │   │       ├─ ES_Descriptor
    │   │               │   │       ├─ DecoderConfigDescriptor
    │   │               │   │       └─ DecoderSpecificInfo (ASC)
    │   │               │   │           → AAC object type, sample rate index, channels
    │   │               │   │
    │   │               │   ├─ alac (Apple Lossless)
    │   │               │   │   └─ alac atom
    │   │               │   │       ├─ frameLength
    │   │               │   │       ├─ compatibleVersion
    │   │               │   │       ├─ bitDepth
    │   │               │   │       ├─ pb / mb / kb (Rice parameters)
    │   │               │   │       └─ maxFrameBytes
    │   │               │   │
    │   │               │   ├─ Opus
    │   │               │   │   └─ dOps
    │   │               │   │       ├─ version
    │   │               │   │       ├─ output_channel_count
    │   │               │   │       ├─ pre_skip
    │   │               │   │       ├─ input_sample_rate
    │   │               │   │       └─ output_gain
    │   │               │   │
    │   │               │   ├─ ac-3 / ec-3 (Dolby Digital)
    │   │               │   │   └─ dac3 / dec3 (bitstream info)
    │   │               │   │
    │   │               │   ├─ avc1 / avc3 (H.264)
    │   │               │   │   └─ avcC (SPS/PPS)
    │   │               │   │
    │   │               │   ├─ hvc1 / hev1 (H.265)
    │   │               │   │   └─ hvcC (VPS/SPS/PPS)
    │   │               │   │
    │   │               │   ├─ dvh1 / dvhe (Dolby Vision)
    │   │               │   │   └─ dvcC / dvvC
    │   │               │   │
    │   │               │   ├─ av01 (AV1)
    │   │               │   │   └─ av1C
    │   │               │   │
    │   │               │   ├─ fLaC (FLAC)
    │   │               │   │   └─ dfLa (FLAC metadata)
    │   │               │   │
    │   │               │   ├─ lpcm (uncompressed PCM)
    │   │               │   │   ├─ sample_size
    │   │               │   │   ├─ sample_rate
    │   │               │   │   └─ channel_count
    │   │               │   │
    │   │               │   ├─ enca / encv (encrypted)
    │   │               │   │   └─ sinf
    │   │               │   │       ├─ frma (original format)
    │   │               │   │       ├─ schm (scheme type)
    │   │               │   │       └─ schi
    │   │               │   │           └─ tenc (default_KID, isEncrypted)
    │   │               │   │
    │   │               │   └─ tx3g / stpp / wvtt (subtitles)
    │   │               │
    │   │               ├─ stts (Decoding Time to Sample)
    │   │               │   └─ (sample_count, sample_delta)
    │   │               │
    │   │               ├─ ctts (Composition Time to Sample)
    │   │               │   └─ (sample_count, sample_offset)
    │   │               │
    │   │               ├─ stsc (Sample-to-Chunk)
    │   │               │   └─ (first_chunk, samples_per_chunk, desc_index)
    │   │               │
    │   │               ├─ stsz / stz2 (Sample Sizes)
    │   │               │   └─ size of each encoded sample
    │   │               │
    │   │               ├─ stco / co64 (Chunk Offsets)
    │   │               │   └─ absolute file offsets into mdat
    │   │               │
    │   │               ├─ stss (Sync Samples)
    │   │               │   └─ keyframe indices
    │   │               │
    │   │               ├─ stsh (Shadow Sync)
    │   │               ├─ stdp (Degradation Priority)
    │   │               ├─ sdtp (Sample Dependencies)
    │   │               ├─ sgpd (Sample Group Description)
    │   │               ├─ sbgp (Sample-to-Group)
    │   │               ├─ subs (Subsample Info)
    │   │               └─ cslg (Composition Shift)
    │   │
    │   ├─ udta (User Data)
    │   │   ├─ meta
    │   │   │   ├─ hdlr
    │   │   │   ├─ ilst
    │   │   │   │   ├─ ©nam (title)
    │   │   │   │   ├─ ©ART (artist)
    │   │   │   │   ├─ ©alb (album)
    │   │   │   │   ├─ ©day (year)
    │   │   │   │   ├─ covr (cover art)
    │   │   │   │   ├─ ©gen (genre)
    │   │   │   │   └─ ---- (custom)
    │   │   │   └─ keys
    │   │   │
    │   │   └─ free
    │   │
    │   ├─ mvex (Movie Extends — fragmented MP4)
    │   │   ├─ mehd
    │   │   └─ trex
    │   │
    │   └─ ipmc (IPMP Control)
    │
    ├─ moof (Movie Fragment)
    │   ├─ mfhd
    │   └─ traf
    │       ├─ tfhd
    │       ├─ tfdt
    │       ├─ trun
    │       ├─ senc
    │       ├─ saiz
    │       ├─ saio
    │       ├─ sgpd
    │       └─ sbgp
    │
    ├─ mdat (Media Data)
    │   └─ raw encoded bytes (AAC/ALAC/Opus/H.264/H.265/AV1/etc.)
    │
    ├─ sidx (Segment Index)
    ├─ ssix (Subsegment Index)
    ├─ prft (Producer Reference Time)
    │
    ├─ meta (Top-level Metadata)
    │   ├─ hdlr
    │   ├─ iloc (Item Locations)
    │   ├─ iinf (Item Info)
    │   ├─ infe (Item Entry)
    │   ├─ iprp (Item Properties)
    │   │   ├─ ipco (Property Container)
    │   │   │   ├─ ispe (image size)
    │   │   │   ├─ pixi (pixel info)
    │   │   │   ├─ colr (color profile)
    │   │   │   ├─ clap (clean aperture)
    │   │   │   ├─ pasp (pixel aspect)
    │   │   │   ├─ av1C / hvcC / avcC
    │   │   │   └─ auxC
    │   │   └─ ipma (property associations)
    │   │
    │   ├─ iref (Item References)
    │   └─ pitm (Primary Item)
    │
    ├─ mfra (Movie Fragment Random Access)
    │   ├─ tfra
    │   └─ mfro
    │
    ├─ pssh (DRM: Widevine, PlayReady, FairPlay)
    │   ├─ system_id
    │   ├─ KIDs[]
    │   └─ data (license info)
    │
    └─ uuid (Vendor Extensions)
        ├─ Avid
        ├─ GoPro
        ├─ DJI
        ├─ Apple
        └─ Adobe
*/




/*
MP4-AudioFile
    ├─ ftyp
    │   ├─ major_brand
    │   ├─ minor_version
    │   └─ compatible_brands[]
    │
    ├─ free / skip (optional)
    │
    ├─ moov
    │   ├─ mvhd
    │   │   ├─ timescale
    │   │   └─ duration
    │   │
    │   ├─ trak (1 per track)
    │   │   ├─ tkhd
    │   │   ├─ edts (opt)
    │   │   │   └─ elst
    │   │   │
    │   │   └─ mdia
    │   │       ├─ mdhd
    │   │       ├─ hdlr   ← "soun"/"vide"/"text"
    │   │       │
    │   │       └─ minf
    │   │           ├─ vmhd/smhd/hmhd
    │   │           ├─ dinf
    │   │           │   └─ dref
    │   │           │
    │   │           └─ stbl
    │   │               ├─ stsd
    │   │               │   ├─ mp4a
    │   │               │   │   └─ esds  ← AAC ASC
    │   │               │   ├─ alac
    │   │               │   │   └─ alac ← ALAC cookie
    │   │               │   ├─ Opus
    │   │               │   │   └─ dOps ← Opus header
    │   │               │   ├─ ac-3 / ec-3
    │   │               │   ├─ fLaC
    │   │               │   └─ lpcm
    │   │               │
    │   │               ├─ stts  ← decode time per sample
    │   │               ├─ ctts  ← composition offsets
    │   │               ├─ stsc  ← sample-to-chunk
    │   │               ├─ stsz  ← sample sizes
    │   │               ├─ stco  ← 32-bit chunk offsets
    │   │               └─ co64  ← 64-bit chunk offsets
    │   │
    │   ├─ udta (opt)
    │   │   ├─ meta
    │   │   │   ├─ hdlr
    │   │   │   └─ ilst
    │   │   │       ├─ ©nam (title)
    │   │   │       ├─ ©ART (artist)
    │   │   │       ├─ ©alb (album)
    │   │   │       ├─ covr (cover art)
    │   │   │       └─ ---- (custom)
    │   │   │
    │   │   └─ free
    │   │
    │   └─ mvex (fragmented MP4 only)
    │       ├─ mehd
    │       └─ trex
    │
    ├─ moof (fragmented MP4)
    │   ├─ mfhd
    │   └─ traf
    │       ├─ tfhd
    │       ├─ tfdt
    │       └─ trun
    │
    ├─ mdat
    │   └─ [raw encoded audio/video bytes]
    │
    └─ sidx (streaming index, optional)
*/


/*
MP4-AudioFile
    ├── ftyp                     (File Type Box)
    │     ├── major_brand
    │     ├── minor_version
    │     └── compatible_brands[]
    │
    ├── free / skip (optional)  (Padding)
    │
    ├── moov                     (Movie Box — ALL metadata lives here)
    │     ├── mvhd               (Movie Header)
    │     │     └── timescale, duration, rate, volume
    │     │
    │     ├── trak (Track Box — one per audio/video/subtitle track)
    │     │     ├── tkhd         (Track Header)
    │     │     │     └── track_id, enabled, duration
    │     │     │
    │     │     ├── edts (optional)
    │     │     │     └── elst   (Edit List)
    │     │     │
    │     │     └── mdia         (Media Box)
    │     │           ├── mdhd   (Media Header)
    │     │           │     └── timescale, duration
    │     │           │
    │     │           ├── hdlr   (Handler Reference)
    │     │           │     └── handler_type = "vide" / "soun" / "text"
    │     │           │           → tells you if this is AUDIO or VIDEO
    │     │           │
    │     │           └── minf   (Media Information)
    │     │                 ├── vmhd / smhd / hmhd (video/sound/hint header)
    │     │                 │
    │     │                 ├── dinf
    │     │                 │     └── dref (Data Reference)
    │     │                 │
    │     │                 └── stbl (Sample Table — HEART of MP4)
    │     │                       ├── stsd (Sample Descriptions)
    │     │                       │     ├── mp4a  (AAC sample entry)
    │     │                       │     │     └── esds  (AAC AudioSpecificConfig)
    │     │                       │     │
    │     │                       │     ├── alac  (ALAC sample entry)
    │     │                       │     │     └── alac atom (ALAC magic cookie)
    │     │                       │     │
    │     │                       │     ├── Opus  (Opus sample entry)
    │     │                       │     │     └── dOps (Opus header)
    │     │                       │     │
    │     │                       │     └── (other codecs: ac-3, ec-3, flac, lpcm)
    │     │                       │
    │     │                       ├── stts (Decoding Time to Sample)
    │     │                       │     └── sample durations
    │     │                       │
    │     │                       ├── ctts (Composition Time to Sample)
    │     │                       │     └── B‑frame offsets (video)
    │     │                       │
    │     │                       ├── stsc (Sample‑to‑Chunk)
    │     │                       │     └── maps chunk → number of samples
    │     │                       │
    │     │                       ├── stsz (Sample Sizes)
    │     │                       │     └── size of each encoded sample
    │     │                       │
    │     │                       ├── stco (Chunk Offsets, 32‑bit)
    │     │                       │
    │     │                       └── co64 (Chunk Offsets, 64‑bit)
    │     │                             └── absolute file offsets into mdat
    │     │
    │     └── udta (optional)
    │
    └── mdat                     (Media Data — RAW BYTES)
          └── [encoded audio/video packets]
*/