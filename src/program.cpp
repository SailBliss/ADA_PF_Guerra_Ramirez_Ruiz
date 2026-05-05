#include "parser.hpp"

#include <exception>
#include <iostream>
#include <vector>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Uso: " << argv[0] << " <ruta_csv>\n";
        return 1;
    }

    try {
        int totalRegistros = 0;
        int totalChargesNulos = 0;
        const vector<Solicitud> solicitudes =
            parseSolicitudes(argv[1], totalRegistros, totalChargesNulos);

        cout << "totalRegistros: " << totalRegistros << '\n';
        cout << "totalChargesNulos: " << totalChargesNulos << '\n';
        cout << "tamanoVector: " << solicitudes.size() << '\n';

        if (!solicitudes.empty()) {
            cout << "primerCustomerID: " << solicitudes.front().customerID << '\n';
            cout << "ultimoCustomerID: " << solicitudes.back().customerID << '\n';
        }
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}
