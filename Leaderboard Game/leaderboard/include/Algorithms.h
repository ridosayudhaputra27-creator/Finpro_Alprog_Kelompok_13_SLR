#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include "RegisteredPlayer.h"

// ============================================================
// ALGORITMA MANUAL — tanpa fungsi library bawaan
// ============================================================

// ------------------------------------------------------------
// MERGE SORT — mengurutkan array Player* berdasarkan score DESC
// Kompleksitas Waktu : O(n log n) — semua kasus (best/avg/worst)
// Kompleksitas Ruang : O(n)      — array bantu saat merge
// ------------------------------------------------------------
namespace SortAlgo {

    void merge(RegisteredPlayer** arr, int left, int mid, int right) {
        int n1 = mid - left + 1;
        int n2 = right - mid;

        RegisteredPlayer** L = new RegisteredPlayer*[n1];
        RegisteredPlayer** R = new RegisteredPlayer*[n2];

        for (int i = 0; i < n1; i++) L[i] = arr[left + i];
        for (int j = 0; j < n2; j++) R[j] = arr[mid + 1 + j];

        int i = 0, j = 0, k = left;
        // Merge DESCENDING (skor terbesar di depan)
        while (i < n1 && j < n2) {
            if (L[i]->getScore() >= R[j]->getScore()) {
                arr[k++] = L[i++];
            } else {
                arr[k++] = R[j++];
            }
        }
        while (i < n1) arr[k++] = L[i++];
        while (j < n2) arr[k++] = R[j++];

        delete[] L;
        delete[] R;
    }

    // Kompleksitas: O(n log n)
    void mergeSort(RegisteredPlayer** arr, int left, int right) {
        if (left >= right) return;
        int mid = left + (right - left) / 2;
        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }

} // namespace SortAlgo


// ------------------------------------------------------------
// BINARY SEARCH — mencari player berdasarkan score
// PRE-CONDITION: array harus sudah terurut DESCENDING
// Kompleksitas Waktu : O(log n) — array terurut
// Kompleksitas Ruang : O(1)    — iteratif, tanpa rekursi
// ------------------------------------------------------------
namespace SearchAlgo {

    // Mengembalikan index pertama yang ditemukan, -1 jika tidak ada
    int binarySearchByScore(RegisteredPlayer** arr, int size, int targetScore) {
        int low = 0, high = size - 1;

        while (low <= high) {
            int mid = low + (high - low) / 2;
            int midScore = arr[mid]->getScore();

            if (midScore == targetScore) {
                return mid;
            } else if (midScore > targetScore) {
                low = mid + 1; // array descending: nilai lebih kecil di kanan
            } else {
                high = mid - 1;
            }
        }
        return -1;
    }

    // Linear Search berdasarkan username — O(n)
    // Kompleksitas Waktu : O(n)
    // Kompleksitas Ruang : O(1)
    int linearSearchByName(RegisteredPlayer** arr, int size, const std::string& name) {
        for (int i = 0; i < size; i++) {
            if (arr[i]->getUsername() == name) return i;
        }
        return -1;
    }

} // namespace SearchAlgo

#endif
