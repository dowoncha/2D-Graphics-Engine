#pragma once

#include <array>

#include "GPoint.h"
#include "GColor.h"
#include "GShader.h"
#include "Utility.h"

class RadialShader : public GShader
{
private:
    const GPoint center;
    float radius;
    GColor c0, c1;
    GMatrix LocalMatrix;
    GMatrix Inverse;
public:
  RadialShader(const GPoint& center, float radius, const GColor colors[2]);
  ~RadialShader();
  bool setContext(const float ctm[6]) override;
  void shadeRow(int x, int y, int count, GPixel row[]) override;
};
