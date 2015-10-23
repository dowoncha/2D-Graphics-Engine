#include "MyCanvas.h"

/**
 * Takes in the bitmap that this canvas will draw to
 * */
MyCanvas::MyCanvas(const GBitmap& bitmap):
	Bitmap(bitmap),
	BmpRect(GIRect::MakeWH(bitmap.width(), bitmap.height()))
{
		std::cout << "\nBmp W: " << bitmap.width() << " H: " << bitmap.height() << "\n";
		MatrixStack.push(GMatrix3x3f::MakeIdentityMatrix());
}

MyCanvas::~MyCanvas() {	}

/**
 * This function will create my implementation of a Canvas
 * and return it out.
 * */
GCanvas* GCanvas::Create(const GBitmap& bitmap)
{
	//If the bitmap width, height are invalid, pixel pointer is null, or the rowbytes size does not match then return NULL
	if (bitmap.width() < 0 || bitmap.height() < 0 || bitmap.pixels() == NULL || bitmap.rowBytes() < (bitmap.width() * sizeof(GPixel)))
		return NULL;

	return new MyCanvas(bitmap);
}
/**
 *This function takes in two bytes and multiplies them together
 * divides the product by 255 and rounds while accouting for error
 **/
unsigned MyCanvas::MulDiv255Round(const COLORBYTE a, const COLORBYTE b)
{
  unsigned prod = a * b * 65793;
  prod += (1 << 23);
  return prod >> 24;
}

GPixel MyCanvas::Blend(const GPixel src, const GPixel dst)
{
  if (GETA(src) == 0xFF)
      return src;

  //Get the blend alpha value from 255 - Source_Alpha
  COLORBYTE src_a_blend = 0xFF - GETA(src);

  /** Using blend formula for each pixel color ARGB
   *  We first get the value from src, and add it to the result of
   *  multiplying a_blend and the color of dest.
   * */
  COLORBYTE alpha = GETA(src) + MulDiv255Round(src_a_blend, GETA(dst));
  COLORBYTE red = GETR(src) + MulDiv255Round(src_a_blend, GETR(dst));
  COLORBYTE green = GETG(src) + MulDiv255Round(src_a_blend, GETG(dst));
  COLORBYTE blue = GETB(src) + MulDiv255Round(src_a_blend, GETB(dst));

  return GPixel_PackARGB(alpha, red, green, blue);
}

GPixel MyCanvas::ColorToPixel(const GColor color)
{
    GColor pinned = color.pinToUnit();  //Make sure color is between 0 and 1

    float fA =  pinned.fA * 255.9999;		//Convert from 0-1 to 0-255
    COLORBYTE uA = (COLORBYTE) fA;
    COLORBYTE uR = (COLORBYTE) (pinned.fR * fA);
    COLORBYTE uG = (COLORBYTE) (pinned.fG * fA);
    COLORBYTE uB = (COLORBYTE) (pinned.fB * fA);

    return GPixel_PackARGB(uA, uR, uG, uB);		//Returned the packed pixel
}

/**
 * This function will clear the canvas according to the color.
 * */
void MyCanvas::clear(const GColor& color)
{
  //Get the start of the bitmap _Pixels array
  GPixel* _Pixels = Bitmap.pixels();
  GPixel pColor = ColorToPixel(color);     //Convert input color into a pixel

  for (int y = 0; y < Bitmap.height(); ++y)
    {
      for (int x = 0; x < Bitmap.width(); ++x)
		_Pixels[x] = pColor;

      _Pixels = (GPixel*) ((char*)_Pixels + Bitmap.fRowBytes);
    }
}

/**
 * This function will take in a rect and a color. If the bounds
 * of the rect are valid it will blend in the color inside the rect using
 * src over blend mode with the current pixel in the bitmap.
 * */
