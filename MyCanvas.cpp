/**
 *  \file MyCanvas.cpp
 *  \author Dowon Cha
 *
 *  Copyright 2015
 */

#include "MyCanvas.h"

MyCanvas::MyCanvas(const GBitmap& bitmap):
	Bitmap(bitmap),
	BmpRect(GIRect::MakeWH(bitmap.width(), bitmap.height())),
	CTM()
{
  /* Push the identity matrix onto the matrix stack */
  MatrixStack.push(CTM);
}

MyCanvas::~MyCanvas() {	}

GCanvas* GCanvas::Create(const GBitmap& bitmap)
{
  //If the bitmap width, height are invalid, pixel pointer is null, or the rowbytes size does not match then return NULL
  if (bitmap.width() < 0 || bitmap.height() < 0 || bitmap.pixels() == NULL || bitmap.rowBytes() < (bitmap.width() * sizeof(GPixel))) {
    return NULL;
  }
  
  return new MyCanvas(bitmap);
}

void MyCanvas::clear(const GColor& color)
{
  GPixel* DstPixels = Bitmap.pixels();            //Get the start of the bitmap Pixels array  
  GPixel pColor = Utility::ColorToPixel(color);   //Convert input color into a pixel

  for (int y = 0; y < Bitmap.fHeight; ++y)
  {
    for (int x = 0; x < Bitmap.fWidth; ++x)
    {
      DstPixels[x] = pColor;
    }
    
    DstPixels = (GPixel*)((char*)DstPixels + Bitmap.fRowBytes);
  }
}

void MyCanvas::fillRect(const GRect& rect, const GColor& color)
{
  if (rect.isEmpty()) 
  {
    printf("Error: FillRect input rect is empty\n");
    return;
  }

  GShader* shader = GShader::FromColor(color);
  shadeRect(rect, shader);
  delete shader;
}

void MyCanvas::fillBitmapRect(const GBitmap& src, const GRect& dst)
{
  if (dst.isEmpty())
  {
    printf("Error: FillBitmapRect dst rect is empty\n");
    return;
  }

  /* Get the matrix of the conversion from src to dst rect
   * Use the rect to rect matrix to create bitmap shader */
  auto LocalMatrix = Utility::RectToRect(GRect::MakeWH(src.width(), src.height()), dst);
  float localArr[6];
  LocalMatrix.GetTwoRows(localArr);
  
  GShader* shader = GShader::FromBitmap(src, localArr);
  shadeRect(dst, shader);
  delete shader;
}

void MyCanvas::fillConvexPolygon(const GPoint points[], int count, const GColor& color)
{
  if (count < 3)
  {
    printf("Error: FillConvexPolygon needs at least 3 points to draw a polygon\n");
    return;
  }
  
  GShader* shader = GShader::FromColor(color);  //Make the shader for one color
  shadeConvexPolygon(points, count, shader);
  delete shader;
}

void MyCanvas::shadeRect(const GRect& rect, GShader* shader)
{
  assert(shader != nullptr);
  if (rect.isEmpty())
  {
    printf("Error: ShadeRect rectangle is empty\n");
    return;
  }

  /* Convert the rectangle into points, then CTM the resulting points */
  auto Points = Utility::RectToPoints(rect);  
  CTMPoints(Points);  
 
  if ( !CTM.preservesRect())  //If the CTM does not preserve a rectangle, draw a polygon
  {
    auto Edges = pointsToEdges(Points);
    shadeDevicePolygon(Edges, shader);
    return;
  }
  
  //Convert the CTM'd points back into a rect since we know it preserves it
  GIRect ConvertedRect = Utility::PointsToRect(Points).round();

  // Make sure rect is not an empty one and clip the edges from the bitmap with intersect
  if (ConvertedRect.isEmpty() || !ConvertedRect.intersect(BmpRect)) { return; }

  shadeDeviceRect(ConvertedRect, shader);
}

void MyCanvas::shadeConvexPolygon(const GPoint points[], int count, GShader* shader)
{
  assert(shader != nullptr);
  if (count < 3)
  {
    printf("Error: ShadeConvexPolygon needs at least 3 points to draw a polygon\n");
    return;
  }

  std::vector<GPoint> Points(points, points + count);

  CTMPoints(Points);	
  auto Edges = pointsToEdges(Points);

  shadeDevicePolygon(Edges, shader);
}

