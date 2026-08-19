#pragma once
#include <de/Core.h>

namespace de {

// ===================================================================
class SampleType
// ===================================================================
{
public:
    enum eSampleType : uint8_t
    {
        Unknown = 0,
        F32,
        F64,
        S16,
        S24,
        S32,
        S8,
        U8,
        // S12, // WAV spec supports that! Not me
    };

    uint8_t m_sampleType;

    SampleType() noexcept : m_sampleType(Unknown) {}
    SampleType(eSampleType sampleType) noexcept : m_sampleType(sampleType) {}
    std::string str() const noexcept { return getString(m_sampleType); }
    operator int32_t() const noexcept { return m_sampleType; }
    int32_t bitsPerSample() const noexcept { return getBitsPerSample(m_sampleType); }
    int32_t bytesPerSample() const noexcept { return getBytesPerSample(m_sampleType); }

    static std::string getString(int sampleType) noexcept
    {
        switch (sampleType)
        {
            case U8: return "U8";
            case S8: return "S8";
            // case S12: return "S12";
            case S16: return "S16";
            case S24: return "S24";
            case S32: return "S32";
            case F32: return "F32";
            case F64: return "F64";
            default: return "Unknown";
        }
    }

    static int32_t getBitsPerSample(int sampleType) noexcept
    {
        switch (sampleType)
        {
            case S8: return 8;
            case S16: return 16;
            case S24: return 24;
            case S32: return 32;
            case F32: return 32;
            case F64: return 64;
            default:
            {
                DE_ERROR("Invalid sampleType ",getString(sampleType))
                return 0;
            }
        }
    }

    static int32_t getBytesPerSample(int sampleType) noexcept
    {
        switch (sampleType)
        {
            case S8: return 1;
            case S16: return 2;
            case S24: return 3;
            case S32: return 4;
            case F32: return 4;
            case F64: return 8;
            default:
            {
                DE_ERROR("Invalid sampleType ",getString(sampleType))
                return 0;
            }
        }
    }

};

} // end namespace de.
