#include <de/file/mp4/Parser.h>

namespace de {
namespace file {
namespace mp4 {

/*
✔ Only the atoms you actually need

mvhd
tkhd
mdhd
hdlr
stsd
stts
ctts
stsc
stsz
stco

These are the atoms required to build:

track metadata
sample tables
decode timestamps
composition timestamps
chunk offsets
sample sizes

Everything else is skipped.
*/
// ============================================================================
// static
bool MP4_Parser::parse( const std::string & uri, MP4_File & mp4 )
{
    PerformanceTimer perfTimer;
    perfTimer.start();

    DE_OK("====================================")
    DE_OK("[Parser] ",uri)
    DE_OK("====================================")

    File file(uri, eFileMode::Read);
    if (!file.is_open())
    {
        DE_ERROR("Cannot open ", uri)
        return false;
    }

    const int64_t fileSize = file.size();

    char rootName[4] = {'r','o','o','t'};
    mp4.m_root = Atom( rootName, 0, 0, fileSize );

    MiniParser::parse(file, mp4.m_root.dataBeg(), mp4.m_root.dataEnd(),
    [&](const Atom& atom)
    {
        auto & children = mp4.m_root.m_children;
        children.emplace_back(atom);

        // ✔ TopLevel Atoms [file]:
        // ftyp     normal MP4
        // styp     fragmented MP4
        // free     padding
        // skip     padding
        // uuid     user‑defined box
        // moov     movie metadata
        // mdat     media data
        // meta     metadata container
        if (atom.is("ftyp"))
        {
            mp4.m_ftyp.emplace_back();
            Atom_ftyp & ftyp = mp4.m_ftyp.back();
            ftyp.atom = atom;
            ftyp.parse(file);
            DE_OK(ftyp.str())
        }
        else if (atom.is("styp"))
        {
            mp4.m_styp.emplace_back( atom );
            DE_OK(atom.str())
        }
        else if (atom.is("free"))
        {
            mp4.m_free.emplace_back( atom );
            DE_OK(atom.str())
        }
        else if (atom.is("skip"))
        {
            mp4.m_skip.emplace_back( atom );
            DE_OK(atom.str())
        }
        else if (atom.is("moov"))
        {
            mp4.m_moov.emplace_back();
            Atom_moov & moov = mp4.m_moov.back();
            moov.atom = atom;
            moov.parse(file);
            DE_OK(moov.str())
        }
        else if (atom.is("mdat"))
        {
            mp4.m_mdat.emplace_back( atom );
            DE_OK(atom.str())
        }
        else if (atom.is("meta"))
        {
            mp4.m_meta.emplace_back( atom );
            DE_OK(atom.str())
        }
    });

    perfTimer.stop();
    DE_OK("[Parser] Needed ", perfTimer.ms(), " ms for ", uri )
    return true;
}

#if 0

Parser::Parser()
{

}

/*
bool Parser::parse( const std::string & uri )
{
    PerformanceTimer m_perfTimer;
    m_perfTimer.start();


    std::vector< uint8_t > byteVector;
    FileSystem::loadBlob( byteVector, uri );

    if ( byteVector.size() < 14 ) // No space for a midi header
    {
        return false;
    }

    size_t expectBytes = byteVector.size();
    uint8_t const* beg = byteVector.data();
    uint8_t const* end = beg + expectBytes;
    size_t resultBytes = parse( beg, end, uri );

    m_perfTimer.stop();
    DE_OK("[Parser] Needed ", m_perfTimer.ms(), " ms for ", uri )

    // Validate parser result.
    if ( expectBytes == resultBytes )
    {
        DE_OK("[Parser] OK.")
    }
    else
    {
        DE_ERROR("[Parser] resultBytes(",resultBytes,") != expectBytes(",expectBytes,").")
    }
    return true;
}
*/

struct FtypBox {
    char major_brand[4];
    uint32_t minor_version;
    std::vector<std::array<char,4>> compatible_brands;
};

// ftyp payload starts at byte offset 8 of the atom

// Layout is fixed:

// 4 bytes major_brand

// 4 bytes minor_version

// N × 4‑byte compatible brands

// Number of compatible brands = (size - 16) / 4

// All fields are big‑endian except the ASCII brand strings

/*
bool parse_ftyp(int fd, uint32_t atom_size, FtypBox& out)
{
    // atom_size includes the 8-byte header
    uint32_t payload = atom_size - 8;
    if (payload < 8)
        return false; // invalid ftyp

    // read major_brand
    if (_read(fd, out.major_brand, 4) != 4)
        return false;

    // read minor_version
    out.minor_version = read_be32(fd);

    payload -= 8;

    // read compatible brands
    while (payload >= 4) {
        std::array<char,4> brand;
        if (_read(fd, brand.data(), 4) != 4)
            return false;

        out.compatible_brands.push_back(brand);
        payload -= 4;
    }

    return true;
}
*/

bool Parser::parse( const std::string & uri )
{
    PerformanceTimer m_perfTimer;
    m_perfTimer.start();

    DE_OK("====================================")
    DE_OK("[Parser] ",uri)
    DE_OK("====================================")

    uint32_t header_size = 8;

    m_file.close();
    m_file.open(uri, eFileMode::Read);
    const int64_t byteCount = m_file.size();

    uint8_t const* beg = nullptr;
    uint8_t const* end = beg + byteCount;

    onParserStart( beg, end, uri );

    parse();


    onParserEnd();

    m_perfTimer.stop();
    DE_OK("[Parser] Needed ", m_perfTimer.ms(), " ms for ", uri )
    return true;
}

void Parser::parse()
{
    PerformanceTimer m_perfTimer;
    m_perfTimer.start();

    MiniParser::parse(m_file, atom.dataBeg(), atom.dataEnd(),
        [&](const Atom& found)
        {
            if (found.is("tkhd"))
            {
                Atom_tkhd tkhd;
                tkhd.atom = found;
                tkhd.parse(file);
                m_tkhd = tkhd;
                DE_OK(m_tkhd->str())
            }
            else
            {
                DE_OK(found.str())
            }
        });
}

// ✔ Reject only if:
// size < 8
// type contains non‑ASCII
// size > file size
// infinite loop / invalid structure

bool Parser::parse( const std::string & uri )
{
    PerformanceTimer m_perfTimer;
    m_perfTimer.start();

    DE_OK("====================================")
    DE_OK("[Parser] ",uri)
    DE_OK("====================================")

    uint32_t header_size = 8;

    m_file.close();
    m_file.open(uri, eFileMode::Read);
    const int64_t byteCount = m_file.size();

    uint8_t const* beg = nullptr;
    uint8_t const* end = beg + byteCount;

    onParserStart( beg, end, uri );

    uint8_t const* src = beg;

    while (src < end)
    {
        auto topLevel = parse_toplevel_atom( src, end );
        if (!topLevel)
        {
            uint64_t address = reinterpret_cast<uint64_t>(src);
            DE_ERROR("Nothing consumed at src = ", dbHex(address))
            break;
        }

        src += topLevel->atomSize();
    }


    // Inside [moov]:
    // Atom	Purpose
    // mvhd	movie header
    // trak	track
    // tkhd	track header
    // mdia	media
    // mdhd	media header
    // hdlr	handler (audio/video)
    // minf	media info
    // stbl	sample table
    // stsd	sample descriptions
    // stts	time‑to‑sample
    // stsc	sample‑to‑chunk
    // stsz	sample sizes
    // stco / co64	chunk offsets

    onParserEnd();

    m_perfTimer.stop();
    DE_OK("[Parser] Needed ", m_perfTimer.ms(), " ms for ", uri )
    return true;
}

std::optional<Atom>
Parser::parse_toplevel_atom( uint8_t const* const beg, uint8_t const* const end )
{
    int64_t offset = reinterpret_cast<int64_t>(beg);
    int64_t remain = reinterpret_cast<int64_t>(end-beg);
    if (remain < 8)
    {
        DE_ERROR("remain < 8")
        return std::nullopt;
    }

    m_file.seek(offset,eSeekMode::Set);

    uint8_t const* src = beg;

    uint32_t smallSize = 0;
    src += m_file.read_u32_be(&smallSize);

    char atomName[4];
    src += m_file.read_char4(atomName);

    int64_t headerSize = 8;
    int64_t atomSize = smallSize;
    if (smallSize == 1)
    {
        uint64_t big_size = 0;
        src += m_file.read_u64_be(&big_size);

        headerSize = 16;
        atomSize = big_size;
    }

    Atom atom(atomName,headerSize,offset,atomSize);

    // onAtom( atom );

    m_atoms.emplace_back(atom);

    // ✔ TopLevel Atoms [file]:
    // ftyp     normal MP4
    // styp     fragmented MP4
    // free     padding
    // skip     padding
    // uuid     user‑defined box
    // moov     movie metadata
    // mdat     media data
    // meta     metadata container
    if (atom.is("ftyp"))
    {
        m_ftyp.emplace_back();
        Atom_ftyp & ftyp = m_ftyp.back();
        ftyp.atom = atom;
        ftyp.parse(m_file);
        DE_OK(ftyp.str())
    }
    else if (atom.is("styp"))
    {
        m_styp.emplace_back( atom );
        DE_OK(atom.str())
    }
    else if (atom.is("free"))
    {
        m_free.emplace_back( atom );
        DE_OK(atom.str())
    }
    else if (atom.is("skip"))
    {
        m_skip.emplace_back( atom );
        DE_OK(atom.str())
    }
    else if (atom.is("moov"))
    {
        m_moov.emplace_back();
        Atom_moov & moov = m_moov.back();
        moov.atom = atom;
        moov.parse(m_file);
        DE_OK(moov.str())
    }
    else if (atom.is("mdat"))
    {
        m_mdat.emplace_back( atom );
        DE_OK(atom.str())
    }
    else if (atom.is("meta"))
    {
        m_meta.emplace_back( atom );
        DE_OK(atom.str())
    }
    // if (memcmp(fourCC, "ftyp", 4) == 0)
    // {
    //     DE_OK("Found MP4 ftyp")
    // }
    // else
    // {
    //     DE_ERROR("No MP4 ftyp found, abort")
    //     return false;
    // }

    return atom;
}

bool
Parser::parse_ftyp( Atom_ftyp & atom )
{
    return true;
}

size_t
Parser::parse( uint8_t const* const beg, uint8_t const* const end, std::string const & uri )
{
    onParserStart( beg, end, uri );

    uint8_t const* src = beg;

    m_fileType = 0;
    m_trackCount = 0;
    m_resolution = 0;

    m_trackIndex = 0;
    m_runStatus = 0;  // per track reset running status

    m_trackDataSize = 0;
    m_trackDataStart = nullptr;

    m_currTick = 0;   // per track reset tick counter



    //DE_DEBUG("MIDI.fileSize = ", end - beg )
    //DE_DEBUG("MIDI.data = \n", hexStr( beg, end, 16 ) )

    size_t n = parseFileHeader( src, end );
    if ( n != 14 )
    {
    DE_ERROR("[",(void*)0,"] No Midi header ",uri )
    return 0;
    }
    src += n;

    size_t i = 0;                 // reset event counter
    size_t loopMax = 10000000000; // Prevent dead lock 10^10 events.

    while ( src < end ) //  && m_trackIndex < m_trackCount
    {
    n = parseMessage(src,end);
    if ( n < 1 )
    {
    DE_ERROR("[",(void*)(src-beg),"] Track(",m_trackIndex+1,"/",m_trackCount,") :: "
    "No track header ",uri )
    break;
    }
    src += n;

    i++;
    if ( i >= loopMax )
    {
    DE_ERROR("[",(void*)(src-beg),"] Track(",m_trackIndex+1,"/",m_trackCount,") :: "
    "Reached loop max ",uri )
    break;
    }
    }

    // Check outer parser state
    if ( src != end )
    {
    DE_WARN("Did not parse(",src-beg,") all bytes(",end-beg,"), ",uri)
    }

    //DE_WARN("\n",m_uri)


    onParserEnd();

    return static_cast< size_t >(src - beg);
}

size_t
Parser::parseFileHeader( uint8_t const* const beg, uint8_t const* const end )
{
   // Not a 14 byte MIDI file header.
   if ((end - beg) < 14)
   {
      DE_ERROR("No enough bytes for file header" )
      return 0; // Means parser cant process this or advance(0).
   }

   // Dont touch 'beg', so introduce src/ptr/cur.
   uint8_t const* src = beg;

   // Read 8 bytes - must be same for all MIDI filestreams.
   if ( *src++ != 'M' ) { /*DE_ERROR("No M" )*/ return 0; }
   if ( *src++ != 'T' ) { /*DE_ERROR("No T" )*/ return 0; }
   if ( *src++ != 'h' ) { /*DE_ERROR("No h" )*/ return 0; }
   if ( *src++ != 'd' ) { /*DE_ERROR("No d" )*/ return 0; }
   if ( *src++ != 0 )   { /*DE_ERROR("No 0" )*/ return 0; }
   if ( *src++ != 0 )   { /*DE_ERROR("No 0" )*/ return 0; }
   if ( *src++ != 0 )   { /*DE_ERROR("No 0" )*/ return 0; }
   if ( *src++ != 6 )   { /*DE_ERROR("No 6" )*/ return 0; }

   // For better printing we emit 2 tokens for long midi header. 8+6 byte tokens.
   //mpToken( beg, src, "MIDI header signature, 8 bytes, has size 6." );

   // We touch 'src', so introduce 'dat' to const store begin of 2nd token.
   //uint8_t const* dat = src;

   // Read last 6 bytes with 3x infos:
   // Make no additional error checks, because the 1st if(<14) covers them all.
   auto
   // Read 2 bytes for FileFormat
   n = MidiUtil::readU16_be( src, end, m_fileType );
   src += n;
   // Read 2 bytes for TrackCount
   n = MidiUtil::readU16_be( src, end, m_trackCount );
   src += n;
   // Read 2 bytes for TicksPerQuarterNote ( no 15th bit set ) or else.
   n = MidiUtil::readU16_be( src, end, m_resolution );
   src += n;

   // DEBUG
   std::string s = StringUtil::join( "MIDI file header fileType(", m_fileType, "), trackCount(", m_trackCount,"), " );

   // Parse MIDI clock resolution ( ticks ) and validate.
   int ticksPerQuarterNote = 96; // Give a default value ( kinda crude resolution )
   if ( m_resolution & 0x8000 ) // is bit 15 set in time resolution?
   {
      // Nobody explained to me what a frame is in the MIDI context. Never encountered yet.
      // DEBUG
      s += StringUtil::join( "ERROR clock(SMPTE,", m_resolution,",[ticks per frame])" );
   }
   else
   {
      // DEBUG
      s += StringUtil::join( "clock(normal,",m_resolution,"[ticks per quarter note])" );
      // Parse ticksPerQuarterNote:
      // kinda the most important information of MIDI file specific clock.
      ticksPerQuarterNote = int(m_resolution & 0x7FFF);
      if ( ticksPerQuarterNote < 1 )
      {
         // MORE DEBUG
         s += StringUtil::join( ", WARN no ticksPerQuarterNote, fallback to 96." );
         ticksPerQuarterNote = 96;
      }
   }

   // Validate FileType + TrackCount
   if ( m_fileType == 0 && m_trackCount != 1 )
   {
      s += StringUtil::join( ", WARN malformed format" );
      m_trackCount = 1;
   }

   mpToken( beg, src, s );
   mpFileHeader( int(m_fileType), int(m_trackCount), ticksPerQuarterNote );

   DE_DEBUG(s)

   return static_cast< size_t >(src - beg); // Must be 14 (bytes) for a valid header.
}

size_t
Parser::parseTrackHeader( uint8_t const* const beg, uint8_t const* const end, uint32_t & dataSize )
{
   // Should be atleast 8 bytes ( a guess ).
   if ((end - beg) < 8)
   {
      //DE_ERROR("Not enough bytes for track header" )
      return 0;
   }

   uint8_t const* src = beg;
   if ( *src != 'M' ) { /* DE_ERROR("No M of MTrk") */ return 0; }
   else { src++; } // advance
   if ( *src != 'T' ) { /* DE_ERROR("No T of MTrk" ) */ return 0; }
   else { src++; } // advance
   if ( *src != 'r' ) { /* DE_ERROR("No r of MTrk" ) */ return 0; }
   else { src++; } // advance
   if ( *src != 'k' ) { /* DE_ERROR("No k of MTrk" ) */ return 0; }
   else { src++; } // advance

   // We still expect a size...
   src += MidiUtil::readU32_be( src, end, m_trackDataSize );
   if ( m_trackDataSize < 1 )
   {
      DE_ERROR("No track dataSize found" )
   }

   m_trackDataStart = src;

   auto s = StringUtil::join( "NewTrack(",m_trackIndex+1,"/",m_trackCount,") header, dataSize = ", m_trackDataSize );

   mpToken( beg, src, s );
   mpTrackStart( m_trackIndex );

   //DE_DEBUG(s)

   return static_cast< size_t >(src - beg);
}

size_t
Parser::parseTrackEnd( uint8_t const* const beg, uint8_t const* const end )
{
   // Should be exactly 4 bytes ( 00 FF 2F 00 ) - EndOfTrack
   if ((end - beg) < 4)
   {
      //DE_ERROR("Not enough bytes for track end" )
      return 0;
   }

   uint8_t const* src = beg;
   if ( *src != 0x00 ) { /* DE_ERROR("No 0x00 of TrackEnd") */ return 0; }
   else { src++; } // advance
   if ( *src != 0xFF ) { /* DE_ERROR("No 0xFF of TrackEnd" ) */ return 0; }
   else { src++; } // advance
   if ( *src != 0x2F ) { /* DE_ERROR("No 0x2F of TrackEnd" ) */ return 0; }
   else { src++; } // advance
   if ( *src != 0x00 ) { /* DE_ERROR("No 0x00 of TrackEnd" ) */ return 0; }
   else { src++; } // advance

   auto s = StringUtil::join( "TrackEnd(",m_trackIndex+1,"/",m_trackCount,")" );

   mpToken( beg, src, s );
   mpTrackEnd();

   //DE_DEBUG(s)

   m_trackIndex++;   // advance track index
   m_currTick = 0;   // reset tick counter
   //m_runStatus = 0;  // reset running status

   return static_cast< size_t >(src - beg);
}

#endif

} // end namespace mp4.
} // end namespace file.
} // end namespace de.
