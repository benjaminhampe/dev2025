#pragma once
#include <de/audio/plugin/IPlugin.h>

namespace de {
namespace audio {

typedef std::shared_ptr<IPlugin> SharedPlugin;

// ===========================================================================
class IPluginFactory
// ===========================================================================
{
public:
    virtual ~IPluginFactory() = default;

    //===================================
    // PluginApi: VST2|VST3|CLAP|LV2
    //===================================

    virtual SharedPlugin createPlugin( std::string uri ) = 0;

};

// ===========================================================================
class PluginFactory : public IPluginFactory
// ===========================================================================
{
public:
    PluginFactory();
    ~PluginFactory() override;

    //===================================
    // PluginApi: VST2|VST3|CLAP|LV2
    //===================================

    SharedPlugin createPlugin( std::string uri ) override;

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
