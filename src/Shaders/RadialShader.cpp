#include "Shaders/RadialShader.h"

RadialShader::RadialShader(const GPoint& c, float r, const GColor col[2])
 : center(c), radius(r), colors({col[0], col[1]})
{
}

RadialShader::~RadialShader()
{
}

bool RadialShader::setContext(const float ctm[6])
{
  return true;
}

void RadialShader::shadeRow(int x, int y, int count, GPixel row[])
{
  float srcX = x + .5;
  float srcY = y + .5;

  for ( int i = 0; i < count; ++i)
  {
    float t = std::sqrt(srcX * srcX+ srcY * srcY);

    t = Utility::clamp(0.0f, t, 1.0f);
    t = Utility::lerp(0.0f, radius, t);

    //Sx += 2;
    //Sy += 2;    //Temporarys

    //Sx += a, Sy += d
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
