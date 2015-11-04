#include "MyCanvas.h"

/**
 * Takes in the bitmap that this canvas will draw to
 * */
MyCanvas::MyCanvas(const GBitmap& bitmap):
	Bitmap(bitmap),
	BmpRect(GIRect::MakeWH(bitmap.width(), bitmap.height()))
{
			/* Push the identity matrix onto the matrix stack */
		MatrixStack.push(GMatrix::MakeIdentityMatrix());
}

MyCanvas::~MyCanvas() {	}

// This function will create my implementation of a Canvas and return it out.
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
	//If src alpha is 255 then just return the src
  if (GETA(src) == 0xFF)
      return src;
	//If src alpha is 0 then return the dst
	if (GETA(src) == 0)
		return dst;

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

	// We might get rgb values that are not premultiplied so we clamp to the alpha
	// I think we can use Clamp function here but not sure whether COLORBYTE (uint_8) -> int conversion is safe
	red = (red <= alpha) ? red : alpha;
	green = (green <= alpha) ? green : alpha;
	blue = (blue <= alpha) ? blue : alpha;

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
  GPixel* DstPixels = Bitmap.pixels();
  GPixel pColor = ColorToPixel(color);     //Convert input color into a pixel

  for (int y = 0; y < Bitmap.height(); ++y)
    {
      for (int x = 0; x < Bitmap.width(); ++x)
			{
				DstPixels[x] = pColor;
			}

      DstPixels = (GPixel*) ((char*)DstPixels + Bitmap.rowBytes());
    }
}

void MyCanvas::fillRect(const GRect& rect, const GColor& color)
{
	//Get the points from the input rectangle
	std::vector<GPoint> Points = QuadToPoints(rect);
	//Convert all the points by the CTM
	CTMPoints(Points);
	//If the CTM does not preserve a rectangle then we draw a polygon instead
	if (!MatrixStack.top().preservesRect())
	{
		fillDevicePolygon(Points, color);
		return;
	}

  //Convet Points that have been CTM back into a rect and then round the values
  GIRect iRect = PointsToQuad(Points).round();

  // Make sure rect is not an empty one
  if (rect.isEmpty() || !iRect.intersect(BmpRect))
      return;

	//Get the pixel address for dst bitmap
  GPixel* DstPixels = Bitmap.pixels();    //Get pointer to array of pixels in bitmap
  GPixel pColor = ColorToPixel(color);  //Convert input color into a pixel value

  // Increment by the number of rows we have to skip to the start of the rectangle
  DstPixels = (GPixel*) ((char*)DstPixels + Bitmap.rowBytes() * iRect.top());

	// Run through all pixels in the rect
  for (int y = iRect.top(); y < iRect.bottom(); ++y)
	{
		for (int x = iRect.left(); x < iRect.right(); ++x)
		{
			DstPixels[x] = Blend(pColor, DstPixels[x]);
		}

		// Increment Pixel pointer to next row
    DstPixels = (GPixel*) ((char*)DstPixels + Bitmap.rowBytes());
    }
}

std::vector<GPoint> MyCanvas::QuadToPoints(const GRect& Rect)
{
	std::vector<GPoint> PreTransform({
		GPoint::Make(Rect.x(), Rect.y()),
	  GPoint::Make(Rect.x() + Rect.width(), Rect.y()),
	  GPoint::Make(Rect.x(), Rect.y() + Rect.height()),
	  GPoint::Make(Rect.x() + Rect.width(), Rect.y() + Rect.height())
	});

	return PreTransform;
}

void MyCanvas::CTMPoints(std::vector<GPoint>& Points)
{
	auto CTM = GetCTM();

	/* Convert all points by the CTM*/
	for (auto &Point: Points) {
		CTM.convertPoint(Point);
	}
}

GRect MyCanvas::PointsToQuad(const std::vector<GPoint>& Points)
{
	assert(Points.size() == 4);
	/* Return */
	return GRect::MakeLTRB(Points[0].x(), Points[0].y(), Points[3].x(), Points[3].y());
}

