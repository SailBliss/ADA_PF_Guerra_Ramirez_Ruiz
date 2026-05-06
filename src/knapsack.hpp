#ifndef KNAPSACK_HPP
#define KNAPSACK_HPP

#include "parser.hpp"

#include <string>
#include <vector>

/* ─────────────────────────────────────────────────────────────
   Ítem de la mochila 0-1 (Módulo C)
   w = round(TotalCharges)
   v = round(MonthlyCharges * 10)
   ───────────────────────────────────────────────────────────── */
struct ItemMochila {
    std::string customerID;
    int w; // peso  : unidades de ancho de banda requeridas
    int v; // valor : ingreso mensual en centavos
};

/* Construye los 50 ítems a partir del vector ya ordenado (Módulo A).
   Toma las primeras 50 solicitudes con Churn == "No". */
std::vector<ItemMochila> construirItems(const std::vector<Solicitud>& ordenadas);

/* Mochila 0-1 por tabulación.
   Devuelve la tabla dp de dimensiones (n+1) x (W+1). */
std::vector<std::vector<int>> knapsack01(
    const std::vector<ItemMochila>& items, int W);

/* Backtracking sobre la tabla dp.
   Devuelve los índices (0-based) de los ítems seleccionados. */
std::vector<int> backtrack(
    const std::vector<std::vector<int>>& dp,
    const std::vector<ItemMochila>& items,
    int W);

/* Escribe results/asignacion_bw.txt con:
   - Tabla de ítems
   - Solución óptima y customerIDs seleccionados
   - Contraejemplo codicioso con tabla comparativa
   - Análisis de complejidad y pseudopolinomialidad */
void escribirResultadosKnapsack(
    const std::string& path,
    const std::vector<ItemMochila>& items,
    const std::vector<std::vector<int>>& dp,
    const std::vector<int>& seleccionados,
    int W);

#endif // KNAPSACK_HPP
