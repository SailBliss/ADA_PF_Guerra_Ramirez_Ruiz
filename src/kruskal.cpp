#include "kruskal.hpp"

#ifdef _WIN32
#include <direct.h>
#define MKDIR(p) _mkdir(p)
#else
#include <sys/stat.h>
#define MKDIR(p) mkdir(p, 0755)
#endif

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>

using namespace std;

MSTResult kruskal(const Graph& graph) {
    vector<Edge> sorted = graph.getEdges();
    sort(sorted.begin(), sorted.end());

    UnionFind uf(graph.numNodes());
    MSTResult result;
    result.totalWeight = 0.0;
    result.mstEdges.reserve(graph.numNodes() - 1);

    for (const Edge& e : sorted) {
        if (uf.unite(e.u, e.v)) {
            result.mstEdges.push_back(e);
            result.totalWeight += e.weight;
            if ((int)result.mstEdges.size() == graph.numNodes() - 1)
                break;
        }
    }

    if ((int)result.mstEdges.size() != graph.numNodes() - 1)
        throw runtime_error("grafo no conexo; MST incompleto");

    return result;
}

MSTResult kruskalSubgraph5(const Graph& graph) {
    const int SUB = 5;
    vector<Edge> sub;
    for (const Edge& e : graph.getEdges())
        if (e.u < SUB && e.v < SUB)
            sub.push_back(e);

    sort(sub.begin(), sub.end());

    UnionFind uf(SUB);
    MSTResult result;
    result.totalWeight = 0.0;
    result.mstEdges.reserve(SUB - 1);

    for (const Edge& e : sub) {
        if (uf.unite(e.u, e.v)) {
            result.mstEdges.push_back(e);
            result.totalWeight += e.weight;
            if ((int)result.mstEdges.size() == SUB - 1)
                break;
        }
    }

    return result;
}

void escribirMST(const string& path, const MSTResult& full,
                 const MSTResult& sub5, const Graph& graph) {
    size_t pos = path.find_last_of("/\\");
    if (pos != string::npos)
        MKDIR(path.substr(0, pos).c_str());

    ofstream out(path);
    if (!out.is_open()) {
        cerr << "Error abriendo " << path << "\n";
        return;
    }

    const auto& avg = graph.getAvg();

    out << fixed << setprecision(2);

    double totalW = 0.0;
    for (const Edge& e : graph.getEdges()) totalW += e.weight;

    out << "====== GRAFO DE INFRAESTRUCTURA ======\n\n";
    out << "Nodos   : " << graph.numNodes() << "\n";
    out << "Aristas : " << graph.numEdges() << "\n";
    out << "Peso promedio de arista: " << (totalW / graph.numEdges()) << "\n\n";

    out << "Promedios MonthlyCharges por grupo (Mk):\n";
    for (int k = 0; k < graph.numNodes(); k++)
        out << "  grupo " << k << " : " << avg[k] << " USD\n";
    out << "\n";

    out << "====== MST (Kruskal + Union-Find) ======\n\n";
    out << "n = " << graph.numNodes() << " nodos  |  aristas evaluadas = " << graph.numEdges() << "\n\n";
    out << left << setw(5) << "#" << setw(10) << "nodo_u" << setw(10) << "nodo_v" << "peso\n";
    out << string(30, '-') << "\n";

    for (int i = 0; i < (int)full.mstEdges.size(); i++) {
        const Edge& e = full.mstEdges[i];
        out << setw(5) << (i + 1) << setw(10) << e.u << setw(10) << e.v << e.weight << "\n";
    }

    out << string(30, '-') << "\n";
    out << "Peso total MST: " << full.totalWeight << "\n";
    out << "Aristas en MST: " << full.mstEdges.size() << " (= n-1 = " << graph.numNodes() - 1 << ")\n\n";

    out << "====== VERIFICACION MANUAL - Subgrafo {0,1,2,3,4} ======\n\n";

    vector<Edge> sub5edges;
    for (const Edge& e : graph.getEdges())
        if (e.u < 5 && e.v < 5)
            sub5edges.push_back(e);
    sort(sub5edges.begin(), sub5edges.end());

    out << "Todas las aristas del subgrafo K_5 (10 aristas):\n";
    out << left << setw(10) << "nodo_u" << setw(10) << "nodo_v" << "peso\n";
    out << string(25, '-') << "\n";
    for (const Edge& e : sub5edges)
        out << setw(10) << e.u << setw(10) << e.v << e.weight << "\n";
    out << "\n";

    out << "MST subgrafo (4 aristas):\n";
    out << left << setw(5) << "#" << setw(10) << "nodo_u" << setw(10) << "nodo_v" << "peso\n";
    out << string(30, '-') << "\n";
    for (int i = 0; i < (int)sub5.mstEdges.size(); i++) {
        const Edge& e = sub5.mstEdges[i];
        out << setw(5) << (i + 1) << setw(10) << e.u << setw(10) << e.v << e.weight << "\n";
    }
    out << string(30, '-') << "\n";
    out << "Peso total subgrafo MST: " << sub5.totalWeight << "\n\n";

    const Edge* minE = &full.mstEdges[0];
    for (const auto& e : full.mstEdges)
        if (e.weight < minE->weight) minE = &e;

    out << "====== PROPIEDAD DE ELECCION CODICIOSA - LEMA DEL CICLO ======\n\n";
    out << "Lema del ciclo:\n";
    out << "  Para cualquier ciclo C en G, la arista de mayor peso en C\n";
    out << "  no pertenece a ningun MST de G.\n\n";
    out << "Perspectiva de corte:\n";
    out << "  Para cualquier corte (S, V\\S), la arista de menor peso\n";
    out << "  que cruza el corte pertenece a todos los MST de G.\n\n";
    out << "Por que Kruskal es correcto:\n";
    out << "  Procesa aristas en orden ascendente de peso.\n";
    out << "  Si find(u) != find(v): e es la arista minima del corte -> se incluye.\n";
    out << "  Si find(u) == find(v): formaria ciclo -> se descarta.\n";
    out << "  En ambos casos la decision es optima (no requiere backtracking).\n\n";
    out << "Arista concreta del grafo generado:\n";
    out << "  e = (" << minE->u << ", " << minE->v << ")";
    out << "  peso = floor(" << avg[minE->u] << " + " << avg[minE->v] << ")";
    out << " = " << minE->weight << "\n";
    out << "  Al procesarla, nodos " << minE->u << " y " << minE->v;
    out << " estaban en componentes distintas.\n";
    out << "  Era la arista minima del corte -> incluida en el MST.\n";
    out << "  Cualquier reemplazo aumentaria el peso total.\n";

    out.close();
    cout << "Archivo escrito: " << path << "\n";
}