GMatrix RectToRect(const GRect& src, const GRect& dst)
{
	// Make translation matrix of the source from the origin
	auto SrcTranslate = GMatrix::MakeTranslationMatrix(
		-1 * src.left(), -1 * src.top()
	);

	// Make scale matrix of dx, dy. Dst / Src
	auto Scale = GMatrix::MakeScaleMatrix(
		dst.width() / src.width(),
		dst.height() / src.height()
	);

	// Get tranlsation matrix of the dst rectangle
	auto DstTranslate = GMatrix::MakeTranslationMatrix(dst.left(), dst.top());

	// Concatenate the 3 matrices. DstTranslate * Scale * SrcTranslate
	DstTranslate.concat(Scale).concat(SrcTranslate);

	return DstTranslate;
}

void MyCanvas::fillBitmapRect(const GBitmap& src, const GRect& dst)
{
	assert(!dst.isEmpty());

	/* Get the matrix of the conversion from src to dst rect */
	auto Src2Dst = RectToRect(GRect::MakeWH(src.width(), src.height()), dst);
	/* Convert the destination rect into points*/
	std::vector<GPoint> Points = QuadToPoints(dst);
	/* Convert The Points by the CTM*/
	CTMPoints(Points);
	/* Get current CTM and concat the rect to rect onto it*/
	auto CTM = GetCTM();
	/* Concat the */
	CTM.concat(Src2Dst);
	/* Send the inverse of the converson matrix and converted points for drawing*/
	fillDeviceBitmap(src, Points, CTM.inverse());
}

void MyCanvas::fillConvexPolygon(const GPoint Points[], const int count, const GColor& color)
{
	if (count < 3) return;

	std::vector<GPoint> Transformed(Points, Points + count);	// Declare a vector of the points
	CTMPoints(Transformed);																		// Multiply all points in the vector by the CTM
	fillDevicePolygon(Transformed, color);										// Fill polygon made by the converted points
}

/* Assumptions: Vector of points coming in should be in device coordinates (i.e. already converted by the CTM)
 * The InverseRect coming in should be (CTM * R2R)^-1
 **/
void MyCanvas::fillDeviceBitmap(const GBitmap& src, std::vector<GPoint> Points, const GMatrix& InverseRect)
{
		SortPointsForConvex(Points);								// Sort the points for convex edge creation
		auto Edges = MakeConvexEdges(Points);				// Create Edges from the sorted points
		ClipEdges(Edges);														// Clip the Edges from the current bitmap

		DrawBitmapPolygon(Edges, src, InverseRect);	// Draw the polygon filling it with the bitmap
}

/* Assumptions: Vector of points coming in should be in device coordinates (i.e. already converted by the CTM)
 **/
void MyCanvas::fillDevicePolygon(std::vector<GPoint> Points, const GColor& color)
{
	assert(Points.size() > 2);										// There should be at least 3 points to make a polygon

	const GPixel& pColor = ColorToPixel(color);		// Convert the color into pixel data

	SortPointsForConvex(Points);									// Sort the given points clockwise into an order we can make Edges from.
	auto Edges = MakeConvexEdges(Points);					// Make the edges from the sorted points Edges
	ClipEdges(Edges);															// Clip the Edges from the output bitmap

	DrawPolygon(Edges, pColor);										// Draw the polgyon from the set of edges and the color
}

void MyCanvas::save()
{
	MatrixStack.push(MatrixStack.top());	//Push a copy of the CTM onto the stack
}

void MyCanvas::restore()
{
	if (!MatrixStack.empty())		//Check to make sure stack is not empty
	{
		MatrixStack.pop();				//Remove the CTM
	}
}

void MyCanvas::concat(const float matrix[6])
{
	auto& CTM = MatrixStack.top();	//We get a reference to the CTM here to modify it

	/* Make a new matrix using the input matrix values*/
	CTM.concat(GMatrix(matrix));				//Concat the input matrix onto the CTM
}

