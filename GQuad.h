/**
 * \file GQuad.h 
 *
 * \author Dowon Cha
 *
 *  Copyright 2015
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
    // Make AB vector of length half the width. Then use AB vector to make AB perpendicular
    GPoint AB = Utility::unitVector(A, B);
    float rad = width / 2;
    AB.fX *= rad;
    AB.fY *= rad;
    GPoint ABT = GPoint::Make(-AB.fY, AB.fX);
    
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
