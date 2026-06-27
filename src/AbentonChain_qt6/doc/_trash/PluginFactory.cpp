#include <de/audio/plugin/PluginFactory.h>


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
