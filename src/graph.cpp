#include "graph.hpp"

#include <cmath>
#include <stdexcept>
#include <string>

using namespace std;

UnionFind::UnionFind(int n) : parent(n), rank(n, 0) {
    for (int i = 0; i < n; i++)
        parent[i] = i;
}

int UnionFind::find(int x) {
    if (parent[x] != x)
        parent[x] = find(parent[x]);
    return parent[x];
}

bool UnionFind::unite(int x, int y) {
    int rx = find(x);
    int ry = find(y);
    if (rx == ry) return false;
    if (rank[rx] < rank[ry])      parent[rx] = ry;
    else if (rank[rx] > rank[ry]) parent[ry] = rx;
    else { parent[ry] = rx; rank[rx]++; }
    return true;
}

Graph::Graph(const vector<Solicitud>& solicitudes) {
    buildAvg(solicitudes);
    buildEdges();
}

void Graph::buildAvg(const vector<Solicitud>& solicitudes) {
    avg.assign(N, 0.0);
    vector<double> suma(N, 0.0);
    vector<int> cuenta(N, 0);

    for (int i = 0; i < (int)solicitudes.size(); i++) {
        int g = i % N;
        suma[g] += solicitudes[i].monthlyCharges;
        cuenta[g]++;
    }

    for (int k = 0; k < N; k++) {
        if (cuenta[k] == 0)
            throw runtime_error("grupo " + to_string(k) + " sin registros");
        avg[k] = round(suma[k] / cuenta[k] * 100.0) / 100.0;
    }
}

void Graph::buildEdges() {
    edges.clear();
    edges.reserve(N * (N - 1) / 2);
    for (int u = 0; u < N; u++)
        for (int v = u + 1; v < N; v++)
            edges.push_back({u, v, floor(avg[u] + avg[v])});
}
