//実装が重い（関数をたくさん作る）ので、トップダウンで（大きな関数ほどコードの先頭になるように）実装します。
//枠の検出にて、関節点を探索開始点にするとバグるので、関節点以外を最初に選択するようにしましょう。関節点はmakeWaku()あたりで列挙しておけば大丈夫だと思います。(今は実装しません)
#include "GeneratePolys.h"
#include "DxLib.h"
#include <stack>
#include "Line.h"

//コンストラクタ
GeneratePolys::GeneratePolys(DrawInfo * drawInfo, Keyboard * keyboard)
{
	this->drawInfo = drawInfo;
	this->keyboard = keyboard;
}

//メイン
tuple<int, vector<Poly>, vector<Poly>> GeneratePolys::main(vector<Point> kotens, vector<vector<int>> edges)
{
	init();
	makeConnectedGraphs(Graph(kotens, edges));
	for (int i = 0; i < graphs.size(); i++) {
		makePolys(graphs[i]);
	}

	//結果表示
	int endKeycode[3] = { KEY_INPUT_ESCAPE, KEY_INPUT_LEFT, KEY_INPUT_RIGHT };		//終了キーの集合. 順番入れ替え可.
	int t = 0;

	while (ScreenFlip() == 0 && ProcessMessage() == 0 && ClearDrawScreen() == 0) {
		keyboard->update();
		drawInfo->update(keyboard);

		for (int i = 0; i < 3; i++) {
			if (keyboard->is_click(endKeycode[i])) {
				return tuple<int, vector<Poly>, vector<Poly>>(endKeycode[i], wakus, pieces);
			}
		}
		draw(t);
		t++;
	}

	return tuple<int, vector<Poly>, vector<Poly>>(endKeycode[0], wakus, pieces);
}

//変数の初期化
void GeneratePolys::init()
{
	graphs.clear();
	wakus.clear();
	pieces.clear();
}

//(橋のない)グラフを, 連結なグラフの集合graphsで表す
void GeneratePolys::makeConnectedGraphs(Graph & graph)
{
	int n = graph.first.size();
	vector<int> label(n, -1);
	queue<int> que;
	int id = 0;

	for (int i = 0; i < n; i++) {
		if (label[i] != -1) continue;
		que.push(i);
		label[i] = id;
		while (!que.empty()) {
			int v = que.front(); que.pop();
			for (int j = 0; j < graph.second[v].size(); j++) {
				if (label[graph.second[v][j]] == -1) {
					que.push(graph.second[v][j]);
					label[graph.second[v][j]] = id;
				}
			}
		}
		id++;
	}

	vector<vector<int>> labelUnion;
	labelUnion.resize(id);
	for (int i = 0; i < n; i++) {
		if (label[i] == -1) continue;
		labelUnion[label[i]].push_back(i);
	}

	for (int i = 0; i < labelUnion.size(); i++) {
		if (labelUnion[i].size() <= 1) continue;
		//labelUnion[i]を頂点集合V, Vの持っている辺を辺集合EとしたグラフG = <V, E>を作成する。
		vector<Point> points;
		vector<vector<int>> edges;
		for (int j = 0; j < labelUnion[i].size(); j++) {
			int v = labelUnion[i][j];
			points.push_back(graph.first[v]);
			edges.push_back(graph.second[v]);
		}
		graphs.push_back(Graph(points, edges));
	}
}

//頂点数2以上の連結なグラフgraphから, ピースと枠を作成する。
void GeneratePolys::makePolys(Graph & graph)
{
	vector<vector<bool>> usedEdges;
	vector<int> remEdgeNums;
	initMakePolysFlag(graph, usedEdges, remEdgeNums);
	if (!makeWaku(graph, usedEdges, remEdgeNums)) return;
	while (makePiece(graph, usedEdges, remEdgeNums));
}

//描画
void GeneratePolys::draw(int t)
{
	//ピース
	for (int i = 0; i < pieces.size(); i++) {
		double baseX = 0, baseY = 0;
		for (int j = 0; j < (int)pieces[i].size() - 1; j++) {
			baseX += pieces[i][j].real();
			baseY += pieces[i][j].imag();
			Line(pieces[i][j], pieces[i][j + 1]).draw(drawInfo, GetColor(0, 255, 0));
		}
		baseX /= (int)pieces[i].size() - 1;
		baseY /= (int)pieces[i].size() - 1;
		DrawFormatString(drawInfo->toDrawX(baseX), drawInfo->toDrawY(baseY), GetColor(0, 0, 255), "%d", i);
	}
	//枠穴
	if (t % 90 >= 45) {
		for (int i = 0; i < wakus.size(); i++) {
			for (int j = 0; j < (int)wakus[i].size() - 1; j++) {
				Line(wakus[i][j], wakus[i][j + 1]).draw(drawInfo, GetColor(255, 0, 255));
			}
		}
	}
	//個数
	DrawFormatString(drawInfo->window_width() - 200, drawInfo->window_height() - 100, 0, "枠の個数 = %d", wakus.size());
	DrawFormatString(drawInfo->window_width() - 200, drawInfo->window_height() - 70, 0, "ピースの個数 = %d", pieces.size());
}


