/* =========================================================================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

#pragma once

namespace BinaryData
{
    extern const char*   seqRandom_png;
    const int            seqRandom_pngSize = 374;

    extern const char*   seqSequential_png;
    const int            seqSequential_pngSize = 300;

    extern const char*   seqUpDown_png;
    const int            seqUpDown_pngSize = 319;

    extern const char*   synisterLogoSmall_png;
    const int            synisterLogoSmall_pngSize = 1356;

    extern const char*   infoScreen_png;
    const int            infoScreen_pngSize = 46272;

    extern const char*   QUL_Logo_png;
    const int            QUL_Logo_pngSize = 82009;

    extern const char*   delayReverse_png;
    const int            delayReverse_pngSize = 362;

    extern const char*   dottedNote_png;
    const int            dottedNote_pngSize = 229;

    extern const char*   lfoGain_png;
    const int            lfoGain_pngSize = 525;

    extern const char*   lfoSampleHold_png;
    const int            lfoSampleHold_pngSize = 203;

    extern const char*   lfoSineWave_png;
    const int            lfoSineWave_pngSize = 355;

    extern const char*   lfoSquareWave_png;
    const int            lfoSquareWave_pngSize = 158;

    extern const char*   oscWaveForms_png;
    const int            oscWaveForms_pngSize = 1154;

    extern const char*   recordCutoff_png;
    const int            recordCutoff_pngSize = 491;

    extern const char*   SynisterLogo_block_png;
    const int            SynisterLogo_block_pngSize = 3499;

    extern const char*   synisterLogo_png;
    const int            synisterLogo_pngSize = 2844;

    extern const char*   tempoSync_png;
    const int            tempoSync_pngSize = 439;

    extern const char*   toggleOff_png;
    const int            toggleOff_pngSize = 465;

    extern const char*   toggleOn_png;
    const int            toggleOn_pngSize = 661;

    extern const char*   triplets_png;
    const int            triplets_pngSize = 464;

    extern const char*   world_of_water_ttf;
    const int            world_of_water_ttfSize = 83900;

    // Points to the start of a list of resource names.
    extern const char* namedResourceList[];

    // Number of elements in the namedResourceList array.
    const int namedResourceListSize = 21;

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding data and its size (or a null pointer if the name isn't found).
    const char* getNamedResource (const char* resourceNameUTF8, int& dataSizeInBytes) throw();
}
