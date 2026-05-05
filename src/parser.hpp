#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <vector>

struct Solicitud {
    std::string customerID;
    int tenure;
    double monthlyCharges;
    double totalCharges;
    std::string churn;
};

std::vector<Solicitud> parseSolicitudes(const std::string& path,
                                        int& totalRegistros,
                                        int& totalChargesNulos);

#endif
