#include "parser.hpp"

#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#endif

#include <fstream>
#include <iomanip>
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

void escribirCampoCSV(ofstream& archivo, const string& valor) {
    const bool requiereComillas = valor.find_first_of(",\"\r\n") != string::npos;

    if (!requiereComillas) {
        archivo << valor;
        return;
    }

    archivo << '"';
    for (const char c : valor) {
        if (c == '"') {
            archivo << "\"\"";
        } else {
            archivo << c;
        }
    }
    archivo << '"';
}

string obtenerDirectorioPadre(const string& path) {
    const size_t posicion = path.find_last_of("/\\");
    if (posicion == string::npos) {
        return "";
    }

    return path.substr(0, posicion);
}

void crearDirectorioSiHaceFalta(const string& directorio) {
    if (directorio.empty()) {
        return;
    }

#ifdef _WIN32
    _mkdir(directorio.c_str());
#else
    mkdir(directorio.c_str(), 0755);
#endif
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

void escribirSolicitudesOrdenadas(const string& path,
                                  const vector<Solicitud>& solicitudes) {
    crearDirectorioSiHaceFalta(obtenerDirectorioPadre(path));

    ofstream archivo(path);
    if (!archivo.is_open()) {
        throw runtime_error("No se pudo crear el archivo CSV: " + path);
    }

    archivo << "customerID,tenure,MonthlyCharges,TotalCharges,Churn\n";
    archivo << fixed << setprecision(2);

    for (const Solicitud& solicitud : solicitudes) {
        escribirCampoCSV(archivo, solicitud.customerID);
        archivo << ',' << solicitud.tenure << ','
                << solicitud.monthlyCharges << ','
                << solicitud.totalCharges << ',';
        escribirCampoCSV(archivo, solicitud.churn);
        archivo << '\n';
    }
}
