#include "SinePlugin.h"

// ------------------ Plugin Instantiation ------------------

AudioEffect* createEffectInstance(audioMasterCallback audioMaster)
{
    return new Plugin(audioMaster);
}

// ------------------ Synth Implementation ------------------

Synth::Synth( Cfg* cfg )
    : m_cfg(cfg)
{
    m_notes.resize(20); // 20 note polyphony
}

/*
void Synth::calcPhaseIncrements( std::vector<Partial> & partials, float baseFrequency, float sampleRate )
{
    const float fScale = de::TWO_PI * baseFrequency / sampleRate;

    for (Partial & partial : partials)
    {
        partial.phaseIncrement = fScale * partial.partial;
    }
}
void Synth::setSampleRate(float sampleRate)
{
    m_sampleRate = sampleRate;
    calcPhaseIncrements( m_partials, m_baseFrequency, m_sampleRate );
}
*/


/*

void Synth::processReplacing(float** inputs, float** outputs, VstInt32 sampleFrames)
{
    float* outL = outputs[0];
    float* outR = outputs[1];

    for (int i = 0; i < sampleFrames; i++)
    {
        float sample = 0.0f;
        float Asum = 0.0f;

        for (Partial & partial : m_partials)
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

void Synth::noteOn(int channel, int note, int velocity)
{
    m_baseFrequency = 440.0 * pow(2.0, (note - 69) / 12.0);  // MIDI to Hz
    // Optionally: trigger envelopes, voices, etc.
    calcPhaseIncrements( m_partials, m_baseFrequency, m_sampleRate );
}

void Synth::noteOff(int channel, int note, int velocity)
{
    // If you're using a simple monophonic synth:
    if (note == currentNote)
    {
        m_baseFrequency = 0.0f;  // silence the oscillator
        currentNote = -1;
    }

    // For polyphony, you'd deactivate the voice assigned to this note
}


*/

// ------------------ Plugin Implementation ------------------

void
Plugin::processReplacing(float** inputs, float** outputs, VstInt32 sampleFrames)
{
    float* outL = outputs[0];
    float* outR = outputs[1];

    size_t nNotes = 0;

    //DE_OK("m_synth.m_notes.size() = ", m_synth.m_notes.size(), ", "
    //        "sampleFrames = ", sampleFrames)

    for (Note & note : m_synth.m_notes)
    {
        if (note.m_frameCount > 0)
        {
            note.computeSamples(sampleFrames);

            for (int32_t i = 0; i < sampleFrames; i++)
            {
                outL[i] += note.m_L.at(i);
            }
            for (int32_t i = 0; i < sampleFrames; i++)
            {
                outR[i] += note.m_R.at(i);
            }
        }
    }

/*
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
*/
}

#if 0
void
Plugin::processDoubleReplacing (double** inputs, double** outputs, VstInt32 sampleFrames)
{

}
#endif

