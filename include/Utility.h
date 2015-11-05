#pragma once

#include <cmath>

namespace Utility
{

template<typename T>
inline T clamp(T min, T value, T max) {
        return std::max(min, std::min(value, max));
}

template <typename T>
inline T lerp(T v0, T v1, T t) {
    return v0 - t * (v1 - v0);
}

inline int round(float a)
{
  return (int)std::floor(a + .5);
}

// 1. Clamp Min(max(0,t), 1)
// 2. t - floor(t)
// 3. t=(t-floor(t))*2; if (t > 1) t = 2-t;
// 4. cos ((1-t)*pi)

} //namespace
