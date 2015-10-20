#include "GEdge.h"

GEdge::GEdge(const GPoint& p1, const GPoint& p2)
{
	/* Whichever has the higher y value becomes the top point */
	const GPoint& TopPoint = (p1.y() < p2.y()) ? p1 : p2;
	const GPoint& BotPoint = (p1.y() < p2.y()) ? p2 : p1;

	/* Calculate dx and dy from the two points */
	float dx = TopPoint.x() - BotPoint.x();
	float dy = TopPoint.y() - BotPoint.y();

	/* Get the slope from unmodified values */
	Slope = dx / dy;

	/* Find the rounded top and bottom values */
	Top = (int)(TopPoint.y() + .5);
	Bottom = (int)(BotPoint.y() + .5);

	/* Find the currentX value */
	CurrentX = TopPoint.x() + Slope * (Top - TopPoint.y());
}

bool GEdge::operator<(const GEdge& c) const
{
	if (Top != c.GetTop())				//Check the tops first
		return Top < c.GetTop();			//Return which top is higher up
	else if (std::fabs(Slope - c.GetSlope()) > .0001)
		return Slope < c.GetSlope();		//Check slope next
	else
		return CurrentX < c.GetCurrentX();	//Finally check the current x location
}

bool GEdge::PinTopAndBot(const int& Height)
{
	//Error edge segment is entirely out of bitmap just ignore
	if (Bottom < 0 || Top >= Height)
	{
		return false;
	}

	/* First pin the top point if it is above the bitmap*/
	if ( Top < 0 )
	{
		//std::cout << "Top being pinned to 0: " << Top << " Old CurrentX: " << CurrentX;
		/* Move the currentX by the slope * dx */
		CurrentX += Slope * ( 0 - Top );
		Top = 0;
	}

	/* If the bottom of the point is below the bitmap just set bottom of line to bitmap height */
	if (Bottom > Height)
	{
		Bottom = Height;
	}

	/* Delete all edges that are horizontal AKA infinite slope*/
	if (isinf(Slope))
	{
		return false;
	}

	return true;
}

std::string GEdge::ToString() const
{
  using std::to_string;
	std::string EdgeInfo = "Slope: " + to_string(Slope) +
		" Top Point X: " + to_string(CurrentX) + " Y: " + to_string(Top) +
		" Bot Point X: " + to_string(GetBottomX()) + " Y: " + to_string(Bottom) + '\n';

	return EdgeInfo;
}

void GEdge::PrintEdges(std::vector<GEdge>& Edges)
{
	for (auto &Edge: Edges)
		std::cout << Edge.ToString();
}