void shadeRect(const GRect& rect, GShader* shader)
{

}

void shadeConvexPolygon(const GPoint[] points, int count, GShader* shader)
{
	std::vector<GPoint> Points(points, points + count);
	CTMPoints(Points);
	float ctm[6];
	GetCTM().GetTwoRows(ctm);
	if (!(shader->SetContext(ctm))) {
		std::printf("CTM set context failed\n");
		return;
	}

	SortPointsForConvex(Points);
	auto Edges = MakeConvexEdges(Points);
	ClipEdges(Edges);

	if (Edges.size() < 2)
	{
		std::cout << "Edges has less than 3 values\n";
		return;
	}

	// Sort the edges from top to bottom, left to right
	std::sort(Edges.begin(), Edges.end());

	// The first two edges will be the top most edges
	GEdge LeftEdge = Edges[0];
	GEdge RightEdge = Edges[1];

	/* Move DstPixels to y offset.*/
	GPixel *DstPixels = Bitmap.pixels();
	/* Offset the pixel pointer to the proper row*/
	DstPixels = (GPixel*)((char*)DstPixels + Bitmap.rowBytes() * LeftEdge.GetTop());

	int EdgeCounter = 2;
	GPixel *row;
	for (int y = LeftEdge.GetTop(); y < Edges.back().GetBottom(); ++y)
	{
		float x0 = LeftEdge.GetCurrentX() + .5;
		float x1 = RightEdge.GetCurrentX();
		int count = (int)(x1 - x0);
		row = new GPixel[count];

		shader->shadeRow((int)x0, y, count, row );

		delete[] row;  //Free the Pixel row since it was dynamically allocated

		// Move left and right edges currentX to next row
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

void MyCanvas::SortPointsForConvex(std::vector<GPoint>& Points)
{
	/* Sort Points by y and then x if they are equal */
	std::sort(Points.begin(), Points.end(), [](const GPoint& a, const GPoint& b) -> bool{
		return ( std::fabs(a.y() - b.y() ) < .0001) ? a.x() > b.x() : a.y() < b.y();
	});

	/* At this points points are sorted top to bottom, right to left */

	std::queue<GPoint> RightPoints;			//Queue to hold all Points right of the pivot
	std::stack<GPoint> LeftPoints;			//Stack to hold all Points left of the pivot

	/* Start from 2nd value because top is the pivot */
	for (auto Point = Points.begin() + 1; Point != Points.end() - 1; ++Point)
	{
		/* If the x value of the Point is greater than or equal to the top point x*/
		if ( Point->x() >= Points.front().x())
			RightPoints.push(*Point);		//Add to right queue
		else
			LeftPoints.push(*Point);		//Add to left stack
	}

	/* We store the value of RightPoints to get a proper increment later */
	const auto RightSize = RightPoints.size();

	/* Points: Order Diagram - TopPivot, RightPoints ... RightPoints.end() - 1, BottomPivot, LeftPoints ... LeftPoints.end() */
	for (auto Point = Points.begin() + 1; !RightPoints.empty(); ++Point)
	{
		*Point = RightPoints.front();
		RightPoints.pop();
	}

	/* Set the middle point to be the bottom most point */
	Points[RightSize + 1] = Points.back();

	/* If the LeftPoints are not empty store them*/
	for (auto i = RightSize + 2; !LeftPoints.empty(); ++i)
	{
		Points[i] = LeftPoints.top();					//Copy over from left stack
		LeftPoints.pop();											//Pop from left points
	}
}

std::vector<GEdge> MyCanvas::MakeConvexEdges(const std::vector<GPoint>& Points)
{
	std::vector<GEdge> Edges;							//Will hold the edges
	auto Penultimate = --Points.end();		//2nd to last Point

	/* For all values except last connect to adjacent value */
	for (auto Point = Points.begin(); Point != Penultimate; ++Point)
	{
		Edges.emplace_back(*Point, *(Point + 1));			//Create a new edge of two adjacent points
	}

	Edges.emplace_back(Points.front(), Points.back());	// Make an edge of the last point to the first point

	return Edges;
}

void MyCanvas::ClipEdges(std::vector<GEdge>& Edges)
{
	std::vector<GEdge> NewEdges;			//New edges that will be created from side clipping

	ClipEdgesTopAndBottom(Edges);			//Pin top and bottom edges
	ClipEdgesLeft(Edges, NewEdges);		//Pin left edges, create new edges from clipping
	ClipEdgesRight(Edges, NewEdges);	//Pin right edges, create new edges from clipping

	/* Copy over all clipped new edges into the original edge vector */
	Edges.insert(Edges.end(), NewEdges.begin(), NewEdges.end());
}

void MyCanvas::ClipEdgesTopAndBottom(std::vector<GEdge>& Edges)
{
	const int height = BmpRect.height();			//The height of the bitmap

	/* Clip all Edges*/
	for (auto Edge = Edges.begin(); Edge != Edges.end(); )
	{
		/* Pin the top and bottom of the edges that are out of bitmap vertical*/
		/* Will return false if the value needs to be removed*/
		if ( !Edge->PinTopAndBot(height) )
		{
			Edge = Edges.erase(Edge);	//Remove all horizontal edges, and edges off screen
		}
		else
		{
			++Edge;		//If we don't remove a value then we increment the iterator
		}
	}
}

void MyCanvas::ClipEdgesLeft(std::vector<GEdge>& Edges, std::vector<GEdge>& NewEdges)
{
	/* Check all Edges*/
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
			Edge.SetTop((int)(ClipY + .5));				//Potential change in round function to fabs(ClipY + .5)?
		}

		/* If the bottom point of the edge is to the left of the bitmap*/
		if (Edge.GetBottomX() < 0)
		{
			/* The Clipped y value is the top - currentX (pos) / slope (neg)*/
			float ClipY = Edge.GetTop() - Edge.GetCurrentX() / Edge.GetSlope();
			/* Add new clipped edge to NewEdges */
			NewEdges.emplace_back(
				GEdge(GPoint::Make(0, ClipY), GPoint::Make(0, Edge.GetBottom()))
			);

			Edge.SetBottom((int)(ClipY+.5));		//Set the bottom of the edge to the new clipY
		}
	}
}

