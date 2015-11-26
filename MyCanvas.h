/**
 * Copyright 2015 Dowon Cha
 * This is my implementation of a canvas. It is a subclass of the GCanvas class.
 * */

#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include <queue>
#include <algorithm>
#include <assert.h>
#include <cmath>

#include "GCanvas.h"
#include "GPoint.h"
#include "GBitmap.h"
#include "GPixel.h"
#include "GColor.h"
#include "GRect.h"
#include "GQuad.h"
#include "GEdge.h"
#include "GMatrix.h"
#include "GShader.h"
#include "Utility.h"
#include "BitmapShader.h"
#include "LinearShader.h"
#include "RadialShader.h"

#define GETA(a)    		  GPixel_GetA(a)
#define GETR(a)    		  GPixel_GetR(a)
#define GETG(a)    		  GPixel_GetG(a)
#define GETB(a)    		  GPixel_GetB(a)

class MyCanvas : public GCanvas
{
public:
  MyCanvas(const GBitmap& bitmap);
  ~MyCanvas();

  using COLORBYTE = uint8_t;

  class SizeViolation {};

  /* C style arrays are used here due to the assignment restrictions in implementing functions definitions in GCanvas*/
  /** Fill the entire canvas with the specified color, using SRC porter-duff mode. **/
  void clear(const GColor& color) override;
  /* Fill a rect with a color or a bitmap*/
  void fillRect(const GRect&, const GColor&) override;
  void fillBitmapRect(const GBitmap& src, const GRect& dst) override;
  void fillConvexPolygon(const GPoint Points[], int count, const GColor& color) override;

  void shadeRect(const GRect& rect, GShader* shader) override;
  void shadeConvexPolygon(const GPoint points[], int count, GShader* shader) override;

  void strokePolygon(const GPoint[], int n, bool isClosed, const Stroke&, GShader*) override;

  /* Current Transformation Matrix functions*/
  void save() override;
  void restore() override;
  void concat(const float matrix[6]) override;
private:
  /* These functions tend to use vectors and stl containers more because
   * they are not limited by the GCanvas function definitions */
  
  /* Points coming in should already be in device coordinates AKA multipled by the CTM*/
  void shadeDevicePolygon(std::vector<GEdge>& Edges, GShader* shader);
  
  /* Currently draws a line according to given points */
  void strokeLine(const GPoint& A, const GPoint& B, GShader* shader);
  /* Points will be sorted in place, and then edges will be created and clipped*/
  std::vector<GEdge> pointsToEdges(std::vector<GPoint>& Points) const;
  /* Convert input points by the CTM */
  void CTMPoints(std::vector<GPoint>& Points) const;
  // Multiply Divide multiply again by 255 and round 2 Colors into another
  static unsigned MulDiv255Round(const COLORBYTE a, const COLORBYTE b);
  // Blends two Pixel's into a new pixel
  GPixel blend(const GPixel src, const GPixel dst);
  //Blend an entire row of pixels
  void blendRow(GPixel *Dst, int startX, GPixel row[], int count);
  // Sort the points for the convex
  static void SortPointsForConvex(std::vector<GPoint>& Points);
  // This will take a set of points and make them into edges for a convex polygon
  static std::vector<GEdge> MakeConvexEdges(const std::vector<GPoint>& Points);
  // Any edge with a point outside the bitmap width is now pinned and a in bound edge is now created
  void ClipEdges(std::vector<GEdge>& Edges) const;
  void ClipEdgesTopAndBottom(std::vector<GEdge>& Edges) const;
  void ClipEdgesLeft(std::vector<GEdge>& Edges, std::vector<GEdge>& NewEdges) const;
  void ClipEdgesRight(std::vector<GEdge>& Edges, std::vector<GEdge>& NewEdges) const;
private:
  const GBitmap Bitmap;
  const GIRect BmpRect;
  std::stack<GMatrix<float>> MatrixStack;
  GMatrix<float> CTM;
};

inline unsigned MyCanvas::MulDiv255Round(const COLORBYTE a, const COLORBYTE b)
{
  unsigned prod = a * b * 65793;
  prod += (1 << 23);
  return prod >> 24;
}
