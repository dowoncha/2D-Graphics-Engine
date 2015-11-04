 #include "GMatrix.h"

GMatrix::GMatrix(const float in[6])
{
  for (int i = 0; i < 6; ++i) {
    Matrix[i] = in[i];
  }

  Matrix[6] = 0.0f; Matrix[7] = 0.0f; Matrix[8] = 1.0f;
}

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

GMatrix GMatrix::MakeIdentityMatrix()
{
  float Iden[6] = {1.0f, 0.0f, 0.0f,
                   0.0f, 1.0f, 0.0f};
  return GMatrix(Iden);
}

void GMatrix::convertPoint(GPoint& P) const
{
  //Get Converted X Y Points using dotproduct
  auto NewX = Matrix[0] * P.x() + Matrix[1] * P.y() + Matrix[2];
  auto NewY = Matrix[3] * P.x() + Matrix[4] * P.y() + Matrix[5];

  P.set(NewX, NewY);
}

GMatrix& GMatrix::concat(const GMatrix& InMat)
{
  float ConcatMat[9];
  float Row[3];
  float Col[3];

  int counter = 0;
  for (int i = 0; i < 9; i += 3)
  {
    Row[0] = Matrix[i];
    Row[1] = Matrix[i + 1];
    Row[2] = Matrix[i + 2];
    for (int j = 0; j < 3; ++j, ++counter)
    {
      Col[0] = InMat.Matrix[j];
      Col[1] = InMat.Matrix[j + 3];
      Col[2] = InMat.Matrix[j + 6];

      ConcatMat[counter] = Row[0] * Col[0] + Row[1] * Col[1] + Row[2] * Col[2];
    }
  }

  Matrix = ConcatMat;

  return *this;
}

GMatrix GMatrix::inverse()
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
