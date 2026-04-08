#pragma once
#include <DarkImage.h>
#include <vector>
#include <memory>
#include <de/midi/IMidiMessageListener.h>
#include <de/audio/dsp/IDspChainElement.h>
#include <de/audio/plugin/PluginEditorWindow.h>

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

    virtual bool hasEditor() const = 0;

    virtual PluginEditorWindow* getEditor() = 0;

    // virtual QRect getEditorRect() const = 0;

    // virtual void openEditor( u64 parent ) = 0; // { return nullptr; }

    // virtual void closeEditor() = 0; // {}

    // virtual bool isEditorOpen() = 0; // { return false; }

    // virtual void moveEditor( int x, int y ) = 0; // {}

    // virtual bool isEditorVisible() = 0; // { return false; }

    // virtual void setEditorVisible( bool bVisible ) = 0; // {}

    // inline void showEditor() { setEditorVisible( true ); }

    // inline void hideEditor() { setEditorVisible( false ); }
/*
    // =====================================
    // interface: IDspChainElement
    // =====================================
    bool isBypassed() const override { return m_pluginInfo.isBypassed(); }
    bool isSynth() const override { return m_pluginInfo.isSynth(); }
    // =====================================
    // interface: IPlugin
    // =====================================
    de::audio::PluginInfo const & pluginInfo() const { return m_pluginInfo; }
    de::audio::PluginInfo & pluginInfo() { return m_pluginInfo; }
    int pluginId() const { return m_id; }
    QRect pluginEditorRect() const { return m_editorWindow ? m_editorWindow->rect() : QRect(); }
    bool isPluginMinimized() const { return m_pluginInfo.isMinimized(); }
    bool isPluginEditorVisible() const { return m_editorWindow ? m_editorWindow->isVisible() : false; }
    bool isPluginMoreVisible() const { return m_isMoreVisible; }
    bool hasPluginEditor() const { return pluginInfo().hasEditor(); }
    PluginEditorWindow* pluginEditorWindow() { return m_editorWindow; }
    std::wstring pluginUri() const { return pluginInfo().m_uri; }
    std::wstring pluginName() const { return pluginInfo().m_name; }
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
   void setBypassed( bool bypassed ) override;
   void sendMidi( uint8_t byte1, uint8_t data1, uint8_t data2 ) override;
   void setInputSignal( int i, de::audio::IDspChainElement* input ) override;
   void clearInputSignals() override;
   void aboutToStart( uint32_t dstFrames, uint32_t dstChannels, uint32_t dstRate ) override;
   uint64_t readSamples( double pts, float* dst, uint32_t dstFrames, uint32_t dstChannels, uint32_t dstRate ) override;
   // =====================================
   // interface: IPlugin
   // =====================================
   bool openPlugin( de::audio::PluginInfo const & pluginInfo );
   void closePlugin();
   void showEditor() { setEditorVisible( true ); }
   void hideEditor() { setEditorVisible( false ); }
   void moveEditor( int x, int y );
   void setEditorVisible( bool visible );
   void setExtraMoreVisible( bool visible );

protected:
   // =====================================
   // interface: IVst2Plugin|AEffectx
   // =====================================
   bool getFlags( int32_t m ) const
   {
      return m_vst ? ((m_vst->flags & m) == m) : 0;
   }
   static VstIntPtr
   hostCallback_static( AEffect* effect, VstInt32 opcode, VstInt32 index, VstIntPtr value, void *ptr, float opt );
   VstIntPtr
   hostCallback( VstInt32 opcode, VstInt32 index, VstIntPtr value, void* ptr, float opt );
   intptr_t
   dispatcher( int32_t opcode, int32_t index = 0, intptr_t value = 0, void *ptr = nullptr, float opt = 0.0f ) const;
   void
   processVstMidiEvents();
   const char**
   getCapabilities() const;

*/
};

// TODO: Rename to factory. It is not a manager anymore.

// ===========================================================================
class IPluginManager
// ===========================================================================
{
public:
    virtual ~IPluginManager() = default;

    //=========================
    // PluginApi
    //=========================

    virtual IPlugin* createPlugin( std::string uri ) = 0;

    // virtual void removePlugin( IPlugin* plugin ) = 0;

    //=========================
    // PluginApi with IDs
    //=========================

    // virtual IPlugin* getPlugin( u32 id ) = 0;

    // virtual const IPlugin* getPlugin( u32 id ) const = 0;

    // virtual void removePlugin( u32 id ) = 0;
};

// ===========================================================================
class PluginManager : public IPluginManager
// ===========================================================================
{
public:
    PluginManager();
    ~PluginManager() override;

    //static std::shared_ptr< PluginManager >
    //get();

    //=========================
    // PluginApi
    //=========================

    IPlugin* createPlugin( std::string uri ) override;

    // void removePlugin( IPlugin* plugin ) override;

    //=========================
    // PluginApi with IDs
    //=========================

    // IPlugin* getPlugin( u32 id ) override;

    // const IPlugin* getPlugin( u32 id ) const override;

    // void removePlugin( u32 id ) override;

/*
    // ThreadPoolWithTasks &
    // getThreadPool() { return m_threadPoolWithTasks; }

    void initThreadPool()
    {
        //uint32_t nThreads = std::thread::hardware_concurrency() - 1;
        //m_threadPoolWithTasks.reset( nThreads );
    }

    static void
    joinAsync() 
	{ 
		// get()->getThreadPool().wait_for_tasks(); 
	}

    static void
    loadAsync( std::string uri,
                    const ImageLoadOptions& options,
                    const FN_IMAGE_CALLBACK& onLoadFinished )
    {

        auto & g_threadPool = get()->getThreadPool();

        g_threadPool.push_task(
            [&] ()
            {
                auto img = new Image();
                if (get()->loadImage(*img,uri))
                {
                    onLoadFinished(img);
                }
                else
                {
                    delete img;
                }
            }
        );

        //g_threadPool.wait_for_tasks();
    }
*/

private:
    static u32 GetFreeId()
    {
        static u32 s_id = 0;
        return ++s_id;
    }

    // std::vector< IPlugin* > m_plugins;
	
    bool m_bDebug = true;
	bool m_bThrowOnFail = false;
	
	// ThreadPoolWithTasks m_threadPoolWithTasks;	
};

} // end namespace audio.
} // end namespace de.

// std::shared_ptr< de::audio::IPlugin >
// dbLoadAudioPlugin( std::string uri );