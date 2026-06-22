#include <de/audio/plugin/PluginFactory.h>
#include <de/audio/plugin/details/VST2_Plugin.h>
#include <de/audio/plugin/details/VST3_Plugin.h>
#include <de/audio/plugin/details/CLAP_Plugin.h>
#include <de/audio/player/Player.h>

// #if defined(DE_IMAGE_READER_JPG_ENABLED) || defined(DE_IMAGE_WRITER_JPG_ENABLED)
   // #include <de/image/Image_JPG.h>
// #endif

namespace de {
namespace audio {

// ===========================================================================
// ===   PluginFactory
// ===========================================================================

PluginFactory::PluginFactory()
{
// initThreadPool();

// #ifdef DE_IMAGE_READER_JPG_ENABLED
   // m_Reader.push_back( new image::ImageReaderJPG );
// #endif
// #ifdef DE_IMAGE_WRITER_JPG_ENABLED
   // m_Writer.push_back( new image::ImageWriterJPG );
// #endif

// #ifdef _DEBUG
   // DE_DEBUG("Add image codec readers and writers")
   // DE_DEBUG("Supported image reader: ", m_Reader.size())
   // DE_DEBUG("Supported image writer: ", m_Writer.size())
   // // AsciiArt::test();
// #endif
}

PluginFactory::~PluginFactory()
{
/*
    for (size_t i = 0; i < m_plugins.size(); ++i)
    {
        auto p = m_plugins[i];
        if (!p)
        {
            DE_ERROR("Got nullptr at ",i)
        }
        else
        {
            delete p;
        }
    }
    m_plugins.clear();
*/
}


//=========================
// PluginApi
//=========================

SharedPlugin PluginFactory::createPlugin( std::string uri )
{
    PerformanceTimer timer;
    timer.start();

    uri = FileSystem::makeAbsolute( uri );

    IPlugin* plugin = nullptr;



    std::string suffix = FileSystem::fileSuffix( uri );
    if (suffix.empty())
    {
        DE_ERROR("Got empty extension, not able to determine plugin type.")
    }
#ifdef BENNI_USE_VST2
    else if (suffix == "dll")  { plugin = new VST2_Plugin; }
#endif
#ifdef BENNI_USE_VST3
    else if (suffix == "vst3") { plugin = new VST3_Plugin; }
#endif
#ifdef BENNI_USE_CLAP
    else if (suffix == "clap") { plugin = new CLAP_Plugin; }
#endif
    else if (
        (suffix == "mp4") || (suffix == "m4a") ||
        (suffix == "mp3") ||
        (suffix == "wav"))
    {
        plugin = new Player;
    }
    else
    {
        DE_ERROR("Unsupported extension (yet) ", suffix)
    }

    if (plugin)
    {
        plugin->openPlugin(uri);

        if (!plugin->isPluginOpen())
        {
            DE_ERROR("Cant open")
            delete plugin;
            plugin = nullptr;

        }
        else
        {
            plugin->setPluginId( GetFreePluginId() );
            // m_plugins.emplace_back( plugin );
        }
    }

    timer.stop();
    DE_OK("[",suffix,"] ", timer.ms(), "ms|", dbFileName(uri))
    return SharedPlugin(plugin);
}

/*
IPlugin* PluginFactory::getPlugin( u32 pluginId )
{
    // if (pluginId < 1)
    // {
    //     DE_ERROR("Invalid id 0")
    //     return nullptr;
    // }
    auto it = std::find_if( m_plugins.begin(), m_plugins.end(),
                            [pluginId] (IPlugin* p) { return p && (p->id() == pluginId); });
    if (it == m_plugins.end())
    {
        DE_ERROR("No plugin with id ",pluginId)
        return nullptr;
    }
    return *it;
}

const IPlugin* PluginFactory::getPlugin( u32 pluginId ) const
{
    // if (pluginId < 1)
    // {
    //     DE_ERROR("Invalid id 0")
    //     return nullptr;
    // }
    auto it = std::find_if( m_plugins.begin(), m_plugins.end(),
                            [pluginId] (IPlugin* p) { return p && (p->id() == pluginId); });
    if (it == m_plugins.end())
    {
        DE_ERROR("No plugin with id ",pluginId)
        return nullptr;
    }
    return *it;
}

void PluginFactory::removePlugin( u32 pluginId )
{
    IPlugin* plugin = getPlugin( pluginId );
    if (!plugin)
    {
        DE_ERROR("No plugin with id ", pluginId)
        return;
    }

    // std::lock_guard<std::mutex> lock(m_mutex);
    m_plugins.erase(
        std::remove(m_plugins.begin(), m_plugins.end(), plugin),
        m_plugins.end()
    );
}
*/

// static
// std::shared_ptr< PluginFactory >
// PluginFactory::get()
// {
//    static std::shared_ptr< PluginFactory > s_manager( new PluginFactory() );
//    return s_manager;
// }

} // end namespace audio.
} // end namespace de.
