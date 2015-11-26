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

  class SizeViolation {};

  /* Fill the entire canvas with the specified color */
  void clear(const GColor& color) override;
  
  /* Fill a rect with a color or a bitmap */
  void fillRect(const GRect&, const GColor&) override;
  
  /* Fills the input rectangle from input bitmap onto destination bitmap */
  void fillBitmapRect(const GBitmap& src, const GRect& dst) override;
  
  /* Fills a convex polygon with the input color */
  void fillConvexPolygon(const GPoint Points[], int count, const GColor& color) override;

  /* Shade a rectangle */
  void shadeRect(const GRect& rect, GShader* shader) override;
  
  /* Fill the polygon with a shader*/
  void shadeConvexPolygon(const GPoint points[], int count, GShader* shader) override;

  /* Stroke the set of points. Adjacent points make up a line, uses miter or flat for joints*/
  void strokePolygon(const GPoint[], int n, bool isClosed, const Stroke&, GShader*) override;

  /* Current Transformation Matrix functions*/
  void save() override;
  
  /* Restores the save matrix in the stack*/
  void restore() override;
  
  /* Concats the input matrix onto the CTM*/
  void concat(const float matrix[6]) override;
private:
  /* These functions tend to use vectors and stl containers more because
   * they are not limited by the GCanvas function definitions */

  /* Fill in the input rectangle with the shader. CTM will be set to shader context within
   * rect points must have been pre-CTM. Will clip the rectangle */
  void shadeDeviceRect(const GIRect& rect, GShader* shader);

  /* Points coming in should already be in device coordinates AKA multipled by the CTM*/
  void shadeDevicePolygon(std::vector<GEdge>& Edges, GShader* shader);
  
  /* Points will be sorted in place, and then edges will be created and clipped*/
  std::vector<GEdge> pointsToEdges(std::vector<GPoint>& Points) const;
  
  /* Convert input points by the CTM */
  void CTMPoints(std::vector<GPoint>& Points) const;
  
  /* Multiply Divide multiply again by 255 and round 2 Colors into another */
  static unsigned MulDiv255Round(uint8_t a, uint8_t b);
  
  /* Blends two Pixel's into a new pixel */
  GPixel blend(GPixel src, GPixel dst);
  
  /* Blend an entire row of pixels */
  void blendRow(GPixel *Dst, int startX, GPixel row[], int count);
  
  /* Sort the points for the convex */
  static void SortPointsForConvex(std::vector<GPoint>& Points);
  
  /* This will take a set of points and make them into edges for a convex polygon */
  static std::vector<GEdge> MakeConvexEdges(const std::vector<GPoint>& Points);
  
  /* Edge clip functions. Begin at ClipEdges which will call the functions for each side. The edges
   * will be clipped to the bitmap, for clipping left and right: all clipped edges also create a new
   * clipped bounded edge which is added to the NewEdges vector and combined in the end.   */
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

inline unsigned MyCanvas::MulDiv255Round(uint8_t a, uint8_t b)
{
  unsigned prod = a * b * 65793;
  prod += (1 << 23);
  return prod >> 24;
}
