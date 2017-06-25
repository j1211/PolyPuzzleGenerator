#include "Line.h"
#include "DrawInfo.h"
#include "DxLib.h"

Line::Line()
{
}

Line::Line(Point s, Point e)
{
	this->s = s;
	this->e = e;
}

bool Line::ishit(Line & l)
{
	return ccw(s, e, l.s) * ccw(s, e, l.e) <= 0 && ccw(l.s, l.e, s) * ccw(l.s, l.e, e) <= 0;
}

bool Line::isParallel(Line & l)
{
	return cross(e - s, l.e - l.s) == 0;
}

//前提条件：2線分は一直線上にあり, 接触している
Line Line::marge_parallel_line(Line l)
{
	Point p[4] = { s, e, l.s, l.e };
	sort(p, p + 4);
	return Line(p[0], p[3]);
}

//交点を求める. 前提条件：2線分は一直線上にない.
Point Line::cross_point(Line & l)
{
	double A = cross(e - s, l.e - l.s);
	double B = cross(e - s, e - l.s);
	return l.s + B / A * (l.e - l.s);
}

//線分に乗っているか？
bool Line::on_line(Point p) {
	return cross(p - s, e - s) == 0 && dot(p - s, e - s) >= 0 && dot(p - e, s - e) >= 0;
}

//描画
void Line::draw(DrawInfo * drawInfo, int color, int thickness)
{
	int x1 = drawInfo->toDrawX(s.real());
	int y1 = drawInfo->toDrawY(s.imag());
	int x2 = drawInfo->toDrawX(e.real());
	int y2 = drawInfo->toDrawY(e.imag());
	DrawLine(x1, y1, x2, y2, color, thickness);
}

double Line::dist(Point &p)
{
	if (dot(e - s, p - s) <= 0) return abs(p - s);
	if (dot(s - e, p - e) <= 0) return abs(p - e);
	return abs(cross(e - s, p - s)) / abs(e - s);
}
