//Copyright 2015 Dowon Cha

#pragma once

#include "GPoint.h"
#include "GColor.h"
#include "GShader.h"
#include "GMatrix.h"
#include "Utility.h"

class LinearShader : public GShader
{
private:
  GColor c0, c1;
  GMatrix<float> LocalMatrix;
  GMatrix<float> Inverse;
public:
  LinearShader(const GPoint pts[2], const GColor colors[2]);
  ~LinearShader();

  bool setContext(const float ctm[6]) override;
  void shadeRow(int x, int y, int count, GPixel row[]) override;
};
