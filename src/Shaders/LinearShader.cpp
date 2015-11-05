#include "Shaders/LinearShader.h"

LinearShader::LinearShader(const GPoint pts[2], const GColor colors[2])
 : p0(pts[0]), p1(pts[2]), c0(colors[0]), c1(colors[1])
{

}

LinearShader::LinearShader(float x0, float y0, float x1, float y1, const GColor colors[2])
 : p0(GPoint::Make(x0,y0)), p1(GPoint::Make(x1, y1)), c0(colors[0]), c1(colors[1])
{

}

LinearShader::~LinearShader()
{

}

bool LinearShader::setContext(const float ctm[6])
{
  return true;
}

void LinearShader::shadeRow(int x, int y, int count, GPixel row[])
{
  float srcX = x + .5;
  float srcY = y + .5;

  int i = 0;
  const int end = x + count;

  for (int i = 0; i < count; ++i)
  {
    row[i] = 0;
  }
 //Begin Loop

 //If a Linear use SrcX

 //LERP

 //When we loop instead of recalculating Inverse we can add a and to x and y
 //Sx += a , Sy += d

 //End loop
}

GShader* GShader::FromLinearGradient(const GPoint pts[2], const GColor colors[2])
{
  return new LinearShader(pts, colors);
}

// std::vector<GPixel> LinearShader::shadeRow(int x, int y, int count)
// {
//   std::vector<GPixel> row(count);
//   int right = x + count;
//   for ( float srcX = x + .5; srcX < right; srcX += 1.0f)
//   {
//   }
// }
//
// //float pin_unit(float t) {
// // 1. Clamp Min(max(0,t), 1)
// // 2. t - floor(t)
// // 3. t=(t-floor(t))*2; if (t > 1) t = 2-t;
// // 4. cos ((1-t)*pi)
