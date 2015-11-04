#include "GShader.h"

class LinearShader : public GShader
{
public:
  LinearShader();
  ~LinearShader();
  bool setContext(const float ctm[6]) override;
  void shadeRow(int x, int y, int count, GPixel row[]) override;
};

class BitmapShader : public GShader
{
private:
  const GBitmap SrcBmp;
  GMatrix LocalMatrix;
  GMatrix Inverse;
public:
  BitmapShader(GBitmap src, GMatrix local) : SrcBmp(src), LocalMatrix(local) {}
  ~BitmapShader();
  bool setContext(const float ctm[6]) override;
  void shadeRow(int x, int y, int count, GPixel row[]) override;
};

class RadialShader : public GShader
{
public:
  RadialShader();
  ~RadialShader();
  bool setContext(const float ctm[6]) override;
  void shadeRow(int x, int y, int count, GPixel row[]) override;
};

GShader* GShader::FromLinearGradient(const GPoint pts[2], const GColor colors[2])
{
  return new LinearShader();
}

GShader* GShader::FromBitmap(const GBitmap& src, const float localMatrix[6])
{
  return new BitmapShader(src, GMatrix(localMatrix));
}

GShader* GShader::FromRadialGradient(const GPoint& center, float radius, const GColor colors[2])
{
  return new RadialShader();
}
