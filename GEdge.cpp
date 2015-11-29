//Copyright 2015 Dowon Cha

#include "GEdge.h"

GEdge::GEdge(const GPoint& p1, const GPoint& p2)
{
  /* Whichever has the higher y value becomes the top point */
  const GPoint& TopPoint = (p1.fY < p2.fY) ? p1 : p2;  
  const GPoint& BotPoint = (p1.fY < p2.fY) ? p2 : p1;

  /* Get the slope from unmodified values */
  fSlope = (TopPoint.fX - BotPoint.fX) / (TopPoint.fY - BotPoint.fY);

  /* Find the rounded top and bottom values */
  nTop = Utility::round(TopPoint.fY);
  nBottom = Utility::round(BotPoint.fY);

  /* Find the currentX value */
  fCurrentX = TopPoint.x() + fSlope * (nTop + .5 - TopPoint.y()) ;
}

bool GEdge::operator<(const GEdge& c) const
{
  int rX = Utility::round(fCurrentX);
  int cRX = Utility::round(c.fCurrentX);
 
  return std::tie(nTop, rX, fSlope) < std::tie(c.nTop, cRX, c.fSlope);
}

bool GEdge::pinTopAndBot(int Height)
{
  //Error edge segment is entirely out of bitmap just ignore
  if (nBottom < 0 || nTop >= Height) 
  {
    return false;
  }

  // First pin the top point if it is above the bitmap
  if ( nTop < 0 )
  {
    // Move the currentX by the slope * dx 
    fCurrentX -= fSlope * nTop;
    nTop = 0;
  }

  // If the bottom of the point is below the bitmap just set bottom of line to bitmap height
  if (nBottom > Height) {
    nBottom = Height;
  }

  if (std::isinf(fSlope) || std::isnan(fSlope))
  {
    return false;
  }

  return true;
}
