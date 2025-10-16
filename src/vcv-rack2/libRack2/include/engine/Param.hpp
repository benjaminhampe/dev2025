#pragma once
#include <rack_common.hpp>
#include <rack_math.hpp>


namespace rack {
namespace engine {


struct RACK_DLL_API Param {
    /** Unstable API. Use setValue() and getValue() instead. */
    float value = 0.f;

    float getValue() {
        return value;
    }

    void setValue(float value) {
        this->value = value;
    }
};


} // namespace engine
} // namespace rack
