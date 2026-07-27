#if 0

#include <cstdio>
#include <cstdint>
#include <vector>
#include <string>
#include <cmath>

struct AiffInfo {
    bool        isAifc      = false;
    uint16_t    channels    = 0;
    uint32_t    frames      = 0;
    uint16_t    bitDepth    = 0;
    double      sampleRate  = 0.0;
    uint32_t    ssndOffset  = 0;
    uint32_t    ssndBlockSize = 0;
    long        ssndDataPos = 0;
};

static uint16_t read_be16(FILE* f) {
    uint8_t b[2];
    fread(b, 1, 2, f);
    return (uint16_t(b[0]) << 8) | uint16_t(b[1]);
}

static uint32_t read_be32(FILE* f) {
    uint8_t b[4];
    fread(b, 1, 4, f);
    return (uint32_t(b[0]) << 24) |
           (uint32_t(b[1]) << 16) |
           (uint32_t(b[2]) << 8)  |
           (uint32_t(b[3]));
}

// 80-bit IEEE extended float -> double (SampleRate)
static double read_ieee_extended(FILE* f) {
    uint8_t b[10];
    fread(b, 1, 10, f);

    int16_t exponent = (int16_t(((b[0] & 0x7F) << 8) | b[1]));
    uint64_t hiMant = (uint64_t(b[2]) << 24) |
                      (uint64_t(b[3]) << 16) |
                      (uint64_t(b[4]) << 8)  |
                      (uint64_t(b[5]));
    uint64_t loMant = (uint64_t(b[6]) << 24) |
                      (uint64_t(b[7]) << 16) |
                      (uint64_t(b[8]) << 8)  |
                      (uint64_t(b[9]));

    if (exponent == 0 && hiMant == 0 && loMant == 0)
        return 0.0;

    double fMant = hiMant * std::pow(2.0, -31.0) +
                   loMant * std::pow(2.0, -63.0);
    double fExp = std::pow(2.0, exponent - 16383);
    double value = fMant * fExp;

    if (b[0] & 0x80)
        value = -value;

    return value;
}

bool parse_aiff(const std::string& path, AiffInfo& info, std::vector<float>* outPCM = nullptr)
{
    FILE* f = std::fopen(path.c_str(), "rb");
    if (!f) return false;

    char id[4];
    fread(id, 1, 4, f);
    if (std::memcmp(id, "FORM", 4) != 0) {
        std::fclose(f);
        return false;
    }

    uint32_t formSize = read_be32(f);
    (void)formSize;

    fread(id, 1, 4, f);
    if (std::memcmp(id, "AIFF", 4) == 0) {
        info.isAifc = false;
    } else if (std::memcmp(id, "AIFC", 4) == 0) {
        info.isAifc = true;
    } else {
        std::fclose(f);
        return false;
    }

    bool haveCOMM = false;
    bool haveSSND = false;

    while (!haveCOMM || !haveSSND) {
        if (fread(id, 1, 4, f) != 4)
            break;

        uint32_t chunkSize = read_be32(f);
        long chunkStart = std::ftell(f);

        if (std::memcmp(id, "COMM", 4) == 0) {
            info.channels = read_be16(f);
            info.frames   = read_be32(f);
            info.bitDepth = read_be16(f);
            info.sampleRate = read_ieee_extended(f);

            if (info.isAifc) {
                char compType[4];
                fread(compType, 1, 4, f);
                // optional: check compression type
            }

            haveCOMM = true;
        }
        else if (std::memcmp(id, "SSND", 4) == 0) {
            info.ssndOffset    = read_be32(f);
            info.ssndBlockSize = read_be32(f);
            info.ssndDataPos   = std::ftell(f) + info.ssndOffset;
            haveSSND = true;
        }

        std::fseek(f, chunkStart + chunkSize + (chunkSize & 1), SEEK_SET);
    }

    if (!haveCOMM || !haveSSND) {
        std::fclose(f);
        return false;
    }

    if (outPCM) {
        std::fseek(f, info.ssndDataPos, SEEK_SET);

        uint32_t frames = info.frames;
        uint16_t ch     = info.channels;
        uint16_t bits   = info.bitDepth;

        outPCM->resize(size_t(frames) * ch);

        if (bits == 16) {
            for (uint32_t i = 0; i < frames; ++i) {
                for (uint16_t c = 0; c < ch; ++c) {
                    int16_t s = (int16_t)read_be16(f);
                    (*outPCM)[size_t(i) * ch + c] = float(s) / 32768.0f;
                }
            }
        } else if (bits == 24) {
            for (uint32_t i = 0; i < frames; ++i) {
                for (uint16_t c = 0; c < ch; ++c) {
                    uint8_t b[3];
                    fread(b, 1, 3, f);
                    int32_t v = (int32_t(b[0]) << 16) |
                                (int32_t(b[1]) << 8)  |
                                (int32_t(b[2]));
                    if (v & 0x800000)
                        v |= 0xFF000000;
                    (*outPCM)[size_t(i) * ch + c] = float(v) / 8388608.0f;
                }
            }
        } else if (bits == 32) {
            for (uint32_t i = 0; i < frames; ++i) {
                for (uint16_t c = 0; c < ch; ++c) {
                    uint8_t b[4];
                    fread(b, 1, 4, f);
                    uint32_t v = (uint32_t(b[0]) << 24) |
                                 (uint32_t(b[1]) << 16) |
                                 (uint32_t(b[2]) << 8)  |
                                 (uint32_t(b[3]));
                    int32_t s = (int32_t)v;
                    (*outPCM)[size_t(i) * ch + c] = float(s) / 2147483648.0f;
                }
            }
        } else {
            std::fclose(f);
            return false;
        }
    }

    std::fclose(f);
    return true;
}

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::printf("Usage: aiff_parser <file.aiff>\n");
        return 1;
    }

    AiffInfo info;
    std::vector<float> pcm;

    if (!parse_aiff(argv[1], info, &pcm)) {
        std::printf("Failed to parse AIFF\n");
        return 1;
    }

    std::printf("AIFF: %s\n", info.isAifc ? "AIFC" : "AIFF");
    std::printf("Channels: %u\n", info.channels);
    std::printf("Frames:   %u\n", info.frames);
    std::printf("BitDepth: %u\n", info.bitDepth);
    std::printf("SampleRate: %.2f\n", info.sampleRate);
    std::printf("PCM samples: %zu\n", pcm.size());

    return 0;
}

#endif