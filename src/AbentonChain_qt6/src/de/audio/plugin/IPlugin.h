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

namespace de {
namespace audio {

class ITrack;

// ===========================================================================
class IPlugin : public IDspChainElement,
                 public midi::IMidiMessageListener
// ===========================================================================
{
public:
    // virtual ~IPlugin() {}

    virtual const ITrack* getTrack() const = 0;

    virtual ITrack* getTrack() = 0;

    virtual void setTrack(ITrack* track) = 0;


    virtual u32 getPluginId() const = 0;

    virtual void setPluginId( u32 id ) = 0;


    virtual std::string uri() const = 0;
	
    virtual std::string name() const = 0;
	
    virtual std::string vendor() const = 0;


    virtual void openPlugin( std::string uri ) = 0;

    virtual void closePlugin() = 0;

    virtual bool isPluginOpen() const = 0;


    virtual bool isSynth() const = 0;

    virtual PluginEditorWindow* getEditor() = 0;


    virtual bool isBypassed() const = 0;

    virtual void setBypassed( bool bBypassed ) = 0;
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
