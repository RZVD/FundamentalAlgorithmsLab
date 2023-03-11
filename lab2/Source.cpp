#include <algorithm>
#include <array>
#include <limits.h>
#include <cstdio>
#include <iostream>
#include <tuple>
#include "Profiler.h"
#include "algorithm"

using namespace std;

enum Cases {
    AVEREGE = 0,
    WORST
};

enum DemoOptions {
    BU = 0,
    TD,
    HeapSort
};

Profiler profiler("Averege");

void print_array(int* arr, int size) {
    for (int i = 0; i < size; i++) {
        cout << arr[i] << " ";
    }
    cout << '\n';
}

void heapify(int* arr, int heap_size, int i, Operation& comp,
             Operation& assig) {

    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < heap_size) {
        comp.count();
        if (arr[left] > arr[largest]) {
            largest = left;
        }
    }

    if (right < heap_size) {
        comp.count();
        if (arr[right] > arr[largest]) {
            largest = right;
        }
    }

    if (largest != i) {
        assig.count(3);

        swap(arr[i], arr[largest]);
        heapify(arr, heap_size, largest, comp, assig);
    }
}

void build_heap_BU(int* arr, int size) {

    Operation comp = profiler.createOperation("BU_comp", size);
    Operation assig = profiler.createOperation("BU_assig", size);

    for (int i = size / 2 - 1; i >= 0; i--) {
        heapify(arr, size, i, comp, assig);
    }
}

void heap_increase_key(int* arr, int i, int key, Operation& comp,
                       Operation& assig) {

    comp.count();
    if (key < arr[i]) {
        return;
    }
    assig.count();
    arr[i] = key;

    while (i > 0 && arr[(i - 1) / 2] < arr[i]) {
        comp.count();

        assig.count(3);
        swap(arr[i], arr[(i - 1) / 2]);

        i = (i - 1) / 2;
    }
}

void heap_insert(int* arr, int& heap_size, int key, Operation& comp,
                 Operation& assig) {
    heap_size++;
    assig.count();
    arr[heap_size] = INT_MIN;
    heap_increase_key(arr, heap_size, key, comp, assig);
}

void build_heap_TD(int* arr, int size) {
    Operation comp = profiler.createOperation("TD_comp", size);
    Operation assig = profiler.createOperation("TD_assig", size);

    int heap_size = 0;
    for (int i = 1; i < size; i++) {
        heap_insert(arr, heap_size, arr[i], comp, assig);
    }
}

void heap_sort(int* arr, int size) {
    Operation _comp = profiler.createOperation("heapsort_comp", size);
    Operation _assig = profiler.createOperation("heapsort_assig", size);
    build_heap_BU(arr, size);

    for (int i = size - 1; i > 0; i--) {
        swap(arr[0], arr[i]);
        heapify(arr, i, 0, _comp, _assig);
    }
}

void demo(DemoOptions option) {
    int arr[] = {4, 5, 7, 2, 3, -32, 145, 12, 6};
    int size = sizeof(arr) / sizeof(arr[0]);

    switch (option) {
    case BU:
        build_heap_BU(arr, size);
        cout << "BU: \n";
        break;
    case TD:
        build_heap_TD(arr, size);
        cout << "TD:\n";
        break;
    case HeapSort:
        heap_sort(arr, size);
        cout << "HeapSort:\n";
        break;
    default:
        break;
    }

    print_array(arr, size);
}

void perf(Cases Case) {
    const int MAX_S = 10000;
    const int STEP = 100;
    const int NUMBER_OF_TESTS = 5;
    int v[MAX_S], u[MAX_S];
    for (int j = 0; j < NUMBER_OF_TESTS; j++) {

        for (int i = 100; i <= MAX_S; i += STEP) {
            if (Case == AVEREGE) {
                FillRandomArray(v, i, 10, 50000, false, UNSORTED);
            } else {
                FillRandomArray(v, i, 10, 50000, false, ASCENDING);
            }

            CopyArray(u, v, i);
            build_heap_BU(u, i);

            CopyArray(u, v, i);
            build_heap_TD(u, i);
        }

        if (Case != AVEREGE)
            break;
    }
    if (Case == AVEREGE) {
        profiler.divideValues("BU_assig", NUMBER_OF_TESTS);
        profiler.divideValues("BU_comp", NUMBER_OF_TESTS);
        profiler.divideValues("TD_assig", NUMBER_OF_TESTS);
        profiler.divideValues("TD_comp", NUMBER_OF_TESTS);
    }

    profiler.addSeries("BU_op", "BU_assig", "BU_comp");
    profiler.addSeries("TD_op", "TD_assig", "TD_comp");

    profiler.createGroup("op", "BU_op", "TD_op");
    profiler.createGroup("comp", "BU_comp", "TD_comp");
    profiler.createGroup("assig", "BU_assig", "TD_assig");
}

int main() {
    /*
    perf(AVEREGE);
    profiler.reset("WORST");
    perf(WORST);
    profiler.showReport();
     */

    demo(BU);
    demo(TD);
    demo(HeapSort);
    return 0;
}
