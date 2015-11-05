#include "Shaders/RadialShader.h"

RadialShader::RadialShader(const GPoint& c, float r, const GColor col[2])
 : center(c), radius(r), colors(col)
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
  //Begin Loop

 //If a radial use sqrt(SrcX^2 + SrcY^2)

 //LERP

 //When we loop instead of recalculating Inverse we can add a and to x and y
 //Sx += a , Sy += d

 //End loop
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
