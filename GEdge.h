//Copyright 2015 Dowon Cha

#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <tuple>

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

	bool pinTopAndBot(int Height);
	void setTop(int NewTop) { nTop = NewTop; }
	void setBottom(int NewBot) { nBottom = NewBot; }
	void setCurrentX(float NewX) { fCurrentX = NewX; }
	void makeSlopeVertical() { fSlope = 0.0f; }
	void moveCurrentX(float dy) { fCurrentX += fSlope * dy; }

	bool operator< (const GEdge& c) const;
};
