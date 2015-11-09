#pragma once

#include <vector>
#include <cmath>
#include <assert.h>

#include "GPixel.h"
#include "GColor.h"
#include "GPoint.h"
#include "GMatrix.h"
#include "GRect.h"

namespace Utility
{
template<typename T>
inline T clamp(T min, T value, T max) {
        return std::max(min, std::min(value, max));
}

template <typename T>
inline T lerp(T v0, T v1, T t) {
    return (1 - t) * v0 + t * v1;
}

inline int round(float a)
{
  return (int)std::floor(a + .5);
}

GPixel ColorToPixel(const GColor& color);

std::vector<GPoint> QuadToPoints(const GRect& Rect);

GRect PointsToQuad(const std::vector<GPoint>& Points);

GMatrix<float> RectToRect(const GRect& src, const GRect& dst);

// 1. Clamp Min(max(0,t), 1)
// 2. t - floor(t)
// 3. t=(t-floor(t))*2; if (t > 1) t = 2-t;
// 4. cos ((1-t)*pi)

} //namespace
