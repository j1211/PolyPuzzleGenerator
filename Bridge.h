//グラフG = <頂点集合, 辺集合>が与えられたとき、橋となる辺以外をすべて返す
#pragma once
#include "stdafx.h"

class Bridge {
	int n;
	vector<vector<int>> edges;
	vector<int> ord;
	vector<int> lowlink;
	vector<bool> visited;
	vector<int> parent;
	int t;

	int dfs(int v);
	bool is_bridge(int a, int b);

public:
	vector<vector<int>> eraseBridge(vector<vector<int>> edges);
};