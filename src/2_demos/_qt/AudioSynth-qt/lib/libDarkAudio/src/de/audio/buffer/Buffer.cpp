#include <de/audio/buffer/Buffer.hpp>

namespace de {
namespace audio {

// ===========================================================================
Buffer::Buffer()
// ===========================================================================
   : m_SampleType( ST_Unknown )
   , m_ChannelCount( 0 )
   , m_BytesPerSample( 0 )
   , m_SampleRate( 0 )
   , m_FrameCount(0)
   , m_PTS(0.0)
   , m_Uri()
{
   //DE_DEBUG(toString())
}

Buffer::Buffer( ESampleType sampleType, int channels, float sampleRate, uint64_t frames )
   : Buffer()
{
   setFormat( sampleType, channels, sampleRate );
   if ( frames > 0 ) resize( frames );
   //DE_DEBUG(toString())
}

Buffer::~Buffer()
{
   //DE_DEBUG(toString())
   clear();
}

/*
bool
Buffer::saveWAV_noperf( std::string uri, ESampleType dstType ) const
{
   //int audioBitRate = 192000;
   //ESampleType srcType = dst.getSampleType();
   Buffer dst;
   if ( !this->convert( dst, ST_S16I ) )
   {
      DE_ERROR("Cant convert to S16interleaved")
      return false;
   }
   wav::WavOutFile file( uri.c_str(), dst.getSampleRate(), dst.getBitsPerSample(), dst.getChannelCount() );
   file.write_S16( reinterpret_cast< int16_t const* >( dst.data() ), dst.getSampleCount() );
   return true;
}

bool
Buffer::saveWAV( std::string uri, ESampleType dstType ) const
{
   de::PerformanceTimer perf;
   perf.start();
   DE_DEBUG("[Save] uri = ", toString() )
   bool ok = saveWAV_noperf( uri, dstType );
   perf.stop();
   DE_DEBUG("[Save] needed ", perf.toSeconds(), " seconds.")
   return ok;
}

//   bool
//   loadWAV_noperf( std::string uri, ESampleType dstType = ST_S16I ) const
//   {
//      //int audioBitRate = 192000;
//      //ESampleType srcType = dst.getSampleType();
//      wav::WavInFile file( uri.c_str() );

//      Buffer dst;
//      dst.setSampleRate(
//      file. dst.getSampleRate(), dst.getBitsPerSample(), dst.getChannelCount()
//      file.write_S16( reinterpret_cast< int16_t const* >( dst.data() ), dst.getSampleCount() );

//      if ( !this->convert( dst, dstType ) )
//      {
//         DE_ERROR("Cant convert to S16interleaved")
//         return false;
//      }
//      return true;
//   }

//   bool
//   loadWAV( std::string uri, ESampleType dstType = ST_S16I ) const
//   {
//      de::PerformanceTimer perf;
//      perf.start();
//      DE_DEBUG("[Save] uri = ", toString() )
//      bool ok = save_noperf( uri, dstType );
//      perf.stop();
//      DE_DEBUG("[Save] needed ", perf.toSeconds(), " seconds.")
//      return ok;
//   }
*/


void
Buffer::dump()
{
   std::ostringstream s;
   for ( size_t ch = 0; ch < getChannelCount(); ++ch )
   {
      if ( getChannelCount() > 1 && ch > 0 )
      {
         s << " | ";
      }
      s << "Ch" << ch;
   }
   DE_DEBUG(" ",s.str(), ", channelCount = ", getChannelCount(), ", frameCount = ", getFrameCount() )
   DE_DEBUG("------------------------------" )
   for ( size_t i = 0; i < getFrameCount(); ++i )
   {
      std::ostringstream s;
      //s << "\t";
      for ( size_t ch = 0; ch < getChannelCount(); ++ch )
      {
         if ( getChannelCount() > 1 && ch > 0 )
         {
            s << " | ";
         }
         int16_t sample = getSampleS16( i, ch );
         std::string t = std::to_string( sample );
         s << t;
      }
      //s << "\n";

      //DE_DEBUG("Frame[",i,"]: " )
      DE_DEBUG( s.str() )
   }

   // s << ", dur:" << dbSecondsToString( totalDuration() );
   // s << ", bytes:" << dbStrByteCount( totalBytes() );
}

/*
Buffer&
Buffer::operator= ( Buffer const & other )
{
   if ( getSampleType() == ST_Unknown )
   {
      setSampleType( other.getSampleType() );
   }

   setUri( other.getUri() );
   setChannelCount( other.getChannelCount() );
   setSampleRate( other.getSampleRate() );

   if ( getSampleType() == other.getSampleType() )
   {
      if ( getFrameCount() < other.getFrameCount() )
      {
         resize( other.getFrameCount() );
      }
   }
   else
   {
      DE_DEBUG("Must reset format and resize")
      resize( other.getFrameCount() );
   }

   ::memcpy( this->data(), other.data(), size() );

   return *this;
}
*/

//static
Buffer::Ptr
Buffer::createPtr( ESampleType sampleType, int channels, float sampleRate, uint64_t frames )
{
   Ptr p( new Buffer( sampleType, channels, sampleRate, frames ) );
   return p;
}

// static
Buffer::SharedPtr
Buffer::createShared( ESampleType sampleType, int channels, float sampleRate, uint64_t frames )
{
   SharedPtr p = std::make_shared< Buffer >( sampleType, channels, sampleRate, frames );
   return p;
}

// static
Buffer
Buffer::create( ESampleType sampleType, int channels, float sampleRate, uint64_t frames )
{
   return Buffer( sampleType, channels, sampleRate, frames );
}

//static
Buffer*
Buffer::createNew( ESampleType sampleType, int channels, float sampleRate, uint64_t frames )
{
   return new Buffer( sampleType, channels, sampleRate, frames );
}

std::string
Buffer::getSampleTypeStr() const { return ST_toString( getSampleType() ); }

std::string
Buffer::toString( bool printEnergy ) const
{
   std::stringstream s;
   s << "st:" << getSampleTypeStr() << "; "
     << "ch:" << getChannelCount() << "; "
     << "sr:" << getSampleRate() << "; "
     << "d:" << dbStrSeconds( getDuration() ) << "; "
     << "fc:" << getFrameCount() << ";";

   if ( printEnergy )
   {
      for ( size_t c = 0; c < getChannelCount(); ++c )
      {
         double energy = getChannelAverageEnergyPerSample( c );
         s << ", EnergyCh" << c << " = " << energy;
      }
   }

   return s.str();
}

double
Buffer::getPTS() const { return m_PTS; }
void
Buffer::setPTS( double pts ) { m_PTS = pts; }

ESampleType
Buffer::getSampleType() const { return m_SampleType; }
uint32_t
Buffer::getBytesPerSample() const { return m_BytesPerSample; }
uint32_t
Buffer::getBitsPerSample() const { return m_BytesPerSample << 3; }
uint32_t
Buffer::getChannelCount() const { return m_ChannelCount; }
uint32_t
Buffer::getFrameSize() const { return getBytesPerSample() * getChannelCount(); }
bool
Buffer::isInterleaved() const { return ST_isInterleaved( m_SampleType ); }
float
Buffer::getSampleRate() const { return m_SampleRate; }
uint64_t
Buffer::getFrameCount() const { return m_FrameCount; }
uint64_t
Buffer::getSampleCount() const { return m_FrameCount * getChannelCount(); }

//uint64_t
//Buffer::capacity() const { return m_FrameCount; }
uint64_t
Buffer::getByteCount() const { return m_Samples.size(); }

uint64_t
Buffer::size() const { return m_Samples.size(); }
uint8_t*
Buffer::data() { return m_Samples.data(); }
uint8_t const*
Buffer::data() const { return m_Samples.data(); }

double
Buffer::getDuration() const
{
   if ( getSampleRate() < 0.0001f ) { return 0.0; }
   else { return double( getFrameCount() ) / double( getSampleRate() ); }
}

std::string const &
Buffer::getUri() const { return m_Uri; }
void
Buffer::setUri( std::string const & uri ) { m_Uri = uri; }
void
Buffer::setChannelCount( int32_t channels )
{
   m_ChannelCount = uint16_t( channels );
   if ( m_ChannelCount > 16 )
   {
      DE_ERROR("Got m_ChannelCount(",m_ChannelCount,") > 16")
   }
}
void
Buffer::setSampleRate( float rateInHz ) { m_SampleRate = rateInHz; }
void
Buffer::setSampleType( ESampleType sampleType )
{
   m_SampleType = sampleType;
   m_BytesPerSample = ST_getBytesPerSample( sampleType );
}
void
Buffer::setFormat( ESampleType sampleType, int channels, float sampleRate )
{
   setSampleType( sampleType );
   setChannelCount( channels );
   setSampleRate( sampleRate );
}

bool
Buffer::isValid() const
{
   if ( m_BytesPerSample < 1 ) return false;
   if ( m_ChannelCount < 1 ) return false;
   if ( m_SampleRate < 0.0001f ) return false;
   return true;
}

// WaveFormat64 const & getFormat() const { return m_Header; }

void
Buffer::shrink_to_fit()
{
   //DE_ERROR(toString())
   m_Samples.shrink_to_fit();
}

void
Buffer::clear()
{
   //DE_ERROR(toString())
   m_Samples.clear();
   m_FrameCount = 0;
   // m_SampleType = ST_Unknown;
   // m_ChannelCount = m_BytesPerSample = 0;
   // m_SampleRate = 0.f;
}

void
Buffer::resize( uint64_t frameCount )
{
   m_FrameCount = frameCount;
   uint64_t byteCount = frameCount * m_BytesPerSample * m_ChannelCount;
   if ( m_Samples.size() != byteCount )
   {
      //DE_DEBUG("resize(",m_Samples.size()," -> ",byteCount,") :: ", toString() )
      m_Samples.resize( byteCount, 0x00 );
   }
}

uint64_t
Buffer::getStride() const
{
   uint32_t bps = getBytesPerSample();
   if ( bps < 1 ) { return 0; }
   return( isInterleaved() ? bps * getChannelCount() : getFrameCount() * bps );
}

// but ffmpeg want an lumpy signed int 32bit,
// so i dont know how future prove this can be, i guess never.
int32_t
Buffer::getStridei() const
{
   uint64_t stride = getStride();
   if ( stride > uint64_t( INT32_MAX ) )
   {
      //DE_ERROR("Too many frames for ffmpeg!?")
      stride = uint64_t( INT32_MAX );
   }
   return stride;
}

uint64_t
Buffer::computeByteIndex( uint64_t frame, uint32_t channel ) const
{
   if ( getChannelCount() == 1 ) channel = 0;
   if ( isInterleaved() )
   {
      return (frame * getChannelCount() + channel) * getBytesPerSample();
   }
   else
   {
      return (getFrameCount() * channel + frame) * getBytesPerSample();
   }
}


// if planar (ch0,ch0,ch1,ch1): get full frameCount times bps, which is ( in worst case ) already 64bit
// if interleaved (ch0,ch1,ch0,ch1) : get number of channels times bps.

// Frames are x-axis, channels are on y-axis, buffer is like an image.
uint8_t*
Buffer::getSamples( uint64_t frame, uint32_t channel )
{
   uint64_t b = computeByteIndex( frame, channel );
   if ( b >= m_Samples.size() )
   {
      // DE_ERROR("byteIndex(",byteIndex,") >= m_Samples.size(",m_Samples.size(),")")
      return nullptr;
   }
   return &m_Samples[ b ];
}

uint8_t const*
Buffer::getSamples( uint64_t frame, uint32_t channel ) const
{
   uint64_t b = computeByteIndex( frame, channel );
   if ( b >= m_Samples.size() )
   {
      //DE_ERROR("byteIndex(",byteIndex,") >= m_Samples.size(",m_Samples.size(),")")
      return nullptr;
   }
   return &m_Samples[ b ];
}

// Most important data getter, can access any sample like a pixel,
// no matter if you loop over frames or channels first, no-matter the buffer
// is interleaved or planar. Just access the sample you want.
bool
Buffer::getSample( uint64_t frame, uint32_t channel, void * dst ) const
{
   if ( !dst ) { DE_ERROR("No dst") return false; }

   if ( getChannelCount() == 1 ) channel = 0;

   uint8_t const* src = getSamples( frame, channel );
   if ( !src ) { DE_ERROR("No src") return false; }
   uint32_t bps = getBytesPerSample();
   if ( bps < 1 ) { DE_ERROR("No bps") return false; }
   ::memcpy( dst, src, bps );
   return true;
}

bool
Buffer::getSample( uint64_t frame, uint32_t channel, void * dst, SampleTypeConverter::Converter_t const & converter ) const
{
   if ( !dst ) { DE_ERROR("No dst") return false; }
   if ( getChannelCount() == 1 ) channel = 0;
   uint8_t const* src = getSamples( frame, channel );
   if ( !src ) { DE_ERROR("No src") return false; }
   converter( src, dst, 1 );
   return true;
}

bool
Buffer::getSample( uint64_t frame, uint32_t channel, void * dst, ESampleType dstType ) const
{
   if ( !dst ) { DE_ERROR("No dst") return false; }
   if ( getChannelCount() == 1 ) channel = 0;

   uint8_t const* src = getSamples( frame, channel );
   if ( !src ) { DE_ERROR("No src") return false; }
   SampleTypeConverter::Converter_t converter =
      SampleTypeConverter::getConverter( getSampleType(), dstType );
   if ( !converter ) { DE_ERROR("No converter") return false; }
   converter( src, dst, 1 );
   return true;
}

// Most important data setter, can set any single sample like a pixel in an image,
// no matter if you loop over frames or channels first, no-matter the buffer
// is interleaved or planar. Just set the sample you want like setPixel( img,x,y,color ).
bool
Buffer::setSample( uint64_t frame, uint32_t channel, void const * src )
{
   if ( !src ) { DE_ERROR("No src") return false; }
   if ( getChannelCount() == 1 ) channel = 0;
   uint8_t* dst = getSamples( frame, channel );
   if ( !dst ) { DE_ERROR("No dst") return false; }
   uint32_t bps = getBytesPerSample();
   if ( bps < 1 ) { DE_ERROR("No bps") return false; }
   ::memcpy( dst, src, bps );
   return true;
}

bool
Buffer::setSample( uint64_t frame, uint32_t channel, void const * src, ESampleType srcType )
{
   if ( !src ) { DE_ERROR("No src") return false; }

   if ( getChannelCount() == 1 ) channel = 0;

   uint8_t* dst = getSamples( frame, channel );
   if ( !dst ) { DE_ERROR("No dst") return false; }
   SampleTypeConverter::Converter_t converter =
      SampleTypeConverter::getConverter( srcType, getSampleType() );
   if ( !converter ) { DE_ERROR("No converter") return false; }
   converter( src, dst, 1 );
   return true;
}

bool
Buffer::setSampleS16( uint64_t frame, uint32_t channel, int16_t value )
{
   int16_t* dst = reinterpret_cast< int16_t* >( getSamples( frame, channel ) );
   if ( !dst ) { DE_ERROR("No dst") return false; }
   *dst = value;
   return true;
}

bool
Buffer::setSampleF32( uint64_t frame, uint32_t channel, float value )
{
   auto dst = reinterpret_cast< float* >( getSamples( frame, channel ) );
   if (!dst) { DE_ERROR("No dst") return false; }
   *dst = value;
   return true;
}

int16_t
Buffer::getSampleS16( uint64_t frame, uint32_t channel ) const
{
   int16_t const* dst = reinterpret_cast< int16_t const* >( getSamples( frame, channel ) );
   if ( !dst )
   {
      DE_ERROR("OutOfBounds "
               "frame(",frame,"), "
               "channel(",channel,"), "
               "frameCount(",getFrameCount(),"), "
               "channelCount(",getChannelCount(),")")
      return 0;
   }
   return *dst;
}

float
Buffer::getSampleF32( uint64_t frame, uint32_t channel ) const
{
   auto dst = reinterpret_cast< float const* >( getSamples( frame, channel ) );
   if (!dst)
   {
      DE_ERROR("OutOfBounds "
               "frame(",frame,"), "
               "channel(",channel,"), "
               "frameCount(",getFrameCount(),"), "
               "channelCount(",getChannelCount(),")")
      return 0;
   }

   return *dst;
}


// Interleaved <-> Deinterleaved ( its the same operation )
void
Buffer::interleave()
{
   //DE_DEBUG( "interleave")

   std::vector< uint8_t > tmp( m_Samples.size() );

   size_t w = getFrameCount();
   size_t h = getChannelCount();
   size_t bps = getBytesPerSample();

   for ( size_t y = 0; y < h; ++y )
   {
      for ( size_t x = 0; x < w; ++x )
      {
         size_t i = (w * y + x) * bps; // original
         size_t j = (h * x + y) * bps; // rotated 90° ccw ( math positive )
         uint8_t const* src = reinterpret_cast< uint8_t const* >( m_Samples.data() ) + i;
         uint8_t* dst = reinterpret_cast< uint8_t* >( tmp.data() ) + j;
         ::memcpy( dst, src, bps );
      }
   }

   m_Samples = std::move( tmp );
}

// Convert
bool
Buffer::convert( Buffer & dst, ESampleType dstType ) const
{
   ESampleType const srcType = getSampleType();
   if ( srcType == ST_Unknown )
   {
      DE_ERROR("Cant convert empty buffer")
      return false;
   }

   if ( dstType == ST_Unknown )
   {
      dstType = dst.getSampleType();
   }

   if ( srcType == dstType )
   {
      dst = *this;
      DE_WARN("Copies ",dst.getSampleCount()," samples "
            "from srcType(",ST_toString(srcType),") to dstType(",ST_toString(dstType),")")
      return true;
   }

   SampleTypeConverter::Converter_t func = SampleTypeConverter::getConverter( srcType, dstType );
   if ( !func )
   {
      DE_ERROR("No Converter from srcType(",ST_toString(srcType),") "
               "to dstType(",ST_toString(dstType),") with ",dst.getSampleCount()," samples")
      return false;
   }

   dst.clear();
   dst.setFormat( dstType, getChannelCount(), getSampleRate() );
   dst.resize( getFrameCount() );

   func( this->data(), dst.data(), dst.getSampleCount() );

   if ( ST_isInterleaved( srcType ) != ST_isInterleaved( dstType ) )
   {
      dst.interleave();

      // DE_DEBUG("Converted + Interleaved ",dst.getSampleCount()," samples "
      //         "from srcType(",ST_toString(srcType),") "
      //         "to dstType(",ST_toString(dstType),")")
   }
   else {
      //DE_DEBUG("Converted ",dst.getSampleCount()," samples "
      //   "from srcType(",ST_toString(srcType),") "
      //   "to dstType(",ST_toString(dstType),")")
   }

   return true;
}
//uint64_t push_back_samples( void const* src, uint64_t sampleCount, ESampleType srcType );

bool
Buffer::sub( Buffer & dst,
             uint64_t frameStart,
             uint64_t frameCount,
             ESampleType dstType ) const
{
#if 0
   DE_DEBUG("SUB() :: dst(", dst.toString(), "), "
            "frameStart(", frameStart, "), "
            "frameCount(", frameCount, "), "
            "dstType(", ST_toString(dstType), ")" )
#endif
   if ( frameCount < 1 )
   {
      DE_WARN("frameCount < 1")
      return false;
   }

   if ( frameStart > getFrameCount() )
   {
      DE_WARN("frameStart > getFrameCount()")
      return false;
   }

   if ( frameStart + frameCount > getFrameCount() )
   {
      DE_WARN("frameStart + frameCount > getFrameCount()")
      frameCount = getFrameCount() - frameStart;
   }

   ESampleType srcType = getSampleType();
   if ( dstType == ST_Unknown )
   {
      dstType = getSampleType();
   }

   SampleTypeConverter::Converter_t converter =
      SampleTypeConverter::getConverter( srcType, dstType );
   if ( !converter )
   {
      DE_WARN("No converter srcType(",ST_toString(srcType),") to "
                           "dstType(",ST_toString(dstType),")")
      return false;
   }

   dst.setFormat( dstType, getChannelCount(), getSampleRate() );
   dst.resize( frameCount );

   for ( size_t i = 0; i < frameCount; ++i )
   {
      for ( size_t ch = 0; ch < getChannelCount(); ++ch )
      {
         uint8_t const * p_src = getSamples( i + frameStart, ch );
         if ( !p_src )
         {
            DE_ERROR("no src pointer, frame(",i,"), channel(",ch,")")
            continue;
         }

         uint8_t* p_dst = dst.getSamples( i, ch );
         if ( !p_dst )
         {
            DE_ERROR("no dst pointer, frame(",frameStart + i,"), channel(",ch,")")
            continue;
         }
         converter( p_src, p_dst, 1 );
      }
   }

   return true;
}

/*
Buffer
Buffer::sub( uint64_t frameStart, uint64_t frameCount, ESampleType dstType ) const
{
   if ( frameCount < 1 )
   {
      DE_WARN("frameCount < 1")
      return {};
   }

   if ( frameStart > getFrameCount() )
   {
      DE_WARN("frameStart > getFrameCount()")
      return {};
   }

   if ( frameStart + frameCount > getFrameCount() )
   {
      DE_WARN("frameStart + frameCount > getFrameCount()")
      frameCount = getFrameCount() - frameStart;
   }

   ESampleType srcType = getSampleType();
   if ( dstType == ST_Unknown )
   {
      dstType = getSampleType();
   }

   SampleTypeConverter::Converter_t converter =
      SampleTypeConverter::getConverter( srcType, dstType );
   if ( !converter )
   {
      DE_WARN("No converter srcType(",ST_toString(srcType),") to "
                           "dstType(",ST_toString(dstType),")")
      return {};
   }

   Buffer dst;
   dst.setFormat( dstType, getChannelCount(), getSampleRate() );
   dst.resize( frameCount );

   for ( size_t i = 0; i < frameCount; ++i )
   {
      for ( size_t ch = 0; ch < getChannelCount(); ++ch )
      {
         uint8_t const * p_src = getSamples( i + frameStart, ch );
         if ( !p_src )
         {
            DE_ERROR("no src pointer, frame(",i,"), channel(",ch,")")
            continue;
         }

         uint8_t* p_dst = dst.getSamples( i, ch );
         if ( !p_dst )
         {
            DE_ERROR("no dst pointer, frame(",frameStart + i,"), channel(",ch,")")
            continue;
         }
         converter( p_src, p_dst, 1 );
      }
   }

   return dst;
}
*/

uint64_t
Buffer::write( uint64_t dstFrameStart,
      Buffer src, uint64_t srcFrameStart, uint64_t srcFrameCount )
{
   // Check V:
   if ( src.getFrameCount() < 1 )
   {
      //DE_WARN("Nothing to write")
      return 0;
   }

   // Check V:
   if ( srcFrameCount < 1 )
   {
      //DE_WARN("Nothing to write")
      return 0;
   }

   if ( srcFrameStart >= m_FrameCount )
   {
      DE_ERROR("srcFrameStart >= m_FrameCount")
      return 0;
   }

   if ( srcFrameCount > src.getFrameCount() )
   {
      srcFrameCount = src.getFrameCount();
      DE_ERROR("Limit srcFrameCount2 = ",srcFrameCount)
   }

   if ( srcFrameCount + srcFrameStart >= m_FrameCount )
   {
      srcFrameCount = m_FrameCount - srcFrameStart;
      DE_ERROR("Limit srcFrameCount = ",srcFrameCount)
   }

// Check I:
   uint32_t srcChannels = src.getChannelCount();
   uint32_t dstChannels = this->getChannelCount();
   if ( srcChannels != dstChannels )
   {
      //DE_WARN("srcChannels(",srcChannels,") != dstChannels(",dstChannels,"), need limiter.")
      //continue;
   }

// Check II:
   ESampleType srcType = src.getSampleType();
   ESampleType dstType = this->getSampleType();
   bool needConverter = false;
   if ( srcType != dstType )
   {
      needConverter = true;
      //DE_WARN("srcType(",ST_toString(srcType),") != dstType(",ST_toString(dstType),"), need converter.")
      //continue;
   }

// Check III:
   float srcRate = src.getSampleRate();
   float dstRate = this->getSampleRate();
   bool needResampler = false;
   if ( srcRate != dstRate )
   {
      needResampler = true;
      //DE_WARN("srcRate(",srcRate,") != dstRate(",dstRate,"), need resampler!")
   }

// Check IV:
   bool srcInterleaved = ST_isInterleaved( srcType );
   bool dstInterleaved = ST_isInterleaved( dstType );
   bool needInterleave = false;
   if ( srcInterleaved != dstInterleaved )
   {
      needInterleave = true;
      //DE_WARN("srcInterleaved(",srcInterleaved,") != dstInterleaved(",dstInterleaved,"), need interleave!")
      src.interleave();
   }

// Check V: Ok
   SampleTypeConverter::Converter_t converter = SampleTypeConverter::getConverter( srcType, dstType );
   if ( !converter )
   {
      //DE_WARN("No converter srcType(",ST_toString(srcType),") to dstType(",ST_toString(dstType),")")
      // continue;
      return 0;
   }

//   uint32_t src_bps = src.getBytesPerSample();
//   uint32_t dst_bps = this->getBytesPerSample();

//   uint8_t buf[256];
//   memset( buf, 0, 256 );

   for ( size_t i = 0; i < srcFrameCount; ++i )
   {
      for ( size_t ch = 0; ch < std::min( srcChannels, dstChannels ); ++ch )
      {
         size_t srcFrameIndex = i + srcFrameStart;
         uint8_t const * p_src = src.getSamples( srcFrameIndex, ch );
         if ( !p_src )
         {
            DE_ERROR("no src pointer, frame(",srcFrameIndex,"), channel(",ch,")")
            continue;
         }

         size_t dstFrameIndex = i + dstFrameStart;
         uint8_t* p_dst = this->getSamples( dstFrameIndex, ch );
         if ( !p_dst )
         {
            DE_ERROR("no dst pointer, frame(",dstFrameIndex,"), channel(",ch,")")
            continue;
         }
         converter( p_src, p_dst, 1 );
      }
   }

   return srcFrameCount;
}


uint64_t
Buffer::write( uint64_t frameStart, Buffer src )
{
#if 0
   DE_DEBUG("Write src(",src.toString(),") to dst(",toString(),") at frameStart(",frameStart,")")
#endif
   // Check V:
   uint64_t srcFrames = src.getFrameCount();

   if ( frameStart >= m_FrameCount )
   {
      DE_ERROR("frameStart >= m_FrameCount")
      return 0;
   }

   if ( srcFrames + frameStart >= m_FrameCount )
   {
      srcFrames = m_FrameCount - frameStart;
      DE_ERROR("Limit srcFrames = ",srcFrames)
   }

   if ( srcFrames < 1 )
   {
      //DE_WARN("Nothing to write")
      return 0;
   }

// Check I:
   uint32_t srcChannels = src.getChannelCount();
   uint32_t dstChannels = this->getChannelCount();
   if ( srcChannels != dstChannels )
   {
      //DE_WARN("srcChannels(",srcChannels,") != dstChannels(",dstChannels,"), need limiter.")
      //continue;
   }

// Check II:
   ESampleType srcType = src.getSampleType();
   ESampleType dstType = this->getSampleType();
   bool needConverter = false;
   if ( srcType != dstType )
   {
      needConverter = true;
      //DE_WARN("srcType(",ST_toString(srcType),") != dstType(",ST_toString(dstType),"), need converter.")
      //continue;
   }

// Check III:
   float srcRate = src.getSampleRate();
   float dstRate = this->getSampleRate();
   bool needResampler = false;
   if ( srcRate != dstRate )
   {
      needResampler = true;
      //DE_WARN("srcRate(",srcRate,") != dstRate(",dstRate,"), need resampler!")
   }

// Check IV:
   bool srcInterleaved = ST_isInterleaved( srcType );
   bool dstInterleaved = ST_isInterleaved( dstType );
   bool needInterleave = false;
   if ( srcInterleaved != dstInterleaved )
   {
      needInterleave = true;
      //DE_WARN("srcInterleaved(",srcInterleaved,") != dstInterleaved(",dstInterleaved,"), need interleave!")
      src.interleave();
   }

// Check V: Ok
   SampleTypeConverter::Converter_t converter = SampleTypeConverter::getConverter( srcType, dstType );
   if ( !converter )
   {
      //DE_WARN("No converter srcType(",ST_toString(srcType),") to dstType(",ST_toString(dstType),")")
      // continue;
      return 0;
   }

//   uint32_t src_bps = src.getBytesPerSample();
//   uint32_t dst_bps = this->getBytesPerSample();

//   uint8_t buf[256];
//   memset( buf, 0, 256 );

   for ( size_t frame = 0; frame < srcFrames; ++frame )
   {
      for ( size_t ch = 0; ch < std::min( srcChannels, dstChannels ); ++ch )
      {
         uint8_t const * p_src = src.getSamples( frame, ch );
         if ( !p_src )
         {
            DE_ERROR("no src pointer, frame(",frame,"), channel(",ch,")")
            continue;
         }

         uint8_t* p_dst = this->getSamples( frameStart + frame, ch );
         if ( !p_dst )
         {
            //DE_ERROR("no dst pointer, frame(",frameStart + frame,"), channel(",ch,")")
            continue;
         }
         converter( p_src, p_dst, 1 );
      }
   }

   return srcFrames;
}

// interleaved or planar. Just set the sample you want like setPixel( img,x,y,color ).
uint64_t
Buffer::read( uint64_t frameStart, uint64_t frameCount, uint32_t dst_channels, void* dst, ESampleType dstType ) const
{
   ST_Validate( getSampleType() );

   uint32_t src_channels = this->getChannelCount();
   ESampleType srcType = this->getSampleType();
   if ( srcType == ST_Unknown )
   {
      DE_ERROR("Cant convert empty buffer")
      return 0;
   }

   if ( dstType == ST_Unknown ) { dstType = srcType; }

   SampleTypeConverter::Converter_t converter =
      SampleTypeConverter::getConverter( srcType, dstType );
   if ( !converter )
   {
      DE_ERROR("[Read] No ST_Converter from "
               "srcType(",ST_toString(srcType),") to "
               "dstType(",ST_toString(dstType),")")
      return 0;
   }
   else
   {
//      DE_DEBUG("ST_Converter from "
//               "srcType(",ST_toString(srcType),") to "
//               "dstType(",ST_toString(dstType),")")
   }
   if ( dst_channels != src_channels )
   {
      //DE_WARN("dst_channels(",dst_channels,") != src_channels(",src_channels,")")
   }
   if ( ST_isInterleaved( srcType ) != ST_isInterleaved( dstType ) )
   {
      DE_WARN("ST_isInterleaved(",ST_toString(srcType),") != ST_isInterleaved(",ST_toString(dstType),")")

   }

   if ( !ST_isInterleaved( dstType ) )
   {
      DE_WARN("!ST_isInterleaved( dstType )")
   }

   uint32_t dst_bps = ST_getBytesPerSample( dstType );
   uint8_t* pDst = reinterpret_cast< uint8_t* >( dst );
   for ( uint64_t i = 0; i < frameCount; ++i )
   {
      for ( uint32_t c = 0; c < dst_channels; ++c )
      {
         auto pSrc = getSamples( frameStart + i, c );
         if ( pSrc )
         {
            converter( pSrc, pDst, 1 );
         }
         else
         {
            DE_ERROR("No pSrc ch(",c,"), frame(",i,")")
         }
         pDst += dst_bps;

      }
   }

   return frameCount;
}


// interleaved or planar. Just set the sample you want like setPixel( img,x,y,color ).
uint64_t
Buffer::readF32I( float* dst,
                  uint64_t dstFrames,
                  uint32_t dstChannels,
                  uint64_t srcFrameStart ) const
{
   if ( getSampleType() != ST_F32I )
   {
      DE_ERROR("Unsupported srcType ",ST_toString(getSampleType()), ", must be ST_F32I.")
      size_t dstSamples = size_t(dstFrames) * dstChannels;
      for ( size_t i = 0; i < dstSamples; ++i )
      {
         *dst++ = 0.0f;
      }
      return dstFrames;
   }

   uint32_t srcChannels = getChannelCount();
   if ( srcChannels == dstChannels )
   {
      if ( (srcFrameStart + dstFrames) * srcChannels >= m_Samples.size() )
      {
         auto pSrc = reinterpret_cast< float const* >( m_Samples.data() ) + (srcFrameStart * srcChannels);
         auto pDst = dst;
         auto availSamples = (srcFrameStart + dstFrames) * srcChannels - m_Samples.size();
         DE_WARN("EOB availSamples(",availSamples,") , (srcFrameStart + dstFrames) * srcChannels > m_Samples.data()")

         for ( size_t i = 0; i < availSamples; ++i )
         {
            *pDst = *pSrc;
            pDst++;
            pSrc++;
         }

      }
      else
      {
         auto pSrc = reinterpret_cast< float const* >( m_Samples.data() ) + (srcFrameStart * srcChannels);
         auto pDst = dst;
         size_t dstSamples = size_t(dstFrames) * dstChannels;
         for ( size_t i = 0; i < dstSamples; ++i )
         {
            *pDst = *pSrc;
            pDst++;
            pSrc++;
         }

      }
   }
   else // if ( srcChannels != dstChannels )
   {
      auto pDst = dst;
      for ( uint64_t i = 0; i < dstFrames; ++i )
      {
         for ( uint32_t c = 0; c < dstChannels; ++c )
         {
            float sample = 0.0f;
            auto pSrc = getSamples( srcFrameStart + i, c );
            if ( pSrc )
            {
               sample = *pSrc;
            }
            else
            {
               DE_ERROR("No pSrc ch(",c,"), frame(",i,")")
            }
            *pDst++ = sample;
         }
      }
   }

   return dstFrames;
}


void
Buffer::zeroMemory( uint64_t frameStart )
{
   if ( frameStart >= getFrameCount() )
   {
      return;
   }

   uint64_t byteCount = (getFrameCount() - frameStart)*getBytesPerSample();
   if ( byteCount < 1 )
   {
      return;
   }

   if ( ST_isInterleaved( getSampleType() ) )
   {
      byteCount *= getChannelCount();
      auto p = getSamples( frameStart );
      if ( p && byteCount )
      {
         memset( p, 0, byteCount );
      }
   }
   else
   {
      for ( uint32_t c = 0; c < getChannelCount(); ++c )
      {
         auto p = getSamples( frameStart, c );
         if ( p )
         {
            memset( p, 0, byteCount );
         }
      }
   }
}

// interleaved or planar. Just set the sample you want like setPixel( img,x,y,color ).
uint64_t
Buffer::read( uint64_t srcFrameStart, Buffer & dst, uint64_t dstFrameStart ) const
{
   if ( dstFrameStart >= dst.getFrameCount() )
   {
      DE_ERROR("dstFrameStart >= dst.getFrameCount()")
      return 0;
   }

   Buffer const & src = *this;
   uint64_t srcFrames = src.getFrameCount();
   if ( srcFrames < 1 )
   {
      DE_ERROR("No srcFrames")
      return 0;
   }

   uint64_t dstFrames = dst.getFrameCount() - dstFrameStart;
   if ( dstFrames < 1 )
   {
      DE_ERROR("No dstFrames")
      return 0;
   }

   ESampleType srcType = src.getSampleType();
   ESampleType dstType = dst.getSampleType();
   if ( srcType == ST_Unknown )
   {
      DE_ERROR("No srcType")
      return 0;
   }
   if ( dstType == ST_Unknown )
   {
      DE_ERROR("No dstType")
      return 0;
   }

   uint32_t srcChannels = src.getChannelCount();
   uint32_t dstChannels = dst.getChannelCount();
   if ( srcChannels != dstChannels )
   {
      DE_ERROR("srcChannels(",srcChannels,") != dstChannels(",dstChannels,")")
   }

   SampleTypeConverter::Converter_t converter =
      SampleTypeConverter::getConverter( srcType, dstType );
   if ( !converter )
   {
      DE_ERROR("[Read] No ST_Converter from "
               "srcType(",ST_toString(srcType),") to "
               "dstType(",ST_toString(dstType),")")
      return 0;
   }

//      DE_DEBUG("ST_Converter from "
//               "srcType(",ST_toString(srcType),") to "
//               "dstType(",ST_toString(dstType),")")
   if ( !ST_isInterleaved( dstType ) )
   {
      DE_ERROR("!isInterleaved( dstType )")
   }


   uint64_t avaFrames = 0;

   if ( srcFrameStart + dstFrames >= srcFrames )
   {
      DE_WARN("READ BUFFER END")
      avaFrames = srcFrames - srcFrameStart;
      if ( avaFrames >= dstFrames ) { DE_ERROR("ERRROOOOR") return 0; }
   }
   else
   {
      avaFrames = dstFrames;
   }

   if ( ST_isInterleaved( srcType ) != ST_isInterleaved( dstType ) )
   {
      DE_WARN("isInterleaved(",ST_toString(srcType),") != isInterleaved(",ST_toString(dstType),")")

   }
//   else
//   {
//      uint32_t dst_bps = ST_getBytesPerSample( dstType );
//      auto pDst = reinterpret_cast< uint8_t* >( dst.data() );
//      auto pSrc = src.getSamples( srcFrameStart, c );

//      for ( size_t i = 0; i < dst.getSampleCount(); ++i )
//      {

//            converter( pSrc, pDst, 1 );
//            pDst += dst_bps;
//         }
//      }

//   }

   // Slow generic very flexible version
   std::array< uint8_t, 8 > sample{ 0 };

   for ( uint64_t i = 0; i < avaFrames; ++i )
   {
      for ( uint32_t c = 0; c < dstChannels; ++c )
      {
         auto pSrc = src.getSamples( srcFrameStart + i, c );
         if (!pSrc)
         {
            continue;
         }

         converter( pSrc, sample.data(), 1 );
         dst.setSample( dstFrameStart + i, c, sample.data() );
      }
   }

   return avaFrames;
}

double
Buffer::getChannelAverageEnergyPerSample( int channel ) const
{
   double energy = 0.0;

   ESampleType srcType = getSampleType();
   ESampleType dstType = ST_F32I;
   SampleTypeConverter::Converter_t converter = SampleTypeConverter::getConverter( srcType, dstType );
   if ( !converter )
   {
      //DE_WARN("No converter srcType(",ST_toString(srcType),") to dstType(",ST_toString(dstType),")")
      return energy;
   }

   for ( size_t i = 0; i < getFrameCount(); ++i )
   {
      float sample;
      converter( getSamples( i, channel ), &sample, 1 );
      energy += double( sample ) * double( sample );
   }

   if ( getFrameCount() > 0 )
   {
      // average energy per sample,
      // gives more info about bugs than just the sum over
      // any (wrong) number of samples/frames, where the sum would stay const.
      energy /= double( getFrameCount() );
   }
   return energy;
}


} // end namespace audio
} // end namespace de
