#include "mergesort.hpp"

#include <cstddef>
#include <vector>

using namespace std;

namespace {

void merge(vector<Solicitud>& solicitudes,
           vector<Solicitud>& auxiliar,
           size_t izquierda,
           size_t medio,
           size_t derecha) {
    size_t i = izquierda;
    size_t j = medio;
    size_t k = izquierda;

    while (i < medio && j < derecha) {
        if (solicitudes[i].tenure >= solicitudes[j].tenure) {
            auxiliar[k] = solicitudes[i];
            ++i;
        } else {
            auxiliar[k] = solicitudes[j];
            ++j;
        }
        ++k;
    }

    while (i < medio) {
        auxiliar[k] = solicitudes[i];
        ++i;
        ++k;
    }

    while (j < derecha) {
        auxiliar[k] = solicitudes[j];
        ++j;
        ++k;
    }

    for (size_t pos = izquierda; pos < derecha; ++pos) {
        solicitudes[pos] = auxiliar[pos];
    }
}

void mergeSortRec(vector<Solicitud>& solicitudes,
                  vector<Solicitud>& auxiliar,
                  size_t izquierda,
                  size_t derecha) {
    if (derecha - izquierda <= 1) {
        return;
    }

    const size_t medio = izquierda + (derecha - izquierda) / 2;
    mergeSortRec(solicitudes, auxiliar, izquierda, medio);
    mergeSortRec(solicitudes, auxiliar, medio, derecha);
    merge(solicitudes, auxiliar, izquierda, medio, derecha);
}

}

void mergeSortSolicitudes(vector<Solicitud>& solicitudes) {
    vector<Solicitud> auxiliar(solicitudes.size());
    mergeSortRec(solicitudes, auxiliar, 0, solicitudes.size());
}
