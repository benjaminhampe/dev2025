// Minimaler RF64-Reader mit m_ Prefix.
// Liest RF64 (ds64), fmt, data. Kein extensible, kein fancy Kram.

#include <cstdint>
#include <cstdio>
#include <stdexcept>
#include <cstring>

class RF64Reader {
public:
    RF64Reader(const char* path)
        : m_file(nullptr),
          m_sampleRate(0),
          m_channels(0),
          m_bitsPerSample(0),
          m_isFloat(false),
          m_dataOffset(0),
          m_dataSize64(0),
          m_sampleCount64(0)
    {
        m_file = std::fopen(path, "rb");
        if (!m_file)
            throw std::runtime_error("RF64Reader: cannot open file");

        parseHeader();
    }

    ~RF64Reader() {
        if (m_file)
            std::fclose(m_file);
    }

    uint32_t sampleRate() const { return m_sampleRate; }
    uint16_t channels()   const { return m_channels; }
    uint16_t bitsPerSample() const { return m_bitsPerSample; }
    bool     isFloat()    const { return m_isFloat; }

    uint64_t dataSize()   const { return m_dataSize64; }
    uint64_t frameCount() const { return m_sampleCount64; }

    // Liest raw interleaved frames in den Buffer.
    // frameCount = Anzahl Frames, nicht Bytes.
    uint64_t readFrames(void* dst, uint64_t frameCount) {
        const uint64_t bytesPerSample = m_bitsPerSample / 8;
        const uint64_t bytesPerFrame  = bytesPerSample * m_channels;
        const uint64_t bytesToRead    = frameCount * bytesPerFrame;

        const uint64_t cur = currentDataPos();
        const uint64_t remaining = m_dataSize64 - cur;

        const uint64_t actualBytes = (bytesToRead <= remaining) ? bytesToRead : remaining;
        if (actualBytes == 0) return 0;

        const size_t n = std::fread(dst, 1, (size_t)actualBytes, m_file);
        return n / bytesPerFrame;
    }

    // Setzt den Lesepointer relativ zum Datenbereich.
    void seekFrame(uint64_t frameIndex) {
        const uint64_t bytesPerSample = m_bitsPerSample / 8;
        const uint64_t bytesPerFrame  = bytesPerSample * m_channels;
        const uint64_t bytePos        = m_dataOffset + frameIndex * bytesPerFrame;

        if (bytePos > m_dataOffset + m_dataSize64)
            throw std::runtime_error("RF64Reader: seek out of range");

        std::fseek(m_file, (long)bytePos, SEEK_SET);
    }

private:
    FILE*    m_file;

    uint32_t m_sampleRate;
    uint16_t m_channels;
    uint16_t m_bitsPerSample;
    bool     m_isFloat;

    uint64_t m_dataOffset;
    uint64_t m_dataSize64;
    uint64_t m_sampleCount64;

    static uint16_t readLE16(FILE* f) {
        uint8_t b[2];
        std::fread(b, 1, 2, f);
        return (uint16_t)(b[0] | (b[1] << 8));
    }

    static uint32_t readLE32(FILE* f) {
        uint8_t b[4];
        std::fread(b, 1, 4, f);
        return (uint32_t)(b[0] | (b[1] << 8) | (b[2] << 16) | (b[3] << 24));
    }

    static uint64_t readLE64(FILE* f) {
        uint8_t b[8];
        std::fread(b, 1, 8, f);
        return (uint64_t)b[0]
             | ((uint64_t)b[1] << 8)
             | ((uint64_t)b[2] << 16)
             | ((uint64_t)b[3] << 24)
             | ((uint64_t)b[4] << 32)
             | ((uint64_t)b[5] << 40)
             | ((uint64_t)b[6] << 48)
             | ((uint64_t)b[7] << 56);
    }

    static void readTag(FILE* f, char out[4]) {
        std::fread(out, 1, 4, f);
    }

    uint64_t currentDataPos() {
        long pos = std::ftell(m_file);
        return (uint64_t)pos - m_dataOffset;
    }

    void parseHeader() {
        char tag[4];

        // "RF64"
        readTag(m_file, tag);
        if (std::memcmp(tag, "RF64", 4) != 0)
            throw std::runtime_error("RF64Reader: not RF64");

        uint32_t riffSize32 = readLE32(m_file); // ignored (0xFFFFFFFF)
        readTag(m_file, tag);
        if (std::memcmp(tag, "WAVE", 4) != 0)
            throw std::runtime_error("RF64Reader: missing WAVE");

        // ds64
        readTag(m_file, tag);
        if (std::memcmp(tag, "ds64", 4) != 0)
            throw std::runtime_error("RF64Reader: missing ds64");

        uint32_t ds64Size = readLE32(m_file);
        if (ds64Size < 28)
            throw std::runtime_error("RF64Reader: invalid ds64 size");

        uint64_t riffSize64   = readLE64(m_file);
        m_dataSize64          = readLE64(m_file);
        m_sampleCount64       = readLE64(m_file);
        uint32_t reserved     = readLE32(m_file);

        // fmt
        readTag(m_file, tag);
        if (std::memcmp(tag, "fmt ", 4) != 0)
            throw std::runtime_error("RF64Reader: missing fmt");

        uint32_t fmtSize = readLE32(m_file);
        if (fmtSize != 16)
            throw std::runtime_error("RF64Reader: only PCM/float supported");

        uint16_t audioFormat = readLE16(m_file);
        m_isFloat = (audioFormat == 3);

        m_channels      = readLE16(m_file);
        m_sampleRate    = readLE32(m_file);

        uint32_t byteRate   = readLE32(m_file);
        uint16_t blockAlign = readLE16(m_file);
        m_bitsPerSample     = readLE16(m_file);

        // data
        readTag(m_file, tag);
        if (std::memcmp(tag, "data", 4) != 0)
            throw std::runtime_error("RF64Reader: missing data chunk");

        uint32_t dataSize32 = readLE32(m_file); // should be 0xFFFFFFFF

        m_dataOffset = (uint64_t)std::ftell(m_file);
    }
};