void MyCanvas::ClipEdgesRight(std::vector<GEdge>& Edges, std::vector<GEdge>& NewEdges)
{
	const int width = BmpRect.width();		//Get width of the bitmap

	/* Run for all edges */
	for (auto &Edge: Edges)
	{
		/* If the edge is completely to the right of the bitmap*/
		if (Edge.GetCurrentX() > width && Edge.GetBottomX() > width)
		{
			Edge.SetCurrentX(width);	//Set the CurrentX to the bitmap width
			Edge.MakeSlopeVertical();	//Set the slope to 0. WARNING: This implementation could be better
			continue;									//Go to next edge
		}

		/* If the top of the edge is to the right of the bitmap
		 * Assumptions: Slope: negative, CurrentX > width, BottomX < width */
		if (Edge.GetCurrentX() > width)
		{
			/* Get the Y value to clip the edge at*/
			float ClipY = Edge.GetTop() + ( width - Edge.GetCurrentX()) / Edge.GetSlope();
			/* Make a new edge from clipped y and old Top*/
			NewEdges.emplace_back(
				GEdge(GPoint::Make(width, Edge.GetTop()), GPoint::Make(width, ClipY))
			);
			Edge.SetCurrentX(width);			//Set the CurrentX to the bitmap width
			Edge.SetTop((int)(ClipY+.5));	//Set the top of clipped edge to the rounded ClipY
		}

		/* If the bottom of the edge needs to be clipped right*/
		/* Assumptions: Slope: positive, CurrentX < width, BottomX > width*/
		if (Edge.GetBottomX() > width)
		{
			/* Get the ClipY value*/
		  float ClipY = Edge.GetTop() + ( width - Edge.GetCurrentX()) / Edge.GetSlope();
			/* Make new edge from the ClipY and the old bottom*/
			NewEdges.emplace_back(
				GEdge(GPoint::Make(width, ClipY), GPoint::Make(width, Edge.GetBottom()))
			);
			Edge.SetBottom((int)(ClipY + .5));	//Set bottom of clipped edge to rounded ClipY
		}
	}
}

