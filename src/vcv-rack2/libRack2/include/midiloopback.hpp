#pragma once
#include <vector>
#include <common.hpp>
#include <midi.hpp>


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
