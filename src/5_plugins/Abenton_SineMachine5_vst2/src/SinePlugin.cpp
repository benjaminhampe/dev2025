#include "SinePlugin.h"

// ------------------ Plugin Instantiation ------------------

AudioEffect* createEffectInstance(audioMasterCallback audioMaster)
{
    return new Plugin(audioMaster);
}

// ------------------ Plugin Implementation ------------------


Plugin::Plugin(audioMasterCallback audioMaster)
                   // audioMasterCallback,
                   // VstInt32 numPrograms = 5, -> 0: Default/None, 1:Square/Rect, 2:Saw, 3:ReverseSaw, 4:Triangle
                   // VstInt32 numParams = nPartials * params_per_partial);
    : AudioEffectX(audioMaster, kNumParams, NUM_PARTIALS)
    , m_editor(this)
{
    setNumInputs(kNumInputs);
    setNumOutputs(kNumOutputs);
    setUniqueID(kUniqueId);
    //isSynth(true);
    //hasEditor(true);        // plugin has GUI
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
/*
🎯 Mapping your list to VST2 canDo() strings

Here is your list, mapped to the actual VST2 strings:
Your Concept            VST2 canDo() String     Where to Set
isSynth                 (not in canDo)          isSynth(true) in constructor
noRealTime              "noRealTime"            canDo()
hasEditor               "hasEditor"             canDo() AND hasEditor(true)
canMono                 "1in1out"               canDo()
canStereo               "2in2out"               canDo()
midiProgramNames        "midiProgramNames"      canDo()
conformsToWindowRules	"conformsToWindowRules"	canDo()
supportsBypass          "bypass"                canDo()
supportsOpen            (not needed)            host always calls effEditOpen
supportsClose           (not needed)            host always calls effEditClose
supportsRewire          "supportsReWire"        canDo()
supportsMultiChannel	"multiChannel"          canDo()
supportsShell           "shellCategory"         canDo()

🎯 VST2 has TWO event types
1. VstMidiEvent

This is the normal MIDI event:

    note on

    note off

    CC

    pitch bend

    etc.

2. VstEvent

This is a generic event struct that can contain:

    VstMidiEvent
    VstSysExEvent
    VstAudioEvent (rare, unused)
    VstVideoEvent (unused)
    VstEventType::kMidiType
    VstEventType::kSysExType

🎹 ASCII Table: VST2 canDo() Capabilities Explained

*/


    Envelope::test();

    m_synth.init();


}

Plugin::~Plugin()
{}

