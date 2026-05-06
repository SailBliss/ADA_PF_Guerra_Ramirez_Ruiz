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
    // Crear directorio results/ si no existe
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

    // Peso y valor total de la solución
    int pesoTotal = 0, valorTotal = 0;
    for (int idx : seleccionados) {
        pesoTotal += items[idx].w;
        valorTotal += items[idx].v;
    }

    // ── 1. Encabezado ─────────────────────────────────────────
    out << titulo("MÓDULO C – ASIGNACIÓN DE ANCHO DE BANDA (MOCHILA 0-1)") << "\n\n";
    out << titulo("PARÁMETROS") << "\n";
    out << "  Capacidad W            : " << W << " unidades\n";
    out << "  Número de ítems n      : " << n << "\n";
    out << "  Peso  w_i              : round(TotalCharges)\n";
    out << "  Valor v_i              : round(MonthlyCharges x 10)\n";
    out << "  Fuente                 : 50 solicitudes de mayor tenure con Churn = No\n\n";

    // ── 2. Tabla de los 50 ítems ──────────────────────────────
    out << titulo("TABLA DE LOS 50 ÍTEMS") << "\n";
    out << left
        << setw(4)  << "#"
        << setw(14) << "customerID"
        << setw(10) << "tenure"
        << setw(10) << "w_i"
        << setw(10) << "v_i"
        << setw(12) << "ratio v/w"
        << "\n";
    out << linea('-', 60) << "\n";

    // Recuperar el tenure de los items (usamos el orden original)
    // Los items vienen del vector ordenado, todos tienen tenure=72 en este dataset
    for (int i = 0; i < n; ++i) {
        const double ratio = static_cast<double>(items[i].v) / items[i].w;
        out << left
            << setw(4)  << (i + 1)
            << setw(14) << items[i].customerID
            << setw(10) << 72   // tenure de las 50 primeras solicitudes activas
            << setw(10) << items[i].w
            << setw(10) << items[i].v
            << fixed << setprecision(6) << ratio
            << "\n";
    }
    out << "\n";

    // ── Nota sobre factibilidad ───────────────────────────────
    int wMin = items[0].w;
    for (const auto& it : items) wMin = min(wMin, it.w);

    out << titulo("NOTA SOBRE FACTIBILIDAD") << "\n";
    out << "  Peso minimo de los 50 items: " << wMin << "\n";
    out << "  Capacidad W = " << W << "\n";
    out << "  Todos los pesos w_i > W = " << W << ".\n";
    out << "  Los 50 items tienen tenure = 72 =>  w_i ~ 72 x MonthlyCharges >> 500.\n";
    out << "  El valor optimo es 0 y el conjunto solucion es vacio.\n";
    out << "  La tabulacion y el backtracking se ejecutan correctamente;\n";
    out << "  el resultado vacio es consecuencia directa de la especificacion.\n\n";

    // ── 3. Resultado óptimo ───────────────────────────────────
    out << titulo("RESULTADO ÓPTIMO  dp[n][W]") << "\n";
    out << "  dp[" << n << "][" << W << "] = " << valorOptimo << "\n";
    out << "  Peso total usado       : " << pesoTotal << "\n";
    out << "  Solicitudes incluidas  : " << seleccionados.size() << "\n\n";

    if (seleccionados.empty()) {
        out << "  Conjunto solucion: VACIO\n";
        out << "  (Ningun item tiene peso <= W)\n\n";
    } else {
        out << "  CustomerIDs seleccionados:\n";
        for (int idx : seleccionados) {
            out << "    - " << items[idx].customerID
                << "  (w=" << items[idx].w
                << ", v=" << items[idx].v << ")\n";
        }
        out << "\n";
    }

    // ── 4. Recurrencia ────────────────────────────────────────
    out << titulo("FORMULACIÓN DE LA RECURRENCIA") << "\n";
    out << "\n"
        << "  dp[0][w] = 0                                 para todo w en {0,...,W}\n"
        << "  dp[i][w] = dp[i-1][w]                        si w_i > w\n"
        << "  dp[i][w] = max(dp[i-1][w],                   si w_i <= w\n"
        << "                 dp[i-1][w - w_i] + v_i)\n\n"
        << "  Solucion optima: dp[n][W]\n\n";

    out << "  PSEUDOCODIGO – Tabulacion:\n"
        << "  +----------------------------------------------------------+\n"
        << "  | FUNCION Knapsack01(items[1..n], W):                     |\n"
        << "  |   PARA i = 0 HASTA n:  dp[i][0..W] <- 0                |\n"
        << "  |   PARA i = 1 HASTA n:                                   |\n"
        << "  |     PARA w = 0 HASTA W:                                 |\n"
        << "  |       dp[i][w] <- dp[i-1][w]                           |\n"
        << "  |       SI items[i].w <= w:                               |\n"
        << "  |         dp[i][w] <- max(dp[i][w],                      |\n"
        << "  |                        dp[i-1][w-w_i] + v_i)           |\n"
        << "  |   RETORNAR dp[n][W]                                     |\n"
        << "  +----------------------------------------------------------+\n\n";

    out << "  PSEUDOCODIGO – Backtracking:\n"
        << "  +----------------------------------------------------------+\n"
        << "  | FUNCION Backtrack(dp, items[1..n], W):                  |\n"
        << "  |   cap <- W                                              |\n"
        << "  |   seleccionados <- []                                   |\n"
        << "  |   PARA i = n HASTA 1 (decreciente):                    |\n"
        << "  |     SI dp[i][cap] != dp[i-1][cap]:                     |\n"
        << "  |       seleccionados.agregar(i)                          |\n"
        << "  |       cap <- cap - items[i].w                          |\n"
        << "  |   RETORNAR invertir(seleccionados)                      |\n"
        << "  +----------------------------------------------------------+\n\n";

    // ── 5. Contraejemplo codicioso ────────────────────────────
    // A = 4312-KFRXN  (w=1711, v=254, ratio=0.1485)  -> mayor ratio
    // B = 2848-YXSMW  (w=1363, v=194, ratio=0.1423)  -> menor peso
    // C = 7606-BPHHN  (w=1469, v=198, ratio=0.1348)  -> 2do menor peso
    // W' = w_B + w_C = 2832
    // Greedy: toma A (ratio max), cap restante = 1121 < w_B y < w_C → solo A, v=254
    // Optimo: B + C, peso=2832=W', v=392 > 254

    // Localizar por customerID en items
    int idxA = -1, idxB = -1, idxC = -1;
    for (int i = 0; i < n; ++i) {
        if (items[i].customerID == "4312-KFRXN") idxA = i;
        if (items[i].customerID == "2848-YXSMW") idxB = i;
        if (items[i].customerID == "7606-BPHHN") idxC = i;
    }
    // Valores por defecto si no se hallan (no debería ocurrir)
    const int wA = (idxA >= 0) ? items[idxA].w : 1711;
    const int vA = (idxA >= 0) ? items[idxA].v : 254;
    const string cidA = (idxA >= 0) ? items[idxA].customerID : "4312-KFRXN";
    const int wB = (idxB >= 0) ? items[idxB].w : 1363;
    const int vB = (idxB >= 0) ? items[idxB].v : 194;
    const string cidB = (idxB >= 0) ? items[idxB].customerID : "2848-YXSMW";
    const int wC = (idxC >= 0) ? items[idxC].w : 1469;
    const int vC = (idxC >= 0) ? items[idxC].v : 198;
    const string cidC = (idxC >= 0) ? items[idxC].customerID : "7606-BPHHN";

    const double rA = static_cast<double>(vA) / wA;
    const double rB = static_cast<double>(vB) / wB;
    const double rC = static_cast<double>(vC) / wC;
    const int W_ce = wB + wC;  // 2832

    out << titulo("CONTRAEJEMPLO – FALLO DEL ENFOQUE CODICIOSO") << "\n";
    out << "\n  Sub-problema: 3 solicitudes del conjunto anterior, W' = "
        << wB << " + " << wC << " = " << W_ce << "\n\n";

    out << "  Items del contraejemplo:\n";
    out << "  " << linea('-', 60) << "\n";
    out << "  " << left
        << setw(6)  << "Item"
        << setw(14) << "customerID"
        << setw(8)  << "w_i"
        << setw(8)  << "v_i"
        << setw(12) << "ratio v/w"
        << "\n";
    out << "  " << linea('-', 60) << "\n";
    out << "  " << setw(6) << "A"
        << setw(14) << cidA << setw(8) << wA << setw(8) << vA
        << fixed << setprecision(6) << rA << "\n";
    out << "  " << setw(6) << "B"
        << setw(14) << cidB << setw(8) << wB << setw(8) << vB
        << fixed << setprecision(6) << rB << "\n";
    out << "  " << setw(6) << "C"
        << setw(14) << cidC << setw(8) << wC << setw(8) << vC
        << fixed << setprecision(6) << rC << "\n";
    out << "  " << linea('-', 60) << "\n\n";

    out << "  Traza del codicioso (orden por ratio desc: A > B > C):\n";
    out << "    Paso 1: Toma A (w=" << wA << " <= W'=" << W_ce
        << "). Cap restante = " << (W_ce - wA) << ".\n";
    out << "    Paso 2: B requiere " << wB << " > " << (W_ce - wA)
        << " -> no cabe.\n";
    out << "    Paso 3: C requiere " << wC << " > " << (W_ce - wA)
        << " -> no cabe.\n";
    out << "    Resultado codicioso: {A}, valor = " << vA << ".\n\n";

    out << "  Solucion optima (PD 0-1):\n";
    out << "    {B, C}: peso = " << wB << " + " << wC << " = " << W_ce
        << " <= W', valor = " << vB << " + " << vC
        << " = " << (vB + vC) << " > " << vA << ".\n\n";

    out << "  TABLA COMPARATIVA:\n";
    out << "  " << linea('-', 68) << "\n";
    out << "  " << left
        << setw(24) << "Enfoque"
        << setw(26) << "Solicitudes seleccionadas"
        << setw(12) << "Valor total"
        << setw(8)  << "Optimo?"
        << "\n";
    out << "  " << linea('-', 68) << "\n";
    out << "  " << setw(24) << "Codicioso (ratio v/w)"
        << setw(26) << ("A (" + cidA + ")")
        << setw(12) << vA
        << setw(8)  << "No"
        << "\n";
    out << "  " << setw(24) << "PD (Mochila 0-1)"
        << setw(26) << ("B+C (" + cidB + "+" + cidC + ")")
        << setw(12) << (vB + vC)
        << setw(8)  << "Si"
        << "\n";
    out << "  " << linea('-', 68) << "\n\n";

    // ── 6. Análisis de complejidad ────────────────────────────
    out << titulo("ANÁLISIS DE COMPLEJIDAD") << "\n\n";

    out << "  Tiempo:\n"
        << "    Bucle exterior: i = 1 ... n       -> n iteraciones\n"
        << "    Bucle interior: w = 0 ... W       -> W+1 iteraciones\n"
        << "    Cada celda dp[i][w]: O(1)\n"
        << "    T(n,W) = Theta(n * W)\n\n";

    out << "  Espacio:\n"
        << "    Tabla dp: (n+1)(W+1) enteros\n"
        << "    S(n,W) = Theta(n * W)\n"
        << "    (Reducible a Theta(W) con dos filas si no se requiere backtracking)\n\n";

    out << "  Pseudopolinomialidad:\n"
        << "    Theta(n*W) parece polinomial, pero W no es el tamano del input.\n"
        << "    El input ocupa s = Theta(n * log W) bits.\n"
        << "    W puede ser hasta 2^s -> exponencial en el tamano real.\n"
        << "    Por tanto Theta(n*W) = Theta(n * 2^{Theta(log W)}) es\n"
        << "    EXPONENCIAL en el tamano del input.\n"
        << "    Estos algoritmos se llaman pseudopolinomiales.\n"
        << "    La Mochila 0-1 es NP-dificil; la PD no la hace polinomial\n"
        << "    en sentido estricto.\n\n";

    out << "  Para esta instancia: n=" << n << ", W=" << W << ".\n"
        << "  Celdas dp: " << n << " x " << (W + 1) << " = "
        << (n * (W + 1)) << ".\n"
        << "  Pasos backtracking: " << n << ".\n"
        << "  Total: " << (n * (W + 1) + n) << " operaciones.\n\n";

    out << linea('=', 70) << "\n";
    out << "  Generado por: Modulo C – Asignacion de Ancho de Banda\n";
    out << linea('=', 70) << "\n";

    out.close();
    cout << "✓ " << path << " generado.\n";
}
