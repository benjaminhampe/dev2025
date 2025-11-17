#include "plugin.hpp"

rack::plugin::Plugin* pluginInstance;

extern "C" {

void RACK_DLL_CALL init(rack::plugin::Plugin* p)
{
    pluginInstance = p;

    // Add modules here
    p->addModel(modelDisplayCV_as_MidiNote);

    p->addModel(modelPartial);

    p->addModel(modelHostVst2);
    // Any other plugin initialization may go here.
    // As an alternative, consider lazy-loading assets and lookup tables when your
    // module is created to reduce startup times of Rack.
}

} // end extern "C"
