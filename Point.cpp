#include "Point.h"

bool operator<(const Point & a, const Point & b)
{
	return a.real() != b.real() ? a.real() < b.real() : a.imag() < b.imag();
}

double cross(Point &a, Point &b) {
	return imag(conj(a) * b);
}

double dot(Point &a, Point &b) {
	return real(conj(a) * b);
}

int ccw(Point a, Point b, Point c) {
	b -= a;
	c -= a;
	if (cross(b, c) > 0) return +1;		//counter clockwise
	if (cross(b, c) < 0) return -1;		//clockwise
	if (dot(b, c) < 0) return +2;		//c--a--b on line
	if (norm(b) < norm(c)) return -2;	//a--b--c on line
	return 0;							//a--c--b on line or a--c==b
}