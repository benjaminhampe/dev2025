#include "Plugin.h"
#include "Editor.h"

AudioEffect* createEffectInstance(audioMasterCallback audioMaster) { return new Plugin(audioMaster); }

// ------------------ Plugin Implementation ------------------

VstIntPtr
Plugin::dispatcher(VstInt32 opCode, VstInt32 index, VstIntPtr value, void* ptr, float opt)
{
    switch (opCode)
    {
        case effOpen:
            return 0;
        case effClose:
            delete this; // End lifecycle. We must delete ourselfs to prevent leaks.
            return 0;
        case effEditOpen:
            m_editor.create(ptr);
            return 1;
        case effEditClose:
            m_editor.destroy();
            return 1;
        case effEditGetRect:
            *(ERect**)ptr = m_editor.getEditorRect();
            return 1;
        case effSetSampleRate:
            m_sampleRate = int32_t(opt);
            m_synth.setSampleRate(opt);
            return 1;
        case effSetBlockSize:
            m_frameCount = (int)value;
            return 1;
        case effMainsChanged:
            if (value) {
                // Audio processing is starting
            } else {
                // Audio processing is stopping
            }
            return 1;
/*
struct VstSpeakerArrangement {
    VstInt32 type;       // e.g., kSpeakerArr51 for 5.1
    VstInt32 numChannels;
    VstSpeakerProperties speakers[kMaxSpeakers];
};

VstSpeakerArrangement** arrangements = (VstSpeakerArrangement**)ptr;
VstSpeakerArrangement* inputArrangement = arrangements[0];
VstSpeakerArrangement* outputArrangement = arrangements[1];

case effSetSpeakerArrangement: {
    VstSpeakerArrangement** sa = (VstSpeakerArrangement**)ptr;
    inputSpeakerArrangement = sa[0];   // may be nullptr
    outputSpeakerArrangement = sa[1];  // may be nullptr
    return 1;
}
*/

        default:
            return AudioEffectX::dispatcher(opCode, index, value, ptr, opt);
    }
}

Plugin::Plugin(audioMasterCallback audioMaster)
    : AudioEffectX(audioMaster, kNumParams, NUM_PARTIALS)
    , m_sampleRate(44100)
    , m_frameCount(64)
    , m_masterAmplitude(100) // [%]
    , m_editor(this)
    , m_synth()
{
    // 🎧 Audio & Processing
    //     "receiveVstEvents" — receive VST events (e.g. MIDI)
    //     "receiveVstMidiEvent" — receive MIDI events
    //     "sendVstEvents" — send VST events
    //     "sendVstMidiEvent" — send MIDI events
    //     "receiveVstTimeInfo" — receive time info (tempo, position, etc.)
    //     "offline" — supports offline processing
    //     "plugAsChannelInsert" — can be used as a channel insert effect
    //     "plugAsSend" — can be used as a send effect
    //     "bypass" — supports bypass processing

    // 🎹 Instrument & Plugin
    //     "isSynth" — plugin is a synth (no audio input, generates sound)
    //     "noRealTime" — plugin doesn’t require real-time processing

    // 🖥️ GUI & Automation
    //     "hasEditor" — plugin has a custom GUI editor
    //     "canMono" — supports mono processing
    //     "canStereo" — supports stereo processing
    //     "midiProgramNames" — supports MIDI program name queries
    //     "conformsToWindowRules" — GUI follows host windowing rules

    // 🧪 Host-Specific or Rare
    //     "supportsBypass" — supports host bypass
    //     "supportsOpen" — supports opening plugin window
    //     "supportsClose" — supports closing plugin window
    //     "supportsRewire" — supports ReWire
    //     "supportsMultiChannel" — supports more than stereo
    //     "supportsShell" — part of a shell plugin (multiple plugins in one binary)

    m_canDo.insert("plugAsChannelInsert");  // plug-in can be used as a channel insert effect.
    m_canDo.insert("plugAsSend");           // plug-in can be used as a send effect.
    m_canDo.insert("x2in2out");             // Stereo?
    m_canDo.insert("receiveVstEvents");     // ?
    m_canDo.insert("receiveVstMidiEvent");  // Midi
    m_canDo.insert("hasEditor");            // Editor
    m_canDo.insert("isSynth");              // Does not really need audio inputs.

    setNumInputs(kNumInputs);
    setNumOutputs(kNumOutputs);
    setUniqueID(kUniqueId);

    //canProcessReplacing();      // supports output replacing
    //canDoubleReplacing();       // supports double precision processing
    //programsAreChunks(true);    // chunks are parameters as binary blob (a preset)

    // ✅
    AEffect* aeffect = getAeffect();
    aeffect->flags |= effFlagsIsSynth;
    aeffect->flags |= effFlagsHasEditor;
    aeffect->flags |= effFlagsCanReplacing;
    //aeffect->flags |= effFlagsCanDoubleReplacing;
    //aeffect->flags |= effFlagsCanMono;
    aeffect->flags |= effFlagsProgramChunks;

    vst_strncpy (m_programName, "Default", kVstMaxProgNameLen); // default program name
}

