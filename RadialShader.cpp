#include "MyShaders.h"

RadialShader::RadialShader()
{

}

RadialShader::~RadialShader()
{

}

bool RadialShader::setContext(const float ctm[6])
{
  GMatrix CTM = new GMatrix(ctm);

  Inverse = CTM.concat(LocalMatrix).inverse();

  return true;
}

void RadialShader::shadeRow(int x, int y, int count, GPixel row[])
{
  GPoint SrcPoint = Inverse.convertPoint(GPoint{x + .5, y + .5});

  //Begin Loop

 //If a radial use sqrt(SrcX^2 + SrcY^2)

 //LERP

 //When we loop instead of recalculating Inverse we can add a and to x and y
 //Sx += a , Sy += d

 //End loop
}

//float pin_unit(float t) {
// 1. Clamp Min(max(0,t), 1)
// 2. t - floor(t)
// 3. t=(t-floor(t))*2; if (t > 1) t = 2-t;
// 4. cos ((1-t)*pi)
