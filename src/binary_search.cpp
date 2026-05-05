#include "binary_search.hpp"

#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#endif

#include <fstream>
#include <iomanip>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;

namespace {

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

int binarySearchRec(const vector<Solicitud>& v, int left, int right, int k) {
    if (left > right) {
        return -1;
    }

    const int mid = left + (right - left) / 2;

    if (v[mid].tenure >= k) {
        const int candidate = binarySearchRec(v, left, mid - 1, k);
        if (candidate != -1) {
            return candidate;
        }
        return mid;
    }

    return binarySearchRec(v, left, mid - 1, k);
}

}

int findFirstTenureGE(const vector<Solicitud>& v, int k) {
    if (v.empty()) {
        return -1;
    }

    return binarySearchRec(v, 0, static_cast<int>(v.size()) - 1, k);
}

void escribirResultadosBusqueda(const string& path,
                                const vector<Solicitud>& v) {
    crearDirectorioSiHaceFalta(obtenerDirectorioPadre(path));

    ofstream archivo(path);
    if (!archivo.is_open()) {
        throw runtime_error("No se pudo crear el archivo de busquedas: " + path);
    }

    const vector<int> consultas = {72, 60, 45, 30, 12};
    for (size_t i = 0; i < consultas.size(); ++i) {
        const int k = consultas[i];
        const int idx = findFirstTenureGE(v, k);
        const string customerID = idx != -1 ? v[static_cast<size_t>(idx)].customerID : "N/A";

        archivo << "Q_A" << setw(2) << setfill('0') << (i + 1)
                << " k=" << k
                << " → idx=" << idx
                << " → customerID=" << customerID
                << '\n';
    }
}
