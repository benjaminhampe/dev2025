#pragma once
#include <DarkImage.h>
#include <de/midi/IMidiMessageListener.h>
#include <de/audio/dsp/IDspChainElement.h>
#include <de/audio/plugin/IPlugin.h>

namespace de {
namespace audio {

// ===========================================================================
class ITrack : public IDspChainElement
// ===========================================================================
{
public:
    virtual ~ITrack() = default;
	
    virtual u32 getTrackId() const = 0;

    virtual void setTrackId(u32 trackId) = 0;

    // virtual void addPlugin(u32 pluginIds, int index = -1) = 0;

    virtual std::string getTrackName() const = 0;

    //=========================
    // PluginApi
    //=========================

    virtual IPlugin* createPlugin( std::string uri, int index = -1) = 0;

    virtual void removePlugin( IPlugin* plugin ) = 0;

    // virtual void addPlugin(IPlugin* pluginIds, int index = -1) = 0;

    // virtual void confPlugin(std::vector<u32> pluginIds) = 0;

    // virtual const std::vector<u32>& getPluginIds() const = 0;

    // virtual void confPlugin(std::vector<u32> pluginIds) = 0;

 //    virtual const std::vector<u32>& getPluginIds() const = 0;



};

} // end namespace audio.
} // end namespace de.
