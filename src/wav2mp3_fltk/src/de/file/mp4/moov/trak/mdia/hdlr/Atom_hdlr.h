#pragma once
#include <de/file/mp4/Atom.h>

namespace de {
namespace file {
namespace mp4 {

/*
🎯 MP4
    └─ moov  (Movie Box — ALL metadata)
        └─ trak (Track Box — one per stream)
            └─ mdia (Media Box)
                └─ hdlr (Handler)
                    └─ handler_type = "vide" / "soun" / "text"
                       → tells you if this is audio or video
*/

struct hdlrData
{
    uint8_t  version;
    uint32_t flags;
    uint32_t pre_defined;   // always 0
    uint32_t handler_type;  // 'vide', 'soun', 'meta'
    uint32_t reserved[3];   // always 0
    std::string name;       // UTF-8, null-terminated (optional)
};

struct Atom_hdlr
{
    Atom atom;

    hdlrData m_data;

    int version() const { return int(m_data.version); }

    std::string media_type() const { return Atom::fourcc_to_str(m_data.handler_type); }

    std::string name() const { return m_data.name; }


    void parse(File & file)
    {
        file.seek(atom.dataBeg());

        file.read_u8(&m_data.version);          // 1 Byte
        file.read_u24_be(&m_data.flags);        // 3 Bytes

        file.read_u32_be(&m_data.pre_defined);  // 4 Bytes
        file.read_u32_be(&m_data.handler_type); // 4 Bytes

        file.read_u32_be(&m_data.reserved[0]);  // 4 Bytes
        file.read_u32_be(&m_data.reserved[1]);  // 4 Bytes
        file.read_u32_be(&m_data.reserved[2]);  // 4 Bytes

        // Remaining bytes = name string (UTF-8, null-terminated)
        int64_t bytes_left = atom.dataSize() - 24;

        if (bytes_left > 0)
        {
            m_data.name.resize(bytes_left);

            file.read(&m_data.name[0], bytes_left);

            // Some files include a trailing '\0'
            if (m_data.name.back() == '\0')
                m_data.name.pop_back();
        }
    }

    std::string str() const
    {
        std::ostringstream o;
        o << atom.str() << ", "
            //"version(" << version() << "), "
            "media(" << media_type() << "), "
            "name(" << name() << ")";
        return o.str();
    }
};

/*
handler_type in ISO/IEC 14496‑12 + QuickTime extensions + de‑facto industry values.

This is the real list used by MP4 demuxers (FFmpeg, Bento4, GPAC).
And I’ll group them by category so you can see the landscape clearly.
🎯 Core ISO BMFF handler types (official spec)

    These are the canonical ones you will see in almost every MP4.

        vide — Video track
        soun — Audio track
        hint — Hint track (RTSP)
        meta — Metadata track
        auxv — Auxiliary video (HEIF/AVIF alpha planes)

    These five are the core MP4 handler types.

🎬 QuickTime handler types (widely used in MP4)

    Apple defined many more handler types in QuickTime.
    MP4 players still support them because Apple dominates the ecosystem.
    Video‑related

        pict — Still image
        clcp — Closed captions
        tmcd — Timecode track
        text — Timed text (legacy)
        subt — Subtitles
        sbtl — Subtitle (QuickTime flavor)

    Audio‑related

        musi — Music track (legacy QuickTime)

    Metadata‑related

        mdir — Music directory
        mdta — iTunes metadata keys
        alis — Alias data
        url  — URL data
        dhlr — Data handler (QuickTime)
        mhlr — Media handler (QuickTime)

🖼️ HEIF / AVIF handler types

    HEIF/AVIF extends MP4 and adds:

        pict — Primary image
        auxv — Auxiliary image (alpha)
        grid — Image grid
        iovl — Image overlay
        iden — Identity transform
        thmb — Thumbnail image

    These appear in .heic, .heif, .avif.
    📡 3GPP / MPEG‑4 extensions

    Used in .3gp, .3g2, and some mobile MP4s.

        sdsm — Scene description stream
        odsm — Object descriptor stream
        crsm — Clock reference stream
        m7sm — MPEG‑7 stream
        ocsm — Object content info
        ipsm — IPMP stream
        mjsm — MPEG‑J stream

🧪 Rare / legacy QuickTime handlers

    These exist but are uncommon:

        alis — Alias
        url  — URL
        gmin — Generic media
        wave — Wave audio
        tmcd — Timecode
        chap — Chapter track
        text — Legacy text track

🧠 Which ones you actually need to support?

    For a real MP4 demuxer, you only need to handle:

        vide → video
        soun → audio
        meta → metadata
        subt / sbtl / text → subtitles
        pict / auxv → HEIF/AVIF images
        hint → ignore
        everything else → ignore

🎯 TL;DR — All handler types that matter

    Core MP4:
    vide, soun, meta, hint, auxv

    QuickTime:
    text, subt, sbtl, tmcd, clcp, pict, mdta, mdir, alis, url

    HEIF/AVIF:
    pict, auxv, grid, iovl, iden, thmb

    3GPP/MPEG‑4:
    sdsm, odsm, crsm, m7sm, ocsm, ipsm, mjsm

    Everything else is either obsolete or ignorable.

🎯 Binary layout of hdlr (Handler Reference Box)

    hdlr is a FullBox, so it starts with:

    uint8    version
    uint24   flags
    uint32   pre_defined        // always 0
    uint32   handler_type       // 'vide', 'soun', 'meta', ...
    uint32   reserved[3]        // always 0
    string   name               // UTF-8, null-terminated (optional)

    The name field is not required and may be empty.

🎯 C++ struct for hdlr

struct Hdlr
{
    uint8_t  version;
    uint32_t flags;
    uint32_t pre_defined;
    uint32_t handler_type;
    uint32_t reserved[3];
    std::string name;
};

🎯 C++ parser for hdlr This matches the ISO/IEC 14496‑12 spec exactly.

Hdlr parse_hdlr_payload(std::istream& in, const BoxHeader& h)
{
    Hdlr hd{};

    hd.version = read_u8(in);
    hd.flags   = read_u24_be(in);

    hd.pre_defined  = read_u32_be(in);
    hd.handler_type = read_u32_be(in);

    hd.reserved[0] = read_u32_be(in);
    hd.reserved[1] = read_u32_be(in);
    hd.reserved[2] = read_u32_be(in);

    // Remaining bytes = name string (UTF-8, null-terminated)
    uint64_t bytes_left = h.size - h.header_size - 4 - 4 - 12 - 4;
    // Explanation:
    // version+flags = 4
    // pre_defined   = 4
    // handler_type  = 4
    // reserved[3]   = 12
    // total so far  = 24 bytes

    if (bytes_left > 0) {
        hd.name.resize(bytes_left);
        in.read(&hd.name[0], bytes_left);

        // Some files include a trailing '\0'
        if (!hd.name.empty() && hd.name.back() == '\0')
            hd.name.pop_back();
    }

    return hd;
}
*/
} // end namespace mp4.
} // end namespace file.
} // end namespace de.
