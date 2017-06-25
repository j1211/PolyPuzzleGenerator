//線分。セル座標（マス目1個＝座標変化1)で記録しているものとする。
#pragma once
#include "Point.h"
#include "DrawInfo.h"

class Line {
	Point s, e;

public:
	Line();
	Line(Point s, Point e);
	bool ishit(Line &l);
	bool isParallel(Line &l);
	Line marge_parallel_line(Line l);
	Point cross_point(Line &l);
	bool on_line(Point p);
	void draw(DrawInfo *drawInfo, int color = 0, int thickness = 2);
	double dist(Point &p);
};