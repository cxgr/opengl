#pragma once

#include "stb_image.h"

constexpr int MAX_POINT_LIGHTS = 12;
constexpr int MAX_SPOT_LIGHTS = 3;

constexpr float NEAR_PLANE = 0.01f;
constexpr float FAR_PLANE = 100.f;

const char PATH_SEPARATOR =
#ifdef _WIN32
'\\';
#else
'/';
#endif