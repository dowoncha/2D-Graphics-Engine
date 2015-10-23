#pragma once

#include "GMatrix.h"
#include "GPoint.h"

class GMatrix3x3f : public GMatrix<float>
{
public:
  GMatrix3x3f(std::initializer_list<float> Elements);

  GMatrix3x3f(std::vector<float> Elements);

  static GMatrix3x3f MakeTranslationMatrix(float x, float y);

  static GMatrix3x3f MakeScaleMatrix(float dx, float dy);

  static GMatrix3x3f MakeIdentityMatrix();

  static GMatrix3x3f MakeMatrix(std::initializer_list<float> Elements);

  /* Check whether the matrix will preserve the rectanglular shape*/
  bool PreservesRect();

  void Round();

  void concat(const GMatrix3x3f& a);

  GMatrix3x3f operator*(const GMatrix3x3f& a);

  GPoint ConvertPoint(const GPoint& P) const;

  GMatrix3x3f GetInverse() const;
};
