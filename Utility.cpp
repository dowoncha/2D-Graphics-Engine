#include "Utility.h"

namespace Utility
{
  GPixel ColorToPixel(const GColor& color)
  {
    using COLORBYTE = uint8_t;

    GColor pinned = color.pinToUnit();  //Make sure color is between 0 and 1

    float fA =  pinned.fA * 255.9999f;		//Convert from 0-1 to 0-255
    COLORBYTE uA = (COLORBYTE) fA;
    COLORBYTE uR = (COLORBYTE) (pinned.fR * fA);  //Multiply rgb values by the new alpha
    COLORBYTE uG = (COLORBYTE) (pinned.fG * fA);
    COLORBYTE uB = (COLORBYTE) (pinned.fB * fA);

    return GPixel_PackARGB(uA, uR, uG, uB);		//Returned the packed pixel
  }

  std::vector<GPoint> QuadToPoints(const GRect& Rect)
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

  GRect PointsToQuad(const std::vector<GPoint>& Points)
  {
  	assert(Points.size() == 4);
  	/* Return */
  	return GRect::MakeLTRB(Points[0].x(), Points[0].y(), Points[3].x(), Points[3].y());
  }

  GMatrix<float> RectToRect(const GRect& src, const GRect& dst)
  {
  	// Make translation matrix of the source from the origin
  	GMatrix<float> SrcTranslate;
  		SrcTranslate.setTranslation(-1 * src.left(), -1 * src.top());

  	// Make scale matrix of dx, dy. Dst / Src
  	GMatrix<float> Scale;
    Scale.setScale(	dst.width() / src.width(), dst.height() / src.height());

  	// Get translation matrix of the dst rectangle
  	GMatrix<float> DstTranslate;
    DstTranslate.setTranslation(dst.left(), dst.top());

  	// Concatenate the 3 matrices. DstTranslate * Scale * SrcTranslate
  	DstTranslate.concat(Scale).concat(SrcTranslate);

  	return DstTranslate;
  }
} //Namespace Utility
