//実装が重い（関数をたくさん作る）ので、トップダウンで（大きな関数ほどコードの先頭になるように）実装します。
//枠の検出にて、関節点を探索開始点にするとバグるので、関節点以外を最初に選択するようにしましょう。関節点はmakeWaku()あたりで列挙しておけば大丈夫だと思います。(今は実装しません)
//最終的に, 枠穴は時計回り, ピースは反時計回りで取ります。
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

	vector<int> soutaiId(n);
	for (int i = 0; i < labelUnion.size(); i++) {
		for (int j = 0; j < labelUnion[i].size(); j++) {
			soutaiId[labelUnion[i][j]] = j;
		}
	}

	for (int i = 0; i < labelUnion.size(); i++) {
		if (labelUnion[i].size() <= 1) continue;
		//labelUnion[i]を頂点集合V, Vの持っている辺を辺集合EとしたグラフG = <V, E>を作成する。
		//頂点の番号に注意！！辺の行先頂点番号はgraph全体の番号ではなく、graphs[i]についての番号にすること！
		vector<Point> points;
		vector<vector<int>> edges;
		for (int j = 0; j < labelUnion[i].size(); j++) {
			int v = labelUnion[i][j];
			points.push_back(graph.first[v]);
			
			vector<int> edgeV;
			for (int k = 0; k < graph.second[v].size(); k++) {
				edgeV.push_back(soutaiId[graph.second[v][k]]);
			}
			edges.push_back(edgeV);
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
		Point pos = getDrawPosition(pieces[i]);
		DrawFormatString(pos.real(), pos.imag(), GetColor(0, 0, 255), "%d", i);
		for (int j = 0; j < pieces[i].size() - 1; j++) {
			Line(pieces[i][j], pieces[i][j + 1]).draw(drawInfo, GetColor(0, 255, 0));
		}
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
	DrawFormatString(drawInfo->window_width() - 300, drawInfo->window_height() - 110, GetColor(255, 0, 255), "枠の個数 = %d", wakus.size());
	DrawFormatString(drawInfo->window_width() - 300, drawInfo->window_height() - 70, GetColor(0, 255, 0), "ピースの個数 = %d", pieces.size());
}

//多角形(始点 = 終点)を与えると, 文字を表示する場所を返す.
Point GeneratePolys::getDrawPosition(Poly & poly)
{
	int n = poly.size() - 1;
	Point base = Point(0, 0);
	for (int i = 0; i < n; i++) {
		base += poly[i];
	}
	base /= n;

	Point pos;
	if (contain(poly, base) == in) {
		pos = base;
	}
	else {
		pos = poly[1] - poly[0]; pos /= abs(pos);
		if (area(poly) > 0) {
			pos *= Point(0, 1);
		}
		else {
			pos *= Point(0, -1);
		}

		//倍率
		double dist = 11451419;
		Line l = Line((poly[0] + poly[1]) * 0.5, (poly[0] + poly[1]) * 0.5 + pos * dist); 
		for (int i = 1; i < n; i++) {
			if (l.ishit(Line(poly[i], poly[i + 1]))) {
				dist = min(dist, Line(poly[i], poly[i + 1]).dist((poly[0] + poly[1]) * 0.5));
			}
		}
		pos *= dist / 2;

		pos += (poly[0] + poly[1]) * 0.5;
	}
	return Point((double)drawInfo->toDrawX(pos.real()) - 8, (double)drawInfo->toDrawY(pos.imag()) - 15);
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

//is_clockwise = trueなら時計周りっぽく見ながら枠を作成. （時計回りの枠ができるとは限らないけど）
bool GeneratePolys::makeWaku(Graph & graph, bool is_clockwise, vector<vector<bool>> &usedEdges, vector<int> &remEdgeNums)
{
	vector<Point> &points = graph.first;
	vector<vector<int>> &edges = graph.second;
	vector<int> cycle;

	int startPos = startPointId(points, remEdgeNums); if (startPos == -1) return false;
	int eid = startEdgeId(startPos, points, edges[startPos], usedEdges[startPos]); if (eid == -1) return false;
	int pos = edges[startPos][eid];
	Point lastMove = points[pos] - points[startPos];

	cycle.push_back(startPos);
	while (pos != startPos) {
		cycle.push_back(pos);
		eid = nextEdgeId(graph, lastMove, pos, usedEdges[pos], is_clockwise);
		if (eid < 0) return false;
		int nextPos = edges[pos][eid];
		lastMove = points[nextPos] - points[pos];
		pos = nextPos;
	}
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
	vector<Poly> createdPolys;
	for (int i = 0; i < wakuCycles.size(); i++) {
		Poly waku;
		for (int j = wakuCycles[i].size() - 1; j >= 0; j--) {
			int v = wakuCycles[i][j];
			waku.push_back(points[v]);
		}
		createdPolys.push_back(waku);
	}

	//エラー処理(どの枠穴にも入っていない点があったら矛盾)
	for (int i = 0; i < used.size(); i++) {
		if (used[i]) continue;
		int j;
		for (j = 0; j < createdPolys.size(); j++) {
			if (contain(createdPolys[j], points[i]) != out) break;
		}
		if (j == createdPolys.size()) {
			return false;
		}
	}

	//枠穴更新
	bool revCycleFlag = false;
	for (int i = 0; i < createdPolys.size(); i++) {
		if (area(createdPolys[i]) > 0) {
			reverse(createdPolys);
			revCycleFlag = true;
		}
		wakus.push_back(createdPolys[i]);
	}

	if (revCycleFlag) { reverse(cycle); }

	//usedEdges, remEdgeNumsを更新
	for (int i = 0; i < (int)cycle.size() - 1; i++) {
		int id;
		for (id = 0; id < edges[cycle[i]].size(); id++) {
			if (edges[cycle[i]][id] == cycle[i + 1]) break;
		}
		usedEdges[cycle[i]][id] = true;
		remEdgeNums[cycle[i]]--;
	}
	return true;
}

//is_clockwise = trueなら時計方向にたどってピースを作成. 時計回りのピースができるとは限らない。
bool GeneratePolys::makePiece(Graph & graph, bool is_clockwise, vector<vector<bool>> &usedEdges, vector<int> &remEdgeNums)
{
	vector<Point> &points = graph.first;
	vector<vector<int>> &edges = graph.second;
	vector<int> cycle;

	int startPos = startPointId(points, remEdgeNums); if (startPos == -1) return false;
	int eid = startEdgeId(startPos, points, edges[startPos], usedEdges[startPos]); if (eid == -1) return false;
	int pos = edges[startPos][eid];
	Point lastMove = points[pos] - points[startPos];

	cycle.push_back(startPos);
	while (pos != startPos) {
		cycle.push_back(pos);
		eid = nextEdgeId(graph, lastMove, pos, usedEdges[pos], is_clockwise);
		if (eid < 0) return false;
		int nextPos = edges[pos][eid];
		lastMove = points[nextPos] - points[pos];
		pos = nextPos;
	}
	cycle.push_back(startPos);

	//ピースを生成します
	Poly piece;
	for (int i = 0; i < cycle.size(); i++) {
		piece.push_back(points[cycle[i]]);
	}
	
	//反時計回りにします
	bool revCycleFlag = false;
	if (area(piece) < 0) {
		reverse(piece);
		revCycleFlag = true;
	}
	if (revCycleFlag) { reverse(cycle); }

	//エラー処理（{サイクル} - {終点}に同じ頂点が2つ以上含まれていたら矛盾。）->それ以外なら、単純多角形が得られる。
	//すでに全く同じピースが存在したら矛盾
	//（サイクルの頂点集合をKとおく。K->K上の未使用辺であって、pieceの内部にあるものが存在したら矛盾。pieceの内部に点があれば矛盾）
	vector<bool> used(points.size(), false);
	for (int i = 0; i < cycle.size() - 1; i++) {
		if (used[cycle[i]]) return false;
		used[cycle[i]] = true;
	}
	for (int i = 0; i < pieces.size(); i++) {
		int j;
		for (j = 0; j < pieces[i].size(); j++) {
			if (pieces[i][j] != piece[j]) break;
		}
		if (j == pieces[i].size()) return false;
	} 
	for (int i = 0; i < cycle.size() - 1; i++) {
		int u = cycle[i];
		for (int j = 0; j < edges[u].size(); j++) {
			int v = edges[u][j];
			if (usedEdges[u][j] || !used[v]) continue;
			if (contain(piece, (points[u] + points[v]) * 0.5) == in) return false;
		}
	}
	for (int i = 0; i < points.size(); i++) {
		if (contain(piece, points[i]) == in) return false;
	}

	//ピース集合の更新
	pieces.push_back(piece);

	//usedEdges, remEdgeNumsを更新
	for (int i = 0; i < (int)cycle.size() - 1; i++) {
		int id;
		for (id = 0; id < edges[cycle[i]].size(); id++) {
			if (edges[cycle[i]][id] == cycle[i + 1]) break;
		}
		usedEdges[cycle[i]][id] = true;
		remEdgeNums[cycle[i]]--;
	}
	return true;
}

//探索開始点（外側にある点）を返す. なければ-1を返す.
int GeneratePolys::startPointId(vector<Point>& points, vector<int>& remEdgeNums)
{
	double minX = 1145141919;
	int ret = -1;
	for (int i = 0; i < points.size(); i++) {
		if (remEdgeNums[i] == 0) {
			continue;
		}
		if (minX > points[i].real()) {
			minX = points[i].real();
			ret = i;
		}
	}
	return ret;
}

//points[pos]から次に向かう場所を考える。そのときに使う辺の番号を返す。
int GeneratePolys::startEdgeId(int pos, vector<Point>& points, vector<int>& edges, vector<bool>& usedEdges)
{
	double minDot = 1145141919;
	int ret = -1;

	for (int i = 0; i < edges.size(); i++) {
		if (usedEdges[i]) {
			continue;
		}
		Point vec = points[edges[i]] - points[pos]; vec /= abs(vec);
		if (minDot > dot(vec, Point(1, 0))) {
			minDot = dot(vec, Point(1, 0));
			ret = i;
		}
	}
	return ret;
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
template<class T> void GeneratePolys::reverse(T & value)
{
	int l = 0, r = value.size() - 1;
	while (l < r) {
		swap(value[l], value[r]);
		l++;
		r--;
	}
}

//点pointが多角形polyの内部ならin, 外周ならon, 外側ならoutを返す
CONTAIN GeneratePolys::contain(Poly & poly, Point & point)
{
	bool isin = false;
	for (int i = 0; i < poly.size() - 1; i++) {
		Point a = poly[i] - point;
		Point b = poly[i + 1] - point;
		if (imag(a) > imag(b)) swap(a, b);
		if (imag(a) <= 0 && 0 < imag(b))
			if (cross(a, b) > 0) isin = !isin;
		if (cross(a, b) == 0 && dot(a, b) <= 0) return on;
	}
	return isin ? in : out;
}