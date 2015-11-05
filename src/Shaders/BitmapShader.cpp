#include "Shaders/BitmapShader.h"

BitmapShader::BitmapShader(const GBitmap& src, const float localMatrix[6])
: SrcBmp(src), LocalMatrix(localMatrix)
{

}

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
  float srcX = x + .5;
  float srcY = y + .5

  srcX = pin_unit(srcX);
  srcY = pin_unit(srcY);

  int i = 0;
  const int end = x + count;
  GPoint ConvertedPoint;
  for ( ; srcX < end; srcX += 1.0f)
  {
    ConvertedPoint.set(srcX, srcY);
    Inverse.convertPoint(ConvertedPoint);
    row[i] = SrcBmp.getAddr(srcX, srcY);
  }
}

GShader* GShader::FromBitmap(const GBitmap& src, const float localMatrix[6])
{
  return new BitmapShader(src, localMatrix);
}

// std::vector<GPixel> BitmapShader::shadeRow(int x, int y, int count) const
// {
//   float srcX = x + .5;
//   float srcY = y + .5;
//
//   std::vector<GPixel> row;
//   GPoint ConvertedPdoint;
//   for ( ; srcX < x + count; srcX += 1.0f)
//   {
//     ConvertedPoint.set(srcX, srcY);
//     Inverse.convertPoint(ConvertedPoint);
//     row.emplace_back(Src.getAddr(ConvertedPoint.x(), ConvertedPoint.y()));
//   }
//
//   return row;
// }
