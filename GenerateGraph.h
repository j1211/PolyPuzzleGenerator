#pragma once
#include "Line.h"
#include "Point.h"
#include "stdafx.h"

class GenerateGraph {
	DrawInfo *drawInfo;
	Keyboard *keyboard;
	void drawLines(vector<Line> &lines);
	void drawGraph(vector<Point>& kotens, vector<vector<int>>& edges);

public:
	GenerateGraph(DrawInfo *drawInfo, Keyboard *keyboard);
	tuple<int, vector<Point>, vector<vector<int>>> main(vector<Line> lines);
};