void MyCanvas::fillRect(const GRect& rect, const GColor& color)
{
	std::vector<GPoint> Points = QuadToPoints(rect);

	CTMPoints(Points);

	if (!MatrixStack.top().PreservesRect())
	{
		fillDevicePolygon(Points, color);
		return;
	}

  //Round the read in rectangle values
  GIRect iRect = PointsToQuad(Points).round();

  /* Make sure rect is not an empty one*/
  if (rect.isEmpty() || !iRect.intersect(BmpRect))
      return;

  GPixel* _Pixels = Bitmap.pixels();    //Get pointer to array of pixels in bitmap
  GPixel pColor = ColorToPixel(color);  //Convert input color into a pixel value

  /* Increment by the number of rows we have to skip to the start of the rectangle*/
  _Pixels = (GPixel*) ((char*)_Pixels + Bitmap.rowBytes() * iRect.top());

  for (int y = iRect.top(); y < iRect.bottom(); ++y)
	{
		for (int x = iRect.left(); x < iRect.right(); ++x)
		{
			_Pixels[x] = Blend(pColor, _Pixels[x]);
		}

      _Pixels = (GPixel*) ((char*)_Pixels + Bitmap.rowBytes());
    }
}

std::vector<GPoint> MyCanvas::QuadToPoints(const GRect& Rect)
{
	std::vector<GPoint> PreTransform(4);

	PreTransform[0] = GPoint::Make(Rect.x(), Rect.y());
	PreTransform[1] = GPoint::Make(Rect.x() + Rect.width(), Rect.y());
	PreTransform[2] = GPoint::Make(Rect.x(), Rect.y() + Rect.height());
	PreTransform[3] = GPoint::Make(Rect.x() + Rect.width(), Rect.y() + Rect.height());

	return PreTransform;
}

void MyCanvas::CTMPoints(std::vector<GPoint>& Points)
{
	/* Get a reference to the CTM so we can modify it*/
	auto CTM = GetCTM();

	for (auto &Point: Points)
	{
		Point = CTM.ConvertPoint(Point);
	}
}

GRect MyCanvas::PointsToQuad(const std::vector<GPoint>& Points)
{
	assert(Points.size() == 4);
	/* Return */
	return GRect::MakeLTRB(Points[0].x(), Points[0].y(), Points[3].x(), Points[3].y());
}

GMatrix3x3f RectToRect(const GRect& src, const GRect& dst)
{
	auto SrcTranslate = GMatrix3x3f::MakeTranslationMatrix(
		-1 * src.left(), -1 * src.top()
	);

	/* Scale by the width and height*/
	auto RectScale = GMatrix3x3f::MakeScaleMatrix(
		dst.width() / src.width(),
		dst.height() / src.height()
	);

	auto DstTranslate = GMatrix3x3f::MakeTranslationMatrix(dst.left(), dst.top());

	DstTranslate.concat(RectScale);
	DstTranslate.concat(SrcTranslate);

	return DstTranslate;
}

void MyCanvas::fillBitmapRect(const GBitmap& src, const GRect& dst)
{
	assert(!dst.isEmpty());

	/* Get the matrix of the conversion from src to dst rect */
	auto BmpToRectMat = RectToRect(GRect::MakeWH(src.width(), src.height()), dst);
	/* Convert the destination rect into points*/
	std::vector<GPoint> Points = QuadToPoints(dst);
	/* Convert The Points by the CTM*/
	CTMPoints(Points);

	/* If the CTM does not preserve a rect then we draw bmp into a polygon*/
	if (!MatrixStack.top().PreservesRect())
	{
		fillDeviceBitmap(src, Points, BmpToRectMat);
		return;
	}

	/* Convert the input vector of points back into a quad after being transformed, we also round the values of the quad*/
	// auto TransformedQuad = PointsToQuad(Points).round();
	//
	// /* Makes sure the bitmaps are touching, also clips the rectangle i believe.*/
	// if (!TransformedQuad.intersect(BmpRect)) 			//If the rounded rectangle does not intersect the bitmap
	// 	return;
	//
	// /* Get the beginning of rht pixel bitmap addresses from src and dst bitmaps*/
	// GPixel* SrcPixels = src.pixels();
	// GPixel* DstPixels = Bitmap.pixels();
	//
	// /* Get the inversion matrix from the rect to rect and CTM conversion*/
	// GMatrix3x3f CTM = MatrixStack.top();
	// CTM.concat(BmpToRectMat);
}

void MyCanvas::fillConvexPolygon(const GPoint Points[], const int count, const GColor& color)
{
	assert(count > 2);

	/* Declare a vector of the points*/
	std::vector<GPoint> Transformed(Points, Points + count);
	CTMPoints(Transformed);

	fillDevicePolygon(Transformed, color);
}

