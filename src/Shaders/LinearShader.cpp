#include "Shaders/LinearShader.h"

LinearShader::LinearShader(const GPoint pts[2], const GColor colors[2])
 : p0(pts[0]), p1(pts[1]), c0(colors[0]), c1(colors[1])
{
  float dx = pts[0].fX - pts[1].fX;
  float dy = pts[0].fY - pts[1].fY;
  float LocalArr[6];
  LocalArr[0] = dx;
  LocalArr[1] = -1 * dy;
  LocalArr[2] = pts[0].fX;
  LocalArr[3] = dy;
  LocalArr[4] = -1 * dx;
  LocalArr[5] = pts[0].fY;

  LocalMatrix = GMatrix::MakeMatrix(LocalArr);
}

LinearShader::~LinearShader()
{

}

bool LinearShader::setContext(const float ctm[6])
{
  GMatrix CTM(ctm);
  Inverse = CTM.concat(LocalMatrix).inverse();

  return true;
}

void LinearShader::shadeRow(int x, int y, int count, GPixel row[])
{
  float srcX = x + .5;
  float srcY = y + .5;

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

// //float pin_unit(float t) {
// // 1. Clamp Min(max(0,t), 1)
// // 2. t - floor(t)
// // 3. t=(t-floor(t))*2; if (t > 1) t = 2-t;
// // 4. cos ((1-t)*pi)
