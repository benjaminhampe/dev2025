#pragma once
#include <DarkImage.h>
//#include <vector>
//#include <memory>
#include <de/midi/IMidiMessageListener.h>
#include <de/audio/dsp/IDspChainElement.h>
#include <de/audio/plugin/PluginEditorWindow.h>

#ifndef BENNI_USE_VST2
#define BENNI_USE_VST2
#endif

#ifndef BENNI_USE_VST3
#define BENNI_USE_VST3
#endif

#ifndef BENNI_USE_CLAP
#define BENNI_USE_CLAP
#endif

#ifndef BENNI_USE_LV2
#define BENNI_USE_LV2
#endif

namespace de {
namespace audio {

class IPlugin;
class DspTrack;

// ================================================================
struct Parameter
// ================================================================
{
    // kNoFlags = 0,
    // kCanAutomate // Parameter can be automated.
    // kIsReadOnly // Parameter cannot be changed from outside the plug-in, (implies that kCanAutomate is NOT set).
    // kIsWrapAround // Attempts to set the parameter value out of the limits will result in a wrap around.
    // kIsList
    // kIsHidden
    // kIsProgramChange
    // kIsBypass
    // CLAP_PARAM_IS_STEPPED
    // CLAP_PARAM_IS_PERIODIC
    // CLAP_PARAM_IS_HIDDEN
    // CLAP_PARAM_IS_READONLY
    // CLAP_PARAM_IS_BYPASS
    // CLAP_PARAM_IS_AUTOMATABLE
    // CLAP_PARAM_IS_AUTOMATABLE_PER_NOTE_ID
    // CLAP_PARAM_IS_AUTOMATABLE_PER_KEY
    // CLAP_PARAM_IS_AUTOMATABLE_PER_CHANNEL
    // CLAP_PARAM_IS_AUTOMATABLE_PER_PORT
    // CLAP_PARAM_IS_MODULATABLE
    // CLAP_PARAM_IS_MODULATABLE_PER_NOTE_ID
    // CLAP_PARAM_IS_MODULATABLE_PER_KEY
    // CLAP_PARAM_IS_MODULATABLE_PER_CHANNEL
    // CLAP_PARAM_IS_MODULATABLE_PER_PORT

    enum eFlags
    {
        kNoFlags = 0,        //
        kIsReadOnly = 1,    // e.g. LevelMeter, // e.g. Select preset, things that trigger expensive reconfig
        kIsList = 2, // Parameter should be displayed as list in generic editor or automation editing.
        kCanAutomate = 4, // e.g. LFO Rate, things that can change sample accurate multiple times per block.
        kIsHidden = 8,  // Parameter should be NOT displayed and cannot be changed from outside the plug-in. It implies that kCanAutomate is NOT set and kIsReadOnly is set.     // e.g. By plugin injected MIDI CC legacy params.
        kIsWrapAround = 16, // Attempts to set the parameter value out of the limits will result in a wrap around.
        kIsProgramChange = 32, // Parameter is a program change (unitId gives info about associated unit - see \ref vst3ProgramLists).
        kIsBypass = 64, // Special bypass parameter (only one allowed): plug-in can handle bypass. Highly recommended to export a bypass parameter for effect plug-in.

    };

    IPlugin* m_plugin;
    uint32_t m_id = 0;      ///< unique identifier of this parameter (named tag too)
    uint32_t m_flags = 0;
     int32_t m_unitId = 0;    ///< id of unit this parameter belongs to (see \ref vst3Units)
    uint32_t m_stepCount = 0; // number of discrete steps (0: continuous, 1: toggle, discrete value
                              // otherwise (corresponding to max - min, for example:
                              // 127 for min = 0 and max = 127) - see \ref vst3ParameterIntro)
    float m_defValue = 0.0f; ///< default normalized value [0,1]
                                       /// in case of discrete value:
                                       /// defaultNormalizedValue = defDiscreteValue/stepCount
    float m_nowValue = 0.0f;
    float m_minValue = 0.0f;
    float m_maxValue = 1.0f;

    std::string m_name; ///< parameter title (e.g. "Volume")
    std::string m_disp; ///< parameter shortTitle (e.g. "Vol") displayName|shortName;
    std::string m_unit; ///< parameter unit (e.g. "dB")


    std::string str() const
    {
        std::ostringstream o;
        o << "[" << m_id << "] " << m_name;
        o << ", flags(" << dbHex(m_flags) << ")";
        o << ", now(" << m_nowValue << ")";
        o << ", def(" << m_defValue << ")";
        o << ", min(" << m_minValue << ")";
        o << ", max(" << m_maxValue << ")";
        o << ", unit(" << m_unit << ")";
        o << ", disp(" << m_disp << ")";
        return o.str();
    }
};

typedef std::vector<Parameter> Parameters;

// ================================================================
struct Program
// ================================================================
{
    int m_progIndex = 0;  // The index inside current list.
    int m_listIndex = 0;     // Overall list index
    uint32_t m_listId = 0;   // Overall list id
    std::string m_name;

    std::string str() const
    {
        std::ostringstream o;
        o << "[" << m_progIndex << "][" << m_listId << "] " << m_name;
        return o.str();
    }
};

typedef std::vector<Program> Programs;


// ===========================================================================
class IPlugin : public IDspChainElement,
                 public midi::IMidiMessageListener
// ===========================================================================
{
public:
    enum eType {
        eT_Unknown = 0,
        eT_VST2,
        eT_VST3,
        eT_CLAP,
        eT_LV2,
        eT_Benni,
        eT_LADSPA,
        eT_DSSI,
        eT_AAX,
        eT_AU
    };

    // virtual ~IPlugin() {}

    virtual const DspTrack* getTrack() const = 0;

    virtual DspTrack* getTrack() = 0;

    virtual void setTrack(DspTrack* track) = 0;


    virtual u32 getPluginId() const = 0;

    virtual void setPluginId( u32 id ) = 0;


    virtual eType getType() const = 0;

    virtual std::string getTypeStr() const
    {
        switch (getType())
        {
            case eT_VST2: return "VST2";
            case eT_VST3: return "VST3";
            case eT_CLAP: return "CLAP";
            case eT_LV2: return "LV2";
            case eT_Benni: return "Benni";
            case eT_LADSPA: return "LADSPA";
            case eT_DSSI: return "DSSI";
            case eT_AAX: return "AAX";
            case eT_AU: return "AU";
            default: return "Unknown";
        }
    }

    virtual std::string getUri() const = 0;

    virtual std::string getName() const = 0;

    virtual std::string getVendor() const = 0;

    virtual std::string getVersion() const = 0;

    virtual double getRuntime() const = 0; // runtime of dsp_read() in seconds. -> CLAP seems slow, so we added for perf analysis.


    virtual void openPlugin( std::string uri ) = 0;

    virtual void closePlugin() = 0;

    virtual bool isPluginOpen() const = 0;


    virtual bool isSynth() const = 0;

    virtual PluginEditorWindow* getEditor() = 0;


    virtual bool isBypassed() const = 0;

    virtual void setBypassed( bool bBypassed ) = 0;


    // virtual u32 getProgramCount() const = 0;

    // virtual std::string getProgramName(int i) const = 0;

    virtual int getProgram() const = 0;

    virtual void setProgram( int i ) = 0;


    // virtual u32 getParameterCount() const = 0;

    // virtual std::string getParameterName(int i) const = 0;


    virtual const Programs& getPrograms() const = 0;

    virtual const Parameters& getParameters() const = 0;

    virtual f64 getParameterValue(uint32_t id) const = 0;

    virtual void setParameterValue(uint32_t id, f64 value, int64_t framePos = 0) = 0;


    // virtual int getActiveProgram( int sublist = 0 ) const = 0;

    // virtual void setActiveProgram( int i, int sublist = 0 ) = 0;

    // virtual void setParameter(uint32_t id, double value, int64_t framePos = 0) = 0;

    // virtual std::vector<ParamInfo> getParamInfos() = 0;

    enum eSpecialValue {
        eSV_NormalizedSumL,
        eSV_NormalizedSumR,
    };

    virtual float getSpecialValue( eSpecialValue type ) const = 0;
/*
    // =====================================
    // interface: IDspChainElement
    // =====================================
    bool isBypassed() const override { return m_pluginInfo.isBypassed(); }
    void setBypassed( bool bypassed ) override;

    // =====================================
    // interface: IPlugin
    // =====================================
    int pluginProgramCount() const { return pluginInfo().numPrograms(); }
    int pluginParamCount() const { return pluginInfo().numParams(); }
    int pluginInputCount() const { return pluginInfo().numInputs(); }
    int pluginOutputCount() const  { return pluginInfo().numOutputs(); }
    int pluginVendorVersion() const { return 1; }
    std::string pluginVendorString() const { return "IPluginVendor"; }
    std::string pluginProductString() const { return "IPluginProduct"; }
    //uint32_t pluginSampleRate() const override;
    //uint64_t pluginSamplePos() const;
    //uint64_t pluginBlockSize() const;
    //uint64_t pluginChannelCount() const;

   // =====================================
   // interface: IDspChainElement
   // =====================================


*/

    // GUI state:

    bool m_bCollapsed{false};

    void setCollapsed( bool bCollapsed ) { m_bCollapsed = bCollapsed; }
    bool isCollapsed() const { return m_bCollapsed; }
};

} // end namespace audio.
} // end namespace de.