/* The points coming in should already be in device space */
void MyCanvas::fillDeviceBitmap(const GBitmap& src, std::vector<GPoint> Points, const GMatrix3x3f& InverseRect)
{
		/* Sort the points for convex edge creation */
		SortPointsForConvex(Points);
		/* Create Edges from the sorted points*/
		auto Edges = MakeConvexEdges(Points);
		/* Clip the Edges from the current bitmap */
		ClipEdges(Edges);

		DrawBitmapPolygon(Edges, src, InverseRect);
}

/* Points should be pre transformed when coming in*/
void MyCanvas::fillDevicePolygon(std::vector<GPoint> Points, const GColor& color)
{
	assert(Points.size() > 2);
	/* Convert the color into pixel data*/
	const GPixel& pColor = ColorToPixel(color);
	/* Sort the given points into the vector into line adjacent pieces */
	SortPointsForConvex(Points);
	/* Make convex edges from the sorted points Edges come out pre vertical clipped*/
	auto Edges = MakeConvexEdges(Points);
	/* Clip the Edges from the output bitmap*/
	ClipEdges(Edges);
	/* Draw the polgyon from the set of edges and the color*/
	DrawPolygon(Edges, pColor);
}

void MyCanvas::save()
{
	MatrixStack.push(MatrixStack.top());
}

void MyCanvas::restore()
{
	if (!MatrixStack.empty())
	{
		MatrixStack.pop();
	}
}

void MyCanvas::concat(const float matrix[6])
{
	auto& CTM = MatrixStack.top();
	GMatrix3x3f ConcatMatrix = GMatrix3x3f::MakeMatrix({matrix[0], matrix[1], matrix[2],
	 																						 				matrix[3], matrix[4], matrix[5],
																							 				0, 0, 1 });
	CTM.concat(ConcatMatrix);
}

void MyCanvas::SortPointsForConvex(std::vector<GPoint>& Points)
{
	/* Sort by y and then x if they are equal */
	std::sort(Points.begin(), Points.end(), [](const GPoint& a, const GPoint& b) -> bool{
		return ( std::fabs(a.y() - b.y() ) < .0001) ? a.x() > b.x() : a.y() < b.y();
	});

	/* We get the bottom most point and use it as a pivot and bottom most Points */
	std::queue<GPoint> RightPoints;
	std::stack<GPoint> LeftPoints;

	for (auto Point = Points.begin() + 1; Point != Points.end() - 1; ++Point)
	{
		/* If the x value of the Point is greater than the top pivot add to right list */
		if ( Point->x() >= Points.front().x())
			RightPoints.push(*Point);
		else
			LeftPoints.push(*Point);		//Add to left list otherwise
	}

	/* We store the value of RightPoints to get a proper increment later */
	const auto RightSize = RightPoints.size();

	/* Points: Order Diagram - TopPivot, RightPoints ... RightPoints.end() - 1, BottomPivot, LeftPoints ... LeftPoints.end() */
	for (auto Point = Points.begin() + 1; !RightPoints.empty(); ++Point)
	{
		*Point = RightPoints.front();
		RightPoints.pop();
	}

	/* Set the middle pivot*/
	Points[RightSize + 1] = Points.back();

	/* If the LeftPoints are not empty store them*/
	for (auto i = RightSize + 2; !LeftPoints.empty(); ++i)
	{
		Points[i] = LeftPoints.top();
		LeftPoints.pop();
	}
}

/* This will convert the vector of input points i */
std::vector<GEdge> MyCanvas::MakeConvexEdges(const std::vector<GPoint>& Points)
{
	std::vector<GEdge> Edges;

	/* For all values except last connect to adjacent value */
	auto End = --Points.end();
	for (auto Point = Points.begin(); Point != End; ++Point)
		Edges.emplace_back(*Point, *(Point + 1));

	/* Make an edge of the last point to the first point*/
	Edges.emplace_back(Points.front(), Points.back());

	return Edges;
}

void MyCanvas::ClipEdges(std::vector<GEdge>& Edges)
{
	std::vector<GEdge> NewEdges;
	/* Clip top and bot */
	ClipEdgesTopAndBottom(Edges);
  /* Clip Left Right */
	ClipEdgesLeft(Edges, NewEdges);

	ClipEdgesRight(Edges, NewEdges);

	Edges.insert(Edges.end(), NewEdges.begin(), NewEdges.end());
}

