#pragma once
#include <de/sound/Sound.h>

// #ifdef USE_LIBSNDFILE
#include <sndfile.h>

namespace de {
namespace sound {

struct Utils
{
    static int
    getSndTypeFromFileExt(const std::string & ext)
    {
        if (ext.empty())
        {
            DE_ERROR("Got empty suffix")
            return -1;
        }
             if (ext=="wav")  return SF_FORMAT_WAV;
        else if (ext=="ogg")  return SF_FORMAT_OGG;
        else if (ext=="aif")  return SF_FORMAT_AIFF;
        else if (ext=="aiff") return SF_FORMAT_AIFF;
        else if (ext=="flac") return SF_FORMAT_FLAC;
        else if (ext=="au")   return SF_FORMAT_AU;
        else if (ext=="raw")  return SF_FORMAT_RAW;
        else if (ext=="paf")  return SF_FORMAT_PAF;
        else if (ext=="svx")  return SF_FORMAT_SVX;
        else if (ext=="nist") return SF_FORMAT_NIST;
        else if (ext=="voc")  return SF_FORMAT_VOC;
        else if (ext=="sf")   return SF_FORMAT_IRCAM;
        else if (ext=="w64")  return SF_FORMAT_W64;
        else if (ext=="mat4") return SF_FORMAT_MAT4;
        else if (ext=="mat5") return SF_FORMAT_MAT5;
        else if (ext=="pvf")  return SF_FORMAT_PVF;
        else if (ext=="xi")   return SF_FORMAT_XI;
        else if (ext=="htk")  return SF_FORMAT_HTK;
        else if (ext=="sds")  return SF_FORMAT_SDS;
        else if (ext=="avr")  return SF_FORMAT_AVR;
        else if (ext=="sd2")  return SF_FORMAT_SD2;
        else if (ext=="caf")  return SF_FORMAT_CAF;
        else if (ext=="wve")  return SF_FORMAT_WVE;
        else if (ext=="mpc2k")return SF_FORMAT_MPC2K;
        else if (ext=="rf64") return SF_FORMAT_RF64;
        else
        {
            DE_ERROR("Unsupported suffix for SNDFILE ",ext)
            return -1;
        }
    }

    static std::string
    getSndTypeStr(int format)
    {
        switch( format & SF_FORMAT_TYPEMASK )
        {
        case SF_FORMAT_WAV: return "WAV";
        case SF_FORMAT_OGG: return "OGG";
        case SF_FORMAT_AIFF: return "AIFF";
        case SF_FORMAT_FLAC: return "FLAC";
        case SF_FORMAT_AU: return "AU";
        case SF_FORMAT_RAW: return "RAW";
        case SF_FORMAT_PAF: return "PAF";
        case SF_FORMAT_SVX: return "SVX";
        case SF_FORMAT_NIST: return "NIST";
        case SF_FORMAT_VOC: return "VOC";
        case SF_FORMAT_IRCAM: return "IRCAM";
        case SF_FORMAT_W64: return "W64";
        case SF_FORMAT_MAT4: return "MAT4";
        case SF_FORMAT_MAT5: return "MAT5";
        case SF_FORMAT_PVF: return "PVF";
        case SF_FORMAT_XI: return "XI";
        case SF_FORMAT_HTK: return "HTK";
        case SF_FORMAT_SDS: return "SDS";
        case SF_FORMAT_AVR: return "AVR";
        case SF_FORMAT_SD2: return "SD2";
        case SF_FORMAT_CAF: return "CAF";
        case SF_FORMAT_WVE: return "WVE";
        case SF_FORMAT_MPC2K: return "MPC2K";
        case SF_FORMAT_RF64: return "RF64";
        default:
            return dbStr("SF_FORMAT_UNKNOWN with ",format & SF_FORMAT_TYPEMASK);
        }
    }

