//フォントのサイズは（変数に入れておくのが面倒くさいため）, PaintLinesでは20, それ以外では30とします。
#include "DxLib.h"
#include "Mouse.h"
#include "Keyboard.h"
#include "DrawInfo.h"
#include "PaintLines.h"
#include "GenerateGraph.h"
#include "GeneratePolys.h"

const int windowSizeX = 800;
const int windowSizeY = 600;
Mouse mouse;
Keyboard keyboard;
DrawInfo drawInfo(windowSizeX, windowSizeY, 30, 2, 2);
PaintLines paintLines(&keyboard, &mouse, &drawInfo);
GenerateGraph generateGraph(&drawInfo, &keyboard);
GeneratePolys generatePolys(&drawInfo, &keyboard);

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	ChangeWindowMode(TRUE);
	SetBackgroundColor(255, 255, 255);
	SetGraphMode(windowSizeX, windowSizeY, 32);
	DxLib_Init();
	SetDrawScreen(DX_SCREEN_BACK);

	//力技ページ制御
	pair<int, vector<Line>> lines;
	tuple<int, vector<Point>, vector<vector<int>>> graph;
	tuple<int, vector<Poly>, vector<Poly>> polys;

	int call_id = 0;
	while (true) {
		int state;
		
		//呼び出し
		if (call_id == 0) {
			SetFontSize(20);
			lines = paintLines.main();
			state = lines.first;
		}
		else if (call_id == 1) {
			SetFontSize(30);
			graph = generateGraph.main(lines.second);
			state = get<0>(graph);
		}
		else if (call_id == 2) {
			polys = generatePolys.main(get<1>(graph), get<2>(graph));
			state = get<0>(polys);
		}
		else {
			break;
		}

		//ページ番号制御
		if (state == KEY_INPUT_ESCAPE) break;
		if (state == KEY_INPUT_RIGHT) call_id++;
		if (state == KEY_INPUT_LEFT) call_id--;
	}
	DxLib_End();
	return 0;
}