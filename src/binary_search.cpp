#include "binary_search.hpp"

#include <vector>

using namespace std;

namespace {

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