VstInt32 Plugin::canDo(char* text)
{
/*
    //  1 = yes,
    //  0 = don't know (neutral)
    // -1 = no (harsh),
    +---------------------------+-----------------------------+-----------------------------------------------------------+
    | VST2 canDo() String       | Category                    | What It Actually Means                                    |
    +---------------------------+-----------------------------+-----------------------------------------------------------+
    | receiveVstEvents          | MIDI/SysEx Input            | Receive ANY VstEvent (MIDI, SysEx, etc).                  |
    | receiveVstMidiEvent       | MIDI Input                  | Receive MIDI events (note on/off, CC, etc).               |
    | sendVstEvents             | Event Output                | Send VstEvent blocks (SysEx-capable).                     |
    | sendVstMidiEvent          | MIDI Output                 | Send MIDI events to host.                                 |
    | receiveVstTimeInfo        | Transport/Sync              | Receive tempo, PPQ, position, etc.                        |
    | midiProgramNames          | MIDI Programs               | Host may query MIDI program names.                        |
    | bypass                    | Host Bypass                 | Plugin supports host bypass.                              |
    | hasEditor                 | GUI                         | Plugin has a custom GUI editor window.                    |
    | conformsToWindowRules     | GUI                         | Plugin follows host windowing rules.                      |
    | noRealTime                | Processing                  | Plugin does NOT require real-time processing.             |
    | 1in1out                   | Audio I/O                   | Mono in → mono out.                                       |
    | 2in2out                   | Audio I/O (official)        | Stereo in → stereo out.                                   |
    | x2in2out                  | Audio I/O (extended)        | Same as 2in2out, used by some hosts for sidechain.        |
    | 1in2out                   | Audio I/O                   | Mono in → stereo out.                                     |
    | 2in1out                   | Audio I/O                   | Stereo in → mono out.                                     |
    | plugAsChannelInsert       | Host Routing                | Can be used as channel insert.                            |
    | plugAsSend                | Host Routing                | Can be used as send effect.                               |
    | mixDryWet                 | Host Mixing                 | Host can use dry/wet mixing.                              |
    | supportsReWire            | ReWire                      | Plugin supports ReWire (rare).                            |
    | multiChannel              | Audio I/O                   | Supports >2 channels (surround, etc).                     |
    | shellCategory             | Shell Plugins               | Plugin is part of a VST2 shell.                           |
    | offline                   | Offline Processing          | Plugin supports offline processing (Wavelab).             |
    | midiSingleNoteTuning      | MIDI Tuning                 | Supports single-note tuning messages.                     |
    | midiKeyBasedInstrumentCtl | MIDI Control                | Supports key-based instrument control.                    |
    | midiSendVstEvents         | MIDI Output                 | Alias for sendVstEvents (rare).                           |
    | midiSendVstMidiEvent      | MIDI Output                 | Alias for sendVstMidiEvent.                               |            |
    +---------------------------+-----------------------------+-----------------------------------------------------------+
*/

    if (!strcmp(text, "plugAsChannelInsert")) return 1;
    if (!strcmp(text, "plugAsSend")) return 1;
    //if (!strcmp(text, "1in1out")) return 1;
    //if (!strcmp(text, "1in2out")) return 1;
    if (!strcmp(text, "2in2out")) return 1;
    //if (!strcmp(text, "2in1out")) return 1;
    if (!strcmp(text, "x2in2out")) return 1;

    if (!strcmp(text, "receiveVstEvent")) return 1;
    if (!strcmp(text, "receiveVstEvents")) return 1;
    if (!strcmp(text, "receiveVstMidiEvent")) return 1;
    if (!strcmp(text, "receiveVstMidiEvents")) return 1;
    if (!strcmp(text, "receiveVstTimeInfo")) return 1;

    // if (!strcmp(text, "sendVstEvents")) return 1;
    // if (!strcmp(text, "sendVstMidiEvent")) return 1;

    if (!strcmp(text, "hasEditor")) return 1;
    if (!strcmp(text, "isSynth")) return 1;

    if (!strcmp(text, "bypass")) return 1;
    if (!strcmp(text, "midiProgramNames")) return 1;
    if (!strcmp(text, "conformsToWindowRules")) return 1;

    if (!strcmp(text, "mixDryWet")) return 1;

    return 0;
}

void Plugin::processReplacing(float** inputs, float** outputs, VstInt32 sampleFrames)
{
    const uint32_t nChannelBytes = static_cast<uint32_t>(sampleFrames) * sizeof(float);

    const float* __restrict__ const Lin = inputs[0];
    const float* __restrict__ const Rin = inputs[1];

    float* __restrict__ const Lout = outputs[0];
    float* __restrict__ const Rout = outputs[1];

    // We hate bad hosts that write outputs to same input buffer!
    DE_ASSUME_NO_OVERLAP(Lin,Lout,nChannelBytes);
    DE_ASSUME_NO_OVERLAP(Rin,Rout,nChannelBytes);

    if (!m_bPluginOpen || m_bBypassed)
    {
        // Relay L+R
        memcpy(Lout, Lin, nChannelBytes);
        memcpy(Rout, Rin, nChannelBytes);
        return;
    }

    //<With-DSP>

        m_synth.process(sampleFrames);

        const float* __restrict__ const Ldsp = m_synth.m_L.data();
        const float* __restrict__ const Rdsp = m_synth.m_R.data();

        DE_ASSUME_NO_OVERLAP(Ldsp,Lin,nChannelBytes);
        DE_ASSUME_NO_OVERLAP(Rdsp,Rin,nChannelBytes);

        DE_ASSUME_NO_OVERLAP(Ldsp,Lout,nChannelBytes);
        DE_ASSUME_NO_OVERLAP(Rdsp,Rout,nChannelBytes);

        // Output = Input(Relay) + Synth
        for (int i = 0; i < sampleFrames; ++i)
        {
            Lout[i] = Lin[i] + Ldsp[i];
            Rout[i] = Rin[i] + Rdsp[i];
        }

    //</With-DSP>
}

void Plugin::processDoubleReplacing (double** inputs, double** outputs, VstInt32 sampleFrames)
{

}

