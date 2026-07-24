#pragma once
#include <de/sound/Sound.h>

// #ifdef USE_LIBSNDFILE
#include <sndfile.h>

namespace de {
namespace sound {

struct Utils
{
    static int
    getSndFormatFromFileExt(const std::string & ext)
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
    getFormatStr(int format)
    {
        int ext = format & 0x0FFF0000;
        switch(ext)
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
        default: return dbStr("SF_FORMAT_UNKNOWN with ",format);
        }
    }

    static std::string
    getSampleTypeStr(int format)
    {
        int subtype = format & SF_FORMAT_SUBMASK;
        switch (subtype)
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
            default: return dbStr("SF_SUBTYPE_UNKNOWN_",subtype);
        }
    }

    static std::string
    getEndianessStr(int format)
    {
        int endian = format & SF_FORMAT_ENDMASK;
        switch (endian)
        {
            case SF_ENDIAN_FILE: return "ENDIAN_FILE";
            case SF_ENDIAN_LITTLE: return "ENDIAN_LITTLE";
            case SF_ENDIAN_BIG:  return "ENDIAN_BIG";
            case SF_ENDIAN_CPU: return "ENDIAN_CPU";
            default: return dbStr("ENDIAN_UNKNOWN_",endian);
        }
    }

    static uint32_t
    getBitsPerSampleFromSndFormat(int format )
    {
        uint32_t result = 0;
        switch ( format )
        {
            case SF_FORMAT_PCM_S8: result = 8; break;
            case SF_FORMAT_PCM_16: result = 16; break;
            case SF_FORMAT_PCM_24: result = 24; break;
            case SF_FORMAT_PCM_32: result = 32; break; /* Signed 32 bit data */
            case SF_FORMAT_PCM_U8: result = 8; break;
            case SF_FORMAT_FLOAT: result = 32; break;
            case SF_FORMAT_DOUBLE: result = 64; break;
            case SF_FORMAT_ULAW: result = 8; break;
            case SF_FORMAT_ALAW: result = 8; break;
            case SF_FORMAT_IMA_ADPCM: result = 16; break;
            case SF_FORMAT_MS_ADPCM: result = 16; break;
            default:
                DE_ERROR("Unsupported SNDFILE format ",format)
                break;
        }

        return result;
    }

    static Sound::eSampleType
    getSampleTypeFromFormat( int format )
    {
        Sound::eSampleType result = Sound::ST_Unknown;
        switch ( format )
        {
            case SF_FORMAT_PCM_S8: result = Sound::ST_S8; break;
            case SF_FORMAT_PCM_16: result = Sound::ST_S16; break;
            case SF_FORMAT_PCM_24: result = Sound::ST_S24; break;
            case SF_FORMAT_PCM_32: result = Sound::ST_S32; break; /* Signed 32 bit data */
            //case SF_FORMAT_PCM_U8: result = Sound::ST_U8; break;
            case SF_FORMAT_FLOAT: result = Sound::ST_F32; break;
            case SF_FORMAT_DOUBLE: result = Sound::ST_F64; break;
            //        case SF_FORMAT_ULAW: result = EST_ULAW_8; break;
            //        case SF_FORMAT_ALAW: result = EST_ALAW_8; break;
            //        case SF_FORMAT_IMA_ADPCM: result = EST_S16; break;
            //        case SF_FORMAT_MS_ADPCM: result = EST_S16; break;
            default:
                DE_ERROR("Unsupported SNDFILE format ",format)
                break;
        }

        return result;
    }

};


} // end namespace sound
} // end namespace de