void MyCanvas::ClipEdgesTopAndBottom(std::vector<GEdge>& Edges)
{
	/* For all edges*/
	const auto &height = BmpRect.height();
	for (auto Edge = Edges.begin(); Edge != Edges.end(); )
	{
		/* Pin the top and bottom of the edges that are out of bitmap vertical*/
		/* Will return false if the value needs to be removed*/
		if ( !Edge->PinTopAndBot(height) )
		{
			/* Remove all horizontal edges and edges off screen*/
			Edge = Edges.erase(Edge);
		}
		else
		{
			++Edge;
		}
	}
}

void MyCanvas::ClipEdgesLeft(std::vector<GEdge>& Edges, std::vector<GEdge>& NewEdges)
{
	for (auto &Edge: Edges)
	{
		/* If entire segment is to the left pin it to the edge*/
		if (Edge.GetCurrentX() < 0 && Edge.GetBottomX() < 0)
		{
			Edge.SetCurrentX(0);
			Edge.MakeSlopeVertical();
			continue;
		}

		/* If the top point is to the left of the bitmap*/
		if (Edge.GetCurrentX() < 0)
		{
			/* Find the dx between edge and current x and the y point at the new x*/
			float ClipY = Edge.GetTop() - Edge.GetCurrentX() / Edge.GetSlope();
			/* Add in new clipped edge*/
			NewEdges.emplace_back(
				GEdge(GPoint::Make(0, Edge.GetTop()),GPoint::Make(0, ClipY))
			);
			/* Move the CurrentX to the edge (0, ClipY)*/
			Edge.SetCurrentX(0);
			/* Change the top of clipped edge from old to new ClipY value*/
			Edge.SetTop((int)(ClipY+.5));
		}
		/* If the bottom point of the edge is to the left of the bitmap*/
		if (Edge.GetBottomX() < 0)
		{
			/* The Clipped y value is the top - currentX (pos) / slope (neg)*/
			float ClipY = Edge.GetTop() - Edge.GetCurrentX() / Edge.GetSlope();
			NewEdges.emplace_back(
				GEdge(GPoint::Make(0, ClipY), GPoint::Make(0, Edge.GetBottom()))
			);
			Edge.SetBottom((int)(ClipY+.5));
		}
	}
}

void MyCanvas::ClipEdgesRight(std::vector<GEdge>& Edges, std::vector<GEdge>& NewEdges)
{
	const int& width = BmpRect.width();
	for (auto &Edge: Edges)
	{
		if (Edge.GetCurrentX() > width && Edge.GetBottomX() > width)
		{
			Edge.SetCurrentX(width);
			Edge.MakeSlopeVertical();
			continue;
		}

		/* If the top of the edge is to the right of the bitmap*/
		if (Edge.GetCurrentX() > width)
		{
			float ClipY = Edge.GetTop() + ( width - Edge.GetCurrentX()) / Edge.GetSlope();
			NewEdges.emplace_back(
				GEdge(GPoint::Make(width, Edge.GetTop()), GPoint::Make(width, ClipY))
			);
			Edge.SetCurrentX(width);
			Edge.SetTop((int)(ClipY+.5));
		}

		if (Edge.GetBottomX() > width)
		{
		  float ClipY = Edge.GetTop() + ( width - Edge.GetCurrentX()) / Edge.GetSlope();
			NewEdges.emplace_back(
				GEdge(GPoint::Make(width, ClipY), GPoint::Make(width, Edge.GetBottom()))
			);
			Edge.SetBottom((int)(ClipY + .5));
		}
	}
}

