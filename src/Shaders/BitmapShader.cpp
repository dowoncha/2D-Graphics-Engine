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
  float srcY = y + .5;

  int i = 0;
  const int end = x + count;
  GPoint ConvertedPoint{srcX, srcY};
  for ( ; srcX < end; srcX += 1.0f, ++i)
  {
    ConvertedPoint.set(srcX, srcY);
    Inverse.convertPoint(ConvertedPoint);
    int NewX = Utility::clamp(0, (int)ConvertedPoint.fX, SrcBmp.width() - 1);
    int NewY = Utility::clamp(0, (int)ConvertedPoint.fY, SrcBmp.height() - 1);
    row[i] = *(SrcBmp.getAddr(NewX, NewY));
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
