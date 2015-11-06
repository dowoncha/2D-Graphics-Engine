#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <algorithm>

#include "GPoint.h"
#include "GRect.h"
#include "Utility.h"

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

	bool PinTopAndBot(int Height);
	void SetTop(int NewTop) { nTop = NewTop; }
	void SetBottom(int NewBot) { nBottom = NewBot; }
	void SetCurrentX(float NewX) { fCurrentX = NewX; }
	void MakeSlopeVertical() { fSlope = 0.0f; }
	void MoveCurrentX(float dy) { fCurrentX += fSlope * dy; }

	bool operator< (const GEdge& c) const;
};
