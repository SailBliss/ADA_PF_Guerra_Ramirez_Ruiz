// main :D

#include "mergesort.hpp"
#include "parser.hpp"

#include <algorithm>
#include <exception>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

inline void imprimirPrimeros(const vector<Solicitud> &solicitudes, size_t cantidad)
{
    const size_t limite = min(cantidad, solicitudes.size());
    for (size_t i = 0; i < limite; ++i)
    {
        cout << solicitudes[i].customerID << " tenure=" << solicitudes[i].tenure << '\n';
    }
}

inline int ejecutarPruebaMergeSort(int argc, char *argv[])
{
    if (argc < 2)
    {
        cerr << "Uso: " << argv[0] << " <ruta_csv>\n";
        return 1;
    }

    try
    {
        int totalRegistros = 0;
        int totalChargesNulos = 0;
        vector<Solicitud> solicitudes =
            parseSolicitudes(argv[1], totalRegistros, totalChargesNulos);

        cout << "totalRegistros: " << totalRegistros << '\n';
        cout << "totalChargesNulos: " << totalChargesNulos << '\n';
        cout << "tamanoVector: " << solicitudes.size() << '\n';

        cout << "\nPrimeros 5 antes de ordenar:\n";
        imprimirPrimeros(solicitudes, 5);

        mergeSortSolicitudes(solicitudes);

        cout << "\nPrimeros 10 despues de ordenar por tenure DESC:\n";
        imprimirPrimeros(solicitudes, 10);

        const string salida = "results/solicitudes_ordenadas.csv";
        escribirSolicitudesOrdenadas(salida, solicitudes);
        cout << "\nArchivo escrito: " << salida << '\n';

        if (!solicitudes.empty())
        {
            cout << "\ntenureMaximo: " << solicitudes.front().tenure << '\n';
        }
    }
    catch (const exception &e)
    {
        cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}
