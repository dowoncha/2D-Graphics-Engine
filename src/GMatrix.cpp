 #include "GMatrix.h"

GMatrix::GMatrix()
{
  //The identity matrix is the default matrix;
  for (int i = 0; i < 9; ++i)
  {
    Matrix[i] = 0.0f;
  }

  Matrix[0] = 1.0f; Matrix[4] = 1.0f; Matrix[8] = 1.0f;
}

GMatrix::GMatrix(const float in[6])
{
  for (int i = 0; i < 6; ++i) {
    Matrix[i] = in[i];
  }

  Matrix[6] = 0.0f; Matrix[7] = 0.0f; Matrix[8] = 1.0f;
}

GMatrix::GMatrix(const GMatrix& a)
: Matrix(a.Matrix)
{}

GMatrix GMatrix::MakeTranslationMatrix(float x, float y)
{
  float TransArray[6] = {1.0f, 0.0f, x,
                         0.0f, 1.0f, y};
  return GMatrix(TransArray);
}

GMatrix GMatrix::MakeScaleMatrix(float dx, float dy)
{
  float ScaleArray[6] = {dx, 0.0f, 0.0f,
                         0.0f, dy, 0.0f};
  return GMatrix(ScaleArray);
}

void GMatrix::convertPoint(GPoint& P) const
{
  //Get Converted X Y Points using dotproduct
  auto NewX = Matrix[0] * P.x() + Matrix[1] * P.y() + Matrix[2];
  auto NewY = Matrix[3] * P.x() + Matrix[4] * P.y() + Matrix[5];

  P.set(NewX, NewY);
}

void GMatrix::convertPoint(float& x, float& y) const
{
  auto NewX = Matrix[0] * x + Matrix[1] * y + Matrix[2];
  auto NewY = Matrix[3] * x + Matrix[4] * y + Matrix[5];

  x = NewX;
  y = NewY;
}

GMatrix& GMatrix::concat(const GMatrix& InMat)
{
  std::array<float, 9> ConcatMat;
  std::array<float, 6> RowCol;

  int counter = 0;
  for (int i = 0; i < 9; i += 3)
  {
    RowCol[0] = Matrix[i];
    RowCol[1] = Matrix[i + 1];
    RowCol[2] = Matrix[i + 2];
    for (int j = 0; j < 3; ++j, ++counter)
    {
      RowCol[3] = InMat.Matrix[j];
      RowCol[4] = InMat.Matrix[j + 3];
      RowCol[5] = InMat.Matrix[j + 6];

      ConcatMat[counter] = RowCol[0] * RowCol[3] + RowCol[1] * RowCol[4] + RowCol[2] * RowCol[5];
    }
  }

  Matrix = ConcatMat;

  return *this;
}

GMatrix GMatrix::inverse() const
{
  //CHECK: I want to change this but I dont know what I would use? using, typedef, define?
  float a = Matrix[0], b = Matrix[1], c = Matrix[2];
  float d = Matrix[3], e = Matrix[4], f = Matrix[5];
  float g = Matrix[6], h = Matrix[7], i = Matrix[8];
  //Calculate the determinant
  float determinant = a * (e * i - f * h) - b * (d * i - f * g ) + c * (d * h - e * g );

  float Inverse[9] = {
    e * i - f * h,    c * h - b * i,      b * f - c * e,
    f * g - d * i,    a * i - c * g,      c * d - a * f,
    d * h - e * g,    b * g - a * h,      a * e - b * d
  };

  for (int i = 0; i < 9; ++i) {
    Inverse[i] /= determinant;
  }

  return GMatrix(Inverse);
}
