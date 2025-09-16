#pragma once
#include <cstdint>
#include <cstdio>
#include <string>
#include <sstream>
#include <cstring>
#include <assert.h>
#include <limits.h>

#include <de/Core.h>

namespace de {
namespace audio {
namespace wav {


/* Example

   Looks like interleaved sample data has to be given to the encoder
   and is coming out of decoder.

   { // Block to enable auto destruct at end of block.

      de::audio::wav::WavOutFile wof( "hello.wav", 44100, 16, 2 );
      for ( int i = 0; i < interleaved_packet_count; ++i )
      {
         wof.write( packet.data(), packet.sampleCount ); // not frame count!.
      }

   } // The end of block:
     //     auto closes file and finishes header with correct data length.

*/

/// WAV audio file 'riff' section header
struct WavRiff
{
   uint8_t riff_char[4];
   uint32_t package_len;
   uint8_t wave[4];
};

/// WAV audio file 'format' section header
struct WavFormat
{
   uint8_t fmt[4];
   uint32_t format_len;
   uint16_t fixed;
   uint16_t channel_number;
   uint32_t sample_rate;
   uint32_t byte_rate;
   uint16_t byte_per_sample;
   uint16_t bits_per_sample;
};

/// WAV audio file 'fact' section header
struct WavFact
{
   uint8_t fact_field[4];
   uint32_t fact_len;
   uint32_t fact_sample_len;
};

/// WAV audio file 'data' section header
struct WavData
{
   uint8_t data_field[4];
   uint32_t data_len;
};

/// WAV audio file header
struct WavHeader
{
   WavRiff   riff;
   WavFormat format;
   WavFact   fact;
   WavData   data;
};

/// Base class for processing WAV audio files.
class WavFileBase
{
private:
   /// Conversion working buffer;
   uint8_t *convBuff;
   int32_t convBuffSize;

protected:
   WavFileBase();
   virtual ~WavFileBase();

   /// Get pointer to conversion buffer of at min. given size
   void *getConvBuffer(int32_t sizeByte);
};


/// Class for reading WAV audio files.
class WavInFile : protected WavFileBase
{
   DE_CREATE_LOGGER("de.audio.WavInFile")

private:
   /// File pointer.
   FILE *m_File;

   /// Position within the audio stream
   int32_t m_Position;

   /// Counter of how many bytes of sample data have been read from the file.
   int32_t m_BytesWritten;

   /// WAV header information
   WavHeader m_Header;

   /// Init the WAV file stream
   void init();

   /// Read WAV file headers.
   /// \return zero if all ok, nonzero if file format is invalid.
   int32_t readWavHeaders();

   /// Checks WAV file header tags.
   /// \return zero if all ok, nonzero if file format is invalid.
   int32_t checkCharTags() const;

   /// Reads a single WAV file header block.
   /// \return zero if all ok, nonzero if file format is invalid.
   int32_t readHeaderBlock();

   /// Reads WAV file 'riff' block
   int32_t readRIFFBlock();

public:
   /// Constructor: Opens the given WAV file. If the file can't be opened,
   /// throws 'runtime_error' exception.
   WavInFile(std::string const & filename);

   WavInFile(FILE *file);

   /// Destructor: Closes the file.
   ~WavInFile();

   // Benni function:
   bool is_open() const { return m_File != nullptr; }

   /// Rewind to beginning of the file
   void rewind();

   /// Get sample rate.
   uint32_t getSampleRate() const;

   /// Get number of bits per sample, i.e. 8 or 16.
   uint32_t getNumBits() const;

   /// Get sample data size in bytes. Ahem, this should return same information as
   /// 'getBytesPerSample'...
   uint32_t getDataSizeInBytes() const;

   /// Get total number of samples in file.
   uint32_t getNumSamples() const;

   /// Get number of bytes per audio sample (e.g. 16bit stereo = 4 bytes/sample)
   uint32_t getBytesPerSample() const;

