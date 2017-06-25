#pragma once
#include "Line.h"
#include "Point.h"
#include "stdafx.h"

class MakeGraph {
	tuple<int, vector<Point>, vector<vector<int>>> main(vector<Line> lines);
};