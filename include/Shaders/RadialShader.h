#pragma once

#include "GPoint.h"
#include "GColor.h"
#include "GShader.h"

class RadialShader : public GShader
{
private:
    const GPoint center;
    float radius;
    const GColor colors[2];
public:
  RadialShader(const GPoint, float radius, const GColor colors[2]);
  ~RadialShader();
  bool setContext(const float ctm[6]) override;
  void shadeRow(int x, int y, int count, GPixel row[]) override;
};
