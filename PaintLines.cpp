#include "PaintLines.h"

PaintLines::PaintLines(Keyboard * keyboard, Mouse * mouse, DrawInfo * drawInfo)
{
	this->keyboard = keyboard;
	this->mouse = mouse;
	this->drawInfo = drawInfo;
}

void PaintLines::drawLines()
{
	for (int i = 0; i < lines.size(); i++) {
		lines[i].draw(drawInfo);
	}
}

void PaintLines::drawGrid()
{
	int lCol = drawInfo->toCellX(0);
	int rCol = drawInfo->toCellX(drawInfo->window_width());
	int lRow = drawInfo->toCellY(0);
	int rRow = drawInfo->toCellY(drawInfo->window_height());
	
	if (lCol > rCol) swap(lCol, rCol);
	if (lRow > rRow) swap(lRow, rRow);

	for (int i = lCol; i <= rCol; i++) {
		int x = drawInfo->toDrawX(i);
		DrawLine(x, 0, x, drawInfo->window_height(), 0, 1);
	}

	for (int i = lRow; i <= rRow; i++) {
		int y = drawInfo->toDrawY(i);
		DrawLine(0, y, drawInfo->window_width(), y, 0, 1);
	}

	DrawCircle(drawInfo->toDrawX(0), drawInfo->toDrawY(0), 3, GetColor(255, 0, 0), TRUE);
}

void PaintLines::addLine(Point s, Point e)
{
	Line line = Line(s, e);

	for (int i = 0; i < lines.size(); i++) {
		if (!line.ishit(lines[i])) continue;
		if (line.isParallel(lines[i])) continue;
		Point p = line.cross_point(lines[i]);
		if (p.real() != (int)p.real() || p.imag() != (int)p.imag()) return;
	}

	vector<Line> newLines;

	for (int i = 0; i < lines.size(); i++) {
		if (line.ishit(lines[i]) && line.isParallel(lines[i])) {
			line = line.marge_parallel_line(lines[i]);
		}
		else {
			newLines.push_back(lines[i]);
		}
	}

	lines = newLines;
	lines.push_back(line);
}

pair<int, vector<Line>> PaintLines::main()
{
	int endKeyCode[2] = { KEY_INPUT_ESCAPE, KEY_INPUT_RIGHT };		//終了キーの集合. 順番入れ替え可.
	Point s;

	while (ScreenFlip() == 0 && ProcessMessage() == 0 && ClearDrawScreen() == 0)
	{
		keyboard->update();
		mouse->update();
		drawInfo->update(keyboard);

		for (int i = 0; i < 2; i++) {
			if (keyboard->is_push(endKeyCode[i])) {
				return pair<int, vector<Line>>(endKeyCode[i], lines);
			}
		}

		if (mouse->is_click()) {
			s = Point((double)drawInfo->toCellX(mouse->X()), (double)drawInfo->toCellY(mouse->Y()));
		}

		Point e = Point((double)drawInfo->toCellX(mouse->X()), (double)drawInfo->toCellY(mouse->Y()));
		if (mouse->is_release()) {
			addLine(s, e);
		}

		if (keyboard->is_click(KEY_INPUT_BACK)) {
			if (lines.size() > 0) {
				lines.pop_back();
			}
		}

		drawGrid();
		drawLines();
		if (mouse->is_push()) {
			Line(s, e).draw(drawInfo);
		}
	}
	return pair<int, vector<Line>>(endKeyCode[0], lines);
}