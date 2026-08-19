#include "audioeffectx.h"
#include "RackEngineWrapper.h"

class RackVST : public AudioEffectX {
public:
    RackEngineWrapper* rack;

    RackVST(audioMasterCallback audioMaster)
        : AudioEffectX(audioMaster, 2, 2) {
        setNumInputs(2);
        setNumOutputs(2);
        setUniqueID('Rvst');
        canProcessReplacing();
        canDo("sendVstEvents");
        canDo("receiveVstEvents");

        rack = new RackEngineWrapper();
        rack->init();
    }

    ~RackVST() {
        rack->shutdown();
        delete rack;
    }

    void processReplacing(float** inputs, float** outputs, VstInt32 sampleFrames) override {
        rack->lock();
        rack->processAudio(inputs, outputs, sampleFrames);
        rack->unlock();
    }

    VstInt32 processEvents(VstEvents* events) override {
        rack->lock();
        rack->processMidi(events);
        rack->unlock();
        return 1;
    }

    void open() override {
        rack->openEditor(getEditorWindow());
    }

    void close() override {
        rack->closeEditor();
    }
};

AudioEffect* createEffectInstance(audioMasterCallback audioMaster) {
    return new RackVST(audioMaster);
}
