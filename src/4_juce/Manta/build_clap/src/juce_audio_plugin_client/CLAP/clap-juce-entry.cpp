#include <juce_audio_plugin_client/CLAP/clap-juce-entry.h>
#include <juce_audio_plugin_client/CLAP/clap-juce-wrapper.h>
#include <Processor.h>

#ifdef __cplusplus
extern "C" {
#endif

// Define the features array
const char* features[] = {
    CLAP_FEATURES,
    nullptr
};

extern __declspec(dllexport) const struct clap_plugin_descriptor clap_desc = {
    CLAP_VERSION,
    CLAP_ID,
    JucePlugin_Name,
    JucePlugin_Manufacturer,
    JucePlugin_ManufacturerWebsite,
    CLAP_MANUAL_URL,
    CLAP_SUPPORT_URL,
    JucePlugin_VersionString,
    JucePlugin_Desc,
    features
};

#ifdef __cplusplus
}
#endif




// JUCE_BEGIN_IGNORE_WARNINGS_GCC_LIKE("-Wredundant-decls")
// juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter();
// JUCE_END_IGNORE_WARNINGS_GCC_LIKE


namespace ClapAdapter
{
    static bool
    clap_init(const char *) { return true; }

    static void
    clap_deinit(void) {}

    static uint32_t
    clap_get_plugin_count(const struct clap_plugin_factory *) { return 1; }

    static const clap_plugin_descriptor*
    clap_get_plugin_descriptor(const struct clap_plugin_factory*, uint32_t)
    {
        return &clap_desc;
    }

    const clap_plugin*
    clap_create_plugin(const struct clap_plugin_factory *, const clap_host *host, const char *plugin_id)
    {
        juce::ScopedJuceInitialiser_GUI libraryInitialiser;

        if (strcmp(plugin_id, clap_desc.id))
        {
            std::cout << "Warning: CLAP asked for plugin_id '" << plugin_id
                      << "' and JuceCLAPWrapper ID is '" << clap_desc.id << "'"
                      << std::endl;
            return nullptr;
        }
        clap_juce_extensions::clap_properties::building_clap = true;
        clap_juce_extensions::clap_properties::clap_version_major = CLAP_VERSION_MAJOR;
        clap_juce_extensions::clap_properties::clap_version_minor = CLAP_VERSION_MINOR;
        clap_juce_extensions::clap_properties::clap_version_revision = CLAP_VERSION_REVISION;
        clap_juce_extensions::clap_juce_audio_processor_capabilities::clapHostStatic = host;
        auto *const pluginInstance = ::createPluginFilter();
        clap_juce_extensions::clap_properties::building_clap = false;
        clap_juce_extensions::clap_juce_audio_processor_capabilities::clapHostStatic = nullptr;
        auto *wrapper = new ClapJuceWrapper(host, pluginInstance);
        return wrapper->clapPlugin();
    }

    static const struct clap_plugin_factory juce_clap_plugin_factory = {
        ClapAdapter::clap_get_plugin_count,
        ClapAdapter::clap_get_plugin_descriptor,
        ClapAdapter::clap_create_plugin,
    };

    static const void*
    clap_get_factory(const char *factory_id)
    {
        if (strcmp(factory_id, CLAP_PLUGIN_FACTORY_ID) == 0)
        {
            return &juce_clap_plugin_factory;
        }

#if CLAP_SUPPORTS_CUSTOM_FACTORY
        return ::clapJuceExtensionCustomFactory(factory_id);
#endif

        return nullptr;
    }

} // namespace ClapAdapter

#ifdef __cplusplus
extern "C" {
#endif

extern __declspec(dllexport) const struct clap_plugin_entry clap_entry =
{
    CLAP_VERSION,
    ClapAdapter::clap_init,
    ClapAdapter::clap_deinit,
    ClapAdapter::clap_get_factory
};

#ifdef __cplusplus
}
#endif
