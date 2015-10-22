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

	/**
	 *  Fill the rectangle with the color, using SRC_OVER porter-duff mode.
	 *  The affected pixels are those whose centers are "contained" inside the rectangle:
	 *      e.g. contained == center > min_edge && center <= max_edge
	 *  Any area in the rectangle that is outside of the bounds of the canvas is ignored.
	 */
	void fillRect(const GRect&, const GColor&) override;

	/**
	 * Scale and translate the ibtmap such that it fills the specific rectangle.
	 * Any area in the rectangle that is outside of the bounds of the canvas is ignored.
	 * If a given pixel in the bitmap is not opaque (e.g. GPixel_GetA() < 255) then blend it
	 * using SRCOVER blend mode
	 * */
	void fillBitmapRect(const GBitmap& src, const GRect& dst) override;

	void fillDeviceBitmap(const GBitmap& src, const std::vector<GPoint>& Points, const GMatrix3x3f& InverseRect);

	/**
   *  Fill the convex polygon with the color, following the same "containment" rule as
   *  rectangles.
   *  Any area in the polygon that is outside of the bounds of the canvas is ignored.
   *  If the color's alpha is < 1, blend it using SRCOVER blend mode.
   **/
	void fillConvexPolygon(const GPoint Points[], int count, const GColor& color) override;
	/* My Fill convex using a vector*/
	void fillDevicePolygon(const std::vector<GPoint>& Points, const GColor& color);
	/**
   *  Saves a copy of the CTM, allowing subsequent modifications (by calling concat()) to be
   *  undone when restore() is called.
   */
	void save() override;
	/**
   *  Balances calls to save(), returning the CTM to the state it was in when the corresponding
   *  call to save() was made. These calls can be nested.
   */
	void restore() override;
	/* Concat the input matrix onto the matrix on top of the stack*/
	void concat(const float matrix[6]) override;

	std::vector<GPoint> QuadToPoints(const GRect& Rect);

	void CTMPoints(std::vector<GPoint>& Points);

	GRect PointsToQuad(const std::vector<GPoint>& Points);

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
	void ClipEdgesTopAndBottom(std::vector<GEdge>& Edges);
	void ClipEdgesLeft(std::vector<GEdge>& Edges, std::vector<GEdge>& NewEdges);
	void ClipEdgesRight(std::vector<GEdge>& Edges, std::vector<GEdge>& NewEdges);

	void DrawBitmapPolygon(std::vector<GEdge>& Edges, const GBitmap& src);
	/* Draw the polygon using the container of edges to the input color*/
	void DrawPolygon(std::vector<GEdge>& Edges, const GPixel& Color);
private:
	const GBitmap Bitmap;
	const GIRect BmpRect;
	std::stack<GMatrix3x3f> MatrixStack;
};
