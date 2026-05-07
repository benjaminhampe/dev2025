#pragma once
#include <DarkImage.h>
#include <vector>
#include <memory>
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

// #ifndef BENNI_USE_LV2
// #define BENNI_USE_LV2
// #endif

namespace de {
namespace audio {

class Track;

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

    virtual const Track* getTrack() const = 0;

    virtual Track* getTrack() = 0;

    virtual void setTrack(Track* track) = 0;


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


    virtual u32 getProgramCount() const = 0;

    virtual int getProgram() const = 0;

    virtual void setProgram( int i ) = 0;


    virtual u32 getParameterCount() const = 0;

    virtual std::string getParameterName(int i) const = 0;

    virtual f32 getParameter(int i) const = 0;

    virtual void setParameter(int i, f32 value) = 0;

    struct ProgramInfo
    {
        int id = 0;
        std::string name;

        std::string str() const
        {
            std::ostringstream o;
            o << "[" << id << "] " << name;
            return o.str();
        }
    };

    // virtual std::vector<ProgramInfo> getProgramInfos() const = 0;

    struct ParamInfo
    {
        int id = 0;
        float nowValue = 0.0f;
        float defValue = 0.0f;
        float minValue = 0.0f;
        float maxValue = 1.0f;
        std::string name;
        std::string unit;
        std::string disp; // displayName

        std::string str() const
        {
            std::ostringstream o;
            o << "[" << id << "] " << name;
            o << ", now(" << nowValue << ")";
            o << ", def(" << defValue << ")";
            o << ", min(" << minValue << ")";
            o << ", max(" << maxValue << ")";
            o << ", unit(" << unit << ")";
            o << ", disp(" << disp << ")";
            return o.str();
        }
    };

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
};

} // end namespace audio.
} // end namespace de.
