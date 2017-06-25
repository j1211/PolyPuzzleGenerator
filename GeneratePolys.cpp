//�������d���i�֐�������������j�̂ŁA�g�b�v�_�E���Łi�傫�Ȋ֐��قǃR�[�h�̐擪�ɂȂ�悤�Ɂj�������܂��B
//�g�̌��o�ɂāA�֐ߓ_��T���J�n�_�ɂ���ƃo�O��̂ŁA�֐ߓ_�ȊO���ŏ��ɑI������悤�ɂ��܂��傤�B�֐ߓ_��makeWaku()������ŗ񋓂��Ă����Α��v���Ǝv���܂��B(���͎������܂���)
#include "GeneratePolys.h"
#include "DxLib.h"
#include <stack>
#include "Line.h"

//�R���X�g���N�^
GeneratePolys::GeneratePolys(DrawInfo * drawInfo, Keyboard * keyboard)
{
	this->drawInfo = drawInfo;
	this->keyboard = keyboard;
}

//���C��
tuple<int, vector<Poly>, vector<Poly>> GeneratePolys::main(vector<Point> kotens, vector<vector<int>> edges)
{
	init();
	makeConnectedGraphs(Graph(kotens, edges));
	for (int i = 0; i < graphs.size(); i++) {
		makePolys(graphs[i]);
	}

	//���ʕ\��
	int endKeycode[3] = { KEY_INPUT_ESCAPE, KEY_INPUT_LEFT, KEY_INPUT_RIGHT };		//�I���L�[�̏W��. ���ԓ���ւ���.
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

//�ϐ��̏�����
void GeneratePolys::init()
{
	graphs.clear();
	wakus.clear();
	pieces.clear();
}

//(���̂Ȃ�)�O���t��, �A���ȃO���t�̏W��graphs�ŕ\��
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
		//labelUnion[i]�𒸓_�W��V, V�̎����Ă���ӂ�ӏW��E�Ƃ����O���tG = <V, E>���쐬����B
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

//���_��2�ȏ�̘A���ȃO���tgraph����, �s�[�X�Ƙg���쐬����B
void GeneratePolys::makePolys(Graph & graph)
{
	vector<vector<bool>> usedEdges;
	vector<int> remEdgeNums;
	initMakePolysFlag(graph, usedEdges, remEdgeNums);
	if (!makeWaku(graph, usedEdges, remEdgeNums)) return;
	while (makePiece(graph, usedEdges, remEdgeNums));
}

//�`��
void GeneratePolys::draw(int t)
{
	//�s�[�X
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
	//�g��
	if (t % 90 >= 45) {
		for (int i = 0; i < wakus.size(); i++) {
			for (int j = 0; j < (int)wakus[i].size() - 1; j++) {
				Line(wakus[i][j], wakus[i][j + 1]).draw(drawInfo, GetColor(255, 0, 255));
			}
		}
	}
	//��
	DrawFormatString(drawInfo->window_width() - 200, drawInfo->window_height() - 100, 0, "�g�̌� = %d", wakus.size());
	DrawFormatString(drawInfo->window_width() - 200, drawInfo->window_height() - 70, 0, "�s�[�X�̌� = %d", pieces.size());
}


//makePolys()�֐��Ŏg���ϐ��̏�����
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

//�g[1 + �֐ߓ_�̌�]���쐬
bool GeneratePolys::makeWaku(Graph & graph, vector<vector<bool>>& usedEdges, vector<int>& remEdgeNums)
{
	if (makeWaku(graph, false, usedEdges, remEdgeNums)) return true;
	if (makeWaku(graph, true, usedEdges, remEdgeNums)) return true;
	return false;
}

//�s�[�X1���쐬
bool GeneratePolys::makePiece(Graph & graph, vector<vector<bool>>& usedEdges, vector<int>& remEdgeNums)
{
	if (makePiece(graph, false, usedEdges, remEdgeNums)) return true;
	if (makePiece(graph, true, usedEdges, remEdgeNums)) return true;
	return false;
}

//is_clockwise = true�Ȃ玞�v����̘g���쐬. ������y������̂Ƃ��Ɏ��v�����i�E���j�Ȃ̂ŁA�\����͋t�ɂȂ��Ă��邱�Ƃɒ���.
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

	//cycle���֐ߓ_���o�R���Ȃ�, �܂Ƃ���cycle�ɕ�������
	vector<bool> used(points.size(), false);
	stack<int> stk;
	vector<vector<int>> wakuCycles;

	for (int i = 0; i < cycle.size(); i++) {
		stk.push(cycle[i]);
		if (used[cycle[i]]) {	//�lcycle[i]��stk��2�����Ă���ꍇ
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

	//�g(ring-buffer)���쐬����
	for (int i = 0; i < wakuCycles.size(); i++) {
		Poly waku;
		for (int j = wakuCycles[i].size() - 1; j >= 0; j--) {
			int v = wakuCycles[i][j];
			waku.push_back(points[v]);
		}
		//���܁[��waku��is_clockwise�ƈႤ�����łł��Ă��邱�Ƃ�����̂ŁA���̂Ƃ��͂����ŕ␳����
		if ((area(waku) < 0) != is_clockwise) {
			reverse(waku);
		}
		wakus.push_back(waku);
	}

	//usedEdges, remEdgeNums���X�V
	for (int i = 0; i < (int)cycle.size() - 1; i++) {
		usedEdges[cycle[i]][historyEdgeId[i]] = true;
		remEdgeNums[cycle[i]]--;
	}
	return true;
}

//is_clockwise = true�Ȃ玞�v�����ɂ��ǂ��ăs�[�X���쐬.
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
	if (cycle.size() == points.size()) return false;	//�s�[�X�����ĂȂ��p�^�[��
	cycle.push_back(startPos);

	//�s�[�X�𐶐����܂�
	Poly piece;
	for (int i = 0; i < cycle.size(); i++) {
		piece.push_back(points[cycle[i]]);
	}
	if ((area(piece) < 0) != is_clockwise) {
		reverse(piece);
	}
	pieces.push_back(piece);
	
	//usedEdges, remEdgeNums���X�V
	for (int i = 0; i < (int)cycle.size() - 1; i++) {
		usedEdges[cycle[i]][historyEdgeId[i]] = true;
		remEdgeNums[cycle[i]]--;
	}
	return true;
}

//�T���J�n�_�̔ԍ���Ԃ�. �Ȃ����-1��Ԃ��B
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

//points[pos]���玟�Ɍ������ꏊ���l����B���̂Ƃ��Ɏg���ӂ̔ԍ���Ԃ��B
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

//�T���J�n�_�̔ԍ���Ԃ�. �Ȃ����-1��Ԃ��B
int GeneratePolys::startPiecePointId(vector<int>& remEdgeNums)
{
	for (int i = 0; i < remEdgeNums.size(); i++) {
		if (remEdgeNums[i] > 0) return i;
	}
	return -1;
}

//�܂��g���Ă��Ȃ��ӂ̔ԍ��̂���, �ŏ��̂��̂�Ԃ�
int GeneratePolys::startPieceEdgeId(vector<bool>& usedEdges)
{
	for (int i = 0; i < usedEdges.size(); i++) {
		if (!usedEdges[i]) return i;
	}
	return -1;
}

//���ɉ��Ԃ̕ӂ��g������Ԃ��B�Ȃ����-1��Ԃ��B
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

		if (is_clockwise) {	//angle�ő�����
			if (first_loop || tmp < angle) {
				tmp = angle;
				ret = i;
				first_loop = false;
			}
		}
		else {	//angle�ŏ������
			if (first_loop || tmp > angle) {
				tmp = angle;
				ret = i;
				first_loop = false;
			}
		}
	}
	return ret;
}

//�����t���ʐ�, �����v���Ȃ琳�̒l�ɂȂ�炵��
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

//���_��̌������t�ɂ���
void GeneratePolys::reverse(Poly & poly)
{
	int l = 0, r = poly.size() - 1;
	while (l < r) {
		swap(poly[l], poly[r]);
		l++;
		r--;
	}
}
