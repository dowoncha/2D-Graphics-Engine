/**
 * Copyright 2015 Dowon Cha
 *
 *  This class will be used to hold 4 points and maybe do some manipulation wthin.
 *  Call the draw function to draw the polygon. 
 */

#include <vector>
#include "GPoint.h"
#include "Utility.h"

class GQuad
{
public:
  GPoint A;
  GPoint B;
  GPoint ABT;

  static GQuad Make(const GPoint& A, const GPoint& B, float width)
  {
    GPoint AB = Utility::unitVector(A, B);
    float rad = width * 0.5f;
    GPoint ABT = GPoint::Make(-AB.fY * rad, AB.fX * rad);

    GQuad Shell = {A, B, ABT};

    return Shell;
  }

  std::vector<GPoint> getPoints()
  {
    std::vector<GPoint> Points;
    Points.emplace_back(GPoint{A.fX + ABT.fX, A.fY + ABT.fY});
    Points.emplace_back(GPoint{A.fX - ABT.fX, A.fY - ABT.fY});
    Points.emplace_back(GPoint{B.fX + ABT.fX, B.fY + ABT.fY});
    Points.emplace_back(GPoint{B.fX - ABT.fX, B.fY - ABT.fY});

    return Points;
  }
};
