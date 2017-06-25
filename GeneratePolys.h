//�s�[�X��g�����B
#pragma once
#include "Point.h"
#include "stdafx.h"
#include "DrawInfo.h"
#include "Keyboard.h"

typedef pair<vector<Point>, vector<vector<int>>> Graph;
typedef vector<Point> Poly;

class GeneratePolys
{
	vector<Graph> graphs;	//���_����2�ȏ�̘A���ȃO���t�i�������݂��Ȃ�, �֐ߓ_�͂��邩���j�̏W��
	vector<Poly> wakus;
	vector<Poly> pieces;
	DrawInfo *drawInfo;
	Keyboard *keyboard;

public:
	GeneratePolys(DrawInfo *drawInfo, Keyboard *keyboard);
	tuple<int, vector<Poly>, vector<Poly>> main(vector<Point> kotens, vector<vector<int>> edges);

private:
	void init();
	void makeConnectedGraphs(Graph &graph);
	void makePolys(Graph &graph);
	void draw(int t);

	//�g, �s�[�X�̐���. �G���[����false��Ԃ�.
	void initMakePolysFlag(Graph &graph, vector<vector<bool>> &usedEdges, vector<int> &remEdgeNums);
	bool makeWaku(Graph &graph, vector<vector<bool>> &usedEdges, vector<int> &remEdgeNums);
	bool makePiece(Graph &graph, vector<vector<bool>> &usedEdges, vector<int> &remEdgeNums);
	bool makeWaku(Graph &graph, bool is_clockwise, vector<vector<bool>> &usedEdges, vector<int> &remEdgeNums);
	bool makePiece(Graph &graph, bool is_clockwise, vector<vector<bool>> &usedEdges, vector<int> &remEdgeNums);

	//pos = point_id, �G���[����-1��Ԃ��B
	int startWakuPointId(vector<Point> &points);
	int startWakuEdgeId(int pos, vector<Point> &points, vector<int> &edges);
	int startPiecePointId(vector<int> &remEdgeNums);
	int startPieceEdgeId(vector<bool> &usedEdges);
	int nextEdgeId(Graph &graph, Point lastMove, int pos, vector<bool> &usedEdges, bool is_clockwise);
	double area(Poly &poly);
	void reverse(Poly &poly);
};