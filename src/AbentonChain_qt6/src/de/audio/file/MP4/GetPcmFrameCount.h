#pragma once

/*
uint64_t Bento4_GetPcmFrameCount(AP4_Track* track, AP4_AudioSampleDescription* asd)
{
    uint64_t pcmFrames = 0;

    if (asd->GetFormat() == AP4_SAMPLE_FORMAT_MP4A) {
        // -------------------------
        // AAC
        // -------------------------
        // Determine AAC frame length from AudioSpecificConfig
        AP4_EsdsAtom* esds = nullptr;
        asd->GetChild(esds);

        uint32_t frameLength = 1024; // default AAC-LC

        if (esds) {
            const AP4_DataBuffer& asc =
                esds->GetDecoderConfigDescriptor()
                    ->GetDecoderSpecificInfo()
                    ->GetData();

            if (asc.GetDataSize() >= 2) {
                uint8_t audioObjectType = (asc.GetData()[0] >> 3);
                if (audioObjectType == 23) // AAC-LD
                    frameLength = 512;
                if (audioObjectType == 39) // AAC-ELD
                    frameLength = 512;
            }
        }

        pcmFrames = uint64_t(track->GetSampleCount()) * frameLength;
    }
    else if (asd->GetFormat() == AP4_SAMPLE_FORMAT_ALAC) {
        // -------------------------
        // ALAC
        // -------------------------
        auto* alac = AP4_DYNAMIC_CAST(AP4_AlacAudioSampleEntry, asd);
        uint32_t spf = alac->GetSamplesPerFrame(); // PCM samples per ALAC packet

        pcmFrames = uint64_t(track->GetSampleCount()) * spf;
    }

    return pcmFrames;
}

*/