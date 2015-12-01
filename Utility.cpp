#include "Utility.h"

namespace Utility
{
  
GPoint UnitVector(const GPoint& A, const GPoint& B)
{
  GPoint AB = B - A;
  float length = std::sqrt(AB.fX * AB.fX + AB.fY * AB.fY);
  AB.fX /= length;
  AB.fY /= length;

  return AB;
}

GPixel ColorToPixel(const GColor& color)
{
  GColor pinned = color.pinToUnit();            //Make sure color is between 0 and 1

  float fA =  pinned.fA * 255.9999f;		  //Convert from 0-1 to 0-255
  uint8_t uA = (uint8_t) fA;
  uint8_t uR = (uint8_t) (pinned.fR * fA);  //Multiply rgb values by the new alpha
  uint8_t uG = (uint8_t) (pinned.fG * fA);
  uint8_t uB = (uint8_t) (pinned.fB * fA);

  return GPixel_PackARGB(uA, uR, uG, uB);	  //Returned the packed pixel
}

std::vector<GPoint> RectToPoints(const GRect& Rect)
{
  //Convert the input rect into a vector with 4 point corners
  std::vector<GPoint> PreTransform({
    GPoint::Make(Rect.x(), Rect.y()),
    GPoint::Make(Rect.x() + Rect.width(), Rect.y()),
    GPoint::Make(Rect.x(), Rect.y() + Rect.height()),
    GPoint::Make(Rect.x() + Rect.width(), Rect.y() + Rect.height())
  });

  return PreTransform;
}

GRect PointsToRect(const std::vector<GPoint>& Points)
{
  assert(Points.size() == 4);

  return GRect::MakeLTRB(Points[0].x(), Points[0].y(), Points[3].x(), Points[3].y());
}

GMatrix<float> RectToRect(const GRect& src, const GRect& dst)
{
  // Make translation matrix of the source from the origin
  GMatrix<float> SrcTranslate;
  SrcTranslate.setTranslation(-src.left(), -src.top());

  // Make scale matrix of dx, dy. Dst / Src
  GMatrix<float> Scale;
  Scale.setScale(dst.width() / src.width(), dst.height() / src.height());

  // Get translation matrix of the dst rectangle
  GMatrix<float> DstTranslate;
  DstTranslate.setTranslation(dst.left(), dst.top());

  // Concatenate the 3 matrices. DstTranslate * Scale * SrcTranslate
  DstTranslate.concat(Scale).concat(SrcTranslate);

  return DstTranslate;
}

} //namespace Utility
