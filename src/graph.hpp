#ifndef GRAPH_HPP
#define GRAPH_HPP

#include "parser.hpp"
#include <vector>
#include <string>

struct Edge {
    int u, v;
    double weight;
    bool operator<(const Edge& other) const {
        return weight < other.weight;
    }
};

class UnionFind {
public:
    explicit UnionFind(int n);
    int find(int x);
    bool unite(int x, int y);
private:
    std::vector<int> parent;
    std::vector<int> rank;
};

class Graph {
public:
    static const int N = 20;
    explicit Graph(const std::vector<Solicitud>& solicitudes);
    int numNodes() const { return N; }
    int numEdges() const { return (int)edges.size(); }
    const std::vector<Edge>& getEdges() const { return edges; }
    const std::vector<double>& getAvg() const { return avg; }
private:
    std::vector<double> avg;
    std::vector<Edge> edges;
    void buildAvg(const std::vector<Solicitud>& solicitudes);
    void buildEdges();
};

#endif
