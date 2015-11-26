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
  Inverse = CTM.concat(LocalMatrix).inverse();

  return true;
}

void BitmapShader::shadeRow(int x, int y, int count, GPixel row[])
{
  for ( int i = 0; i < count; ++i)
  {
    GPoint Point{i + x + 0.5f, y + 0.5f};
    Inverse.convertPoint(Point);
    int NewX = Utility::clamp(0, (int)Point.fX, SrcBmp.width() - 1);
    int NewY = Utility::clamp(0, (int)Point.fY, SrcBmp.height() - 1);
    row[i] = *(SrcBmp.getAddr(NewX, NewY));
  }
}

GShader* GShader::FromBitmap(const GBitmap& src, const float localMatrix[6])
{
  return new BitmapShader(src, localMatrix);
}