VstIntPtr
Plugin::dispatcher(VstInt32 opCode, VstInt32 index, VstIntPtr value, void* ptr, float opt)
{
    switch (opCode)
    {
        // case effOpen:
        //     DE_BENNI("effOpen")
        //     return 0;
        // case effClose:
        //     DE_BENNI("effClose")
        //     //delete this; // End lifecycle. We must delete ourselfs to prevent leaks.
        //     return 0;
        case effEditOpen:
            DE_BENNI("effEditOpen")
            m_editor.create(ptr);
            return 1;
        case effEditClose:
            DE_BENNI("effEditClose")
            m_editor.destroy();
            return 1;
        case effEditGetRect:
            DE_BENNI("effEditGetRect")
            *(ERect**)ptr = m_editor.getEditorRect();
            return 1;
        case effSetSampleRate:
            DE_BENNI("effSetSampleRate")
            m_sampleRate = int32_t(opt);
            m_synth.m_cfg->m_sampleRate = opt;
            return 1;
        case effSetBlockSize:
            DE_BENNI("effSetBlockSize")
            m_frameCount = (int)value;
            return 1;
        case effMainsChanged:
            if (value)
            {
                // Audio processing is starting
                DE_BENNI("effMainsChanged = 1")
            }
            else
            {
                // Audio processing is stopping
                DE_BENNI("effMainsChanged = 0")
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

constexpr int NUM_PARTIALS = 48;

Plugin::Plugin(audioMasterCallback audioMaster)
                   // audioMasterCallback,
                   // VstInt32 numPrograms = 5, -> 0: Default/None, 1:Square/Rect, 2:Saw, 3:ReverseSaw, 4:Triangle
                   // VstInt32 numParams = nPartials * params_per_partial);
    : AudioEffectX(audioMaster, kNumParams, NUM_PARTIALS)
    , m_sampleRate(44100)
    , m_frameCount(64)      // BlockSize
    , m_cfg()
    , m_synth(&m_cfg)
    , m_editor(this)
{
    m_cfg.setNumPartials( NUM_PARTIALS );
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
    aeffect->flags |= effFlagsProgramChunks;
    //aeffect->flags |= effFlagsCanDoubleReplacing;
    //aeffect->flags |= effFlagsCanMono;

    vst_strncpy (m_programName, "Default", kVstMaxProgNameLen); // default program name
}

Plugin::~Plugin()
{}

VstInt32
Plugin::canDo(char *text)
{
    return ( m_canDo.find(text) == m_canDo.end()) ? -1: 1;
} // 1 = yes, -1 = no, 0 = don't know

void
Plugin::setProgramName(char *name)
{
    //vst_strncpy (m_programName, name, kVstMaxProgNameLen);
}

void
Plugin::getProgramName(char *name)
{
    vst_strncpy (name, "DefaultProg", kVstMaxProgNameLen);
}

bool
Plugin::getEffectName(char* name)
{
    vst_strncpy(name, "SineMachine4", kVstMaxEffectNameLen);
    return true;
}

bool
Plugin::getProductString(char* text)
{
    vst_strncpy (text, "Abenton SineMachine4", kVstMaxProductStrLen);
    return true;
}

bool
Plugin::getVendorString(char* text)
{
    vst_strncpy (text, "Abenton", kVstMaxVendorStrLen);
    return true;
}

void
Plugin::open()
{
    DE_DEBUG("")
}

void
Plugin::close()
{
    DE_DEBUG("close()")
}



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
    const float fAmplitude = value;
    m_cfg.setPartial(index,fAmplitude);

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
    if (index < 0 || index >= int(m_cfg.getNumPartials()))
    {
        return 0.0f;
    }
    return m_cfg.m_partials[index].fAmplitude;

#if 0
    switch (index) {
        case kParamA: return A; break;
        default: break; // unknown parameter, shouldn't happen!
    } return 0.0; //we only need to update the relevant name, this is simple to manage
#endif
}

void Plugin::getParameterName(VstInt32 index, char *text)
{
    vst_strncpy(text, "BenniPar", kVstMaxParamStrLen);
#if 0
    vst_strncpy(text, "?", kVstMaxParamStrLen);

    switch (index) {
        //case kParamA: vst_strncpy (text, "Q", kVstMaxParamStrLen); break;
        default: break; // unknown parameter, shouldn't happen!
    } //this is our labels for displaying in the VST host
#endif
}

void Plugin::getParameterDisplay(VstInt32 index, char *text)
{
    vst_strncpy(text, "BenPDisp", kVstMaxParamStrLen);
#if 0
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
#endif
}

void Plugin::getParameterLabel(VstInt32 index, char *text)
{
    vst_strncpy(text, "BenLabel", kVstMaxParamStrLen);
#if 0
    switch (index)
    {
        //case kParamA: vst_strncpy (text, "", kVstMaxParamStrLen); break;
        default: break; // unknown parameter, shouldn't happen!
    }
#endif
}
