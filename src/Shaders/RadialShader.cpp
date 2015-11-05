#include "Shaders/RadialShader.h"

RadialShader::RadialShader(const GPoint& c, float r, const GColor col[2])
 : center(c), radius(r), c0(col[0]), c1(col[1])
{
  LocalMatrix.concat(GMatrix::MakeScaleMatrix(r, r))
             .concat(GMatrix::MakeTranslationMatrix(c.x(), c.y()));
}

RadialShader::~RadialShader()
{
}

bool RadialShader::setContext(const float ctm[6])
{
  GMatrix CTM(ctm);
  Inverse = CTM.concat(LocalMatrix).inverse();

  return true;
}

void RadialShader::shadeRow(int x, int y, int count, GPixel row[])
{
  float srcX = x + .5;
  float srcY = y + .5;

  GPoint ConvertedPoint{srcX, srcY};
  for ( int i = 0; i < count; ++i, srcX += 1.0f)
  {
    ConvertedPoint.set(srcX, srcY);
    Inverse.convertPoint(ConvertedPoint);

    float t = std::sqrt(ConvertedPoint.fX * ConvertedPoint.fX + ConvertedPoint.fY * ConvertedPoint.fY);

    t = Utility::clamp(0.0f, t, 1.0f);
    float a = Utility::lerp(c0.fA, c1.fA, t);
    float r = Utility::lerp(c0.fR, c1.fR, t);
    float g = Utility::lerp(c0.fG, c1.fG, t);
    float b = Utility::lerp(c0.fB, c1.fB, t);

    row[i] = Utility::ColorToPixel(GColor::MakeARGB(a, r, g, b));
  }
}

GShader* GShader::FromRadialGradient(const GPoint& center, float radius, const GColor colors[2])
{
  return new RadialShader(center, radius, colors);
}

// std::vector<GPoint> RadialShader::shadeRow(int x, int y, int count)
// {
//   float srcX = x + .5;
//   float srcY = y + .5
//
//   std::vector<GPoint> row;
//
//   int count = x + count;
//   for ( ; srcX < count; srcX += 1.0f)
//   {
//
//   }
//
//   return row;
// }

//float pin_unit(float t) {
// 1. Clamp Min(max(0,t), 1)
// 2. t - floor(t)
// 3. t=(t-floor(t))*2; if (t > 1) t = 2-t;
// 4. cos ((1-t)*pi)
