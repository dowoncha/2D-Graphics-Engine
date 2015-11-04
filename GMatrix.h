#pragma once

#include "GPoint.h"

class GMatrix
{
private:
  float Matrix[9];
public:
  GMatrix(const float in[6]);

  static GMatrix MakeTranslationMatrix(float x, float y);
  static GMatrix MakeScaleMatrix(float dx, float dy);
  static GMatrix MakeIdentityMatrix();

  void convertPoint(GPoint& P) const;
  GMatrix& concat(const GMatrix& InMat);
  GMatrix inverse();

  void GetTwoRows(float ctm[6])
  {
    for (int i = 0; i < 6; ++i) {
      ctm[i] = Matrix[i];
    }
  }
  bool preservesRect() {
    return Matrix[1] == 0.0f && Matrix[3] == 0.0f;
  }
};
