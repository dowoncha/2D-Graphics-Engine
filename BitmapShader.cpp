#include "MyShaders.h"

BitmapShader::~BitmapShader()
{
}

bool BitmapShader::setContext(const float ctm[6])
{
  GMatrix CTM(ctm);
  Inverse = CTM.concat(LocalMatrix).inverse();

  return true;
}

void BitmapShader::shadeRow(int x, int y, int count, GPixel row[])
{
  GPoint StartP{x + .5, y + .5};
  Inverse.convertPoint(StartP);

  for (float srcX = StartP.x(); i < x + count; srcX += )
  {
    GPixel SrcPixel = SrcBmp.getAddr(x, y);
  }
}
