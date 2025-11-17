#include "Plugin.h"

// ====================================
// ✅✅✅ Main VST entry point ✅✅✅
// ====================================
extern "C"
{
    __declspec(dllexport) AEffect* __cdecl
    VSTPluginMain(audioMasterCallback audioMaster)
    {
        if (!audioMaster) return nullptr;

        AudioEffect* effect = createEffectInstance(audioMaster);
        if (!effect) return nullptr;

        return effect->getAeffect(); // @see Plugin::dispatcher's effClose where the plugin gets deleted to not leak.
    }
}

#if 0
VstIntPtr MyPlugin::dispatcher(VstInt32 opCode, VstInt32 index,
                               VstIntPtr value, void* ptr, float opt) {
    switch (opCode) {
        case effOpen:
            // 🔹 Notification only. Host ignores return value.
            // Convention: return 0.
            return 0;

        case effClose:
            // 🔹 Notification only. Host ignores return value.
            // Plugin should delete itself here.
            delete this;
            return 0;

        case effSetProgram:
            // 🔹 Notification. Host ignores return.
            return 0;

        case effGetProgram:
            // 🔹 Defined semantics: return current program index.
            return currentProgram;

        case effSetProgramName:
            // 🔹 Notification. Host ignores return.
            return 0;

        case effGetProgramName:
            // 🔹 Defined semantics: plugin must fill ptr with name string.
            // Return value ignored.
            strcpy((char*)ptr, "Default");
            return 0;

        case effGetParamLabel:
            // 🔹 Defined semantics: plugin must fill ptr with label string.
            // Return value ignored.
            getParameterLabel(index, (char*)ptr);
            return 0;

        case effGetParamDisplay:
            // 🔹 Defined semantics: plugin must fill ptr with display string.
            // Return value ignored.
            getParameterDisplay(index, (char*)ptr);
            return 0;

        case effGetParamName:
            // 🔹 Defined semantics: plugin must fill ptr with name string.
            // Return value ignored.
            getParameterName(index, (char*)ptr);
            return 0;

        case effEditGetRect:
            // 🔹 Defined semantics: plugin must set *ptr = &ERect.
            // Return 1 if rect is valid, 0 if no editor.
            static ERect editorRect = {0,0,300,400};
            *((ERect**)ptr) = &editorRect;
            return 1;

        case effEditOpen:
            // 🔹 Defined semantics: return 1 if editor successfully opened, 0 if failed.
            HWND parent = (HWND)ptr;
            if (createEditorWindow(parent)) return 1;
            return 0;

        case effEditClose:
            // 🔹 Notification only. Host ignores return.
            destroyEditorWindow();
            return 0;

        case effGetEffectName:
            // 🔹 Defined semantics: plugin must fill ptr with effect name.
            // Return value ignored.
            strcpy((char*)ptr, "MyPlugin");
            return 1; // convention: return 1 to indicate string was set

        case effGetVendorString:
            // 🔹 Defined semantics: plugin must fill ptr with vendor string.
            strcpy((char*)ptr, "MyCompany");
            return 1;

        case effGetProductString:
            // 🔹 Defined semantics: plugin must fill ptr with product string.
            strcpy((char*)ptr, "MyProduct");
            return 1;

        case effGetVendorVersion:
            // 🔹 Defined semantics: return integer version number.
            return 1000; // version 1.0.0

        default:
            // Forward unhandled opcodes to base class
            return AudioEffectX::dispatcher(opCode, index, value, ptr, opt);
    }
}


#endif
