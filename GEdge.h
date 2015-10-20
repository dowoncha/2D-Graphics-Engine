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
	float Slope;
	float CurrentX;
	int Top;
 	int Bottom;
public:
	GEdge(const GPoint& p1, const GPoint& p2);
	~GEdge() {}

	float GetSlope() const { return Slope; }
	float GetCurrentX() const { return CurrentX; }
	int GetTop() const { return Top; }
	int GetBottom() const { return Bottom; }
	float GetBottomX() const { return CurrentX + (Bottom - Top) * Slope; }

	bool PinTopAndBot(const int& Height);
	void SetTop(int NewTop) { Top = NewTop; }
	void SetBottom(int NewBot) { Bottom = NewBot; }
	void SetCurrentX(const float& NewX) { CurrentX = NewX; }
	void MakeSlopeVertical() { Slope = 0; }
	void MoveCurrentX(const float& dy) {CurrentX += Slope * dy; }

	//bool operator== (const GEdge& Compare);
	//GEdge& operator= (const GEdge& Source) con;
	bool operator< (const GEdge& c) const;

	/* Outputs the values of the Edge, slope, x, and y*/
	std::string ToString() const;
	static void PrintEdges(std::vector<GEdge>& Edges);
};