VstIntPtr
Plugin::dispatcher(VstInt32 opCode, VstInt32 index, VstIntPtr value, void* ptr, float opt)
{
    /*
    📌 Return values:

    1. Most plugins return 0 for unknown strings to avoid breaking hosts.

    2. effSetBypass
        1 → you accepted the bypass request
        0 → you don’t support bypass (host may do external bypass)

    3. String-returning opcodes: effGetEffectName, effGetVendorString, effGetProductString.
        1 → you wrote a valid string into ptr
        0 → you did not provide a string

    4. Param opcodes: effGetParamName, effGetParamLabel, effGetParamDisplay.
        1 → string written
        0 → not handled

    5. Lifecycle opcodes: effOpen, effClose, effMainsChanged, effStartProcess, effStopProcess.
        0 always, These opcodes do not define a return value.

    6. Host queries: effGetPlugCategory, effGetTailSize, effGetVstVersion.
        Return the requested integer, not 0/1.
    */
    switch (opCode)
    {
        // case effOpen:
        //     DE_BENNI("effOpen")
        //     return 0;
        case effClose: DE_BENNI("effClose")
            m_bPluginOpen = false; //delete this; // End lifecycle. We must delete ourselfs to prevent leaks.
            return 0;
        case effEditOpen: DE_BENNI("effEditOpen")
            m_editor.create(ptr);
            return 1;
        case effEditClose: DE_BENNI("effEditClose")
            m_editor.destroy();
            return 1;
        case effEditGetRect: DE_BENNI("effEditGetRect")
            *(ERect**)ptr = m_editor.getEditorRect();
            return 1;
        case effSetBypass: DE_BENNI("effSetBypass = ",value)
            m_bBypassed = value > 0 ? true : false; // with value = 1 (bypass on) or value = 0 (bypass off).
            //m_synth.setBypass( value > 0 ? true : false );
            return 1;
        case effSetSampleRate: DE_BENNI("effSetSampleRate = ",int(opt))
            m_synth.setSampleRate(int(opt));
            return 1;
        case effSetBlockSize: DE_BENNI("effSetBlockSize = ",int(value))
            m_synth.setBlockSize(int(value));
            return 1;
        case effMainsChanged: DE_BENNI("effMainsChanged = ",int(value))
            return 1;
    /*
        struct VstSpeakerArrangement
        {
            VstInt32 type; // e.g., kSpeakerArr51 for 5.1
            VstInt32 numChannels;
            VstSpeakerProperties speakers[kMaxSpeakers];
        };

        VstSpeakerArrangement** arrangements = (VstSpeakerArrangement**)ptr;
        VstSpeakerArrangement* inputArrangement = arrangements[0];
        VstSpeakerArrangement* outputArrangement = arrangements[1];

        case effSetSpeakerArrangement:
        {
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

void Plugin::setProgramName(char *name)
{
    //vst_strncpy (m_programName, name, kVstMaxProgNameLen);
}

void Plugin::getProgramName(char *name)
{
    vst_strncpy (name, "DefaultProg", kVstMaxProgNameLen);
}

bool Plugin::getEffectName(char* name)
{
    vst_strncpy(name, "SineMachine4", kVstMaxEffectNameLen);
    return true;
}

bool Plugin::getProductString(char* text)
{
    vst_strncpy (text, "Abenton SineMachine4", kVstMaxProductStrLen);
    return true;
}

bool Plugin::getVendorString(char* text)
{
    vst_strncpy (text, "Abenton", kVstMaxVendorStrLen);
    return true;
}

void Plugin::open()
{
    DE_DEBUG("")
}

void Plugin::close()
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
    m_synth.getConfig().m_partials.setPartial(index,fAmplitude);

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
    if (index < 0 || index >= int(m_synth.getConfig().m_partials.numPartials()))
    {
        return 0.0f;
    }
    return m_synth.getConfig().m_partials.m_partials[index].fAmplitude;

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

VstInt32 Plugin::processEvents(VstEvents* events)
{
    if (!events)
    {
        DE_ERROR("Got nullptr")
        return 0;
    }

    // DE_OK("numEvents = ",events->numEvents)

    for (VstInt32 i = 0; i < events->numEvents; ++i)
    {
        if (events->events[i]->type == kVstMidiType)
        {
            VstMidiEvent* midi = (VstMidiEvent*)events->events[i];
            handleShortMidi(midi->midiData);
            //handleShortMidi(reinterpret_cast<unsigned char*>(midi->midiData));
        }
    }
    return 1;
}

void Plugin::handleShortMidi(char bytes[4])
{
    uint8_t status = bytes[0] & 0xF0;
    uint8_t channel = bytes[0] & 0x0F;
    uint8_t data1 = bytes[1] & 0x7F;
    uint8_t data2 = bytes[2] & 0x7F;

    switch (status)
    {
    case 0x90:  // Note On
        if (data2 > 0)
            m_synth.noteOn(channel, data1, data2);
        else
            m_synth.noteOff(channel, data1, data2);  // velocity 0 = note off
        break;
    case 0x80:  // Note Off
        m_synth.noteOff(channel, data1, data2);
        break;
    case 0xB0:  // Control Change
        m_synth.controlChange(channel, data1, data2);
        break;
    case 0xE0:  // Pitch Bend
        m_synth.pitchBend(channel, ((data2 << 7) | data1) - 8192);
        break;
        // Add more cases as needed
    }
}

/*

void Plugin::processReplacing(float** inputs, float** outputs, VstInt32 sampleFrames)
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

void Plugin::resume()
{
    m_sampleRate = getSampleRate();  // fetch current rate from host
    calcPhaseIncrements( m_partials, m_baseFrequency, m_sampleRate );
}
void Plugin::setSampleRate(float sampleRate)
{
    m_sampleRate = sampleRate;
    calcPhaseIncrements( m_partials, m_baseFrequency, m_sampleRate );
}

VstInt32 Plugin::processEvents(VstEvents* events)
{
    for (VstInt32 i = 0; i < events->numEvents; ++i)
    {
        if (events->events[i]->type == kVstMidiType)
        {
            VstMidiEvent* midi = (VstMidiEvent*)events->events[i];
            handleMidi(reinterpret_cast<unsigned char*>(midi->midiData));
        }
    }
    return 1;
}

void Plugin::handleMidi(unsigned char* data)
{
    int status = data[0] & 0xF0;
    int channel = data[0] & 0x0F;
    int data1 = data[1] & 0x7F;
    int data2 = data[2] & 0x7F;

    switch (status) {
    case 0x90:  // Note On
        if (data2 > 0)
            noteOn(channel, data1, data2);
        else
            noteOff(channel, data1, data2);  // velocity 0 = note off
        break;
    case 0x80:  // Note Off
        noteOff(channel, data1, data2);
        break;
    case 0xB0:  // Control Change
        controlChange(channel, data1, data2);
        break;
    case 0xE0:  // Pitch Bend
        pitchBend(channel, ((data2 << 7) | data1) - 8192);
        break;
        // Add more cases as needed
    }
}

void Plugin::noteOn(int channel, int note, int velocity)
{
    m_baseFrequency = 440.0 * pow(2.0, (note - 69) / 12.0);  // MIDI to Hz
    // Optionally: trigger envelopes, voices, etc.
    calcPhaseIncrements( m_partials, m_baseFrequency, m_sampleRate );
}

void Plugin::noteOff(int channel, int note, int velocity)
{
    // If you're using a simple monophonic synth:
    if (note == currentNote)
    {
        m_baseFrequency = 0.0f;  // silence the oscillator
        currentNote = -1;
    }

    // For polyphony, you'd deactivate the voice assigned to this note
}

void Plugin::controlChange(int channel, int controller, int value)
{
    switch (controller) {
    case 1:  // Mod Wheel
        modDepth = value / 127.0f;
        break;
    case 7:  // Volume
        masterGain = value / 127.0f;
        break;
    case 74: // Filter cutoff (common mapping)
        filterCutoff = value / 127.0f;
        break;
    default:
        // Handle other CCs or ignore
        break;
    }
}

void Plugin::pitchBend(int channel, int bendValue)
{
    // Convert to semitone offset: ±2 semitones range
    float bendSemis = (bendValue / 8192.0f) * 2.0f;

    // Apply bend to current note
    if (currentNote >= 0)
    {
        m_baseFrequency = 440.0f * pow(2.0, (currentNote - 69 + bendSemis) / 12.0);
        calcPhaseIncrements( m_partials, m_baseFrequency, m_sampleRate );
    }
}

void Synth::resume()
{
    m_sampleRate = getSampleRate();  // fetch current rate from host
    calcPhaseIncrements( m_partials, m_baseFrequency, m_sampleRate );
}


void Synth::setParameter(VstInt32 index, float value)
{
    if (index < 0 || index >= int(m_partials.size()))
    {
        return;
    }
    m_partials[index].amplitude = value;
}

float Synth::getParameter(VstInt32 index)
{
    if (index < 0 || index >= int(m_partials.size()))
    {
        return 0.0f;
    }
    return m_partials[index].amplitude;
}

void Synth::open()
{
    std::thread guiThread(
        [this] ()
        {
            m_editor->open();
            m_editor->run();
        }
    );
    guiThread.detach();
}

void Synth::close()
{
    m_editor->requestClose();
}
*/

