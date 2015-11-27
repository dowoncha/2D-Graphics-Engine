/* 
 * Copyright 2015 Dowon Cha
 * Set of helper functions to use in project. Contains various clamp functions
 * Along with polygon conversion functions
 */
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
inline T clamp(T min, T value, T max) 
{
  return std::max(min, std::min(value, max));
}

inline float floor_clamp(float value) 
{
  return value - std::floor(value);
}

template <typename T>
inline T lerp(T v0, T v1, T t) {
    return (1 - t) * v0 + t * v1;
}

inline int round(float a)
{
  return (int)std::floor(a + .5);
}

inline uint8_t floatToByte(float i)
{
  int isx = (int)(i * 255.99999f);
  return (uint8_t)(isx & 0xff);
}

GPoint unitVector(const GPoint& A, const GPoint& B);

inline GPoint operator+(const GPoint& A, const GPoint& B)
{
  return GPoint{A.fX + B.fX, A.fY + B.fY};
}

inline GPoint operator-(const GPoint& A, const GPoint& B)
{
  return GPoint{A.fX - B.fX, A.fY - B.fY};
}

GPixel ColorToPixel(const GColor& color);

std::vector<GPoint> RectToPoints(const GRect& Rect);

GRect PointsToRect(const std::vector<GPoint>& Points);

GMatrix<float> RectToRect(const GRect& src, const GRect& dst);

} //namespace Utility

// Clamp functions for later on
// 3. t=(t-floor(t))*2; if (t > 1) t = 2-t;
// 4. cos ((1-t)*pi)
