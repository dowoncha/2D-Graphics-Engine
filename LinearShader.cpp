//Copyright 2015 Dowon Cha

#include "LinearShader.h"

LinearShader::LinearShader(const GPoint pts[2], const GColor colors[2])
 : c0(colors[0]), c1(colors[1])
{
  float dx = pts[1].fX - pts[0].fX;
  float dy = pts[1].fY - pts[0].fY;
  float LocalArr[6] = { dx, -dy, pts[0].fX,
                        dy, dx,         pts[0].fY };

  LocalMatrix.setMatrix(LocalArr, 6);
}

LinearShader::~LinearShader()
{}

bool LinearShader::setContext(const float ctm[6])
{
  GMatrix<float> CTM(ctm, 6);
  Inverse = CTM.concat(LocalMatrix).inverse();

  return true;
}

void LinearShader::shadeRow(int x, int y, int count, GPixel row[])
{
  GColor Color;
  for (int i = 0; i < count; ++i)
  {
    GPoint Point = {i + x + 0.5f, y + 0.5f};
    Inverse.convertPoint(Point);

    float t = Utility::floor_clamp(Point.fX);

    Color.fA = Utility::lerp(c0.fA, c1.fA, t);
    Color.fR = Utility::lerp(c0.fR, c1.fR, t);
    Color.fG = Utility::lerp(c0.fG, c1.fG, t);
    Color.fB = Utility::lerp(c0.fB, c1.fB, t);

    row[i] = Utility::ColorToPixel(Color);
  }
}

GShader* GShader::FromLinearGradient(const GPoint pts[2], const GColor colors[2])
{
  return new LinearShader(pts, colors);
}