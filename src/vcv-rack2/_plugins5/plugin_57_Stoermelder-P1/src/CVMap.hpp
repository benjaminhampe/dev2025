#pragma once
#include "a_plugin.hpp"

namespace StoermelderPackOne {
namespace CVMap {

struct CVMapCtxBase : Module {
	virtual std::string getCVMapId() { return ""; }
};

} // namespace CVMap
} // namespace StoermelderPackOne