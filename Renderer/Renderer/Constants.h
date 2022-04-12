#pragma once

constexpr int MAX_POINT_LIGHTS = 12;
constexpr int MAX_SPOT_LIGHTS = 3;

const char PATH_SEPARATOR =
#ifdef _WIN32
'\\';
#else
'/';
#endif