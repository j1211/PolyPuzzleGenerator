#pragma once
#include "stdafx.h"
#include "Point.h"

class FileWriter {
public:
	bool writeFile(vector<vector<Point>> &polys, string filename, bool isCastInt);
};