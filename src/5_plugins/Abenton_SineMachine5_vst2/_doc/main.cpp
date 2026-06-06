#include "editor.h"
#include "aeffectx.h"

Editor* editor = nullptr;

extern "C" AEffect* VSTPluginMain(audioMasterCallback audioMaster) {
    AEffect* effect = new AEffect();
    effect->dispatcher = [](AEffect* e, int opcode, int index, int value, void* ptr, float opt) -> int {
        switch (opcode) {
            case effEditOpen:
                editor = new Editor();
                return editor->open(ptr);
            case effEditClose:
                if (editor) {
                    editor->close();
                    delete editor;
                    editor = nullptr;
                }
                return 0;
            case effEditGetRect:
                static ERect rect = {0, 0, 800, 600};
                *(ERect**)ptr = &rect;
                return 1;
            default:
                return 0;
        }
    };
    return effect;
}