void MyCanvas::strokePolygon(const GPoint points[], int n, bool isClosed, const Stroke& stroke, GShader* shader)
{
  assert(shader != nullptr);
  if (n < 2)
  {
    printf("Error: StrokePolygon needs at least 2 points to stroke a line\n");
    return;
  }
  
  std::vector<GQuad> Shells;
  
  // Make a shell for each adjacent pair of points
  for (int i = 0; i < n - 1; ++i)
  {
    Shells.emplace_back(GQuad::Make(points[i], points[i+1], stroke.fWidth));
    //Shells.back().print();
  }

  //If the polygon is closed then we connect the last and first points
  //Otherwise if stroke.fAddCap is set then we set caps to the first and last shells.
  if (isClosed) {
    Shells.emplace_back(GQuad::Make(points[n-1], points[0], stroke.fWidth));
  }
  else if (stroke.fAddCap) {    
    
    GPoint ABT = Shells.front().ABT;
    
    Shells.front().A.fX -= ABT.fY;
    Shells.front().A.fY += ABT.fX;
    
    ABT = Shells.back().ABT;
    
    Shells.back().B.fX += ABT.fY;
    Shells.back().B.fY -= ABT.fX;
  }
    
  //Draw the shells
  for (auto Shell : Shells)
  {
    auto vPoints = Shell.getPoints();
    CTMPoints(vPoints);
    auto Edges = pointsToEdges(vPoints);

    shadeDevicePolygon(Edges, shader);
  }
  
  //Calculate the joint between each pair of shells and draw
  for (int i = 0; i < Shells.size() - 1; ++i)
  {
    auto Joint = calculateJoint(Shells[i], Shells[i + 1], stroke);
    
    if (Joint.empty()) continue;
    
    CTMPoints(Joint);
    auto Edges = pointsToEdges(Joint);
    shadeDevicePolygon(Edges, shader);
  }

  if (isClosed)
  {
    auto Joint = calculateJoint(Shells.back(), Shells.front(), stroke);

    CTMPoints(Joint);
    auto Edges = pointsToEdges(Joint);
    shadeDevicePolygon(Edges, shader);
  }
}

std::vector<GPoint> MyCanvas::calculateJoint(const GQuad& line1, const GQuad& line2, const Stroke& stroke) const
{   
  if (line1.B.fX != line2.A.fX && line1.B.fY != line2.A.fY) 
    {
      printf("Line 1 B and Line 2 A is not the same pivot point\n");
      return std::vector<GPoint>();
    }
    
    using namespace Utility;
    
    std::vector<GPoint> Poly;

    /* Calculate angle between lines to determine if we miter or bevel the joint*/
    GPoint U = Utility::UnitVector(line1.B, line1.A);
    GPoint V = Utility::UnitVector(line2.A, line2.B);
    float costheta = Utility::DotProduct(U, V);
    float sintheta = Utility::CrossProduct(U, V);
    float h = std::sqrt(2.0f / (1.0f - costheta));
    float hsin = std::sqrt(2.0f / (1.0f - sintheta));

    printf("New Joint, Miter: %f\n", stroke.fMiterLimit);
    printf("U: %f %f, V: %f %f\n", U.fX, U.fY, V.fX, V.fY);
    printf("Cos 0: %f, H: %f, Sin 0: %f, H: %f\n", costheta, h, sintheta, hsin);

    /* Both Joints share 3 common points, calculate and add to polygon vector*/
    const GPoint& joint = line1.B;
    const GPoint& BQ = {-line1.ABT.fX, -line1.ABT.fY};
    const GPoint& BR = {-line2.ABT.fX, -line2.ABT.fY};
    
    Poly.push_back(joint);
    Poly.push_back(joint + BQ);
    Poly.push_back(joint + BR);
    
    /* If angle between AB and BC is less than the miter limit then its a miter joint so we add P = B + BP*/
    if ( h <= stroke.fMiterLimit) 
    {
      GPoint BP = Utility::UnitVector(BR, BQ);
      float BPlength = stroke.fWidth / 2.0f * h;
      BP *= BPlength;

      GPoint P {joint.fX + BP.fX, joint.fY - BP.fY};
      
      Poly.push_back(P);
    }

    printf("Created Joint\n");
    for (auto Point : Poly)
      {
	printf("Point %f %f\n", Point.fX, Point.fY);
      }
    
    return Poly;  
}

