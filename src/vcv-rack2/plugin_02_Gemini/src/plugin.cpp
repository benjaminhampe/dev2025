#include "plugin.hpp"

rack::plugin::Plugin* pluginInstance;

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif

extern "C" {

void RACK_DLL_CALL init(rack::plugin::Plugin* p)
{
    pluginInstance = p;

    // Add modules here
    p->addModel(modelGemini);

    // Any other plugin initialization may go here.
    // As an alternative, consider lazy-loading assets and lookup tables when your
    // module is created to reduce startup times of Rack.
}

} // end extern "C"
