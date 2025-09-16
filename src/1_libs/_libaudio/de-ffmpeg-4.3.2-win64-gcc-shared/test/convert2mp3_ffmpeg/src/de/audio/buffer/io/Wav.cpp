#include <de/audio/buffer/io/Wav.hpp>

namespace de {
namespace audio {
// namespace io {
namespace wav {


constexpr char const riffStr[] = "RIFF";
constexpr char const waveStr[] = "WAVE";
constexpr char const fmtStr[]  = "fmt ";
constexpr char const factStr[] = "fact";
constexpr char const dataStr[] = "data";

//////////////////////////////////////////////////////////////////////////////
//
// Helper functions for swapping byte order to correctly read/write WAV files
// with big-endian CPU's: Define compile-time definition _BIG_ENDIAN_ to
// turn-on the conversion if it appears necessary.
//
// For example, Intel x86 is little-endian and doesn't require conversion,
// while PowerPC of Mac's and many other RISC cpu's are big-endian.

#ifdef BYTE_ORDER
   // In gcc compiler detect the byte order automatically
   #if BYTE_ORDER == BIG_ENDIAN
      // big-endian platform.
      #define _BIG_ENDIAN_
   #endif
#endif

#ifdef _BIG_ENDIAN_
   // big-endian CPU, swap bytes in 16 & 32 bit words

   // helper-function to swap byte-order of 32bit integer
   inline int32_t _byteswap32(int32_t &dwData)
   {
      dwData = ((dwData >> 24) & 0x000000FF) |
      ((dwData >> 8)  & 0x0000FF00) |
      ((dwData << 8)  & 0x00FF0000) |
      ((dwData << 24) & 0xFF000000);
      return dwData;
   }

   // helper-function to swap byte-order of 16bit integer
   inline int16_t _byteswap16(int16_t &wData)
   {
      wData = ((wData >> 8) & 0x00FF) |
       ((wData << 8) & 0xFF00);
      return wData;
   }