/***********************************Device Drawing Functions *****************************************/

void MyCanvas::shadeDeviceRect(const GIRect& rect, GShader* shader)
{  
  assert(shader != nullptr);
  if (rect.isEmpty())
  {
    printf("Error: ShadeDeviceRect rect is empty can't draw\n");
    return;
  }

  // Get dst bitmap address and offset to the rect top
  GPixel* DstPixels = (GPixel*)((char*)Bitmap.pixels() + Bitmap.fRowBytes * rect.top());
  
  // Set the context of the shader to the CTM
  float TwoRows[6];
  CTM.GetTwoRows(TwoRows);
  shader->setContext(TwoRows);

  //All rows will have the same number of pixels so we can allocate once for the whole rect
  int count = rect.width();
  GPixel* row = new GPixel[count];
  for (int y = rect.top(); y < rect.bottom(); ++y)
  {
    shader->shadeRow(rect.left(), y, count, row);
    blendRow(DstPixels, rect.left(), row, count);
    DstPixels = (GPixel*)((char*)DstPixels + Bitmap.fRowBytes);
  }

  delete[] row;
}

void MyCanvas::shadeDevicePolygon(std::vector<GEdge>& Edges, GShader* shader)
{
  assert(shader != nullptr);
  if (Edges.size() < 2)
  {
    //printf("Error: ShadeDevicePolygon needs at least 2 edges to draw\n");
    return;
  }

  // Sort the edges from top to bottom, left to right
  std::sort(Edges.begin(), Edges.end());

  // The first two edges will be the top most edges
  GEdge LeftEdge = Edges[0];
  GEdge RightEdge = Edges[1];

  // Get start of pixel address for bitmap and offset to the top row 
  GPixel *DstPixels = (GPixel*)((char*)Bitmap.pixels() + Bitmap.fRowBytes * LeftEdge.top());

  // Set the context in the shader to the CTM
  float TwoRows[6];
  CTM.GetTwoRows(TwoRows);
  shader->setContext(TwoRows);
  
  int EdgeCounter = 2;
  for (int y = LeftEdge.top(); y < Edges.back().bottom(); ++y)
  {
    int startX = Utility::round(LeftEdge.currentX());
    int count = (int)(RightEdge.currentX() - LeftEdge.currentX());

    count = Utility::clamp(1, count, BmpRect.width() - 1);

    /* Allocate a row of pixels to calculate shader pixel values
     * then blend shader pixels into the dst bitmap */
    GPixel *row = new GPixel[count];
    shader->shadeRow(startX, y, count, row);
    blendRow(DstPixels, startX, row, count);
    delete[] row;  

    // Move currentX of the left and right edges to the next row
    LeftEdge.moveCurrentX(1.0f);
    RightEdge.moveCurrentX(1.0f);

    // Check left and right edge for y has passed bottom, if we have edges left then set the next edge
    if (y >= LeftEdge.bottom() && EdgeCounter < Edges.size())
    {
      LeftEdge = Edges[EdgeCounter];
      ++EdgeCounter;
    }
    if (y >= RightEdge.bottom() && EdgeCounter < Edges.size())
    {
      RightEdge = Edges[EdgeCounter];
      ++EdgeCounter;
    }

    DstPixels = (GPixel*)((char*)DstPixels + Bitmap.fRowBytes);
  }
}

GPixel MyCanvas::blend(GPixel src, GPixel dst)
{
  //If src alpha is 255 then just return the src
  if (GETA(src) == 0xFF) {
      return src;
  }
  //If src alpha is 0 then return the dst
  if (GETA(src) == 0) {
    return dst;
  }

  //Get the blend alpha value from 255 - Source_Alpha
  uint8_t src_a_blend = 0xFF - GETA(src);

  /** Using blend formula for each pixel color ARGB
   *  We first get the value from src, and add it to the result of
   *  multiplying a_blend and the color of dest.
   */
  uint8_t alpha = GETA(src) + MulDiv255Round(src_a_blend, GETA(dst));
  uint8_t red = GETR(src) + MulDiv255Round(src_a_blend, GETR(dst));
  red = Utility::clamp((uint8_t)0, red, alpha);
  uint8_t green = GETG(src) + MulDiv255Round(src_a_blend, GETG(dst));
  green = Utility::clamp((uint8_t)0, green, alpha);
  uint8_t blue = GETB(src) + MulDiv255Round(src_a_blend, GETB(dst));
  blue = Utility::clamp((uint8_t)0, blue, alpha);

  return GPixel_PackARGB(alpha, red, green, blue);
}