   /// Get number of audio channels in the file (1=mono, 2=stereo)
   uint32_t getNumChannels() const;

   /// Get the audio file length in milliseconds
   uint32_t getLengthMS() const;

   /// Returns how many milliseconds of audio have so far been read from the file
   ///
   /// \return elapsed duration in milliseconds
   uint32_t getElapsedMS() const;

   /// Reads audio samples from the WAV file. This routine works only for 8 bit samples.
   /// Reads given number of elements from the file or if end-of-file reached, as many
   /// elements as are left in the file.
   ///
   /// \return Number of 8-bit integers read from the file.
   int32_t read(uint8_t *buffer, int32_t maxElems);

   /// Reads audio samples from the WAV file to 16 bit integer format. Reads given number
   /// of elements from the file or if end-of-file reached, as many elements as are
   /// left in the file.
   ///
   /// \return Number of 16-bit integers read from the file.
   int32_t
      read(
         int16_t *buffer,  ///< Pointer to buffer where to read data.
         int32_t maxElems  ///< Size of 'buffer' array (number of array elements).
      );

   /// Reads audio samples from the WAV file to floating point format, converting
   /// sample values to range [-1,1[. Reads given number of elements from the file
   /// or if end-of-file reached, as many elements as are left in the file.
   /// Notice that reading in float format supports 8/16/24/32bit sample formats.
   ///
   /// \return Number of elements read from the file.
   int32_t read(
      float *buffer,     ///< Pointer to buffer where to read data.
      int32_t maxElems       ///< Size of 'buffer' array (number of array elements).
   );

   /// Check end-of-file.
   ///
   /// \return Nonzero if end-of-file reached.
   int32_t eof() const;
};


/// Class for writing WAV audio files.
class WavOutFile : protected WavFileBase
{
private:
   /// Pointer to the WAV file
   FILE *m_File;

   /// WAV file header data.
   WavHeader m_Header;

   /// Counter of how many bytes have been written to the file so far.
   int32_t m_BytesWritten;

   /// Fills in WAV file header information.
   void fillInHeader(const uint32_t sampleRate, const uint32_t bits, const uint32_t channels);

   /// Finishes the WAV file header by supplementing information of amount of
   /// data written to file etc
   void finishHeader();

   /// Writes the WAV file header.
   void writeHeader();

public:
   /// Constructor: Creates a new WAV file. Throws a 'runtime_error' exception
   /// if file creation fails.
   WavOutFile(char const *fileName,    ///< Filename
   int32_t sampleRate,          ///< Sample rate (e.g. 44100 etc)
   int32_t bits,                ///< Bits per sample (8 or 16 bits)
   int32_t channels             ///< Number of channels (1=mono, 2=stereo)
   );

   WavOutFile(FILE *file, int32_t sampleRate, int32_t bits, int32_t channels);

   /// Destructor: Finalizes & closes the WAV file.
   ~WavOutFile();

   // Benni function:
   bool is_open() const { return m_File != nullptr; }

   /// Write data to WAV file. This function works only with 8bit samples.
   /// Throws a 'runtime_error' exception if writing to file fails.
   void
   write(
      const uint8_t *buffer, ///< Pointer to sample data buffer.
      int32_t numElems       ///< How many array items are to be written to file.
   );

   /// Write data to WAV file. Throws a 'runtime_error' exception if writing to
   /// file fails.
   void
   write_S16(
      const int16_t *buffer, ///< Pointer to sample data buffer.
      int32_t numElems       ///< How many array items are to be written to file.
   );

   /// Write data to WAV file in floating point format, saturating sample values to range
   /// [-1..+1[. Throws a 'runtime_error' exception if writing to file fails.
   void
   write(
      const float *buffer,   ///< Pointer to sample data buffer.
      int32_t numElems       ///< How many array items are to be written to file.
   );
};

} // end namespace wav
} // end namespace audio
} // end namespace de
