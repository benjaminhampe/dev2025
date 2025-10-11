#include "a_plugin.hpp"


Plugin* pluginInstance;


void init(Plugin* p) {
	pluginInstance = p;

	// Add modules here
	p->addModel(modelSEQEuclid);
	p->addModel(modelVCS1);
	p->addModel(modelVCS2);

}