void MyCanvas::blendRow(GPixel *Dst, int startX, GPixel row[], int count)
{
  for ( int i = 0; i < count; ++startX, ++i) 
  {
    Dst[startX] = blend(row[i], Dst[startX]);
  }
}

/*******************************Current Transformation Fuctions***************************************/

void MyCanvas::save()
{
  MatrixStack.push(CTM);	//Push a copy of the CTM onto the stack
}

void MyCanvas::restore()
{
  if (!MatrixStack.empty())		//Check to make sure stack is not empty
  {
    CTM = MatrixStack.top();
    MatrixStack.pop();				//Remove the CTM
  }
}

void MyCanvas::concat(const float matrix[6])
{
  /* Make a new matrix using the input matrix values*/
  GMatrix<float> ConcatMat(matrix, 6);
  CTM.concat(ConcatMat);	       //Concat the input matrix onto the CTM
}

void MyCanvas::CTMPoints(std::vector<GPoint>& Points) const
{
  /* Convert all points by the CTM*/
  for (auto &Point: Points) {
    CTM.convertPoint(Point);
  }
}

/*******************************Edge Creation Functions***********************************************/

std::vector<GEdge> MyCanvas::pointsToEdges(std::vector<GPoint>& Points)
{
  SortPointsForConvex(Points);          //Sort the points into an order we can make edges with
  
  auto Edges = MakeConvexEdges(Points); //Make edges out of the sorted points

  auto PreClip = MakeConvexEdges(Points);

  ClipEdges(Edges);                     //Clip the edges from the dst bitmap

  return Edges;
}

void MyCanvas::SortPointsForConvex(std::vector<GPoint>& Points)
{
  /* Sort Points by y and then x if they are equal */
  std::sort(Points.begin(), Points.end(), [](const GPoint& a, const GPoint& b) -> bool  {
      return std::tie(a.fY, b.fX) < std::tie(b.fY, a.fX);
      //return ( std::fabs(a.fY - b.fY) < .001) ? a.fX > b.fX : a.fY < b.fY;
  });

  std::queue<GPoint> RightPoints;  //Hold all points right of the pivot
  std::stack<GPoint> LeftPoints;   //Hold all points left of the pivot

  /* Start from 2nd value because top is the pivot */
  for (auto Point = ++Points.begin(); Point != --Points.end(); ++Point)
  {
    /* If the x value of the Point is greater than or equal to the top point x*/
    if ( Point->fX >= Points.front().fX) {
      RightPoints.push(*Point);  
    }
    else {
      LeftPoints.push(*Point);	 
    }
  }
   
  // We store the value of RightPoints to use as an offset later
  const auto RightSize = RightPoints.size();

  /* Points: Order Diagram - TopPivot, RightPoints - RightPoints.end() - 1, BottomPivot, LeftPoints - LeftPoints.end() */
  for (auto Point = ++Points.begin(); !RightPoints.empty(); ++Point)
  {
    *Point = RightPoints.front();
    RightPoints.pop();
  }

  /* Set the middle point to be the bottom most point */
  Points[RightSize + 1] = Points.back();

  /* If the LeftPoints are not empty store them*/
  for (int i = RightSize + 2; !LeftPoints.empty(); ++i)
  {
    Points[i] = LeftPoints.top();  //Copy over from left stack
    LeftPoints.pop();									     
  }
}

std::vector<GEdge> MyCanvas::MakeConvexEdges(const std::vector<GPoint>& Points)
{
  std::vector<GEdge> Edges;    
  auto Penultimate = --Points.end();

  // Create an edge between all points 
  for (auto Point = Points.begin(); Point != Penultimate; ++Point) 
  {
    Edges.emplace_back(*Point, *(Point + 1)); 
  }

  // Connect the last point to the first point
  Edges.emplace_back(Points.front(), Points.back());

  return Edges;
}

/******************************Edge Clip Functions ***************************************************/

