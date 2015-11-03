#include "MyShader.h"

MyShader::MyShader()
{

}

MyShader::~MyShader()
{

}

bool MyShader::setContext(const float ctm[6])
{
  GMatrix CTM = new GMatrix(ctm);

  Inverse = CTM.concat(LocalMatrix).inverse();

  return true;
}

void MyShader::shadeRow(int x, int y, int count, GPixel row[])
{
  GPoint SrcPoint = Inverse.convertPoint(GPoint{x + .5, y + .5});

  //Begin Loop

 //These 3 are for getting T
 //If a bitmap use SrcX, Srcy
 //If a Linear use SrcX
 //If a radial use sqrt(SrcX^2 + SrcY^2)

 //LERP

 //When we loop instead of recalculating Inverse we can add a and to x and y
 //Sx +=a , Sy += d

 //End loop
}

//float pin_unit(float t) {
// 1. Clamp Min(max(0,t), 1)
// 2. t - floor(t)
// 3. t=(t-floor(t))*2; if (t > 1) t = 2-t;
// 4. cos ((1-t)*pi)
GShader* MyShader::FromBitmap(const GBitmap&, const float localMatrix[6])
{
  LocalMatrix = localMatrix;
}

GShader* MyShader::FromLinearGradient(const GPoint pts[2], const GColor colors[2])
{

}

GShader* MyShader::FromRadialGraident(const GPoint& center, float radius, const GColor colors[2])
{

}
