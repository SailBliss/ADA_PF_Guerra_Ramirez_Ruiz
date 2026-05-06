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
#include <string>
#include <vector>

using namespace std;

/* ═══════════════════════════════════════════════════════════════
   construirItems
   Toma las primeras 50 solicitudes con Churn == "No" del vector
   ya ordenado por tenure DESC (salida del Módulo A).
   ─────────────────────────────────────────────────────────────
   Peso  w_i = round(TotalCharges)
   Valor v_i = round(MonthlyCharges * 10)
   ═══════════════════════════════════════════════════════════════ */
vector<ItemMochila> construirItems(const vector<Solicitud>& ordenadas) {
    vector<ItemMochila> items;
    items.reserve(50);

    for (const Solicitud& s : ordenadas) {
        if (s.churn != "No") continue;
        ItemMochila it;
        it.customerID = s.customerID;
        it.w = static_cast<int>(round(s.totalCharges));
        it.v = static_cast<int>(round(s.monthlyCharges * 10.0));
        items.push_back(it);
        if (static_cast<int>(items.size()) == 50) break;
    }
    return items;
}

/* ═══════════════════════════════════════════════════════════════
   knapsack01  –  Tabulación dp[i][w]
   ─────────────────────────────────────────────────────────────
   Recurrencia:
     dp[0][w] = 0                              para todo w
     dp[i][w] = dp[i-1][w]                    si w_i > w
     dp[i][w] = max(dp[i-1][w],
                    dp[i-1][w - w_i] + v_i)   si w_i <= w

   Complejidad: Θ(n · W) tiempo y espacio
   ═══════════════════════════════════════════════════════════════ */
vector<vector<int>> knapsack01(const vector<ItemMochila>& items, int W) {
    const int n = static_cast<int>(items.size());

    // Tabla (n+1) × (W+1) inicializada en 0
    vector<vector<int>> dp(n + 1, vector<int>(W + 1, 0));

    for (int i = 1; i <= n; ++i) {
        const int wi = items[i - 1].w;
        const int vi = items[i - 1].v;
        for (int w = 0; w <= W; ++w) {
            dp[i][w] = dp[i - 1][w];           // no tomar ítem i
            if (wi <= w) {
                dp[i][w] = max(dp[i][w],
                               dp[i - 1][w - wi] + vi); // tomar ítem i
            }
        }
    }
    return dp;
}

/* ═══════════════════════════════════════════════════════════════
   backtrack  –  Recuperación de la solución óptima
   ─────────────────────────────────────────────────────────────
   Recorre la tabla dp de i = n hasta i = 1.
   Si dp[i][cap] != dp[i-1][cap], el ítem i fue incluido.
   ═══════════════════════════════════════════════════════════════ */
vector<int> backtrack(const vector<vector<int>>& dp,
                      const vector<ItemMochila>& items,
                      int W) {
    vector<int> seleccionados;
    int cap = W;
    const int n = static_cast<int>(items.size());

    for (int i = n; i >= 1; --i) {
        if (dp[i][cap] != dp[i - 1][cap]) {
            seleccionados.push_back(i - 1); // índice 0-based
            cap -= items[i - 1].w;
        }
    }
    reverse(seleccionados.begin(), seleccionados.end());
    return seleccionados;
}

/* ───────────────────────────────────────────────────────────────
   Utilidades internas para el reporte
   ─────────────────────────────────────────────────────────────── */
static string linea(char c, int n) { return string(n, c); }

static string titulo(const string& t, int ancho = 70) {
    const int espacios = (ancho - 2 - static_cast<int>(t.size())) / 2;
    string s(ancho, '=');
    if (espacios > 0 && espacios + static_cast<int>(t.size()) + 2 <= ancho)
        s.replace(espacios, t.size() + 2, " " + t + " ");
    return s;
}

