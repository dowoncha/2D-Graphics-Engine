#pragma once

#include "GPoint.h"
#include "GRect.h"
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>

class GEdge
{
private:
	float fSlope;
	float fCurrentX;
	int nTop;
 	int nBottom;
public:
	GEdge(const GPoint& p1, const GPoint& p2);
	~GEdge() {}

	float slope() const { return fSlope; }
	float currentX() const { return fCurrentX; }
	int top() const { return nTop; }
	int bottom() const { return nBottom; }
	float bottomX() const { return fCurrentX + (nBottom - nTop) * fSlope; }

	bool PinTopAndBot(const int& Height);
	void SetTop(int NewTop) { nTop = NewTop; }
	void SetBottom(int NewBot) { nBottom = NewBot; }
	void SetCurrentX(const float& NewX) { fCurrentX = NewX; }
	void MakeSlopeVertical() { fSlope = 0; }
	void MoveCurrentX(const float& dy) { fCurrentX += fSlope * dy; }

	bool operator< (const GEdge& c) const;
};
