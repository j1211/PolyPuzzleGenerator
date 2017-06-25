//右向き(Real軸正）から下向き(Imag軸正)に倒れる向きを「反時計回り」と定義する。
//見た目上は、逆になるので注意。
#pragma once
#include "stdafx.h"
typedef complex<double> Point;

bool operator<(const Point &a, const Point &b);
double cross(Point &a, Point &b);
double dot(Point &a, Point &b);
int ccw(Point a, Point b, Point c);
