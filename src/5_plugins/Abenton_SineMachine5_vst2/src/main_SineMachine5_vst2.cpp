#include "SinePlugin.h"

// ====================================
// ✅✅✅ Main VST entry point ✅✅✅
// ====================================
extern "C"
{
    __declspec(dllexport) AEffect* __cdecl
    VSTPluginMain(audioMasterCallback audioMaster)
    {
        if (!audioMaster)
        {
            DE_ERROR("No audioMaster")
            return nullptr;
        }

        AudioEffect* effect = createEffectInstance(audioMaster);
        if (!effect)
        {
            DE_ERROR("No audioEffect")
            return nullptr;
        }

        return effect->getAeffect(); // @see Plugin::dispatcher's effClose where the plugin gets deleted to not leak.
    }
}
