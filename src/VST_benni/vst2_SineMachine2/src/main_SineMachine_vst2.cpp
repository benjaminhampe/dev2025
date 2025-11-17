#include "Synth.h"
#include "Editor.h"

int32_t m_screenWidth = 1200;
int32_t m_screenHeight = 900;
std::atomic<bool> shouldClose = false;

// ------------------ Plugin Implementation ------------------

AudioEffect* createEffectInstance(audioMasterCallback audioMaster)
{
    return new Synth(audioMaster);
}

// ✅✅✅ Main VST entry point ✅✅✅

extern "C"
{
    __declspec(dllexport) AEffect* __cdecl
    VSTPluginMain(audioMasterCallback audioMaster)
    {
        if (!audioMaster) return nullptr;

        AudioEffect* effect = createEffectInstance(audioMaster);
        if (!effect) return nullptr;

        AEffect* aeffect = effect->getAeffect();
        aeffect->flags |= effFlagsIsSynth;  // ✅ Set isSynth == true
        aeffect->flags |= effFlagsHasEditor;
        aeffect->flags |= effFlagsCanReplacing;
        aeffect->flags |= effFlagsProgramChunks;

        return effect->getAeffect();
    }
}

