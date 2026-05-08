#include "knapsack.hpp"

#ifdef _WIN32
#include <direct.h>
#define MKDIR(p) _mkdir(p)
#else
#include <sys/stat.h>
#define MKDIR(p) mkdir(p, 0755)
#endif

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>

using namespace std;

vector<ItemMochila> construirItems(const vector<Solicitud>& solicitudes) {
    vector<ItemMochila> items;
    for (int i = 0; i < (int)solicitudes.size() && (int)items.size() < 50; i++) {
        if (solicitudes[i].churn != "No") continue;
        ItemMochila it;
        it.customerID = solicitudes[i].customerID;
        it.w = (int)round(solicitudes[i].totalCharges);
        it.v = (int)round(solicitudes[i].monthlyCharges * 10.0);
        items.push_back(it);
    }
    return items;
}

vector<vector<int>> knapsack01(const vector<ItemMochila>& items, int W) {
    int n = items.size();
    vector<vector<int>> dp(n + 1, vector<int>(W + 1, 0));

    for (int i = 1; i <= n; i++) {
        int wi = items[i-1].w;
        int vi = items[i-1].v;
        for (int w = 0; w <= W; w++) {
            dp[i][w] = dp[i-1][w];
            if (wi <= w && dp[i-1][w - wi] + vi > dp[i][w])
                dp[i][w] = dp[i-1][w - wi] + vi;
        }
    }
    return dp;
}

vector<int> backtrack(const vector<vector<int>>& dp,
                      const vector<ItemMochila>& items, int W) {
    vector<int> resultado;
    int cap = W;
    int n = items.size();

    for (int i = n; i >= 1; i--) {
        if (dp[i][cap] != dp[i-1][cap]) {
            resultado.push_back(i - 1);
            cap -= items[i-1].w;
        }
    }
    reverse(resultado.begin(), resultado.end());
    return resultado;
}

static void encontrarContraejemplo(const vector<ItemMochila>& items,
                                   int& idxA, int& idxB, int& idxC, int& Wprima) {
    int n = items.size();

    vector<int> porPeso(n);
    for (int i = 0; i < n; i++) porPeso[i] = i;
    sort(porPeso.begin(), porPeso.end(),
         [&](int a, int b) { return items[a].w < items[b].w; });

    idxB = porPeso[0];
    idxC = porPeso[1];
    Wprima = items[idxB].w + items[idxC].w;
    int valBC = items[idxB].v + items[idxC].v;

    idxA = -1;
    double mejorRatio = -1.0;
    for (int i = 0; i < n; i++) {
        if (i == idxB || i == idxC) continue;
        if (items[i].w > Wprima || items[i].v >= valBC) continue;
        double r = (double)items[i].v / items[i].w;
        if (r > mejorRatio) {
            mejorRatio = r;
            idxA = i;
        }
    }
}

