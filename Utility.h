//Copyright 2015 Dowon Cha

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

template<typename T>
inline T floor_clamp(T value) 
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

GPixel ColorToPixel(const GColor& color)
{
  GColor pinned = color.pinToUnit();            //Make sure color is between 0 and 1

  float fA =  pinned.fA * 255.9999f;		  //Convert from 0-1 to 0-255
  uint8_t uA = (uint8_t) fA;
  uint8_t uR = (uint8_t) (pinned.fR * fA);  //Multiply rgb values by the new alpha
  uint8_t uG = (uint8_t) (pinned.fG * fA);
  uint8_t uB = (uint8_t) (pinned.fB * fA);

  return GPixel_PackARGB(uA, uR, uG, uB);	  //Returned the packed pixel
}

std::vector<GPoint> RectToPoints(const GRect& Rect)
{
  //Convert the input rect into a vector with 4 point corners
  std::vector<GPoint> PreTransform({
    GPoint::Make(Rect.x(), Rect.y()),
    GPoint::Make(Rect.x() + Rect.width(), Rect.y()),
    GPoint::Make(Rect.x(), Rect.y() + Rect.height()),
    GPoint::Make(Rect.x() + Rect.width(), Rect.y() + Rect.height())
  });

  return PreTransform;
}

GRect PointsToRect(const std::vector<GPoint>& Points)
{
  assert(Points.size() == 4);

  return GRect::MakeLTRB(Points[0].x(), Points[0].y(), Points[3].x(), Points[3].y());
}

GMatrix<float> RectToRect(const GRect& src, const GRect& dst)
{
  // Make translation matrix of the source from the origin
  GMatrix<float> SrcTranslate;
  SrcTranslate.setTranslation(-1 * src.left(), -1 * src.top());

  // Make scale matrix of dx, dy. Dst / Src
  GMatrix<float> Scale;
  Scale.setScale(dst.width() / src.width(), dst.height() / src.height());

  // Get translation matrix of the dst rectangle
  GMatrix<float> DstTranslate;
  DstTranslate.setTranslation(dst.left(), dst.top());

  // Concatenate the 3 matrices. DstTranslate * Scale * SrcTranslate
  DstTranslate.concat(Scale).concat(SrcTranslate);

  return DstTranslate;
}

// Clamp functions for later on
// 3. t=(t-floor(t))*2; if (t > 1) t = 2-t;
// 4. cos ((1-t)*pi)

} //namespace
