#ifndef KNAPSACK_HPP
#define KNAPSACK_HPP

#include "parser.hpp"
#include <vector>
#include <string>

// item para la mochila
struct ItemMochila {
    std::string customerID;
    int w; // peso = TotalCharges redondeado
    int v; // valor = MonthlyCharges * 10 redondeado
};

std::vector<ItemMochila> construirItems(const std::vector<Solicitud>& solicitudes);

// retorna tabla dp completa para poder hacer backtracking despues
std::vector<std::vector<int>> knapsack01(const std::vector<ItemMochila>& items, int W);

std::vector<int> backtrack(const std::vector<std::vector<int>>& dp,
                           const std::vector<ItemMochila>& items, int W);

void escribirResultadosKnapsack(const std::string& path,
                                const std::vector<ItemMochila>& items,
                                const std::vector<std::vector<int>>& dp,
                                const std::vector<int>& seleccionados,
                                int W);

#endif

