#include "GEdge.h"

GEdge::GEdge(const GPoint& p1, const GPoint& p2)
{
	/* Whichever has the higher y value becomes the top point */
	const GPoint& TopPoint = (p1.y() < p2.y()) ? p1 : p2;
	const GPoint& BotPoint = (p1.y() < p2.y()) ? p2 : p1;

	/* Get the slope from unmodified values */
	fSlope = (TopPoint.x() - BotPoint.x()) / (TopPoint.y() - BotPoint.y());

	/* Find the rounded top and bottom values */
	nTop = Utility::round(TopPoint.y());
	nBottom = Utility::round(BotPoint.y());

	/* Find the currentX value */
	fCurrentX = TopPoint.x() + fSlope * (nTop + .5 - TopPoint.y()) ;
}

bool GEdge::operator<(const GEdge& c) const
{
	int rX = Utility::round(fCurrentX);
	int rX1 = Utility::round(fCurrentX + fSlope);
	int cRX = Utility::round(c.fCurrentX);
	int cRX1 = Utility::round(c.fCurrentX + c.fSlope);
	return std::tie(nTop, rX, fSlope) < std::tie(c.nTop, cRX, c.fSlope);
}

bool GEdge::PinTopAndBot(int Height)
{
	//Error edge segment is entirely out of bitmap just ignore
	if (nBottom < 0 || nTop >= Height) {
		return false;
	}

	/* First pin the top point if it is above the bitmap*/
	if ( nTop < 0 )
	{
		//std::cout << "Top being pinned to 0: " << Top << " Old CurrentX: " << CurrentX;
		/* Move the currentX by the slope * dx */
		fCurrentX -= fSlope * nTop;
		nTop = 0;
	}

	/* If the bottom of the point is below the bitmap just set bottom of line to bitmap height */
	if (nBottom > Height) {
		nBottom = Height;
	}

	/* Delete all edges that are horizontal AKA infinite slope*/
	if (std::isinf(fSlope)) {
		return false;
	}

	return true;
}