void MyCanvas::DrawBitmapPolygon(std::vector<GEdge>& Edges, const GBitmap& src, const GMatrix3x3f& InverseRect)
{
	if (Edges.size() < 2)
	{
		std::cout << "Edges has less than 3 values\n";
		return;
	}

	/* Sort the edges from top to bottom, left to right*/
	std::sort(Edges.begin(), Edges.end());

	/* The first two edges will be the top most edges */
	GEdge LeftEdge = Edges[0];
	GEdge RightEdge = Edges[1];

	/* Move DstPixels to y offset.*/
	GPixel *DstPixels = Bitmap.pixels();
	GPixel *SrcPixels = src.pixels();
	/* Offset the pixel pointer to the proper row*/
	DstPixels = (GPixel*)((char*)DstPixels + Bitmap.rowBytes() * LeftEdge.GetTop());

	int EdgeCounter = 2;
	for (float y = LeftEdge.GetTop() + .5; y < Edges.back().GetBottom(); y += 1.0f)
	{
		/* Round the beginning of current x for each scanline draw*/
		for ( float x = LeftEdge.GetCurrentX() + .5; x < RightEdge.GetCurrentX(); x += 1.0f)
		{
			auto Converted = InverseRect.ConvertPoint(GPoint{x, y});
			unsigned index = (int)Converted.x() + (int)Converted.y() *  src.rowBytes();
			GPixel SrcPixel = SrcPixels[index];
		}

		DstPixels = (GPixel*)((char*)DstPixels + Bitmap.rowBytes());

		LeftEdge.MoveCurrentX(1.0f);
		RightEdge.MoveCurrentX(1.0f);

		/* Check left edge to see if y has passed bottom and we have not reached max edges */
		if (y > LeftEdge.GetBottom() && EdgeCounter < Edges.size())
		{
				LeftEdge = Edges[EdgeCounter];
				++EdgeCounter;
		}
		if (y > RightEdge.GetBottom() && EdgeCounter < Edges.size())
		{
				RightEdge = Edges[EdgeCounter];
				++EdgeCounter;
		}
	}
}

void MyCanvas::DrawPolygon(std::vector<GEdge>& Edges, const GPixel& Color)
{
	if (Edges.size() < 2)
	{
		std::cout << "Edges has less than 3 values\n";
		return;
	}

	/* Sort the edges from top to bottom, left to right*/
	std::sort(Edges.begin(), Edges.end());

	/* The first two edges will be the top most edges */
	GEdge LeftEdge = Edges[0];
	GEdge RightEdge = Edges[1];

	/* Move DstPixels to y offset.*/
	GPixel *DstPixels = Bitmap.pixels();
	/* Offset the pixel pointer to the proper row*/
	DstPixels = (GPixel*)((char*)DstPixels + Bitmap.rowBytes() * LeftEdge.GetTop());

	int EdgeCounter = 2;
	for (float y = LeftEdge.GetTop() + .5; y < Edges.back().GetBottom(); y += 1.0f)
	{
		/* Round the beginning of current x for each scanline draw*/
		for ( float x = LeftEdge.GetCurrentX() + .5; x < RightEdge.GetCurrentX(); x += 1.0f)
		{
			DstPixels[(int)x] = Blend(Color, DstPixels[(int)x]);
		}

		DstPixels = (GPixel*)((char*)DstPixels + Bitmap.rowBytes());

		LeftEdge.MoveCurrentX(1.0f);
		RightEdge.MoveCurrentX(1.0f);

		/* Check left edge to see if y has passed bottom and we have not reached max edges */
		if (y > LeftEdge.GetBottom() && EdgeCounter < Edges.size())
		{
				LeftEdge = Edges[EdgeCounter];
				++EdgeCounter;
		}
		if (y > RightEdge.GetBottom() && EdgeCounter < Edges.size())
		{
				RightEdge = Edges[EdgeCounter];
				++EdgeCounter;
		}
	}
}

// void MyCanvas::CheckEdgeValues(const std::vector<GEdge>& Edges)
// {
//   for (const auto Edge: Edges)
//     {
//       if (Edge.GetTop() < 0 || Edge.GetBottom() < 0)
// 	{
// 	  std::cout << "Top or Bot is above bitmap " << Edge.ToString();
// 	}
//       if (Edge.GetTop() > BmpRect.height() || Edge.GetBottom() > BmpRect.height())
// 	{
// 	  std::cout <<"Top or Bot is below the bitmap " << Edge.ToString();
// 	}
//       if (Edge.GetCurrentX() < 0 || Edge.GetBottomX() < 0)
// 	{
// 	  std::cout << "Left Clip failure detected "  << Edge.ToString();
// 	}
//       if (Edge.GetCurrentX() > BmpRect.width() || Edge.GetBottomX() > BmpRect.width())
// 	{
// 	  std::cout << "Right clip failure " << Edge.ToString();
// 	}
//
//     }
// }
