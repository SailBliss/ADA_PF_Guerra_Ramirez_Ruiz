#ifndef KRUSKAL_HPP
#define KRUSKAL_HPP

#include "graph.hpp"
#include <string>
#include <vector>

struct MSTResult {
    std::vector<Edge> mstEdges;
    double totalWeight;
};

MSTResult kruskal(const Graph& graph);
MSTResult kruskalSubgraph5(const Graph& graph);
void escribirMST(const std::string& path, const MSTResult& full,
                 const MSTResult& sub5, const Graph& graph);

#endif
