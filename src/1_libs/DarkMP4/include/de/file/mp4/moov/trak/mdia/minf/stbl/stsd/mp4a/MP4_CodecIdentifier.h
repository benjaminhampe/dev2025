enum MP4_CodecIdentifier : uint8_t
{
    // 0x00–0x0F — MPEG-4 Systems / Reserved
    OTI_FORBIDDEN_00           = 0x00, // Reserved/invalid. Used by YouTube for VP9/AV1 in MP4 (no ESDS).
    OTI_MPEG4_SYSTEMS_01       = 0x01, // ISO/IEC 14496-1 Systems. Rare in MP4 files.
    OTI_MPEG4_SYSTEMS_02       = 0x02, // ISO/IEC 14496-1 Systems extension. Rare.
    OTI_MPEG4_INTERACTIVE_03   = 0x03, // Interactive graphics. Not used in modern MP4.
    OTI_RESERVED_04            = 0x04, // Reserved. No known usage.
    OTI_RESERVED_05            = 0x05, // Reserved. No known usage.
    OTI_RESERVED_06            = 0x06, // Reserved. No known usage.
    OTI_RESERVED_07            = 0x07, // Reserved. No known usage.
    OTI_RESERVED_08            = 0x08, // Reserved. No known usage.
    OTI_RESERVED_09            = 0x09, // Reserved. No known usage.
    OTI_RESERVED_0A            = 0x0A, // Reserved. No known usage.
    OTI_RESERVED_0B            = 0x0B, // Reserved. No known usage.
    OTI_RESERVED_0C            = 0x0C, // Reserved. No known usage.
    OTI_RESERVED_0D            = 0x0D, // Reserved. No known usage.
    OTI_RESERVED_0E            = 0x0E, // Reserved. No known usage.
    OTI_RESERVED_0F            = 0x0F, // Reserved. No known usage.

    // 0x10–0x1F — AAC Audio Object Types (ASC only, not ESDS)
    AOT_AAC_MAIN               = 0x01, // AAC Main profile. Supported by FFmpeg; rarely used in MP4.
    AOT_AAC_LC                 = 0x02, // AAC Low Complexity. Most common AAC profile (Apple, YouTube, Android).
    AOT_AAC_SSR                = 0x03, // AAC SSR. Deprecated; no modern encoder uses it.
    AOT_AAC_LTP                = 0x04, // AAC LTP. Rare; supported by FFmpeg.
    AOT_SBR                    = 0x05, // Spectral Band Replication (HE-AAC). Used by streaming services.
    AOT_AAC_SCAL               = 0x06, // AAC Scalable. Rare.
    AOT_TWINVQ                 = 0x07, // TwinVQ. Obsolete; no MP4 usage.
    AOT_CELP                   = 0x08, // CELP speech codec. Not used in MP4.
    AOT_HVXC                   = 0x09, // HVXC speech codec. Not used in MP4.
    AOT_RESERVED_0A            = 0x0A, // Reserved.
    AOT_RESERVED_0B            = 0x0B, // Reserved.
    AOT_RESERVED_0C            = 0x0C, // Reserved.
    AOT_RESERVED_0D            = 0x0D, // Reserved.
    AOT_RESERVED_0E            = 0x0E, // Reserved.
    AOT_RESERVED_0F            = 0x0F, // Reserved.
    AOT_ER_AAC_LC              = 0x11, // Error-resilient AAC LC. Used in some broadcast profiles.
    AOT_ER_AAC_LTP             = 0x13, // ER AAC LTP. Rare.
    AOT_ER_AAC_SCAL            = 0x14, // ER AAC scalable. Rare.
    AOT_ER_AAC_LD              = 0x17, // AAC Low Delay. Used in VoIP, not common in MP4.
    AOT_PS                     = 0x1D, // Parametric Stereo (HE-AAC v2). Used by streaming services.
    AOT_ER_AAC_ELD             = 0x27, // Enhanced Low Delay. Used in FaceTime/VoIP.

    // 0x20–0x2F — MPEG‑4 Visual ObjectTypeIndications
    OTI_MPEG4_VIS_SIMPLE        = 0x20, // MPEG‑4 Visual Simple Profile. Used in early MP4; supported by FFmpeg/GPAC.
    OTI_MPEG4_VIS_SIMPLE_SCAL   = 0x21, // Simple Scalable Profile. Rare; mostly academic/legacy.
    OTI_MPEG4_VIS_CORE          = 0x22, // Core Profile. Supported by FFmpeg; uncommon in modern MP4.
    OTI_MPEG4_VIS_MAIN          = 0x23, // Main Profile. Used in early MPEG‑4 ASP encoders (DivX/Xvid era).
    OTI_MPEG4_VIS_NBIT          = 0x24, // N‑bit profile. Rare; not used in consumer MP4.
    OTI_MPEG4_VIS_SCAL_TEX      = 0x25, // Scalable Texture. Research/VRML; not used in MP4.
    OTI_MPEG4_VIS_SIMPLE_FBA    = 0x26, // Face Animation (FBA). MPEG‑4 synthetic objects; unused today.
    OTI_MPEG4_VIS_SIMPLE_FACE   = 0x27, // Simple Face Animation. Same as above; no real MP4 usage.
    OTI_MPEG4_VIS_BASIC_ANIM    = 0x28, // Basic Animated Texture. Rare; not used in real MP4 files.
    OTI_MPEG4_VIS_HYBRID        = 0x29, // Hybrid profile. Experimental; no modern usage.
    OTI_MPEG4_VIS_ADV_REALTIME  = 0x2A, // Advanced Real Time. Rare; supported by FFmpeg.
    OTI_MPEG4_VIS_CORE_SCAL     = 0x2B, // Core Scalable. Rare; not used in consumer MP4.
    OTI_MPEG4_VIS_ADV_CODING    = 0x2C, // Advanced Coding (MPEG‑4 ASP). Used by DivX/Xvid in AVI, rarely MP4.
    OTI_MPEG4_VIS_ADV_CORE      = 0x2D, // Advanced Core. Rare; legacy.
    OTI_MPEG4_VIS_ADV_SCAL_TEX  = 0x2E, // Advanced Scalable Texture. Research; not used in MP4.
    OTI_RESERVED_2F             = 0x2F, // Reserved. No known usage.

    // 0x30–0x3F — Reserved / Unused
    OTI_RESERVED_30             = 0x30, // Reserved. No known usage.
    OTI_RESERVED_31             = 0x31, // Reserved. No known usage.
    OTI_RESERVED_32             = 0x32, // Reserved. No known usage.
    OTI_RESERVED_33             = 0x33, // Reserved. No known usage.
    OTI_RESERVED_34             = 0x34, // Reserved. No known usage.
    OTI_RESERVED_35             = 0x35, // Reserved. No known usage.
    OTI_RESERVED_36             = 0x36, // Reserved. No known usage.
    OTI_RESERVED_37             = 0x37, // Reserved. No known usage.
    OTI_RESERVED_38             = 0x38, // Reserved. No known usage.
    OTI_RESERVED_39             = 0x39, // Reserved. No known usage.
    OTI_RESERVED_3A             = 0x3A, // Reserved. No known usage.
    OTI_RESERVED_3B             = 0x3B, // Reserved. No known usage.
    OTI_RESERVED_3C             = 0x3C, // Reserved. No known usage.
    OTI_RESERVED_3D             = 0x3D, // Reserved. No known usage.
    OTI_RESERVED_3E             = 0x3E, // Reserved. No known usage.
    OTI_RESERVED_3F             = 0x3F, // Reserved. No known usage.


    // 0x40–0x5F — MPEG‑4 Audio (AAC) + gap
    OTI_MPEG4_AUDIO            = 0x40, // MPEG‑4 AAC (ISO/IEC 14496-3). Used by Apple, YouTube, Android, FFmpeg, GPAC.
    OTI_RESERVED_41            = 0x41, // Reserved. No known usage.
    OTI_RESERVED_42            = 0x42, // Reserved. No known usage.
    OTI_RESERVED_43            = 0x43, // Reserved. No known usage.
    OTI_RESERVED_44            = 0x44, // Reserved. No known usage.
    OTI_RESERVED_45            = 0x45, // Reserved. No known usage.
    OTI_RESERVED_46            = 0x46, // Reserved. No known usage.
    OTI_RESERVED_47            = 0x47, // Reserved. No known usage.
    OTI_RESERVED_48            = 0x48, // Reserved. No known usage.
    OTI_RESERVED_49            = 0x49, // Reserved. No known usage.
    OTI_RESERVED_4A            = 0x4A, // Reserved. No known usage.
    OTI_RESERVED_4B            = 0x4B, // Reserved. No known usage.
    OTI_RESERVED_4C            = 0x4C, // Reserved. No known usage.
    OTI_RESERVED_4D            = 0x4D, // Reserved. No known usage.
    OTI_RESERVED_4E            = 0x4E, // Reserved. No known usage.
    OTI_RESERVED_4F            = 0x4F, // Reserved. No known usage.
    OTI_RESERVED_50            = 0x50, // Reserved. No known usage.
    OTI_RESERVED_51            = 0x51, // Reserved. No known usage.
    OTI_RESERVED_52            = 0x52, // Reserved. No known usage.
    OTI_RESERVED_53            = 0x53, // Reserved. No known usage.
    OTI_RESERVED_54            = 0x54, // Reserved. No known usage.
    OTI_RESERVED_55            = 0x55, // Reserved. No known usage.
    OTI_RESERVED_56            = 0x56, // Reserved. No known usage.
    OTI_RESERVED_57            = 0x57, // Reserved. No known usage.
    OTI_RESERVED_58            = 0x58, // Reserved. No known usage.
    OTI_RESERVED_59            = 0x59, // Reserved. No known usage.
    OTI_RESERVED_5A            = 0x5A, // Reserved. No known usage.
    OTI_RESERVED_5B            = 0x5B, // Reserved. No known usage.
    OTI_RESERVED_5C            = 0x5C, // Reserved. No known usage.
    OTI_RESERVED_5D            = 0x5D, // Reserved. No known usage.
    OTI_RESERVED_5E            = 0x5E, // Reserved. No known usage.
    OTI_RESERVED_5F            = 0x5F, // Reserved. No known usage.

    // 0x60–0x6F — MPEG‑2 Visual + MPEG‑2 AAC
    OTI_MPEG2_VIS_SIMPLE       = 0x60, // MPEG‑2 Video Simple Profile. Legacy; supported by FFmpeg.
    OTI_MPEG2_VIS_MAIN         = 0x61, // MPEG‑2 Video Main Profile. Used in DVDs, some MP4 test files.
    OTI_MPEG2_VIS_SNR          = 0x62, // MPEG‑2 SNR Profile. Rare.
    OTI_MPEG2_VIS_SPATIAL      = 0x63, // MPEG‑2 Spatial Profile. Rare.
    OTI_MPEG2_VIS_HIGH         = 0x64, // MPEG‑2 High Profile. Broadcast/HDTV; rarely in MP4.
    OTI_MPEG2_VIS_422          = 0x65, // MPEG‑2 4:2:2 Profile. Studio/broadcast; rarely MP4.
    OTI_MPEG2_AAC_MAIN         = 0x66, // MPEG‑2 AAC Main. Legacy; some old streams, supported by FFmpeg.
    OTI_MPEG2_AAC_LC           = 0x67, // MPEG‑2 AAC LC. Legacy; may appear in converted content.
    OTI_MPEG2_AAC_SSR          = 0x68, // MPEG‑2 AAC SSR. Very rare.
    OTI_RESERVED_69            = 0x69, // Reserved. No known usage.
    OTI_RESERVED_6A            = 0x6A, // Reserved. No known usage.
    OTI_RESERVED_6B            = 0x6B, // Reserved. No known usage.
    OTI_RESERVED_6C            = 0x6C, // Reserved. No known usage.
    OTI_RESERVED_6D            = 0x6D, // Reserved. No known usage.
    OTI_RESERVED_6E            = 0x6E, // Reserved. No known usage.
    OTI_RESERVED_6F            = 0x6F, // Reserved. No known usage.

    // 0x70–0x7F — Reserved
    OTI_RESERVED_70            = 0x70, // Reserved. No known usage.
    OTI_RESERVED_71            = 0x71, // Reserved. No known usage.
    OTI_RESERVED_72            = 0x72, // Reserved. No known usage.
    OTI_RESERVED_73            = 0x73, // Reserved. No known usage.
    OTI_RESERVED_74            = 0x74, // Reserved. No known usage.
    OTI_RESERVED_75            = 0x75, // Reserved. No known usage.
    OTI_RESERVED_76            = 0x76, // Reserved. No known usage.
    OTI_RESERVED_77            = 0x77, // Reserved. No known usage.
    OTI_RESERVED_78            = 0x78, // Reserved. No known usage.
    OTI_RESERVED_79            = 0x79, // Reserved. No known usage.
    OTI_RESERVED_7A            = 0x7A, // Reserved. No known usage.
    OTI_RESERVED_7B            = 0x7B, // Reserved. No known usage.
    OTI_RESERVED_7C            = 0x7C, // Reserved. No known usage.
    OTI_RESERVED_7D            = 0x7D, // Reserved. No known usage.
    OTI_RESERVED_7E            = 0x7E, // Reserved. No known usage.
    OTI_RESERVED_7F            = 0x7F, // Reserved. No known usage.

    // 0x80–0x8F — Reserved / vendor
    OTI_RESERVED_80            = 0x80, // Reserved. No known standardized usage.
    OTI_RESERVED_81            = 0x81, // Reserved. No known usage.
    OTI_RESERVED_82            = 0x82, // Reserved. No known usage.
    OTI_RESERVED_83            = 0x83, // Reserved. No known usage.
    OTI_RESERVED_84            = 0x84, // Reserved. No known usage.
    OTI_RESERVED_85            = 0x85, // Reserved. No known usage.
    OTI_RESERVED_86            = 0x86, // Reserved. No known usage.
    OTI_RESERVED_87            = 0x87, // Reserved. No known usage.
    OTI_RESERVED_88            = 0x88, // Reserved. No known usage.
    OTI_RESERVED_89            = 0x89, // Reserved. No known usage.
    OTI_RESERVED_8A            = 0x8A, // Reserved. No known usage.
    OTI_RESERVED_8B            = 0x8B, // Reserved. No known usage.
    OTI_RESERVED_8C            = 0x8C, // Reserved. No known usage.
    OTI_RESERVED_8D            = 0x8D, // Reserved. No known usage.
    OTI_RESERVED_8E            = 0x8E, // Reserved. No known usage.
    OTI_RESERVED_8F            = 0x8F, // Reserved. No known usage.

    // 0x90–0x9F — Reserved / vendor
    OTI_RESERVED_90            = 0x90, // Reserved. No known usage.
    OTI_RESERVED_91            = 0x91, // Reserved. No known usage.
    OTI_RESERVED_92            = 0x92, // Reserved. No known usage.
    OTI_RESERVED_93            = 0x93, // Reserved. No known usage.
    OTI_RESERVED_94            = 0x94, // Reserved. No known usage.
    OTI_RESERVED_95            = 0x95, // Reserved. No known usage.
    OTI_RESERVED_96            = 0x96, // Reserved. No known usage.
    OTI_RESERVED_97            = 0x97, // Reserved. No known usage.
    OTI_RESERVED_98            = 0x98, // Reserved. No known usage.
    OTI_RESERVED_99            = 0x99, // Reserved. No known usage.
    OTI_RESERVED_9A            = 0x9A, // Reserved. No known usage.
    OTI_RESERVED_9B            = 0x9B, // Reserved. No known usage.
    OTI_RESERVED_9C            = 0x9C, // Reserved. No known usage.
    OTI_RESERVED_9D            = 0x9D, // Reserved. No known usage.
    OTI_RESERVED_9E            = 0x9E, // Reserved. No known usage.
    OTI_RESERVED_9F            = 0x9F, // Reserved. No known usage.

    // 0xA0–0xAF — Widely used vendor OTIs (Opus/ALAC/FLAC) + reserved
    OTI_OPUS                   = 0xA0, // Opus in MP4. Used by YouTube, FFmpeg, GPAC (non-standard but de-facto).
    OTI_RESERVED_A1            = 0xA1, // Reserved. No known usage.
    OTI_RESERVED_A2            = 0xA2, // Reserved. No known usage.
    OTI_RESERVED_A3            = 0xA3, // Reserved. No known usage.
    OTI_RESERVED_A4            = 0xA4, // Reserved. No known usage.
    OTI_ALAC                   = 0xA5, // Apple Lossless Audio Codec. Used by Apple, FFmpeg, GPAC.
    OTI_FLAC                   = 0xA6, // FLAC in MP4. Used by FFmpeg/GPAC; non-standard but common.
    OTI_RESERVED_A7            = 0xA7, // Reserved. No known usage.
    OTI_RESERVED_A8            = 0xA8, // Reserved. No known usage.
    OTI_RESERVED_A9            = 0xA9, // Reserved. No known usage.
    OTI_RESERVED_AA            = 0xAA, // Reserved. No known usage.
    OTI_RESERVED_AB            = 0xAB, // Reserved. No known usage.
    OTI_RESERVED_AC            = 0xAC, // Reserved. No known usage.
    OTI_RESERVED_AD            = 0xAD, // Reserved. No known usage.
    OTI_RESERVED_AE            = 0xAE, // Reserved. No known usage.
    OTI_RESERVED_AF            = 0xAF, // Reserved. No known usage.

    // 0xB0–0xBF — Reserved
    OTI_RESERVED_B0            = 0xB0, // Reserved. No known usage.
    OTI_RESERVED_B1            = 0xB1, // Reserved. No known usage.
    OTI_RESERVED_B2            = 0xB2, // Reserved. No known usage.
    OTI_RESERVED_B3            = 0xB3, // Reserved. No known usage.
    OTI_RESERVED_B4            = 0xB4, // Reserved. No known usage.
    OTI_RESERVED_B5            = 0xB5, // Reserved. No known usage.
    OTI_RESERVED_B6            = 0xB6, // Reserved. No known usage.
    OTI_RESERVED_B7            = 0xB7, // Reserved. No known usage.
    OTI_RESERVED_B8            = 0xB8, // Reserved. No known usage.
    OTI_RESERVED_B9            = 0xB9, // Reserved. No known usage.
    OTI_RESERVED_BA            = 0xBA, // Reserved. No known usage.
    OTI_RESERVED_BB            = 0xBB, // Reserved. No known usage.
    OTI_RESERVED_BC            = 0xBC, // Reserved. No known usage.
    OTI_RESERVED_BD            = 0xBD, // Reserved. No known usage.
    OTI_RESERVED_BE            = 0xBE, // Reserved. No known usage.
    OTI_RESERVED_BF            = 0xBF, // Reserved. No known usage.

    // 0xC0–0xCF — Reserved
    OTI_RESERVED_C0            = 0xC0, // Reserved. No known usage.
    OTI_RESERVED_C1            = 0xC1, // Reserved. No known usage.
    OTI_RESERVED_C2            = 0xC2, // Reserved. No known usage.
    OTI_RESERVED_C3            = 0xC3, // Reserved. No known usage.
    OTI_RESERVED_C4            = 0xC4, // Reserved. No known usage.
    OTI_RESERVED_C5            = 0xC5, // Reserved. No known usage.
    OTI_RESERVED_C6            = 0xC6, // Reserved. No known usage.
    OTI_RESERVED_C7            = 0xC7, // Reserved. No known usage.
    OTI_RESERVED_C8            = 0xC8, // Reserved. No known usage.
    OTI_RESERVED_C9            = 0xC9, // Reserved. No known usage.
    OTI_RESERVED_CA            = 0xCA, // Reserved. No known usage.
    OTI_RESERVED_CB            = 0xCB, // Reserved. No known usage.
    OTI_RESERVED_CC            = 0xCC, // Reserved. No known usage.
    OTI_RESERVED_CD            = 0xCD, // Reserved. No known usage.
    OTI_RESERVED_CE            = 0xCE, // Reserved. No known usage.
    OTI_RESERVED_CF            = 0xCF, // Reserved. No known usage.

    // 0xD0–0xDF — Dolby + reserved
    OTI_RESERVED_D0            = 0xD0, // Reserved. No known usage.
    OTI_RESERVED_D1            = 0xD1, // Reserved. No known usage.
    OTI_RESERVED_D2            = 0xD2, // Reserved. No known usage.
    OTI_RESERVED_D3            = 0xD3, // Reserved. No known usage.
    OTI_RESERVED_D4            = 0xD4, // Reserved. No known usage.
    OTI_RESERVED_D5            = 0xD5, // Reserved. No known usage.
    OTI_RESERVED_D6            = 0xD6, // Reserved. No known usage.
    OTI_RESERVED_D7            = 0xD7, // Reserved. No known usage.
    OTI_RESERVED_D8            = 0xD8, // Reserved. No known usage.
    OTI_RESERVED_D9            = 0xD9, // Reserved. No known usage.
    OTI_RESERVED_DA            = 0xDA, // Reserved. No known usage.
    OTI_RESERVED_DB            = 0xDB, // Reserved. No known usage.
    OTI_RESERVED_DC            = 0xDC, // Reserved. No known usage.
    OTI_AC3                    = 0xDD, // AC-3 in MP4. Used by some tools; supported by FFmpeg/GPAC.
    OTI_EAC3                   = 0xDE, // E-AC-3 in MP4. Used by streaming/broadcast; FFmpeg/GPAC support.
    OTI_RESERVED_DF            = 0xDF, // Reserved. No known usage.

    // 0xE0–0xFF — Reserved
    OTI_RESERVED_E0            = 0xE0, // Reserved. No known usage.
    OTI_RESERVED_E1            = 0xE1, // Reserved. No known usage.
    OTI_RESERVED_E2            = 0xE2, // Reserved. No known usage.
    OTI_RESERVED_E3            = 0xE3, // Reserved. No known usage.
    OTI_RESERVED_E4            = 0xE4, // Reserved. No known usage.
    OTI_RESERVED_E5            = 0xE5, // Reserved. No known usage.
    OTI_RESERVED_E6            = 0xE6, // Reserved. No known usage.
    OTI_RESERVED_E7            = 0xE7, // Reserved. No known usage.
    OTI_RESERVED_E8            = 0xE8, // Reserved. No known usage.
    OTI_RESERVED_E9            = 0xE9, // Reserved. No known usage.
    OTI_RESERVED_EA            = 0xEA, // Reserved. No known usage.
    OTI_RESERVED_EB            = 0xEB, // Reserved. No known usage.
    OTI_RESERVED_EC            = 0xEC, // Reserved. No known usage.
    OTI_RESERVED_ED            = 0xED, // Reserved. No known usage.
    OTI_RESERVED_EE            = 0xEE, // Reserved. No known usage.
    OTI_RESERVED_EF            = 0xEF, // Reserved. No known usage.

    OTI_RESERVED_F0            = 0xF0, // Reserved. No known usage.
    OTI_RESERVED_F1            = 0xF1, // Reserved. No known usage.
    OTI_RESERVED_F2            = 0xF2, // Reserved. No known usage.
    OTI_RESERVED_F3            = 0xF3, // Reserved. No known usage.
    OTI_RESERVED_F4            = 0xF4, // Reserved. No known usage.
    OTI_RESERVED_F5            = 0xF5, // Reserved. No known usage.
    OTI_RESERVED_F6            = 0xF6, // Reserved. No known usage.
    OTI_RESERVED_F7            = 0xF7, // Reserved. No known usage.
    OTI_RESERVED_F8            = 0xF8, // Reserved. No known usage.
    OTI_RESERVED_F9            = 0xF9, // Reserved. No known usage.
    OTI_RESERVED_FA            = 0xFA, // Reserved. No known usage.
    OTI_RESERVED_FB            = 0xFB, // Reserved. No known usage.
    OTI_RESERVED_FC            = 0xFC, // Reserved. No known usage.
    OTI_RESERVED_FD            = 0xFD, // Reserved. No known usage.
    OTI_RESERVED_FE            = 0xFE, // Reserved. No known usage.
    OTI_RESERVED_FF            = 0xFF  // Reserved. No known usage.
};

};
