// Minimaler RF64-Writer mit m_ Prefix.
// Unterstützt PCM (1) und Float (3). Kein extensible, kein fancy Kram.

#include <cstdint>
#include <cstdio>
#include <stdexcept>

class RF64Writer {
public:
    RF64Writer(const char* path,
               uint32_t sampleRate,
               uint16_t channels,
               uint16_t bitsPerSample,
               bool isFloat)
        : m_file(nullptr),
          m_sampleRate(sampleRate),
          m_channels(channels),
          m_bitsPerSample(bitsPerSample),
          m_isFloat(isFloat),
          m_dataBytes(0),
          m_sampleCount(0),
          m_riffStart(0),
          m_dataChunkPos(0)
    {
        m_file = std::fopen(path, "wb");
        if (!m_file)
            throw std::runtime_error("RF64Writer: cannot open file");

        writeHeader();
    }

    ~RF64Writer() {
        if (m_file) {
            try { finalize(); } catch (...) {}
            std::fclose(m_file);
        }
    }

    void writeFrames(const void* frames, uint64_t frameCount) {
        const uint64_t bytesPerSample = m_bitsPerSample / 8;
        const uint64_t bytesPerFrame  = bytesPerSample * m_channels;
        const uint64_t bytesToWrite   = bytesPerFrame * frameCount;

        if (bytesToWrite == 0) return;

        if (std::fwrite(frames, 1, (size_t)bytesToWrite, m_file) != bytesToWrite)
            throw std::runtime_error("RF64Writer: write error");

        m_dataBytes   += bytesToWrite;
        m_sampleCount += frameCount;
    }

    void finalize() {
        if (!m_file) return;

        const uint64_t riffSize64 = fileSize() - m_riffStart - 8;
        const uint64_t dataSize64 = m_dataBytes;

        std::fflush(m_file);

        const uint64_t ds64Pos = m_riffStart + 12;
        std::fseek(m_file, (long)ds64Pos + 8, SEEK_SET);

        writeLE64(riffSize64);
        writeLE64(dataSize64);
        writeLE64(m_sampleCount);
        writeLE32(0);

        std::fflush(m_file);
        m_file = nullptr;
    }

private:
    FILE*    m_file;
    uint32_t m_sampleRate;
    uint16_t m_channels;
    uint16_t m_bitsPerSample;
    bool     m_isFloat;

    uint64_t m_dataBytes;
    uint64_t m_sampleCount;

    uint64_t m_riffStart;
    uint64_t m_dataChunkPos;

    static void writeLE16(FILE* f, uint16_t v) {
        uint8_t b[2] = { uint8_t(v), uint8_t(v >> 8) };
        std::fwrite(b, 1, 2, f);
    }

    static void writeLE32(FILE* f, uint32_t v) {
        uint8_t b[4] = {
            uint8_t(v),
            uint8_t(v >> 8),
            uint8_t(v >> 16),
            uint8_t(v >> 24)
        };
        std::fwrite(b, 1, 4, f);
    }

    static void writeLE64(FILE* f, uint64_t v) {
        uint8_t b[8] = {
            uint8_t(v),
            uint8_t(v >> 8),
            uint8_t(v >> 16),
            uint8_t(v >> 24),
            uint8_t(v >> 32),
            uint8_t(v >> 40),
            uint8_t(v >> 48),
            uint8_t(v >> 56)
        };
        std::fwrite(b, 1, 8, f);
    }

    static void writeTag(FILE* f, const char tag[4]) {
        std::fwrite(tag, 1, 4, f);
    }

    uint64_t fileSize() {
        std::fflush(m_file);
        long cur = std::ftell(m_file);
        std::fseek(m_file, 0, SEEK_END);
        long end = std::ftell(m_file);
        std::fseek(m_file, cur, SEEK_SET);
        return (uint64_t)end;
    }

    void writeHeader() {
        m_riffStart = 0;

        writeTag(m_file, "RF64");
        writeLE32(m_file, 0xFFFFFFFFu);
        writeTag(m_file, "WAVE");

        writeTag(m_file, "ds64");
        writeLE32(m_file, 28);
        writeLE64(m_file, 0);
        writeLE64(m_file, 0);
        writeLE64(m_file, 0);
        writeLE32(m_file, 0);

        writeTag(m_file, "fmt ");
        writeLE32(m_file, 16);

        uint16_t audioFormat = m_isFloat ? 3 : 1;
        writeLE16(m_file, audioFormat);
        writeLE16(m_file, m_channels);
        writeLE32(m_file, m_sampleRate);

        uint32_t bytesPerSample = m_bitsPerSample / 8;
        uint32_t byteRate       = m_sampleRate * m_channels * bytesPerSample;
        uint16_t blockAlign     = m_channels * bytesPerSample;

        writeLE32(m_file, byteRate);
        writeLE16(m_file, blockAlign);
        writeLE16(m_file, m_bitsPerSample);

        writeTag(m_file, "data");
        m_dataChunkPos = (uint64_t)std::ftell(m_file);
        writeLE32(m_file, 0xFFFFFFFFu);
    }
};
