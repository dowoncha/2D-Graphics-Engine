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
#include "GMatrix3x3.h"

#define GETA(a)    		  GPixel_GetA(a)
#define GETR(a)    		  GPixel_GetR(a)
#define GETG(a)    		  GPixel_GetG(a)
#define GETB(a)    		  GPixel_GetB(a)
#define COLORBYTE         uint8_t

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
	void fillDeviceBitmap(const GBitmap& src, std::vector<GPoint> Points, const GMatrix3x3f& InverseRect);
	void fillConvexPolygon(const GPoint Points[], int count, const GColor& color) override;
	void fillDevicePolygon(std::vector<GPoint> Points, const GColor& color);

	void save() override;
	void restore() override;
	void concat(const float matrix[6]) override;

	GMatrix3x3f GetCTM() { return MatrixStack.top(); }

	/* Convert input Quad into points*/
	static std::vector<GPoint> QuadToPoints(const GRect& Rect);
	/* Convert input points by the CTM*/
	void CTMPoints(std::vector<GPoint>& Points);
	/* Convert input points into a quad*/
	static GRect PointsToQuad(const std::vector<GPoint>& Points);

	/* Multiply Divide multiply again by 255 and round 2 Colors into another*/
  unsigned MulDiv255Round(const COLORBYTE a, const COLORBYTE b);
	/** Blends two Pixel's depending on color **/
	GPixel Blend(const GPixel src, const GPixel dst);
	/** Converts the input color and returns a premultiplied and converted to GPixel value **/
	GPixel ColorToPixel(const GColor color);
	/* Sort the points for the convex */
	void SortPointsForConvex(std::vector<GPoint>& Points);

	/* This will conver the vector of input points into vector of Edges
	 * The Edges should make a convex shape from top down */
	std::vector<GEdge> MakeConvexEdges(const std::vector<GPoint>& Points);

	/* Any edge with a point outside the bitmap width is now pinned and a in bound edge is now created */
	void ClipEdges(std::vector<GEdge>& Edges);
	void ClipEdgesTopAndBottom(std::vector<GEdge>& Edges);
	void ClipEdgesLeft(std::vector<GEdge>& Edges, std::vector<GEdge>& NewEdges);
	void ClipEdgesRight(std::vector<GEdge>& Edges, std::vector<GEdge>& NewEdges);

	/* Draw the input bitmap into a polygon, InverseRect is the RectToRectMatrix*/
	void DrawBitmapPolygon(std::vector<GEdge>& Edges, const GBitmap& src, const GMatrix3x3f& InverseRect);
	/* Draw the polygon using the container of edges to the input color*/
	void DrawPolygon(std::vector<GEdge>& Edges, const GPixel& Color);

	static int clamp(int min, int value, int max);
private:
	const GBitmap Bitmap;
	const GIRect BmpRect;
	std::stack<GMatrix3x3f> MatrixStack;
};
