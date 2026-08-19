#pragma once
#include <clap/clap.h>

#ifdef __cplusplus
extern "C" {
#endif
    extern __declspec(dllexport) const struct clap_plugin_descriptor clap_desc;

    extern __declspec(dllexport) const struct clap_plugin_entry clap_entry;

#ifdef __cplusplus
}
#endif

/*
// // Forward declaration of the features array
// extern const char* features[];

// // Forward declaration of the descriptor
// extern const clap_plugin_descriptor ClapPluginDescriptor;

// // JUCE factory function
// JUCE_BEGIN_IGNORE_WARNINGS_GCC_LIKE("-Wredundant-decls")
// juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter();
// JUCE_END_IGNORE_WARNINGS_GCC_LIKE

#ifdef __cplusplus
extern "C" {
#endif

extern __declspec(dllexport) void ensureDesktopOpenGL();

extern __declspec(dllexport) long glGetInteger( unsigned long query );

#ifdef __cplusplus
}
#endif


extern "C"
{
#if JUCE_LINUX
#pragma GCC diagnostic ignored "-Wattributes"
#endif


#if 0
#if JUCE_MINGW
    extern
#endif
    const CLAP_EXPORT struct clap_plugin_entry clap_entry =
#endif

    const __declspec(dllexport) struct clap_plugin_entry clap_entry =
    {
        CLAP_VERSION,
        ClapAdapter::clap_init,
        ClapAdapter::clap_deinit,
        ClapAdapter::clap_get_factory
    };
}
*/