void MyCanvas::DrawBitmapPolygon(std::vector<GEdge>& Edges, const GBitmap& src, const GMatrix& InverseRect)
{
	if (Edges.size() < 2)
	{
		std::cout << "Edges has less than 3 values\n";
		return;
	}

	// Sort the edges by Top, CurrentX, finally slope
	// CHECK: Some rect's dont get drawn and i think its due to comparison order
	std::sort(Edges.begin(), Edges.end());

	// The first two edges will be the top left and right edges
	GEdge LeftEdge = Edges[0];
	GEdge RightEdge = Edges[1];

	// Get Pixel addresses for src and dst bitmaps
	GPixel *SrcPixels = src.pixels();
	GPixel *DstPixels = Bitmap.pixels();

	// Offset the dst pixel pointer to the starting row
	DstPixels = (GPixel*)((char*)DstPixels + Bitmap.rowBytes() * LeftEdge.GetTop());

	int EdgeCounter = 2;		//Count the number of edges used so far

	/* This loop currently goes through float values, but i think it can be changed
	 * to go through int and add .5 in the lookup instead*/
	 //CHECK: I tried changing y into an int, but i segfault on the last image
	for ( float y = LeftEdge.GetTop() + .5; y < Edges.back().GetBottom(); y += 1.0f)
	{
		/* Round the beginning of current x for each scanline draw */
		/* Worried about using int's here due to CurrentX being a float ASK REED*/
		for ( float x = LeftEdge.GetCurrentX() + .5; x < RightEdge.GetCurrentX(); x += 1.0f)
		{
			// Lookup x y values from src bitmap
			GPoint Converted = GPoint::Make(x, y);
			InverseRect.convertPoint(Converted);
			// Clamp the lookup values to catch imprecision errors
			int NewX = clamp(0, (int)Converted.x(), src.width() - 1);
			int NewY = clamp(0, (int)Converted.y(), src.height() - 1);
			// Lookup pixel in src bitmap at NewX, NewY
			GPixel SrcPixel = *(src.getAddr(NewX, NewY));
			//Blend Src Pixel onto DstPixel
			DstPixels[(int)x] = Blend(SrcPixel, DstPixels[(int)x]);
		}
		// Move onto the next row
		DstPixels = (GPixel*)((char*)DstPixels + Bitmap.rowBytes());

		// Move the CurrentX by 1 to move onto the next row
		LeftEdge.MoveCurrentX(1.0f);
		RightEdge.MoveCurrentX(1.0f);

		// Check left edge to see if y has passed bottom and we have not reached max edges
		if (y > LeftEdge.GetBottom() && EdgeCounter < Edges.size())
		{
				//Set new leftedge
				LeftEdge = Edges[EdgeCounter];
				++EdgeCounter;
		}
		if (y > RightEdge.GetBottom() && EdgeCounter < Edges.size())
		{
				//Set new righedge
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

	// Sort the edges from top to bottom, left to right
	std::sort(Edges.begin(), Edges.end());

	// The first two edges will be the top most edges
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

		//Move dst pixel pointer to next row
		DstPixels = (GPixel*)((char*)DstPixels + Bitmap.rowBytes());

		// Move left and right edges currentX to next row
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

int MyCanvas::clamp(int min, int value, int max) {
        return std::max(min, std::min(value, max));
}