void MyCanvas::ClipEdges(std::vector<GEdge>& Edges) const
{
  std::vector<GEdge> NewEdges;      //New edges that will be created from side clipping

  ClipEdgesTopAndBottom(Edges);	    //Pin top and bottom edges
  ClipEdgesLeft(Edges, NewEdges);   //Pin left edges, create new edges from clipping
  ClipEdgesRight(Edges, NewEdges);  //Pin right edges, create new edges from clipping

  /* Copy over all clipped new edges into the original edge vector */
  Edges.insert(Edges.end(), NewEdges.begin(), NewEdges.end());
}

void MyCanvas::ClipEdgesTopAndBottom(std::vector<GEdge>& Edges) const
{
  const int height = BmpRect.height();  //The height of the bitmap

  for (auto it = Edges.begin(); it != Edges.end(); )
  {
    /* Pin the top and bottom of the edges that are out of bitmap vertical
     * Will return false if the value needs to be removed*/
    if ( !it->pinTopAndBot(height) ) {
      it = Edges.erase(it);	//Remove all horizontal edges, and edges off screen
    }
    else {
      ++it;		//If we don't remove a value then we increment the iterator
    }
  }
}

void MyCanvas::ClipEdgesLeft(std::vector<GEdge>& Edges, std::vector<GEdge>& NewEdges) const
{
  /* Check all Edges*/
  for (auto &Edge: Edges)
  {
    /* If entire segment is to the left pin it to the edge*/
    if (Edge.currentX() < 0 && Edge.bottomX() < 0)
    {
      Edge.setCurrentX(0);
      Edge.makeSlopeVertical();

      continue;
    }

    /* If the top currentX is to the left of the bitmap*/
    if (Edge.currentX() < 0)
    {
      /* Find the new y between edge and current x and the y point at the new x
       * Make a new edge of the clipped edge on the side
       * Set the current clipped edge to 0 current x and clip the top of the current edge */
      float ClipY = Edge.top() - Edge.currentX() / Edge.slope();
      NewEdges.emplace_back(GPoint::Make(0, Edge.top()),GPoint::Make(0, ClipY));
      Edge.setCurrentX(0);
      Edge.setTop(Utility::round(ClipY));
    }

    /* If the bottom x of the edge is to the left of the bitmap*/
    if (Edge.bottomX() < 0)
    {
      /* The Clipped y value is the top - currentX (pos) / slope (neg)
       * add the pinned edge to the set of new edges and clip the current edge bottom */
      float ClipY = Edge.top() - Edge.currentX() / Edge.slope();
      NewEdges.emplace_back(GPoint::Make(0, ClipY), GPoint::Make(0, Edge.bottom()));
      Edge.setBottom(Utility::round(ClipY));
    }
  }
}

void MyCanvas::ClipEdgesRight(std::vector<GEdge>& Edges, std::vector<GEdge>& NewEdges) const
{
  const int width = BmpRect.width();  //Get width of the bitmap

  for (auto &Edge: Edges)
  {
    // If the edge is completely to the right of the bitmap
    if (Edge.currentX() > width && Edge.bottomX() > width)
    {
      Edge.setCurrentX(width);	//Set the CurrentX to the bitmap width
      Edge.makeSlopeVertical();	//Set the slope to 0. WARNING: This implementation could be better
      continue;          	//Go to next edge
    }

    /* If the top of the edge is to the right of the bitmap
     * Assumptions: Slope: negative, CurrentX > width, BottomX < width */
    if (Edge.currentX() > width)
    {
      /* Get the Y value to clip the edge at
       * Add new pinned edge into new edges vector and clip the current edge */
      float ClipY = Edge.top() + ( width - Edge.currentX()) / Edge.slope();
      NewEdges.emplace_back(GPoint::Make(width, Edge.top()), GPoint::Make(width, ClipY));
      Edge.setCurrentX(width);		
      Edge.setTop(Utility::round(ClipY));
    }

    /* If the bottom of the edge needs to be clipped right
     * Assumptions: Slope: positive, CurrentX < width, BottomX > width */
    if (Edge.bottomX() > width)
    {
      /* Get the ClipY value, add new pinned edge to new edges and clip the current edge*/
      float ClipY = Edge.top() + ( width - Edge.currentX()) / Edge.slope();
      NewEdges.emplace_back(GPoint::Make(width, ClipY), GPoint::Make(width, Edge.bottom()));
      Edge.setBottom(Utility::round(ClipY));
    }
  }
}
