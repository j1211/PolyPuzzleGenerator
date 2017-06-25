#pragma once
#include "Keyboard.h"

class DrawInfo {	//値の変更は不正値代入を防止するため, 「メソッドを介して」おこなってください。
	int windowSizeX;		//画面の横幅
	int windowSizeY;		//画面の縦幅
	int defaultCellSize;	//等倍時の1マスの1辺の長さ
	int ORow, OCol;			//マス座標における原点が, 画面左上から下にORowマス右にOColマス, のところに位置している。
	double transY, transX;	//平行移動量（平行移動 --> 拡大, の順で実行）(なぜ平行移動が先か：画面中心を注視しながら拡大, 縮小するから）
	double scale;			//拡大率

public:
	DrawInfo(int windowSizeX, int windowSizeY, int defaultCellSize, int ORow, int OCol);
	int toDrawY(double cellY);
	int toDrawX(double cellX);
	int toCellY(int drawY);
	int toCellX(int drawX);
	void update(Keyboard *keyboard);
	int window_width();
	int window_height();
};