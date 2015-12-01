/**
 * \file Utility.h
 *
 * \author Dowon Cha
 * \date 2015
 *
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

/**
 *  
 */
namespace Utility
{
  
/*!
 * \brief Clamp a value between the min and max
 *
 * [detailed description]
 *
 * \param[in] min The minimum allowed value
 * \param[in] value The value being clamped
 * \param[in] max The maximum allowed value
 * \return value or if outside min max it will be clamped
 */
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
inline T lerp(T v0, T v1, float t) {
    return (1.0f - t) * v0 + t * v1;
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

inline GPoint& operator*=(GPoint& A, float Scalar)
{
  A.fX *= Scalar;
  A.fY *= Scalar;
  return A;
}

inline float DotProduct(const GPoint& A, const GPoint& B)
{
  return A.fX * B.fX + A.fY * B.fY;
}

inline float CrossProduct(const GPoint& A, const GPoint& B)
{
  return A.fX * B.fY - A.fY * B.fX;
}
 
GPoint UnitVector(const GPoint& A, const GPoint& B);

inline GPoint operator+(const GPoint& A, const GPoint& B)
{
  return GPoint{A.fX + B.fX, A.fY + B.fY};
}
//using Utility::operator+;

inline GPoint operator-(const GPoint& A, const GPoint& B)
{
  return GPoint{A.fX - B.fX, A.fY - B.fY};
}
//using Utility::operator-;

GPixel ColorToPixel(const GColor& color);

std::vector<GPoint> RectToPoints(const GRect& Rect);

GRect PointsToRect(const std::vector<GPoint>& Points);

GMatrix<float> RectToRect(const GRect& src, const GRect& dst);

} //namespace Utility

// Clamp functions for later on
// 3. t=(t-floor(t))*2; if (t > 1) t = 2-t;
// 4. cos ((1-t)*pi)
