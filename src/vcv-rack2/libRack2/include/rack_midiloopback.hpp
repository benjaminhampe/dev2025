#pragma once
#include <vector>
#include <rack_common.hpp>
#include <rack_midi.hpp>


namespace rack {
namespace midiloopback {


struct Device;


struct RACK_DLL_API Context {
    std::vector<Device*> devices;

    Context();
    ~Context();
};


RACK_DLL_API void RACK_DLL_CALL init();


} // namespace midiloopback
} // namespace rack
