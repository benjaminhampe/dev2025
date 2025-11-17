#pragma once
#include <cstdint>
#include <cmath>
#include <algorithm>
#include <thread>

#include "pluginterfaces/vst2.x/audioeffectx.h"

#include <de_nanovg.h>
#include <GLFW/glfw3.h>

#include <DarkImage.h>
#include <de_AlignedVector.h>
#include <de_Approx_Math.h>

// 🎼 Globals & Contants:
constexpr uint32_t NUM_OVERTONES = 60;

constexpr uint32_t NUM_PARTIALS = 32;

extern int32_t m_screenWidth;

extern int32_t m_screenHeight;

extern std::atomic<bool> shouldClose;

