#pragma once
#include <de/audio/ITrack.h>
#include <de/audio/plugin/IPlugin.h>
#include <de/midi/MidiCentral.h>

namespace de {
namespace audio {

// ===========================================================================
class IAudioCentral
// ===========================================================================
{
public:
    virtual ~IAudioCentral() = default;

    //=========================
    // DriverApi
    //=========================

    virtual void confAudio( int outputDevice = -1,
                    int inputDevice = -1,
                    int sampleRate = 48000,
                    int blockSize = 128,
                    int channels = 2 ) = 0;

    virtual void playAudio() = 0;

    virtual void stopAudio() = 0;

    //=========================
    // PluginApi
    //=========================

    virtual PluginManager& getPluginManager() = 0;

    virtual const PluginManager& getPluginManager() const = 0;

    // virtual u32 createPlugin( std::string uri ) = 0;

    // virtual IPlugin* getPlugin( u32 id ) = 0;

    // virtual void removePlugin( u32 id ) = 0;

    //=========================
    // TrackApi
    //=========================

    virtual u32 addTrack( std::string name ) = 0;

    virtual ITrack* getTrack( u32 id ) = 0;

    virtual void removeTrack( u32 id ) = 0;

    //=========================
    // MidiApi
    //=========================

    virtual midi::MidiCentral& getMidiCentral() = 0;

    virtual const midi::MidiCentral& getMidiCentral() const = 0;
};

class AudioCentral_Private;

// ===========================================================================
class AudioCentral : public IAudioCentral
// ===========================================================================
{
public:
    AudioCentral();
    ~AudioCentral() override;

	//=========================
	// DriverApi
	//=========================

    void confAudio( int outputDevice = -1,
					int inputDevice = -1, 
					int sampleRate = 48000, 
					int blockSize = 128, 
                    int channels = 2 ) override;

    void playAudio() override;
	
    void stopAudio() override;
	
	//=========================
	// PluginApi
	//=========================
	
    PluginManager& getPluginManager() override;

    const PluginManager& getPluginManager() const override;

 //    IPlugin* createPlugin( std::string uri ) override;
	
 //    IPlugin* getPlugin( u32 id ) override;

 //    void removePlugin( u32 id ) override;

	//=========================
	// TrackApi
	//=========================

    u32 addTrack( std::string name ) override;

    ITrack* getTrack( u32 id ) override;

    void removeTrack( u32 id ) override;
	
	//=========================
	// MidiApi
	//=========================
	    
    midi::MidiCentral& getMidiCentral() override;

    const midi::MidiCentral& getMidiCentral() const override;

private:
    AudioCentral_Private* _d = nullptr;
};

} // end namespace audio.
} // end namespace de.