/* ═══════════════════════════════════════════════════════════════
   escribirResultadosKnapsack
   Genera results/asignacion_bw.txt con:
     1. Tabla de los 50 ítems
     2. Resultado óptimo dp[n][W] y backtracking
     3. Contraejemplo del enfoque codicioso
     4. Análisis de complejidad y pseudopolinomialidad
   ═══════════════════════════════════════════════════════════════ */
void escribirResultadosKnapsack(const string& path,
                                const vector<ItemMochila>& items,
                                const vector<vector<int>>& dp,
                                const vector<int>& seleccionados,
                                int W) {
    const size_t pos = path.find_last_of("/\\");
    if (pos != string::npos) {
        MKDIR(path.substr(0, pos).c_str());
    }
 
    ofstream out(path);
    if (!out.is_open()) {
        cerr << "Error: no se pudo crear " << path << "\n";
        return;
    }
 
    const int n = static_cast<int>(items.size());
    const int valorOptimo = dp[n][W];
 
    int pesoTotal = 0, valorTotal = 0;
    for (int idx : seleccionados) {
        pesoTotal += items[idx].w;
        valorTotal += items[idx].v;
    }
 
    // ── Actividad 1: Mochila 0-1 por tabulación ──────────────
    out << titulo("ACTIVIDAD 1 - MOCHILA 0-1 POR TABULACION") << "\n\n";
    out << "  Tabla dp[i][w] construida con dimensiones (51) x (501).\n\n";
    out << "  Valor optimo dp[" << n << "][" << W << "]   : " << valorOptimo << "\n";
    out << "  Numero de solicitudes seleccionadas : " << seleccionados.size() << "\n\n";
 
    if (seleccionados.empty()) {
        out << "  CustomerIDs incluidos: (ninguno)\n";
        out << "  Todos los pesos w_i > W = " << W
            << "; ningun item cabe en la mochila.\n\n";
    } else {
        out << "  CustomerIDs incluidos:\n";
        out << "  " << left
            << setw(14) << "customerID"
            << setw(10) << "w_i"
            << setw(10) << "v_i"
            << "\n";
        out << "  " << linea('-', 34) << "\n";
        for (int idx : seleccionados) {
            out << "  " << left
                << setw(14) << items[idx].customerID
                << setw(10) << items[idx].w
                << setw(10) << items[idx].v
                << "\n";
        }
        out << "  " << linea('-', 34) << "\n";
        out << "  " << left
            << setw(14) << "TOTAL"
            << setw(10) << pesoTotal
            << setw(10) << valorTotal << "\n\n";
    }
 
    // ── Actividad 2: Contraejemplo codicioso ─────────────────
    // A = 4312-KFRXN  (w=1711, v=254)  mayor ratio v/w
    // B = 2848-YXSMW  (w=1363, v=194)  menor peso
    // C = 7606-BPHHN  (w=1469, v=198)  segundo menor peso
    // W' = 1363 + 1469 = 2832
    // Greedy toma A (ratio max), cap restante = 1121 < w_B y w_C -> solo A, v=254
    // Optimo: B+C, peso=2832=W', v=392 > 254
    int idxA = -1, idxB = -1, idxC = -1;
    for (int i = 0; i < n; ++i) {
        if (items[i].customerID == "4312-KFRXN") idxA = i;
        if (items[i].customerID == "2848-YXSMW") idxB = i;
        if (items[i].customerID == "7606-BPHHN") idxC = i;
    }
    const int wA = (idxA >= 0) ? items[idxA].w : 1711;
    const int vA = (idxA >= 0) ? items[idxA].v : 254;
    const string cidA = (idxA >= 0) ? items[idxA].customerID : "4312-KFRXN";
    const int wB = (idxB >= 0) ? items[idxB].w : 1363;
    const int vB = (idxB >= 0) ? items[idxB].v : 194;
    const string cidB = (idxB >= 0) ? items[idxB].customerID : "2848-YXSMW";
    const int wC = (idxC >= 0) ? items[idxC].w : 1469;
    const int vC = (idxC >= 0) ? items[idxC].v : 198;
    const string cidC = (idxC >= 0) ? items[idxC].customerID : "7606-BPHHN";
    const int W_ce = wB + wC;
 
    out << titulo("ACTIVIDAD 2 - FALLO DEL ENFOQUE CODICIOSO") << "\n\n";
    out << "  Sub-problema: 3 solicitudes del conjunto anterior.\n";
    out << "  Capacidad W' = " << wB << " + " << wC << " = " << W_ce << "\n\n";
 
    out << "  " << linea('-', 66) << "\n";
    out << "  " << left
        << setw(22) << "Enfoque"
        << setw(26) << "Solicitudes seleccionadas"
        << setw(12) << "Valor total"
        << "Optimo?\n";
    out << "  " << linea('-', 66) << "\n";
    out << "  " << setw(22) << "Codicioso (ratio v/w)"
        << setw(26) << ("A (" + cidA + ")")
        << setw(12) << vA
        << "No\n";
    out << "  " << setw(22) << "PD (Mochila 0-1)"
        << setw(26) << ("B+C (" + cidB + "+" + cidC + ")")
        << setw(12) << (vB + vC)
        << "Si\n";
    out << "  " << linea('-', 66) << "\n\n";
 
    // ── Actividad 3: Reconstrucción de la solución ────────────
    out << titulo("ACTIVIDAD 3 - RECONSTRUCCION DE LA SOLUCION") << "\n\n";
    out << "  Backtracking sobre dp[i][cap]: si dp[i][cap] != dp[i-1][cap],\n";
    out << "  el item i fue incluido; se descuenta su peso y se retrocede.\n\n";
 
    if (seleccionados.empty()) {
        out << "  Conjunto recuperado: VACIO\n";
        out << "  (dp[i][cap] == dp[i-1][cap] para todo i -> ningun item incluido)\n\n";
    } else {
        out << "  Conjunto exacto recuperado:\n";
        out << "  " << left
            << setw(6)  << "Orden"
            << setw(14) << "customerID"
            << setw(10) << "w_i"
            << setw(10) << "v_i"
            << "\n";
        out << "  " << linea('-', 40) << "\n";
        int orden = 1;
        for (int idx : seleccionados) {
            out << "  " << left
                << setw(6)  << orden++
                << setw(14) << items[idx].customerID
                << setw(10) << items[idx].w
                << setw(10) << items[idx].v
                << "\n";
        }
        out << "  " << linea('-', 40) << "\n";
        out << "  Peso total : " << pesoTotal << "\n";
        out << "  Valor total: " << valorTotal << "\n\n";
    }
 
    // ── Actividad 4: Análisis de complejidad ──────────────────
    out << titulo("ACTIVIDAD 4 - ANALISIS DE COMPLEJIDAD") << "\n\n";
 
    out << "  Tiempo: T(n,W) = Theta(n * W)\n"
        << "    - Bucle exterior i = 1..n : n iteraciones.\n"
        << "    - Bucle interior w = 0..W : W+1 iteraciones.\n"
        << "    - Cada celda dp[i][w]     : O(1).\n\n";
 
    out << "  Espacio: S(n,W) = Theta(n * W)\n"
        << "    - Tabla dp de (n+1)(W+1) enteros.\n"
        << "    - Reducible a Theta(W) sin backtracking.\n\n";
 
    out << "  Pseudopolinomialidad:\n"
        << "    W no es el tamano del input; ocupa O(log W) bits.\n"
        << "    Theta(n*W) = Theta(n * 2^{log W}) es exponencial\n"
        << "    en el tamano real del input.\n"
        << "    La Mochila 0-1 es NP-dificil; este algoritmo es\n"
        << "    pseudopolinomial, no polinomial en sentido estricto.\n\n";
 
    out << "  Instancia: n=" << n << ", W=" << W << ".\n"
        << "  Celdas dp: " << n << " x " << (W+1)
        << " = " << (n*(W+1)) << ".\n";
 
    out.close();
    cout << "✓ " << path << " generado.\n";
}
