#ifndef BINARY_SEARCH_HPP
#define BINARY_SEARCH_HPP

#include "parser.hpp"

#include <string>
#include <vector>

int findTenure(const std::vector<Solicitud>& v, int k);
void escribirResultadosBusqueda(const std::string& path,
                                const std::vector<Solicitud>& v);

#endif
