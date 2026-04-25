#pragma once
#include <de/audio/ITrack.h>
#include <de/audio/plugin/PluginFactory.h>
#include <de/midi/MidiCentral.h>
#include <de/audio/dsp/DspSampleCollector.h>

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

    virtual void cleanupAll() = 0;

    //=========================
    // PluginApi
    //=========================

    virtual PluginFactory& getPluginFactory() = 0;

    virtual const PluginFactory& getPluginFactory() const = 0;

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

    //=========================
    // SampleCollector
    //=========================

    virtual DspSampleCollector& getDspSampleCollector() = 0;

    virtual const DspSampleCollector& getDspSampleCollector() const = 0;
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

    void cleanupAll() override;
	
	//=========================
	// PluginApi
	//=========================
	
    PluginFactory& getPluginFactory() override;

    const PluginFactory& getPluginFactory() const override;

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

    //=========================
    // SampleCollector
    //=========================

    DspSampleCollector& getDspSampleCollector() override;

    const DspSampleCollector& getDspSampleCollector() const override;

private:
    AudioCentral_Private* _d = nullptr;
};

} // end namespace audio.
} // end namespace de.
