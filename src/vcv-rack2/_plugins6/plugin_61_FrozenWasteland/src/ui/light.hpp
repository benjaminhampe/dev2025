#pragma once
#include <rack.hpp>

template <typename TBase>
struct LargeSMLight : TBase {
    LargeSMLight() {
        this->box.size = rack::math::Vec(26, 12.5);
    }
};
