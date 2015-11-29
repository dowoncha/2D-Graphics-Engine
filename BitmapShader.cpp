//Copyright 2015 Dowon Cha

#include "BitmapShader.h"

BitmapShader::BitmapShader(const GBitmap& src, const float localMatrix[6])
: SrcBmp(src), LocalMatrix(localMatrix, 6)
{}

BitmapShader::~BitmapShader()
{}

bool BitmapShader::setContext(const float ctm[6])
{
  GMatrix<float> CTM(ctm, 6);
  Inverse = CTM.concat(LocalMatrix).twoRowInverse();

  return true;
}

void BitmapShader::shadeRow(int x, int y, int count, GPixel row[])
{
  for ( int i = 0; i < count; ++i)
  {
    GPoint Point{i + x + 0.5f, y + 0.5f};
    Inverse.convertPoint(Point);
    int pX = Utility::clamp(0, (int)Point.fX, SrcBmp.fWidth - 1);
    int pY = Utility::clamp(0, (int)Point.fY, SrcBmp.fHeight - 1);
    row[i] = *(SrcBmp.getAddr(pX, pY));
    //row[i] = bilerp(Point.fX, Point.fY);
  }
}

GPixel BitmapShader::bilerp(float srcX, float srcY)
{
  int pX = Utility::round(srcX);
  int pY = Utility::round(srcY);

  int xAddr = Utility::clamp(0, pX, SrcBmp.fWidth - 1);
  int yAddr = Utility::clamp(0, pY, SrcBmp.fHeight - 1);

  GPixel p0 = *(SrcBmp.getAddr(xAddr, yAddr));
  GPixel p1 = *(SrcBmp.getAddr(xAddr + 1, yAddr));
  GPixel p2 = *(SrcBmp.getAddr(xAddr, yAddr + SrcBmp.fRowBytes));
  GPixel p3 = *(SrcBmp.getAddr(xAddr + 1, yAddr + SrcBmp.fRowBytes));

  uint8_t cx = Utility::floatToByte(Utility::floor_clamp(srcX) - 0.5f);
  uint8_t cy = Utility::floatToByte(Utility::floor_clamp(srcY) - 0.5f);

  uint8_t w00 = (0xff - cx) * (0xff - cy) >> 8;
  uint8_t w10 = cx * (0xff - cy) >> 8;
  uint8_t w01 = (0xff - cx) * cy >> 8;
  uint8_t w11 = cx * cy >> 8;
}

GShader* GShader::FromBitmap(const GBitmap& src, const float localMatrix[6])
{
  return new BitmapShader(src, localMatrix);
}
