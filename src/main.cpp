// main :D

#include "binary_search.hpp"
#include "mergesort.hpp"
#include "parser.hpp"

#include <algorithm>
#include <chrono>
#include <exception>
#include <iomanip>
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

inline double medirMergeSort(vector<Solicitud> &solicitudes)
{
    const auto inicio = chrono::high_resolution_clock::now();
    mergeSortSolicitudes(solicitudes);
    const auto fin = chrono::high_resolution_clock::now();

    return chrono::duration<double, milli>(fin - inicio).count();
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

        vector<Solicitud> full = solicitudes;
        vector<Solicitud> sub3500(
            solicitudes.begin(),
            solicitudes.begin() + min<size_t>(3500, solicitudes.size()));
        vector<Solicitud> sub1000(
            solicitudes.begin(),
            solicitudes.begin() + min<size_t>(1000, solicitudes.size()));

        const double tiempoFull = medirMergeSort(full);
        const double tiempo3500 = medirMergeSort(sub3500);
        const double tiempo1000 = medirMergeSort(sub1000);

        cout << fixed << setprecision(2);
        cout << "\n--- TIEMPOS MERGESORT ---\n";
        cout << "n=" << full.size() << " → " << tiempoFull << " ms\n";
        cout << "n=" << sub3500.size() << " → " << tiempo3500 << " ms\n";
        cout << "n=" << sub1000.size() << " → " << tiempo1000 << " ms\n";

        mergeSortSolicitudes(solicitudes);

        cout << "\nPrimeros 10 despues de ordenar por tenure DESC:\n";
        imprimirPrimeros(solicitudes, 10);

        cout << "\nConsultas Binary Search tenure >= k:\n";
        const vector<int> consultas = {72, 60, 45, 30, 12};
        for (int k : consultas)
        {
            const int idx = findFirstTenureGE(solicitudes, k);
            cout << "k=" << k << " -> idx=" << idx << " -> customerID=";
            if (idx != -1)
            {
                cout << solicitudes[idx].customerID;
            }
            else
            {
                cout << "N/A";
            }
            cout << '\n';
        }

        const string salida = "results/solicitudes_ordenadas.csv";
        escribirSolicitudesOrdenadas(salida, solicitudes);
        cout << "\nArchivo escrito: " << salida << '\n';

        const string salidaBusquedas = "results/busquedas_A.txt";
        escribirResultadosBusqueda(salidaBusquedas, solicitudes);
        cout << "Archivo escrito: " << salidaBusquedas << '\n';

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
