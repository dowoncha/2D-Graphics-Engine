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
  assert(Elements.size() == 9);

  return GMatrix3x3f(Elements);
}

bool GMatrix3x3f::PreservesRect()
{
  //Matrix = a B c, D e f, g h i
  //Just check the b and d values of the matrix
  return Matrix[1] == 0.0f && Matrix[3] == 0.0f;
}

void GMatrix3x3f::concat(const GMatrix3x3f& a)
{
  /* Vector to hold final quantity. Should hold same amount*/
  std::vector<float> Product;
  for(unsigned i = 0; i < 3; ++i)
  {
    //Get a row of this matrix
    auto row = this->GetRow(i);
    for (unsigned j = 0; j < 3; ++j)
    {
      //Get a column of the input matrix
      auto col = a.GMatrix<float>::GetCol(j);
      //Add to the Product vector the dot product of row and column
      Product.emplace_back(GMatrix<float>::DotProduct(row, col));
    }
  }
  //Copy assignment of new products onto this Matrix
  Matrix = Product;
}

GPoint GMatrix3x3f::ConvertPoint(const GPoint& P) const
{
  //Make a 3x1 vector of the point
  std::vector<float> Column{P.x(), P.y(), 1};
  //Get Converted X Y Points using dotproduct
  auto NewX = GMatrix<float>::DotProduct( this->GetRow(0), Column);
  auto NewY = GMatrix<float>::DotProduct( this->GetRow(1), Column);

  return GPoint::Make(NewX, NewY);
}

void GMatrix3x3f::Round()
{
  //Round all of the element values
  for (auto &Element : Matrix)
    Element = (int)(Element + .5);
}

GMatrix3x3f GMatrix3x3f::GetInverse() const
{
  //CHECK: I want to change this but I dont know what I would use? using, typedef, define?
  float a = Matrix[0], b = Matrix[1], c = Matrix[2];
  float d = Matrix[3], e = Matrix[4], f = Matrix[5];
  float g = Matrix[6], h = Matrix[7], i = Matrix[8];
  //Calculate the determinant
  float determinant = a * (e * i - f * h) - b * (d * i - f * g ) + c * (d * h - e * g );

  GMatrix3x3f Inverse({
    e * i - f * h,    c * h - b * i,      b * f - c * e,
    f * g - d * i,    a * i - c * g,      c * d - a * f,
    d * h - e * g,    b * g - a * h,      a * e - b * d
  });
  //Divide the inverse by the determinant
  Inverse /= determinant;

  return Inverse;
}
