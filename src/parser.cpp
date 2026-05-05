#include "parser.hpp"

#include <fstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

namespace {

string trim(const string& texto) {
    const size_t inicio = texto.find_first_not_of(" \t\r\n");
    if (inicio == string::npos) {
        return "";
    }

    const size_t fin = texto.find_last_not_of(" \t\r\n");
    return texto.substr(inicio, fin - inicio + 1);
}

vector<string> separarCSV(const string& linea) {
    vector<string> campos;
    string actual;
    bool enComillas = false;

    for (size_t i = 0; i < linea.size(); ++i) {
        const char c = linea[i];

        if (c == '"') {
            if (enComillas && i + 1 < linea.size() && linea[i + 1] == '"') {
                actual += '"';
                ++i;
            } else {
                enComillas = !enComillas;
            }
        } else if (c == ',' && !enComillas) {
            campos.push_back(trim(actual));
            actual.clear();
        } else {
            actual += c;
        }
    }

    campos.push_back(trim(actual));
    return campos;
}

int indiceColumna(const unordered_map<string, int>& indices, const string& nombre) {
    const auto it = indices.find(nombre);
    if (it == indices.end()) {
        throw runtime_error("No se encontro la columna requerida: " + nombre);
    }

    return it->second;
}

string campoEn(const vector<string>& campos, int indice, const string& nombreColumna) {
    if (indice < 0 || static_cast<size_t>(indice) >= campos.size()) {
        throw runtime_error("Registro sin campo requerido: " + nombreColumna);
    }

    return campos[static_cast<size_t>(indice)];
}

}

vector<Solicitud> parseSolicitudes(const string& path,
                                   int& totalRegistros,
                                   int& totalChargesNulos) {
    totalRegistros = 0;
    totalChargesNulos = 0;

    ifstream archivo(path);
    if (!archivo.is_open()) {
        throw runtime_error("No se pudo abrir el archivo CSV: " + path);
    }

    string linea;
    if (!getline(archivo, linea)) {
        return {};
    }

    const vector<string> header = separarCSV(linea);
    unordered_map<string, int> indices;
    for (size_t i = 0; i < header.size(); ++i) {
        indices[trim(header[i])] = static_cast<int>(i);
    }

    const int idxCustomerID = indiceColumna(indices, "customerID");
    const int idxTenure = indiceColumna(indices, "tenure");
    const int idxMonthlyCharges = indiceColumna(indices, "MonthlyCharges");
    const int idxTotalCharges = indiceColumna(indices, "TotalCharges");
    const int idxChurn = indiceColumna(indices, "Churn");

    vector<Solicitud> solicitudes;
    while (getline(archivo, linea)) {
        if (trim(linea).empty()) {
            continue;
        }

        const vector<string> campos = separarCSV(linea);
        const string totalChargesTexto = campoEn(campos, idxTotalCharges, "TotalCharges");

        Solicitud solicitud;
        solicitud.customerID = campoEn(campos, idxCustomerID, "customerID");
        solicitud.tenure = stoi(campoEn(campos, idxTenure, "tenure"));
        solicitud.monthlyCharges = stod(campoEn(campos, idxMonthlyCharges, "MonthlyCharges"));
        solicitud.churn = campoEn(campos, idxChurn, "Churn");

        if (trim(totalChargesTexto).empty()) {
            solicitud.totalCharges = 0.0;
            ++totalChargesNulos;
        } else {
            solicitud.totalCharges = stod(totalChargesTexto);
        }

        solicitudes.push_back(solicitud);
        ++totalRegistros;
    }

    return solicitudes;
}
