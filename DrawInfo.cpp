#include "DrawInfo.h"
#include "stdafx.h"

DrawInfo::DrawInfo(int windowSizeX, int windowSizeY, int defaultCellSize, int ORow, int OCol)
{
	this->windowSizeX = windowSizeX;
	this->windowSizeY = windowSizeY;
	this->defaultCellSize = defaultCellSize;
	this->ORow = ORow;
	this->OCol = OCol;
	this->scale = 1.0;
	this->transX = 0;
	this->transY = 0;
}

int DrawInfo::toDrawY(double cellY)
{
	int y = (ORow + cellY) * defaultCellSize;
	return (y + transY - windowSizeY / 2) * scale + windowSizeY / 2;
}

int DrawInfo::toDrawX(double cellX)
{
	int x = (OCol + cellX) * defaultCellSize;
	return (x + transX - windowSizeX / 2) * scale + windowSizeX / 2;
}

int DrawInfo::toCellY(int drawY)
{
	int y = (drawY - windowSizeY / 2) / scale + windowSizeY / 2 - transY;
	return (int)floor((double)(y + defaultCellSize / 2) / defaultCellSize) - ORow;
}

int DrawInfo::toCellX(int drawX)
{
	int x = (drawX - windowSizeX / 2) / scale + windowSizeX / 2 - transX;
	return (int)floor((double)(x + defaultCellSize / 2) / defaultCellSize) - OCol;
}

void DrawInfo::update(Keyboard *keyboard)
{
	const int dy[4] = { -1, 0, 1, 0 };	//↑→↓←
	const int dx[4] = { 0, 1, 0, -1 };
	const int speed = 2;
	const int keys[4] = { KEY_INPUT_W, KEY_INPUT_D, KEY_INPUT_S, KEY_INPUT_A };
	double eps = 1e-7;

	for (int dir = 0; dir < 4; dir++) {
		if (keyboard->is_push(keys[dir])) {
			//(transY, transX)は画面の移動量(ピクセル)であって、注視点の移動ではない。
			transY -= dy[dir] * speed / scale;
			transX -= dx[dir] * speed / scale;
		}
	}

	if (keyboard->is_push(KEY_INPUT_Z)) {
		scale -= 0.02;
		if (abs(scale) < eps) scale -= 0.02;
	}
	if (keyboard->is_push(KEY_INPUT_X)) {
		scale += 0.02;
		if (abs(scale) < eps) scale += 0.02;
	}
}

int DrawInfo::window_width()
{
	return windowSizeX;
}

int DrawInfo::window_height()
{
	return windowSizeY;
}
