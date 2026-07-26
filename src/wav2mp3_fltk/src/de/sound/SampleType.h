#pragma once
#include <de/Core.h>
#include <de/AlignedMemory.h>

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
        U8
    };

    uint8_t m_sampleType;

    SampleType() : m_sampleType(Unknown) {}
    SampleType(eSampleType sampleType) : m_sampleType(sampleType) {}
    std::string str() const noexcept { return getString(m_sampleType); }
    operator int32_t() const noexcept { return m_sampleType; }
    int32_t bitsPerSample() const { return getBitsPerSample(m_sampleType); }
    int32_t bytesPerSample() const { return getBytesPerSample(m_sampleType); }

    static std::string getString(int sampleType) noexcept
    {
        switch (sampleType)
        {
            case S8: return "ST_S8";
            case S16: return "ST_S16";
            case S24: return "ST_S24";
            case S32: return "ST_S32";
            case F32: return "ST_F32";
            case F64: return "ST_F64";
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
