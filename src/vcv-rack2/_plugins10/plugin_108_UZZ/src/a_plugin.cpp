#include "a_plugin.hpp"

Plugin* pluginInstance = nullptr;


void init(Plugin* p) {
	pluginInstance = p;

	// Add modules here

		p->addModel(modelUZZ);   // registra el módulo UZZ
	

	// Any other plugin initialization may go here.
	// As an alternative, consider lazy-loading assets and lookup tables when your module is created to reduce startup times of Rack.
}
