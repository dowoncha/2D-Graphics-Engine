#include "GShader.h"

class MyShader : public GShader
{
private:
  GMatrix LocalMatrix;
  GMatrix Inverse;
public:
  MyShader();
  ~MyShader();

  bool setContext(const float ctm[6]);

  void shadeRow(int x, int y, int count, GPixel row[]);

  static GShader* FromBitmap(const GBitmap&, const float localMatrix[6]);

  static GShader* FromLinearGradient(const GPoint pts[2], const GColor colors[2]);

  static GShader* FromRadialGraident(const GPoint& center, float radius, const GColor colors[2]);
};