    static std::string
    getSndSampleTypeStr(int format)
    {
        switch ( format & SF_FORMAT_SUBMASK )
        {
            case SF_FORMAT_PCM_S8: return "PCM_S8";
            case SF_FORMAT_PCM_16: return "PCM_16";
            case SF_FORMAT_PCM_24: return "PCM_24";
            case SF_FORMAT_PCM_32: return "PCM_32";
            case SF_FORMAT_PCM_U8: return "PCM_U8";
            case SF_FORMAT_FLOAT: return "FLOAT";
            case SF_FORMAT_DOUBLE: return "DOUBLE";
            case SF_FORMAT_ULAW: return "ULAW";
            case SF_FORMAT_ALAW: return "ALAW";
            case SF_FORMAT_IMA_ADPCM: return "IMA_ADPCM";
            case SF_FORMAT_MS_ADPCM: return "MS_ADPCM";
            case SF_FORMAT_GSM610: return "GSM610";
            case SF_FORMAT_VOX_ADPCM: return "VOX_ADPCM";
            case SF_FORMAT_G721_32: return "G721_32";
            case SF_FORMAT_G723_24: return "G723_24";
            case SF_FORMAT_G723_40: return "G723_40";
            case SF_FORMAT_DWVW_12: return "DWVW_12";
            case SF_FORMAT_DWVW_16: return "DWVW_16";
            case SF_FORMAT_DWVW_24: return "DWVW_24";
            case SF_FORMAT_DWVW_N: return "DWVW_N";
            case SF_FORMAT_DPCM_8: return "DPCM_8";
            case SF_FORMAT_DPCM_16: return "DPCM_16";
            case SF_FORMAT_VORBIS: return "VORBIS";
            case SF_FORMAT_ALAC_16: return "ALAC_16";
            case SF_FORMAT_ALAC_20: return "ALAC_20";
            case SF_FORMAT_ALAC_24: return "ALAC_24";
            case SF_FORMAT_ALAC_32: return "ALAC_32";
            default: return dbStr("SF_SUBTYPE_UNKNOWN_",format & SF_FORMAT_SUBMASK);
        }
    }

    static std::string
    getSndEndianStr(int format)
    {
        switch ( format & SF_FORMAT_ENDMASK )
        {
            case SF_ENDIAN_FILE: return "ENDIAN_FILE";
            case SF_ENDIAN_LITTLE: return "ENDIAN_LITTLE";
            case SF_ENDIAN_BIG:  return "ENDIAN_BIG";
            case SF_ENDIAN_CPU: return "ENDIAN_CPU";
            default: return dbStr("ENDIAN_UNKNOWN_",format & SF_FORMAT_ENDMASK);
        }
    }

    static uint32_t
    getBitsPerSample(int format )
    {
        switch ( format & SF_FORMAT_SUBMASK )
        {
            case SF_FORMAT_PCM_S8: return 8;
            case SF_FORMAT_PCM_16: return 16;
            case SF_FORMAT_PCM_24: return 24;
            case SF_FORMAT_PCM_32: return 32; /* Signed 32 bit data */
            case SF_FORMAT_PCM_U8: return 8;
            case SF_FORMAT_FLOAT: return 32;
            case SF_FORMAT_DOUBLE: return 64;
            case SF_FORMAT_ULAW: return 8;
            case SF_FORMAT_ALAW: return 8;
            case SF_FORMAT_IMA_ADPCM: return 16;
            case SF_FORMAT_MS_ADPCM: return 16;
            default:
                DE_ERROR("Unsupported SNDFILE format ",format & SF_FORMAT_SUBMASK)
                return 0;
        }
    }

    static SampleType
    getSampleType( int format )
    {
        switch ( format & SF_FORMAT_SUBMASK )
        {
            case SF_FORMAT_PCM_S8: return SampleType::S8;
            case SF_FORMAT_PCM_16: return SampleType::S16;
            case SF_FORMAT_PCM_24: return SampleType::S24;
            case SF_FORMAT_PCM_32: return SampleType::S32; /* Signed 32 bit data */
            case SF_FORMAT_PCM_U8: return SampleType::U8;
            case SF_FORMAT_FLOAT: return SampleType::F32;
            case SF_FORMAT_DOUBLE: return SampleType::F64;
            //        case SF_FORMAT_ULAW: return EST_ULAW_8;
            //        case SF_FORMAT_ALAW: return EST_ALAW_8;
            //        case SF_FORMAT_IMA_ADPCM: return EST_S16;
            //        case SF_FORMAT_MS_ADPCM: return EST_S16;
            default:
                DE_ERROR("Unsupported SNDFILE format ",format & SF_FORMAT_SUBMASK)
                return SampleType::Unknown;
        }
    }

    static int
    getSndSampleType( SampleType sampleType )
    {
        switch ( sampleType )
        {
            case SampleType::U8: return SF_FORMAT_PCM_U8;
            case SampleType::S8: return SF_FORMAT_PCM_S8;
            case SampleType::S16: return SF_FORMAT_PCM_16;
            case SampleType::S24: return SF_FORMAT_PCM_24;
            case SampleType::S32: return SF_FORMAT_PCM_32;
            case SampleType::F32: return SF_FORMAT_FLOAT;
            case SampleType::F64: return SF_FORMAT_DOUBLE;
            default:
                DE_ERROR("Unsupported SampleType ",sampleType.str())
                return 0;
        }
    }
};


} // end namespace sound
} // end namespace de
