#include "Shaders/RadialShader.h"

RadialShader::RadialShader(const GPoint& c, float r, const GColor col[2])
 : center(c), radius(r), c0(col[0]), c1(col[1]), LocalMatrix()
{
  GMatrix<float> Scale;
  Scale.setScale(r,r);

  GMatrix<float> Trans;
  Trans.setTranslation(c.x(), c.y());

  LocalMatrix.concat(Scale).concat(Trans);
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
  GPoint Point;
  GColor Color;
  for ( int i = 0; i < count; ++i)
  {
    Point.fX = i + x + .5;
    Point.fY = y + .5;
    Inverse.convertPoint(Point);
    Point.fX = Utility::clamp(0.0f, Point.fX, 1.0f);
    Point.fY = Utility::clamp(0.0f, Point.fY, 1.0f);

    float t = std::sqrt(Point.fX * Point.fX + Point.fY * Point.fY);
    t = Utility::clamp(0.0f, t, 1.0f);

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

//float pin_unit(float t) {
// 1. Clamp Min(max(0,t), 1)
// 2. t - floor(t)
// 3. t=(t-floor(t))*2; if (t > 1) t = 2-t;
// 4. cos ((1-t)*pi)
