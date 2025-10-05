#pragma once
#include <rack.hpp>

//using namespace rack;

// Declare the Plugin, defined in plugin.cpp
extern rack::plugin::Plugin* pluginInstance;

// Declare each Model, defined in each module source file
extern rack::plugin::Model* modelGemini;


// #ifdef __cplusplus
// extern "C" {
// #endif

// RACK_DLL_API void RACK_DLL_CALL init(rack::plugin::Plugin*);

// #ifdef __cplusplus
// }
// #endif
