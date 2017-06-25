//橋の検出　（辺(u, v)を取り除くとグラフの連結成分の数が増えるとき, 辺(u, v)を「橋」とよぶ）
#include "Bridge.h"
#include "DxLib.h"

//lowlink(v)を返す.
int Bridge::dfs(int v)
{
	ord[v] = t;
	lowlink[v] = t;
	t++;
	visited[v] = true;

	for (int i = 0; i < edges[v].size(); i++) {
		if (visited[edges[v][i]]) {
			if (parent[v] != edges[v][i]) lowlink[v] = min(lowlink[v], ord[edges[v][i]]);
		}
		else {
			parent[edges[v][i]] = v;
			int llink = dfs(edges[v][i]);
			lowlink[v] = min(lowlink[v], llink);
		}
	}
	return lowlink[v];
}

bool Bridge::is_bridge(int a, int b)
{
	int u, v;

	if (parent[a] == b) {
		u = b;
		v = a;
	}
	else if (parent[b] == a) {
		u = a;
		v = b;
	}
	else {
		return false;
	}

	return (ord[u] < lowlink[v]);
}

vector<vector<int>> Bridge::eraseBridge(vector<vector<int>> edges)
{
	n = edges.size();
	this->edges = edges;
	ord.resize(n);
	lowlink.resize(n);
	visited.resize(n); for (int i = 0; i < n; i++) visited[i] = false;
	parent.resize(n);
	this->t = 0;

	for (int v = 0; v < n; v++) {
		if (visited[v]) continue;
		parent[v] = v;
		dfs(v);
	}

	vector<vector<int>> newEdges;
	newEdges.resize(n);

	for (int i = 0; i < n; i++) {
		for (int j = 0; j < edges[i].size(); j++) {
			if (!is_bridge(i, edges[i][j])) {
				newEdges[i].push_back(edges[i][j]);
			}
		}
	}
	return newEdges;
}
