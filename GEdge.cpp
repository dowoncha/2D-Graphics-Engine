#include "GEdge.h"

GEdge::GEdge(const GPoint& p1, const GPoint& p2)
{
	/* Whichever has the higher y value becomes the top point */
	const GPoint& TopPoint = (p1.y() < p2.y()) ? p1 : p2;
	const GPoint& BotPoint = (p1.y() < p2.y()) ? p2 : p1;

	/* Get the slope from unmodified values */
	fSlope = (TopPoint.x() - BotPoint.x()) / (TopPoint.y() - BotPoint.y());

	/* Find the rounded top and bottom values */
	nTop = (int)(TopPoint.y() + .5);
	nBottom = (int)(BotPoint.y() + .5);

	/* Find the currentX value */
	fCurrentX = TopPoint.x() + fSlope * (nTop - TopPoint.y());
}

bool GEdge::operator<(const GEdge& c) const
{
	if (nTop != c.nTop)				//Check the tops first
		return nTop < c.nTop;			//Return which top is higher up
	else
		return fCurrentX + fSlope < c.fCurrentX + c.fSlope;	//Finally check the current x location
}

bool GEdge::PinTopAndBot(const int& Height)
{
	//Error edge segment is entirely out of bitmap just ignore
	if (nBottom < 0 || nTop >= Height)
	{
		return false;
	}

	/* First pin the top point if it is above the bitmap*/
	if ( nTop < 0 )
	{
		//std::cout << "Top being pinned to 0: " << Top << " Old CurrentX: " << CurrentX;
		/* Move the currentX by the slope * dx */
		fCurrentX += fSlope * ( 0 - nTop );
		nTop = 0;
	}

	/* If the bottom of the point is below the bitmap just set bottom of line to bitmap height */
	if (nBottom > Height)
	{
		nBottom = Height;
	}

	/* Delete all edges that are horizontal AKA infinite slope*/
	if (std::isinf(fSlope))
	{
		return false;
	}

	return true;
}
