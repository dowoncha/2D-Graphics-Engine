//Copyright 2015 Dowon Cha

#include "RadialShader.h"

RadialShader::RadialShader(const GPoint& c, float r, const GColor col[2])
 : c0(col[0]), c1(col[1])
{
  GMatrix<float> Trans = GMatrix<float>::MakeTranslationMatrix(c.x(), c.y());
  GMatrix<float> Scale = GMatrix<float>::MakeScaleMatrix(r, r);

  LocalMatrix = Trans * Scale;
}

RadialShader::~RadialShader()
{
}

bool RadialShader::setContext(const float ctm[6])
{
  GMatrix<float> CTM(ctm, 6);
  Inverse = CTM.concat(LocalMatrix).inverse();

  return true;
}

void RadialShader::shadeRow(int x, int y, int count, GPixel row[])
{
  GColor Color;
  for ( int i = 0; i < count; ++i)
  {
    GPoint Point = {i + x + 0.5f, y + 0.5f};

    Inverse.convertPoint(Point);

    float t = std::sqrt(Point.fX * Point.fX + Point.fY * Point.fY);

    t = Utility::floor_clamp(t);

    Color.fA = Utility::lerp(c0.fA, c1.fA, t);
    Color.fR = Utility::lerp(c0.fR, c1.fR, t);
    Color.fG = Utility::lerp(c0.fG, c1.fG, t);
    Color.fB = Utility::lerp(c0.fB, c1.fB, t);

    row[i] = Utility::ColorToPixel(Color);
  }
}

GShader* GShader::FromRadialGradient(const GPoint& center, float radius, const GColor colors[2])
{
  return new RadialShader(center, radius, colors);
}