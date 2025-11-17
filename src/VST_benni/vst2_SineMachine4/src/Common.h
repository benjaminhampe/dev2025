#pragma once
#include <cstdint>
#include <string>
#include <cmath>
#include <algorithm>
#include <thread>

#include "pluginterfaces/vst2.x/audioeffectx.h"

#ifndef NANOVG_GL3_IMPLEMENTATION
#define NANOVG_GL3_IMPLEMENTATION
#endif
#include "nanovg.h"
#include "nanovg_gl.h"

// #include <GLFW/glfw3.h> // Bad: GLFW uses TopLevelWindows, not well managable by Hosts.

#include <DarkImage.h>
#include <de_AlignedVector.h>
#include <de_Approx_Math.h>
