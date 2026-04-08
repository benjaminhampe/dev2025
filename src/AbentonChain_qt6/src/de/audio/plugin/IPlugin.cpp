#include <de/audio/plugin/IPlugin.h>
//#include <de/audio/plugin/details/NullPlugin.h>
#include <de/audio/plugin/details/VST2_Plugin.h>
#include <DarkImage.h>

// // *.jpg, *.jpeg
// #if defined(DE_IMAGE_READER_JPG_ENABLED) || defined(DE_IMAGE_WRITER_JPG_ENABLED)
   // #include <de/image/Image_JPG.h>
// #endif

namespace de {
namespace audio {

// ===========================================================================
// ===   PluginManager
// ===========================================================================

PluginManager::PluginManager()
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

PluginManager::~PluginManager()
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

IPlugin* PluginManager::createPlugin( std::string uri )
{
    IPlugin* plugin = nullptr;

    PerformanceTimer timer;
    timer.start();

    uri = FileSystem::makeAbsolute( uri );

    std::string suffix = FileSystem::fileSuffix( uri );

    if (suffix == "dll")
    {
        plugin = new VST2_Plugin;
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
            plugin->setPluginId( GetFreeId() );
            // m_plugins.emplace_back( plugin );
        }
    }

    timer.stop();
    DE_OK("[",suffix,"] ", timer.ms(), "ms|", uri)
    return plugin;
}

/*
IPlugin* PluginManager::getPlugin( u32 pluginId )
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

const IPlugin* PluginManager::getPlugin( u32 pluginId ) const
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

void PluginManager::removePlugin( u32 pluginId )
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
// std::shared_ptr< PluginManager >
// PluginManager::get()
// {
//    static std::shared_ptr< PluginManager > s_manager( new PluginManager() );
//    return s_manager;
// }

} // end namespace audio.    
} // end namespace de.


// std::shared_ptr< de::audio::IPlugin >
// dbLoadAudioPlugin( std::string uri )
// {
//     return de::audio::PluginManager::get()->loadPlugin(uri);
// }
	