void escribirResultadosKnapsack(const string& path,
                                const vector<ItemMochila>& items,
                                const vector<vector<int>>& dp,
                                const vector<int>& seleccionados,
                                int W) {
    size_t pos = path.find_last_of("/\\");
    if (pos != string::npos)
        MKDIR(path.substr(0, pos).c_str());

    ofstream out(path);
    if (!out.is_open()) {
        cerr << "Error abriendo " << path << "\n";
        return;
    }

    int n = items.size();
    int valorOptimo = dp[n][W];

    int pesoTotal = 0, valorTotal = 0;
    for (int idx : seleccionados) {
        pesoTotal += items[idx].w;
        valorTotal += items[idx].v;
    }

    int wMin = items[0].w;
    for (int i = 1; i < n; i++)
        if (items[i].w < wMin) wMin = items[i].w;

    // actividad 1
    out << "====== ACTIVIDAD 1 - MOCHILA 0-1 POR TABULACION ======\n\n";
    out << "Tabla dp[i][w] construida: dimensiones (51) x (501)\n";
    out << "n = " << n << " items  |  W = " << W << " unidades\n";
    out << "Peso minimo entre los 50 items: " << wMin << "\n\n";
    out << "Valor optimo dp[" << n << "][" << W << "] = " << valorOptimo << "\n";
    out << "Solicitudes seleccionadas: " << seleccionados.size() << "\n\n";

    if (seleccionados.empty()) {
        out << "CustomerIDs incluidos: (ninguno)\n";
        out << "Todos los pesos w_i >= " << wMin << " > W = " << W;
        out << ". La mochila no puede admitir ningun item;\n";
        out << "el valor optimo 0 es matematicamente correcto.\n\n";
    } else {
        out << left << setw(14) << "customerID" << setw(10) << "w_i" << setw(10) << "v_i" << "\n";
        out << string(34, '-') << "\n";
        for (int idx : seleccionados)
            out << left << setw(14) << items[idx].customerID << setw(10) << items[idx].w << setw(10) << items[idx].v << "\n";
        out << string(34, '-') << "\n";
        out << left << setw(14) << "TOTAL" << setw(10) << pesoTotal << setw(10) << valorTotal << "\n\n";
    }

    // actividad 2
    int idxA, idxB, idxC, Wprima;
    encontrarContraejemplo(items, idxA, idxB, idxC, Wprima);

    double rA = (double)items[idxA].v / items[idxA].w;
    double rB = (double)items[idxB].v / items[idxB].w;
    double rC = (double)items[idxC].v / items[idxC].w;
    int valCodicioso = items[idxA].v;
    int valOptimo2 = items[idxB].v + items[idxC].v;
    int capTrasA = Wprima - items[idxA].w;

    out << "====== ACTIVIDAD 2 - FALLO DEL ENFOQUE CODICIOSO ======\n\n";
    out << "Sub-problema con 3 items del conjunto anterior.\n";
    out << "Capacidad W' = w_B + w_C = " << items[idxB].w << " + " << items[idxC].w << " = " << Wprima << "\n\n";

    out << fixed << setprecision(6);
    out << left << setw(5) << "Item" << setw(14) << "customerID" << setw(8) << "w_i" << setw(8) << "v_i" << "ratio v/w\n";
    out << string(50, '-') << "\n";
    out << setw(5) << "A" << setw(14) << items[idxA].customerID << setw(8) << items[idxA].w << setw(8) << items[idxA].v << rA << "\n";
    out << setw(5) << "B" << setw(14) << items[idxB].customerID << setw(8) << items[idxB].w << setw(8) << items[idxB].v << rB << "\n";
    out << setw(5) << "C" << setw(14) << items[idxC].customerID << setw(8) << items[idxC].w << setw(8) << items[idxC].v << rC << "\n";
    out << string(50, '-') << "\n\n";

    out << "Traza codicioso (orden por ratio desc: A > B > C):\n";
    out << "  Toma A (w=" << items[idxA].w << " <= W'=" << Wprima << "). Cap restante = " << capTrasA << ".\n";
    out << "  B requiere " << items[idxB].w << " > " << capTrasA << " -> no cabe.\n";
    out << "  C requiere " << items[idxC].w << " > " << capTrasA << " -> no cabe.\n\n";

    out << string(64, '-') << "\n";
    out << left << setw(22) << "Enfoque" << setw(22) << "Solicitudes" << setw(12) << "Valor total" << "Optimo?\n";
    out << string(64, '-') << "\n";
    out << setw(22) << "Codicioso (ratio v/w)" << setw(22) << ("A (" + items[idxA].customerID + ")") << setw(12) << valCodicioso << "No\n";
    out << setw(22) << "PD (Mochila 0-1)" << setw(22) << ("B+C (" + items[idxB].customerID + "...)") << setw(12) << valOptimo2 << "Si\n";
    out << string(64, '-') << "\n\n";

    // actividad 3
    out << "====== ACTIVIDAD 3 - RECONSTRUCCION DE LA SOLUCION ======\n\n";
    out << "Backtracking: recorre dp de i=n hasta i=1.\n";
    out << "Si dp[i][cap] != dp[i-1][cap], el item i fue incluido.\n\n";

    if (seleccionados.empty()) {
        out << "Conjunto recuperado: VACIO\n";
        out << "dp[i][cap] == dp[i-1][cap] para todo i en {1..50}.\n\n";
    } else {
        out << left << setw(8) << "Orden" << setw(14) << "customerID" << setw(10) << "w_i" << setw(10) << "v_i" << "\n";
        out << string(42, '-') << "\n";
        int orden = 1;
        for (int idx : seleccionados)
            out << setw(8) << orden++ << setw(14) << items[idx].customerID << setw(10) << items[idx].w << setw(10) << items[idx].v << "\n";
        out << string(42, '-') << "\n";
        out << "Peso total : " << pesoTotal << "\n";
        out << "Valor total: " << valorTotal << "\n\n";
    }

    // actividad 4
    out << "====== ACTIVIDAD 4 - ANALISIS DE COMPLEJIDAD ======\n\n";
    out << "Tiempo: T(n,W) = Theta(n * W)\n";
    out << "  Bucle i = 1..n : n = " << n << " iteraciones.\n";
    out << "  Bucle w = 0..W : W+1 = " << W+1 << " iteraciones.\n";
    out << "  Cada celda dp[i][w] : O(1).\n";
    out << "  Total celdas: " << n << " x " << W+1 << " = " << n*(W+1) << ".\n\n";

    out << "Espacio: S(n,W) = Theta(n * W)\n";
    out << "  Tabla dp: (n+1)(W+1) = " << (n+1)*(W+1) << " enteros.\n";
    out << "  Reducible a Theta(W) prescindiendo del backtracking.\n\n";

    out << "Pseudopolinomialidad:\n";
    out << "  El tamano del input es s = Theta(n * log W) bits.\n";
    out << "  W = 2^{Theta(log W)} es exponencial en log W.\n";
    out << "  Por tanto Theta(n*W) es exponencial en el tamano real del input.\n";
    out << "  La Mochila 0-1 es NP-dificil; este algoritmo es\n";
    out << "  pseudopolinomial, no polinomial en sentido estricto.\n";

    out.close();
    cout << "Archivo escrito: " << path << "\n";
}