Plugin::~Plugin()
{}

VstInt32
Plugin::canDo(char *text) { return ( m_canDo.find(text) == m_canDo.end()) ? -1: 1; } // 1 = yes, -1 = no, 0 = don't know

void
Plugin::setProgramName(char *name) { vst_strncpy (m_programName, name, kVstMaxProgNameLen);}

void
Plugin::getProgramName(char *name) { vst_strncpy (name, m_programName, kVstMaxProgNameLen);}

bool
Plugin::getEffectName(char* name)  { vst_strncpy(name, "SineMachine4", kVstMaxProductStrLen); return true; }

bool
Plugin::getProductString(char* text) { vst_strncpy (text, "hambe SineMachine4", kVstMaxProductStrLen); return true; }

bool
Plugin::getVendorString(char* text) { vst_strncpy (text, "hambe", kVstMaxVendorStrLen); return true; }

void
Plugin::open() { DE_DEBUG("") }

void
Plugin::close() { DE_DEBUG("") }

void
Plugin::processReplacing(float** inputs, float** outputs, VstInt32 sampleFrames)
{
    float* outL = outputs[0];
    float* outR = outputs[1];

    for (int i = 0; i < sampleFrames; i++)
    {
        float sample = 0.0f;
        float Asum = 0.0f;

        for (Partial & partial : m_synth.m_partials)
        {
            float A = partial.A();
            sample += A * sinf(partial.phase);
            partial.phase += partial.phaseIncrement;
            if (partial.phase > de::TWO_PI)
            {
                partial.phase -= de::TWO_PI;
            }
            Asum += A;
        }

        sample /= float(Asum);

        outL[i] = outR[i] = sample;
    }
}

#if 0
void
Plugin::processDoubleReplacing (double** inputs, double** outputs, VstInt32 sampleFrames)
{

}
#endif

VstInt32
Plugin::setChunk (void* data, VstInt32 byteSize, bool isPreset)
{
#if 0
    float *chunkData = (float *)data;
    A = std::clamp(chunkData[0], 0.0f, 1.0f);
    /* We're ignoring byteSize as we found it to be a filthy liar */

    /* calculate any other fields you need here - you could copy in
     code from setParameter() here. */
    return 0;
#else
    return 0;
#endif
}

VstInt32
Plugin::getChunk (void** data, bool isPreset)
{
#if 0
    float *chunkData = (float *)calloc(kNumParameters, sizeof(float));
    chunkData[0] = A;

    *data = chunkData;
    return kNumParameters * sizeof(float);
#else
    return 0;
#endif
}

void
Plugin::setParameter(VstInt32 index, float value)
{
    if (index < 0 || index >= int(m_synth.m_partials.size()))
    {
        return;
    }
    m_synth.m_partials[index].amplitude = value;

#if 0
    switch (index) {
        case kParamA: A = value; break;
        default: throw; // unknown parameter, shouldn't happen!
    }
#endif
}


float
Plugin::getParameter(VstInt32 index)
{
    if (index < 0 || index >= int(m_synth.m_partials.size()))
    {
        return 0.0f;
    }
    return m_synth.m_partials[index].amplitude;

#if 0
    switch (index) {
        case kParamA: return A; break;
        default: break; // unknown parameter, shouldn't happen!
    } return 0.0; //we only need to update the relevant name, this is simple to manage
#endif
}

void Plugin::getParameterName(VstInt32 index, char *text)
{
    vst_strncpy(text, "?", kVstMaxParamStrLen);

    switch (index) {
        //case kParamA: vst_strncpy (text, "Q", kVstMaxParamStrLen); break;
        default: break; // unknown parameter, shouldn't happen!
    } //this is our labels for displaying in the VST host
}

void Plugin::getParameterDisplay(VstInt32 index, char *text)
{
    vst_strncpy(text, "?", kVstMaxParamStrLen);

    switch (index)
    {
    // case kParamA:
    //     switch((VstInt32)( A * 4.999 )) //0 to almost edge of # of params
    //     {
    //         case kA: vst_strncpy (text, "Reso A", kVstMaxParamStrLen); break;
    //         case kB: vst_strncpy (text, "Reso B", kVstMaxParamStrLen); break;
    //         case kC: vst_strncpy (text, "Reso C", kVstMaxParamStrLen); break;
    //         case kD: vst_strncpy (text, "Reso D", kVstMaxParamStrLen); break;
    //         case kE: vst_strncpy (text, "Reso E", kVstMaxParamStrLen); break;
    //         default: break; // unknown parameter, shouldn't happen!
    //     } break;
    default:
        break; // unknown parameter, shouldn't happen!
    } //this displays the values and handles 'popups' where it's discrete choices
}

void Plugin::getParameterLabel(VstInt32 index, char *text)
{
    vst_strncpy(text, "?", kVstMaxParamStrLen);

    switch (index)
    {
        //case kParamA: vst_strncpy (text, "", kVstMaxParamStrLen); break;
        default: break; // unknown parameter, shouldn't happen!
    }
}