   // helper-function to swap byte-order of buffer of 16bit integers
   inline void _byteswap16Buffer(int16_t *pData, int32_t numWords)
   {
      for (int32_t i = 0; i < numWords; i ++)
      {
         pData[i] = _byteswap16(pData[i]);
      }
   }

// LITTLE_ENDIAN e.g. Intel x86, Amd64
#else
   #ifndef _byteswap32
   #define _byteswap32(x) (x)
   #endif
   #ifndef _byteswap16
   #define _byteswap16(x) (x)
   #endif
   #ifndef _byteswap16Buffer
   #define _byteswap16Buffer(x,y)
   #endif

//   inline int32_t _byteswap32(int32_t &dwData)
//   {
//      return dwData;
//   }

//   inline int16_t _byteswap16(int16_t &wData)
//   {
//      return wData;
//   }

//   inline void _byteswap16Buffer(int16_t *pData, int32_t numBytes)
//   {
//      // nothing todo
//   }

#endif

//////////////////////////////////////////////////////////////////////////////
// WavFileBase
//////////////////////////////////////////////////////////////////////////////
WavFileBase::WavFileBase()
{
   convBuff = NULL;
   convBuffSize = 0;
}

WavFileBase::~WavFileBase()
{
   delete[] convBuff;
   convBuffSize = 0;
}

void*
WavFileBase::getConvBuffer(int32_t sizeBytes)
{
   if (convBuffSize < sizeBytes)
   {
      delete[] convBuff;

      convBuffSize = (sizeBytes + 15) & -8;   // round up to following 8-byte bounday
      convBuff = new uint8_t[convBuffSize];
   }
   return convBuff;
}

//////////////////////////////////////////////////////////////////////////////
// WavInFile
//////////////////////////////////////////////////////////////////////////////

WavInFile::WavInFile( std::string const & fileName )
{
   m_File = fopen( fileName.c_str(), "rb");
   if (m_File == NULL)
   {
      std::string msg = "Error : Unable to open file \"";
      msg += fileName;
      msg += "\" for reading.";
      //throw std::runtime_error(msg);
      fprintf( stdout, "%s", msg.c_str() );
   }

   init();
}

WavInFile::WavInFile(FILE *file)
{
   // Try to open the file for reading
   m_File = file;
   if (!file)
   {
      std::string msg = "Error : Unable to access input stream for reading";
      //throw std::runtime_error(msg);
      fprintf( stdout, "%s", msg.c_str() );
   }

   init();
}

void WavInFile::init()
{
   if( !m_File ) return;

   int32_t hdrsOk;

   // assume file stream is already open

   // Read the file headers
   hdrsOk = readWavHeaders();
   if (hdrsOk != 0)
   {
      // Something didn't match in the wav file headers
      throw std::runtime_error("Input file is corrupt or not a WAV file");
   }

   // sanity check for format parameters
   if (  (m_Header.format.channel_number < 1)
      || (m_Header.format.channel_number > 9)
      || (m_Header.format.sample_rate < 4000)
      || (m_Header.format.sample_rate > 192000)
      || (m_Header.format.byte_per_sample < 1)
      || (m_Header.format.byte_per_sample > 320)
      || (m_Header.format.bits_per_sample < 8)
      || (m_Header.format.bits_per_sample > 32)   )
   {
      // Something didn't match in the wav file headers
      throw std::runtime_error("Error: Illegal wav file header format parameters.");
   }

   m_BytesWritten = 0;
}


WavInFile::~WavInFile()
{
   if (m_File)
   {
      ::fclose( m_File );
      m_File = nullptr;
   }
}


void WavInFile::rewind()
{
   if( !m_File ) return;
   int32_t hdrsOk;
   fseek(m_File, 0, SEEK_SET);
   hdrsOk = readWavHeaders();
   assert(hdrsOk == 0);
   m_BytesWritten = 0;
}

int32_t WavInFile::checkCharTags() const
{
   // header.format.fmt should equal to 'fmt '
   if (memcmp(fmtStr, m_Header.format.fmt, 4) != 0)
   {
      DE_ERROR("m_Header.format.fmt != 'fmt '")
      return -1;
   }

   // header.data.data_field should equal to 'data'
   if (memcmp(dataStr, m_Header.data.data_field, 4) != 0)
   {
      DE_ERROR("m_Header.data.data_field != 'data'")
      return -1;
   }
   return 0;
}

int32_t WavInFile::read(uint8_t *buffer, int32_t maxElems)
{
   if( !m_File ) return 0;

   int32_t numBytes;
   uint32_t afterDataRead;

   // ensure it's 8 bit format
   if (m_Header.format.bits_per_sample != 8)
   {
      throw std::runtime_error("Error: WavInFile::read(uint8_t*, int32_t) works only with 8bit samples.");
   }
   assert(sizeof(uint8_t) == 1);

   numBytes = maxElems;
   afterDataRead = m_BytesWritten + numBytes;
   if (afterDataRead > m_Header.data.data_len)
   {
      // Don't read more samples than are marked available in header
      numBytes = (int32_t)m_Header.data.data_len - (int32_t)m_BytesWritten;
      assert(numBytes >= 0);
   }

   assert(buffer);
   numBytes = (int32_t)fread(buffer, 1, numBytes, m_File);
   m_BytesWritten += numBytes;
   return numBytes;
}

int32_t WavInFile::read(int16_t *buffer, int32_t maxElems)
{
   if( !m_File ) return 0;

   uint32_t afterDataRead;
   int32_t numBytes;
   int32_t numElems;
   assert(buffer);
   switch (m_Header.format.bits_per_sample)
   {
   case 8:
      {
         // 8 bit format
         uint8_t *temp = (uint8_t*)getConvBuffer(maxElems);
         int32_t i;
         numElems = read(temp, maxElems);
         // convert from 8 to 16 bit
         for (i = 0; i < numElems; i ++)
         {
            buffer[i] = (int16_t)(((int16_t)temp[i] - 128) * 256);
         }
         break;
      }
   case 16:
      {
         // 16 bit format
         assert(sizeof(int16_t) == 2);
         numBytes = maxElems * 2;
         afterDataRead = m_BytesWritten + numBytes;
         if (afterDataRead > m_Header.data.data_len)
         {
            // Don't read more samples than are marked available in header
            numBytes = (int32_t)m_Header.data.data_len - (int32_t)m_BytesWritten;
            assert(numBytes >= 0);
         }

         numBytes = (int32_t)fread(buffer, 1, numBytes, m_File);
         m_BytesWritten += numBytes;
         numElems = numBytes / 2;

         // 16bit samples, swap byte order if necessary
         _byteswap16Buffer((int16_t *)buffer, numElems);
         break;
      }

   default:
      {
         std::stringstream ss; ss <<
         "Only 8/16 bit sample WAV files supported in integer compilation. "
         "Can't open WAV file with "
         << (int32_t)m_Header.format.bits_per_sample <<
         " bit sample format. ";
         throw std::runtime_error(ss.str().c_str());
      }
   };

   return numElems;
}

/// Read data in float format. Notice that when reading in float format
/// 8/16/24/32 bit sample formats are supported
int32_t
WavInFile::read(float *buffer, int32_t maxElems)
{
   if( !m_File ) return 0;

   uint32_t afterDataRead;
   int32_t numBytes;
   int32_t numElems;
   int32_t bytesPerSample;

   assert(buffer);

   bytesPerSample = m_Header.format.bits_per_sample / 8;
   if ((bytesPerSample < 1) || (bytesPerSample > 4))
   {
      std::stringstream ss;
      ss << "\nOnly 8/16/24/32 bit sample WAV files supported. Can't open WAV file with ";
      ss << (int32_t)m_Header.format.bits_per_sample;
      ss << " bit sample format. ";
      throw std::runtime_error(ss.str().c_str());
   }

   numBytes = maxElems * bytesPerSample;
   afterDataRead = m_BytesWritten + numBytes;
   if (afterDataRead > m_Header.data.data_len)
   {
      // Don't read more samples than are marked available in header
      numBytes = (int32_t)m_Header.data.data_len - (int32_t)m_BytesWritten;
      assert(numBytes >= 0);
   }

   // read raw data into temporary buffer
   uint8_t *temp = (uint8_t*)getConvBuffer(numBytes);
   numBytes = (int32_t)fread(temp, 1, numBytes, m_File);
   m_BytesWritten += numBytes;

   numElems = numBytes / bytesPerSample;

   // swap byte ordert & convert to float, depending on sample format
   switch (bytesPerSample)
   {
   case 1:
      {
         uint8_t *temp2 = (uint8_t*)temp;
         double conv = 1.0 / 128.0;
         for (int32_t i = 0; i < numElems; i ++)
         {
            buffer[i] = (float)(temp2[i] * conv - 1.0);
         }
         break;
      }

   case 2:
      {
         int16_t *temp2 = (int16_t*)temp;
         double conv = 1.0 / 32768.0;
         for (int32_t i = 0; i < numElems; i ++)
         {
            int16_t value = temp2[i];
            buffer[i] = (float)(_byteswap16(value) * conv);
         }
         break;
      }

   case 3:
      {
         uint8_t* temp2 = (uint8_t *)temp;
         double conv = 1.0 / 8388608.0;
         for (int32_t i = 0; i < numElems; i ++)
         {
            int32_t value = *((int32_t*)temp2);
            value = _byteswap32(value) & 0x00ffffff;             // take 24 bits
            value |= (value & 0x00800000) ? 0xff000000 : 0;  // extend minus sign bits
            buffer[i] = (float)(value * conv);
            temp2 += 3;
         }
         break;
      }

   case 4:
      {
         int32_t *temp2 = (int32_t *)temp;
         double conv = 1.0 / 2147483648.0;
         assert(sizeof(int32_t) == 4);
         for (int32_t i = 0; i < numElems; i ++)
         {
            int32_t value = temp2[i];
            buffer[i] = (float)(_byteswap32(value) * conv);
         }
         break;
      }
   }

   return numElems;
}


int32_t WavInFile::eof() const
{
   if( !m_File ) return 1;
   // return true if all data has been read or file eof has reached
   return (m_BytesWritten == m_Header.data.data_len || feof(m_File));
}


// test if character code is between a white space ' ' and little 'z'
int32_t isAlpha(uint8_t c)
{
   return (c >= ' ' && c <= 'z') ? 1 : 0;
}


// test if all characters are between a white space ' ' and little 'z'
int32_t isAlphaStr(char const *str)
{
   uint8_t c;

   c = str[0];
   while (c)
   {
      if (isAlpha(c) == 0) return 0;
      str ++;
      c = str[0];
   }

   return 1;
}


int32_t WavInFile::readRIFFBlock()
{
   if( !m_File ) return -1;

   if (fread(&(m_Header.riff), sizeof(WavRiff), 1, m_File) != 1) return -1;

   // swap 32bit data byte order if necessary
   _byteswap32((int32_t &)m_Header.riff.package_len);

   // header.riff.riff_char should equal to 'RIFF');
   if (memcmp(riffStr, m_Header.riff.riff_char, 4) != 0) return -1;
   // header.riff.wave should equal to 'WAVE'
   if (memcmp(waveStr, m_Header.riff.wave, 4) != 0) return -1;

   return 0;
}


int32_t WavInFile::readHeaderBlock()
{
   if( !m_File ) return -1;

    char label[5];
    std::string sLabel;

    // lead label string
    if (fread(label, 1, 4, m_File) !=4) return -1;
    label[4] = 0;

    if (isAlphaStr(label) == 0) return -1;    // not a valid label

    // Decode blocks according to their label
    if (strcmp(label, fmtStr) == 0)
    {
        int32_t nLen, nDump;

        // 'fmt ' block
        memcpy(m_Header.format.fmt, fmtStr, 4);

        // read length of the format field
        if (fread(&nLen, sizeof(int32_t), 1, m_File) != 1) return -1;
        // swap byte order if necessary
        _byteswap32(nLen);

        // calculate how much length differs from expected
        nDump = nLen - ((int32_t)sizeof(m_Header.format) - 8);

        // verify that header length isn't smaller than expected structure
        if ((nLen < 0) || (nDump < 0)) return -1;

        m_Header.format.format_len = nLen;

        // if format_len is larger than expected, read only as much data as we've space for
        if (nDump > 0)
        {
            nLen = sizeof(m_Header.format) - 8;
        }

        // read data
        if (fread(&(m_Header.format.fixed), nLen, 1, m_File) != 1) return -1;

        // swap byte order if necessary
        _byteswap16((int16_t &)m_Header.format.fixed);            // int16_t int32_t fixed;
        _byteswap16((int16_t &)m_Header.format.channel_number);   // int16_t int32_t channel_number;
        _byteswap32((int32_t &)m_Header.format.sample_rate);        // int32_t sample_rate;
        _byteswap32((int32_t &)m_Header.format.byte_rate);          // int32_t byte_rate;
        _byteswap16((int16_t &)m_Header.format.byte_per_sample);  // int16_t int32_t byte_per_sample;
        _byteswap16((int16_t &)m_Header.format.bits_per_sample);  // int16_t int32_t bits_per_sample;

        // if format_len is larger than expected, skip the extra data
        if (nDump > 0)
        {
            fseek(m_File, nDump, SEEK_CUR);
        }

        return 0;
    }
    else if (strcmp(label, factStr) == 0)
    {
        int32_t nLen, nDump;

        // 'fact' block
        memcpy(m_Header.fact.fact_field, factStr, 4);

        // read length of the fact field
        if (fread(&nLen, sizeof(int32_t), 1, m_File) != 1) return -1;
        // swap byte order if necessary
        _byteswap32(nLen);

        // calculate how much length differs from expected
        nDump = nLen - ((int32_t)sizeof(m_Header.fact) - 8);

        // verify that fact length isn't smaller than expected structure
        if ((nLen < 0) || (nDump < 0)) return -1;

        m_Header.fact.fact_len = nLen;

        // if format_len is larger than expected, read only as much data as we've space for
        if (nDump > 0)
        {
            nLen = sizeof(m_Header.fact) - 8;
        }

        // read data
        if (fread(&(m_Header.fact.fact_sample_len), nLen, 1, m_File) != 1) return -1;

        // swap byte order if necessary
        _byteswap32((int32_t &)m_Header.fact.fact_sample_len);    // int32_t sample_length;

        // if fact_len is larger than expected, skip the extra data
        if (nDump > 0)
        {
            fseek(m_File, nDump, SEEK_CUR);
        }

        return 0;
    }
    else if (strcmp(label, dataStr) == 0)
    {
        // 'data' block
        memcpy(m_Header.data.data_field, dataStr, 4);
        if (fread(&(m_Header.data.data_len), sizeof(uint32_t), 1, m_File) != 1) return -1;

        // swap byte order if necessary
        _byteswap32((int32_t &)m_Header.data.data_len);

        return 1;
    }
    else
    {
        uint32_t len, i;
        uint32_t temp;
        // unknown block

        // read length
        if (fread(&len, sizeof(len), 1, m_File) != 1) return -1;
        // scan through the block
        for (i = 0; i < len; i ++)
        {
            if (fread(&temp, 1, 1, m_File) != 1) return -1;
            if (feof(m_File)) return -1;   // unexpected eof
        }
    }
    return 0;
}


int32_t WavInFile::readWavHeaders()
{
   if ( !m_File )
   {
      DE_ERROR("No m_File")
      return 1;
   }

   memset( &m_Header, 0, sizeof(m_Header) );

   int32_t res = readRIFFBlock();
   if ( res )
   {
      DE_ERROR("No readRIFFBlock()")
      return 1;
   }

   // read header blocks until data block is found
   do
   {
      // read header blocks
      res = readHeaderBlock();
      if ( res < 0 )
      {
         DE_ERROR("Error in file structure")
         return 1;
      }
   }
   while ( res == 0 );

   // check that all required tags are legal
   return checkCharTags();
}


uint32_t WavInFile::getNumChannels() const
{
    return m_Header.format.channel_number;
}


uint32_t WavInFile::getNumBits() const
{
    return m_Header.format.bits_per_sample;
}


uint32_t WavInFile::getBytesPerSample() const
{
    return getNumChannels() * getNumBits() / 8;
}


uint32_t WavInFile::getSampleRate() const
{
    return m_Header.format.sample_rate;
}


uint32_t WavInFile::getDataSizeInBytes() const
{
    return m_Header.data.data_len;
}


uint32_t WavInFile::getNumSamples() const
{
    if (m_Header.format.byte_per_sample == 0) return 0;
    if (m_Header.format.fixed > 1) return m_Header.fact.fact_sample_len;
    return m_Header.data.data_len / (uint16_t)m_Header.format.byte_per_sample;
}


uint32_t WavInFile::getLengthMS() const
{
    double numSamples;
    double sampleRate;

    numSamples = (double)getNumSamples();
    sampleRate = (double)getSampleRate();

    return (uint32_t)(1000.0 * numSamples / sampleRate + 0.5);
}


/// Returns how many milliseconds of audio have so far been read from the file
uint32_t WavInFile::getElapsedMS() const
{
    return (uint32_t)(1000.0 * (double)m_BytesWritten / (double)m_Header.format.byte_rate);
}


//////////////////////////////////////////////////////////////////////////////
//
// Class WavOutFile
//

WavOutFile::WavOutFile(char const *fileName, int32_t sampleRate, int32_t bits, int32_t channels)
{
    m_BytesWritten = 0;
    m_File = fopen(fileName, "wb");
    if (m_File == NULL)
    {
        std::string msg = "Error : Unable to open file \"";
        msg += fileName;
        msg += "\" for writing.";
        //pmsg = msg.c_str;
        throw std::runtime_error(msg);
    }

    fillInHeader(sampleRate, bits, channels);
    writeHeader();
}


WavOutFile::WavOutFile(FILE *file, int32_t sampleRate, int32_t bits, int32_t channels)
{
    m_BytesWritten = 0;
    m_File = file;
    if (m_File == NULL)
    {
        std::string msg = "Error : Unable to access output file stream.";
        throw std::runtime_error(msg);
    }

    fillInHeader(sampleRate, bits, channels);
    writeHeader();
}


WavOutFile::~WavOutFile()
{
    finishHeader();
    if (m_File) fclose(m_File);
    m_File = NULL;
}


void WavOutFile::fillInHeader(uint32_t sampleRate, uint32_t bits, uint32_t channels)
{
    // fill in the 'riff' part..

    // copy string 'RIFF' to riff_char
    memcpy(&(m_Header.riff.riff_char), riffStr, 4);
    // package_len unknown so far
    m_Header.riff.package_len = 0;
    // copy string 'WAVE' to wave
    memcpy(&(m_Header.riff.wave), waveStr, 4);

    // fill in the 'format' part..

    // copy string 'fmt ' to fmt
    memcpy(&(m_Header.format.fmt), fmtStr, 4);

    m_Header.format.format_len = 0x10;
    m_Header.format.fixed = 1;
    m_Header.format.channel_number = (int16_t)channels;
    m_Header.format.sample_rate = (int32_t)sampleRate;
    m_Header.format.bits_per_sample = (int16_t)bits;
    m_Header.format.byte_per_sample = (int16_t)(bits * channels / 8);
    m_Header.format.byte_rate = m_Header.format.byte_per_sample * (int32_t)sampleRate;
    m_Header.format.sample_rate = (int32_t)sampleRate;

    // fill in the 'fact' part...
    memcpy(&(m_Header.fact.fact_field), factStr, 4);
    m_Header.fact.fact_len = 4;
    m_Header.fact.fact_sample_len = 0;

    // fill in the 'data' part..

    // copy string 'data' to data_field
    memcpy(&(m_Header.data.data_field), dataStr, 4);
    // data_len unknown so far
    m_Header.data.data_len = 0;
}


void WavOutFile::finishHeader()
{
    // supplement the file length into the header structure
    m_Header.riff.package_len = m_BytesWritten + sizeof(WavHeader) - sizeof(WavRiff) + 4;
    m_Header.data.data_len = m_BytesWritten;
    m_Header.fact.fact_sample_len = m_BytesWritten / m_Header.format.byte_per_sample;

    writeHeader();
}


void WavOutFile::writeHeader()
{
    WavHeader hdrTemp;
    int32_t res;

    // swap byte order if necessary
    hdrTemp = m_Header;
    _byteswap32((int32_t &)hdrTemp.riff.package_len);
    _byteswap32((int32_t &)hdrTemp.format.format_len);
    _byteswap16((int16_t &)hdrTemp.format.fixed);
    _byteswap16((int16_t &)hdrTemp.format.channel_number);
    _byteswap32((int32_t &)hdrTemp.format.sample_rate);
    _byteswap32((int32_t &)hdrTemp.format.byte_rate);
    _byteswap16((int16_t &)hdrTemp.format.byte_per_sample);
    _byteswap16((int16_t &)hdrTemp.format.bits_per_sample);
    _byteswap32((int32_t &)hdrTemp.data.data_len);
    _byteswap32((int32_t &)hdrTemp.fact.fact_len);
    _byteswap32((int32_t &)hdrTemp.fact.fact_sample_len);

    // write the supplemented header in the beginning of the file
    fseek(m_File, 0, SEEK_SET);
    res = (int32_t)fwrite(&hdrTemp, sizeof(hdrTemp), 1, m_File);
    if (res != 1)
    {
        throw std::runtime_error("Error while writing to a wav file.");
    }

    // jump back to the end of the file
    fseek(m_File, 0, SEEK_END);
}


void WavOutFile::write(const uint8_t *buffer, int32_t numElems)
{
    int32_t res;

    if (m_Header.format.bits_per_sample != 8)
    {
        throw std::runtime_error("Error: WavOutFile::write(const uint8_t*, int32_t) accepts only 8bit samples.");
    }
    assert(sizeof(uint8_t) == 1);

    res = (int32_t)fwrite(buffer, 1, numElems, m_File);
    if (res != numElems)
    {
        throw std::runtime_error("Error while writing to a wav file.");
    }

    m_BytesWritten += numElems;
}


void WavOutFile::write_S16(const int16_t *buffer, int32_t numElems)
{
    int32_t res;

    // 16 bit samples
    if (numElems < 1) return;   // nothing to do

    switch (m_Header.format.bits_per_sample)
    {
        case 8:
        {
            int32_t i;
            uint8_t *temp = (uint8_t *)getConvBuffer(numElems);
            // convert from 16bit format to 8bit format
            for (i = 0; i < numElems; i ++)
            {
                temp[i] = (uint8_t)(buffer[i] / 256 + 128);
            }
            // write in 8bit format
            write(temp, numElems);
            break;
        }

        case 16:
        {
            // 16bit format

            // use temp buffer to swap byte order if necessary
            int16_t *pTemp = (int16_t *)getConvBuffer(numElems * sizeof(int16_t));
            memcpy(pTemp, buffer, numElems * 2);
            _byteswap16Buffer(pTemp, numElems);

            res = (int32_t)fwrite(pTemp, 2, numElems, m_File);

            if (res != numElems)
            {
                throw std::runtime_error("Error while writing to a wav file.");
            }
            m_BytesWritten += 2 * numElems;
            break;
        }

        default:
        {
            std::stringstream ss;
            ss << "\nOnly 8/16 bit sample WAV files supported in integer compilation. Can't open WAV file with ";
            ss << (int32_t)m_Header.format.bits_per_sample;
            ss << " bit sample format. ";
            throw std::runtime_error(ss.str().c_str());
        }
    }
}


/// Convert from float to integer and saturate
inline int32_t saturate(float fvalue, float minval, float maxval)
{
    if (fvalue > maxval)
    {
        fvalue = maxval;
    }
    else if (fvalue < minval)
    {
        fvalue = minval;
    }
    return (int32_t)fvalue;
}


void WavOutFile::write(const float *buffer, int32_t numElems)
{
    int32_t numBytes;
    int32_t bytesPerSample;

    if (numElems == 0) return;

    bytesPerSample = m_Header.format.bits_per_sample / 8;
    numBytes = numElems * bytesPerSample;
    void *temp = getConvBuffer(numBytes + 7);   // round bit up to avoid buffer overrun with 24bit-value assignment

    switch (bytesPerSample)
    {
        case 1:
        {
            uint8_t *temp2 = (uint8_t *)temp;
            for (int32_t i = 0; i < numElems; i ++)
            {
                temp2[i] = (uint8_t)saturate(buffer[i] * 128.0f + 128.0f, 0.0f, 255.0f);
            }
            break;
        }

        case 2:
        {
            int16_t *temp2 = (int16_t *)temp;
            for (int32_t i = 0; i < numElems; i ++)
            {
                int16_t value = (int16_t)saturate(buffer[i] * 32768.0f, -32768.0f, 32767.0f);
                temp2[i] = _byteswap16(value);
            }
            break;
        }

        case 3:
        {
            uint8_t *temp2 = (uint8_t *)temp;
            for (int32_t i = 0; i < numElems; i ++)
            {
                int32_t value = saturate(buffer[i] * 8388608.0f, -8388608.0f, 8388607.0f);
                *((int32_t*)temp2) = _byteswap32(value);
                temp2 += 3;
            }
            break;
        }

        case 4:
        {
            int32_t *temp2 = (int32_t *)temp;
            for (int32_t i = 0; i < numElems; i ++)
            {
                int32_t value = saturate(buffer[i] * 2147483648.0f, -2147483648.0f, 2147483647.0f);
                temp2[i] = _byteswap32(value);
            }
            break;
        }

        default:
            assert(false);
    }

    int32_t res = (int32_t)fwrite(temp, 1, numBytes, m_File);

    if (res != numBytes)
    {
        throw std::runtime_error("Error while writing to a wav file.");
    }
    m_BytesWritten += numBytes;
}

} // end namespace wav
} // end namespace audio
} // end namespace de