//makePolys()関数で使う変数の初期化
void GeneratePolys::initMakePolysFlag(Graph &graph, vector<vector<bool>>& usedEdges, vector<int>& remEdgeNums)
{
	int n = graph.first.size();
	usedEdges.resize(n);
	remEdgeNums.resize(n);
	for (int i = 0; i < n; i++) {
		usedEdges[i] = vector<bool>(graph.second[i].size(), false);
		remEdgeNums[i] = graph.second[i].size();
	}
}

//枠[1 + 関節点の個数]個を作成
bool GeneratePolys::makeWaku(Graph & graph, vector<vector<bool>>& usedEdges, vector<int>& remEdgeNums)
{
	if (makeWaku(graph, false, usedEdges, remEdgeNums)) return true;
	if (makeWaku(graph, true, usedEdges, remEdgeNums)) return true;
	return false;
}

//ピース1個を作成
bool GeneratePolys::makePiece(Graph & graph, vector<vector<bool>>& usedEdges, vector<int>& remEdgeNums)
{
	if (makePiece(graph, false, usedEdges, remEdgeNums)) return true;
	if (makePiece(graph, true, usedEdges, remEdgeNums)) return true;
	return false;
}

//is_clockwise = trueなら時計周りの枠を作成. ただしy軸が上のときに時計方向（右回り）なので、表示上は逆になっていることに注意.
bool GeneratePolys::makeWaku(Graph & graph, bool is_clockwise, vector<vector<bool>> &usedEdges, vector<int> &remEdgeNums)
{
	vector<Point> &points = graph.first;
	vector<vector<int>> &edges = graph.second;
	vector<int> cycle;
	vector<int> historyEdgeId;

	int startPos = startWakuPointId(points); if (startPos == -1) return false;
	int eid = startWakuEdgeId(startPos, points, edges[startPos]); if (eid == -1) return false;
	int pos = edges[startPos][eid];
	Point lastMove = points[pos] - points[startPos];

	cycle.push_back(startPos);
	historyEdgeId.push_back(eid);
	while (pos != startPos) {
		cycle.push_back(pos);
		eid = nextEdgeId(graph, lastMove, pos, usedEdges[pos], is_clockwise);
		if (eid < 0) return false;
		historyEdgeId.push_back(eid);
		int nextPos = edges[pos][eid];
		lastMove = points[nextPos] - points[pos];
		pos = nextPos;
	}
	if (cycle.size() != points.size()) return false;
	cycle.push_back(startPos);

	//cycleを関節点を経由しない, まともなcycleに分解する
	vector<bool> used(points.size(), false);
	stack<int> stk;
	vector<vector<int>> wakuCycles;

	for (int i = 0; i < cycle.size(); i++) {
		stk.push(cycle[i]);
		if (used[cycle[i]]) {	//値cycle[i]がstkに2個入っている場合
			vector<int> wakuCycle;
			wakuCycle.push_back(stk.top()); stk.pop();
			while (stk.top() != cycle[i]) {
				wakuCycle.push_back(stk.top()); stk.pop();
			}
			wakuCycle.push_back(stk.top());
			wakuCycles.push_back(wakuCycle);
		}
		used[cycle[i]] = true;
	}

	//枠(ring-buffer)を作成する
	for (int i = 0; i < wakuCycles.size(); i++) {
		Poly waku;
		for (int j = wakuCycles[i].size() - 1; j >= 0; j--) {
			int v = wakuCycles[i][j];
			waku.push_back(points[v]);
		}
		//たまーにwakuがis_clockwiseと違う向きでできていることがあるので、そのときはここで補正する
		if ((area(waku) < 0) != is_clockwise) {
			reverse(waku);
		}
		wakus.push_back(waku);
	}

	//usedEdges, remEdgeNumsを更新
	for (int i = 0; i < (int)cycle.size() - 1; i++) {
		usedEdges[cycle[i]][historyEdgeId[i]] = true;
		remEdgeNums[cycle[i]]--;
	}
	return true;
}

