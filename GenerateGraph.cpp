#include "GenerateGraph.h"
#include "Bridge.h"

void GenerateGraph::drawLines(vector<Line>& lines)
{
	for (int i = 0; i < lines.size(); i++) {
		lines[i].draw(drawInfo);
	}
}

void GenerateGraph::drawGraph(vector<Point>& kotens, vector<vector<int>>& edges)
{
	for (int i = 0; i < kotens.size(); i++) {
		int x = drawInfo->toDrawX(kotens[i].real());
		int y = drawInfo->toDrawY(kotens[i].imag());
		DrawCircle(x, y, 3, GetColor(255, 0, 0), TRUE);
		DrawFormatString(x + 5, y - 18, 0, "%d", i);
	}
	for (int i = 0; i < edges.size(); i++) {
		for (int j = 0; j < edges[i].size(); j++) {
			int from = i;
			int to = edges[i][j];
			Line(kotens[from], kotens[to]).draw(drawInfo, GetColor(0, 255, 0), 2);
		}
	}
}

//コンストラクタ
GenerateGraph::GenerateGraph(DrawInfo * drawInfo, Keyboard * keyboard)
{
	this->drawInfo = drawInfo;
	this->keyboard = keyboard;
}

//線分集合から, 交点を検出し, 隣り合う交点を双方向に結んだグラフG = <V, E>を生成する。
//無駄な辺はカットする。次数が0の頂点（交点）ができる場合があるので注意。
//前提：線分の始点, 終点, 交点は整数座標とする。平行な2線分が交わることもないとする。
tuple<int, vector<Point>, vector<vector<int>>> GenerateGraph::main(vector<Line> lines)
{
	vector<Point> kotens;

	for (int i = 0; i < lines.size(); i++) {
		for (int j = i + 1; j < lines.size(); j++) {
			if (!lines[i].ishit(lines[j])) continue;
			kotens.push_back(lines[i].cross_point(lines[j]));
		}
	}
	sort(kotens.begin(), kotens.end());
	kotens.erase(unique(kotens.begin(), kotens.end()), kotens.end());

	vector<vector<int>> edges;
	edges.resize(kotens.size());

	for (int i = 0; i < lines.size(); i++) {
		vector<pair<Point, int>> points;
		for (int j = 0; j < kotens.size(); j++) {
			if (lines[i].on_line(kotens[j])) {
				points.push_back(pair<Point, int>(kotens[j], j));
			}
		}
		sort(points.begin(), points.end());
		for (int j = 0; j + 1 < points.size(); j++) {
			edges[points[j].second].push_back(points[j + 1].second);
			edges[points[j + 1].second].push_back(points[j].second);
		}
	}

	//橋削除
	Bridge bridge;
	vector<vector<int>> newEdges = bridge.eraseBridge(edges);

	//結果表示
	int endKeycode[3] = { KEY_INPUT_ESCAPE, KEY_INPUT_LEFT, KEY_INPUT_RIGHT };		//終了キーの集合. 順番入れ替え可.
	
	while (ScreenFlip() == 0 && ProcessMessage() == 0 && ClearDrawScreen() == 0) {
		keyboard->update();
		drawInfo->update(keyboard);
	
		for (int i = 0; i < 3; i++) {
			if (keyboard->is_click(endKeycode[i])) {
				return tuple<int, vector<Point>, vector<vector<int>>>(endKeycode[i], kotens, newEdges);
			}
		}

		drawLines(lines);
		drawGraph(kotens, newEdges);
	}
	return tuple<int, vector<Point>, vector<vector<int>>>(endKeycode[0], kotens, newEdges);
}
