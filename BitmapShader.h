//Copyright 2015 Dowon Cha

#pragma once

#include "GBitmap.h"
#include "GMatrix.h"
#include "GPixel.h"
#include "Utility.h"
#include "GShader.h"

class BitmapShader : public GShader
{
private:
  const GBitmap SrcBmp;
  GMatrix<float> LocalMatrix;
  GMatrix<float> Inverse;
public:
  BitmapShader(const GBitmap& src, const float localMatrix[6]);
  ~BitmapShader();
  bool setContext(const float ctm[6]) override;
  void shadeRow(int x, int y, int count, GPixel row[]) override;
};
