/**
 * Dowon Cha
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
#include "GEdge.h"
#include "GMatrix.h"
#include "GShader.h"
#include "Utility.h"
#include "Shaders/BitmapShader.h"
#include "Shaders/LinearShader.h"
#include "Shaders/RadialShader.h"

#define GETA(a)    		  GPixel_GetA(a)
#define GETR(a)    		  GPixel_GetR(a)
#define GETG(a)    		  GPixel_GetG(a)
#define GETB(a)    		  GPixel_GetB(a)
#define COLORBYTE       uint8_t

class MyCanvas : public GCanvas
{
public:
	MyCanvas(const GBitmap& bitmap);

	~MyCanvas();

	class SizeViolation {};

	/** Fill the entire canvas with the specified color, using SRC porter-duff mode. **/
	void clear(const GColor& color) override;
	void fillRect(const GRect&, const GColor&) override;
	void fillBitmapRect(const GBitmap& src, const GRect& dst) override;
	void fillConvexPolygon(const GPoint Points[], int count, const GColor& color) override;
	void shadeRect(const GRect& rect, GShader* shader) override;
	void shadeConvexPolygon(const GPoint points[], int count, GShader* shader) override;
	void shadeDevicePolygon(std::vector<GPoint>& Points, GShader* shader);

	/* Current Transformation Matrix functions*/
	void save() override;
	void restore() override;
	void concat(const float matrix[6]) override;

	//Get a copy of the current CTM
  GMatrix GetCTM() const { return MatrixStack.top(); }

	//Get the local conversion matrix from src rect to the dst rect
	static GMatrix RectToRect(const GRect& src, const GRect& dst);
	/* Convert input Quad into points*/
	static std::vector<GPoint> QuadToPoints(const GRect& Rect);
	/* Convert input points by the CTM*/
	void CTMPoints(std::vector<GPoint>& Points) const;
	/* Convert input points into a quad*/
	static GRect PointsToQuad(const std::vector<GPoint>& Points);
	/* Multiply Divide multiply again by 255 and round 2 Colors into another*/
  static unsigned MulDiv255Round(const COLORBYTE a, const COLORBYTE b);
	/** Blends two Pixel's depending on color **/
	GPixel Blend(const GPixel src, const GPixel dst);
	//Blend an entire row
	void BlendRow(GPixel *Dst, int startX, GPixel row[], int count);
	/** Converts the input color and returns a premultiplied and converted to GPixel value **/
	static GPixel ColorToPixel(const GColor color);
	/* Sort the points for the convex */
	static void SortPointsForConvex(std::vector<GPoint>& Points);
	/* This will take a set of points and make them into edges for a convex polygon*/
	static std::vector<GEdge> MakeConvexEdges(const std::vector<GPoint>& Points);
	/* Any edge with a point outside the bitmap width is now pinned and a in bound edge is now created */
	void ClipEdges(std::vector<GEdge>& Edges) const;
	void ClipEdgesTopAndBottom(std::vector<GEdge>& Edges) const;
	void ClipEdgesLeft(std::vector<GEdge>& Edges, std::vector<GEdge>& NewEdges) const;
	void ClipEdgesRight(std::vector<GEdge>& Edges, std::vector<GEdge>& NewEdges) const;
private:
	const GBitmap Bitmap;
	const GIRect BmpRect;
	std::stack<GMatrix> MatrixStack;
};

inline unsigned MyCanvas::MulDiv255Round(const COLORBYTE a, const COLORBYTE b)
{
  unsigned prod = a * b * 65793;
  prod += (1 << 23);
  return prod >> 24;
}
