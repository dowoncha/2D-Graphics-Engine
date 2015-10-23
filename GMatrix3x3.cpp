#include "GMatrix3x3.h"

GMatrix3x3f::GMatrix3x3f(std::initializer_list<float> Elements)
  : GMatrix<float>(3, 3, Elements)
{}

GMatrix3x3f::GMatrix3x3f(std::vector<float> Elements)
  : GMatrix<float>(3, 3, Elements)
{}

GMatrix3x3f GMatrix3x3f::MakeTranslationMatrix(float x, float y)
{
  return GMatrix3x3f({1.0f, 0.0f , x, 0.0f, 1.0f, y, 0.0f, 0.0f, 1.0f});
}

GMatrix3x3f GMatrix3x3f::MakeScaleMatrix(float dx, float dy)
{
  return GMatrix3x3f({dx, 0.0f, 0.0f, 0.0f, dy, 0.0f, 0.0f, 0.0f, 1.0f});
}

GMatrix3x3f GMatrix3x3f::MakeIdentityMatrix()
{
  return GMatrix3x3f({1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f});
}

GMatrix3x3f GMatrix3x3f::MakeMatrix(std::initializer_list<float> Elements)
{
  return GMatrix3x3f(Elements);
}

/* Check whether the matrix will preserve the rectanglular shape*/
bool GMatrix3x3f::PreservesRect()
{
  return Matrix[1] == 0.0f && Matrix[3] == 0.0f;
}

void GMatrix3x3f::concat(const GMatrix3x3f& a)
{
  /* Vector to hold final quantity. Should hold same amount*/
  std::vector<float> Product;
  for(unsigned i = 0; i < 3; ++i)
  {
    auto row = this->GetRow(i);
    for (unsigned j = 0; j < 3; ++j)
    {
      auto col = a.GMatrix<float>::GetCol(j);
      Product.emplace_back(GMatrix<float>::DotProduct(row, col));
    }
  }

  Matrix = Product;
}

GPoint GMatrix3x3f::ConvertPoint(const GPoint& P) const
{
  std::vector<float> Column{P.x(), P.y(), 1};
  auto NewX = GMatrix<float>::DotProduct( this->GetRow(0), Column);
  auto NewY = GMatrix<float>::DotProduct( this->GetRow(1), Column);

  return GPoint::Make(NewX, NewY);
}

void GMatrix3x3f::Round()
{
  for (auto &Element : Matrix)
    Element = (int)(Element + .5);
}

/* Assume the point coming in has not modifed the 3rd row for z values, they should
be 0, 0, 1. No way to directly modify the zero row*/
GMatrix3x3f GMatrix3x3f::GetInverse() const
{
  //Add the bottom 3 numbers later

  float a = Matrix[0], b = Matrix[1], c = Matrix[2];
  float d = Matrix[3], e = Matrix[4], f = Matrix[5];
  float g = Matrix[6], h = Matrix[7], i = Matrix[8];

  float determinant = a * e - b * d;

  GMatrix3x3f Inverse({
    e * i - f * h,    c * h - b * i,      b * f - c * e,
    f * g - d * i,    a * i - c * g,      c * d - a * f,
    d * h - e * g,    b * g - a * h,      determinant
  });

  return Inverse;
}
