#pragma once
#include "Line.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "DrawInfo.h"
#include "stdafx.h"

class PaintLines
{
	Keyboard *keyboard;
	Mouse *mouse;
	DrawInfo *drawInfo;
	vector<Line> lines;

	void drawLines();
	void drawGrid();
	void addLine(Point s, Point e);

public:
	PaintLines(Keyboard *keyboard, Mouse *mouse, DrawInfo *drawInfo);
	pair<int, vector<Line>> main();		//ÅŒã‚É‰Ÿ‚µ‚½ƒL[, ü•ªW‡, ‚ğ•Ô‚·
};