//is_clockwise = trueなら時計方向にたどってピースを作成.
bool GeneratePolys::makePiece(Graph & graph, bool is_clockwise, vector<vector<bool>> &usedEdges, vector<int> &remEdgeNums)
{
	vector<Point> &points = graph.first;
	vector<vector<int>> &edges = graph.second;
	vector<int> cycle;
	vector<int> historyEdgeId;

	int startPos = startPiecePointId(remEdgeNums); if (startPos == -1) return false;
	int eid = startPieceEdgeId(usedEdges[startPos]); if (eid == -1) return false;
	int pos = edges[startPos][eid];
	Point lastMove = points[pos] - points[startPos];

	cycle.push_back(startPos);
	historyEdgeId.push_back(eid);
	while (pos != startPos) {
		cycle.push_back(pos);
		eid = nextEdgeId(graph, lastMove, pos, usedEdges[pos], is_clockwise);
		if (eid < 0) return false;
		historyEdgeId.push_back(eid);
		int nextPos = edges[pos][eid];
		lastMove = points[nextPos] - points[pos];
		pos = nextPos;
	}
	if (cycle.size() == points.size()) return false;	//ピースが取れてないパターン
	cycle.push_back(startPos);

	//ピースを生成します
	Poly piece;
	for (int i = 0; i < cycle.size(); i++) {
		piece.push_back(points[cycle[i]]);
	}
	if ((area(piece) < 0) != is_clockwise) {
		reverse(piece);
	}
	pieces.push_back(piece);
	
	//usedEdges, remEdgeNumsを更新
	for (int i = 0; i < (int)cycle.size() - 1; i++) {
		usedEdges[cycle[i]][historyEdgeId[i]] = true;
		remEdgeNums[cycle[i]]--;
	}
	return true;
}

//探索開始点の番号を返す. なければ-1を返す。
int GeneratePolys::startWakuPointId(vector<Point>& points)
{
	double minX = points[0].real();
	int ret = 0;
	for (int i = 1; i < points.size(); i++) {
		if (minX > points[i].real()) {
			minX = points[i].real();
			ret = i;
		}
	}
	return ret;
}

//points[pos]から次に向かう場所を考える。そのときに使う辺の番号を返す。
int GeneratePolys::startWakuEdgeId(int pos, vector<Point>& points, vector<int>& edges)
{
	double minDot = 1145141919;
	int ret = -1;
	
	for (int i = 0; i < edges.size(); i++) {
		Point vec = points[edges[i]] - points[pos]; vec /= abs(vec);
		if (minDot > dot(vec, Point(1, 0))) {
			minDot = dot(vec, Point(1, 0));
			ret = i;
		}
	}
	return ret;
}

//探索開始点の番号を返す. なければ-1を返す。
int GeneratePolys::startPiecePointId(vector<int>& remEdgeNums)
{
	for (int i = 0; i < remEdgeNums.size(); i++) {
		if (remEdgeNums[i] > 0) return i;
	}
	return -1;
}

//まだ使っていない辺の番号のうち, 最小のものを返す
int GeneratePolys::startPieceEdgeId(vector<bool>& usedEdges)
{
	for (int i = 0; i < usedEdges.size(); i++) {
		if (!usedEdges[i]) return i;
	}
	return -1;
}

//次に何番の辺を使うかを返す。なければ-1を返す。
int GeneratePolys::nextEdgeId(Graph & graph, Point lastMove, int pos, vector<bool> &usedEdges, bool is_clockwise)
{
	const double PAI = 3.14159265358979;
	double tmp;
	bool first_loop = true;
	int ret = -1;

	lastMove *= -1;

	for (int i = 0; i < usedEdges.size(); i++) {
		if (usedEdges[i]) continue;

		Point vec = graph.first[graph.second[pos][i]] - graph.first[pos];
		if (vec == lastMove) continue;
		vec /= lastMove;
		double angle = arg(vec);
		if (angle < 0) angle += 2 * PAI;

		if (is_clockwise) {	//angle最大を取る
			if (first_loop || tmp < angle) {
				tmp = angle;
				ret = i;
				first_loop = false;
			}
		}
		else {	//angle最小を取る
			if (first_loop || tmp > angle) {
				tmp = angle;
				ret = i;
				first_loop = false;
			}
		}
	}
	return ret;
}

//符号付き面積, 反時計回りなら正の値になるらしい
double GeneratePolys::area(Poly & poly)
{
	int n = poly.size() - 1;
	double ret = 0;
	for (int i = 0; i < n; i++) {
		ret += cross(poly[i], poly[i + 1]);
	}
	ret /= 2;
	return ret;
}

//頂点列の向きを逆にする
void GeneratePolys::reverse(Poly & poly)
{
	int l = 0, r = poly.size() - 1;
	while (l < r) {
		swap(poly[l], poly[r]);
		l++;
		r--;
	}
}
