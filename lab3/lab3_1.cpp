#include <cstdlib>
#include <ctime>
#include <limits.h>
#include <cstdio>
#include <iostream>
#include "Profiler.h"

using namespace std;

/*
 * Altough QuickSort is not an optimal algorithm like HeapSort (O(n^2) !=
 * Omega(nlgn)) It usually beats heapsort in practice Entering a worst case
 * scenario for quicksort (unbalanced partitions) can be avoided relatively
 * easily by selecting a random pivot, effectively making the worst and averege
 * cases perform exactly the same
 *
 *
 *  QuickSelect finds the i-th element in a would-be sorted array
 *  By repeatedly partitioning the array
 *  This is achieved in O(n)
 *
 * */

enum Cases {
    AVEREGE = 0,
    WORST,
    BEST
};

enum DemoOptions {
    QUICKSORT,
    HEAPSORT,
    QUICKSELECT
};

Profiler profiler = Profiler("Averege");

void print_array(int* arr, int size) {
    for (int i = 0; i < size; i++) {
        cout << arr[i] << ' ';
    }
    cout << '\n';
}

int partition(int* arr, int l, int r, Operation& op) {

    op.count();
    int x = arr[r];
    int i = l - 1;

    for (int j = l; j < r; j++) {
        op.count();
        if (arr[j] <= x) {
            i++;
            op.count(3);
            swap(arr[i], arr[j]);
        }
    }
    op.count(3);
    swap(arr[i + 1], arr[r]);
    return i + 1;
}

void quick_sort(int* arr, int l, int r, Operation& op) {
    if (l < r) {
        int p = partition(arr, l, r, op);
        quick_sort(arr, l, p - 1, op);
        quick_sort(arr, p + 1, r, op);
    }
}

int randomized_partition(int* arr, int l, int r, Operation& op) {
    srand(time(NULL));
    int i = l + rand() % (r - l);

    op.count(3);
    swap(arr[r], arr[i]);
    return partition(arr, l, r, op);
}

void randomized_quick_sort(int* arr, int l, int r, Operation& op) {
    if (l < r) {
        int p = randomized_partition(arr, l, r, op);
        randomized_quick_sort(arr, l, p - 1, op);
        randomized_quick_sort(arr, p + 1, r, op);
    }
}

void quick_sort_helper(int* arr, int size, Cases Case = AVEREGE) {
    Operation op = profiler.createOperation("QS_op", size);

    if (Case == BEST) {
        randomized_quick_sort(arr, 0, size - 1, op);
    } else {
        quick_sort(arr, 0, size - 1, op);
    }
}

void heapify(int* arr, int heap_size, int i, Operation& op) {

    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < heap_size) {
        op.count();
        if (arr[left] > arr[largest]) {
            largest = left;
        }
    }

    if (right < heap_size) {
        op.count();
        if (arr[right] > arr[largest]) {
            largest = right;
        }
    }

    if (largest != i) {
        op.count(3);

        swap(arr[i], arr[largest]);
        heapify(arr, heap_size, largest, op);
    }
}
void build_heap_BU(int* arr, int size, Operation& op) {

    for (int i = size / 2 - 1; i >= 0; i--) {
        heapify(arr, size, i, op);
    }
}
void heap_sort(int* arr, int size) {
    Operation op = profiler.createOperation("heapsort_op", size);
    build_heap_BU(arr, size, op);

    for (int i = size - 1; i > 0; i--) {
        swap(arr[0], arr[i]);
        heapify(arr, i, 0, op);
    }
}

int quick_select(int* arr, int l, int r, int i) {
    Operation _op = profiler.createOperation("unused_op", 0);

    if (l == r)
        return arr[l];

    int p = randomized_partition(arr, l, r, _op);
    int k = p - l + 1;

    if (i == k) {
        return arr[p];
    } else if (i < k) {
        return quick_select(arr, l, p - 1, i);
    }
    return quick_select(arr, p + 1, r, i - k);
}

void demo(DemoOptions algo) {
    int arr[] = {4, 5, 7, 2, 3, -32, 145, 12, 6};
    int size = sizeof(arr) / sizeof(arr[0]);
    switch (algo) {
    case QUICKSORT:
        cout << "QuickSort:\n";
        quick_sort_helper(arr, size, BEST);
        break;
    case HEAPSORT:
        cout << "HeapSort:\n";
        heap_sort(arr, size);
        break;
    case QUICKSELECT:
        cout << "QuickSelect(arr, 8):\n"
             << quick_select(arr, 0, size - 1, 8) << '\n';
        break;
    default:
        cout << "Err\n";
        exit(1);
        break;
    }
    if (algo != QUICKSELECT)
        print_array(arr, size);
}

void perf(Cases Case) {
    const int MAX_S = 10000;
    const int STEP = 100;
    const int NUMBER_OF_TESTS = 5;

    int v[MAX_S], u[MAX_S];
    for (int j = 0; j < NUMBER_OF_TESTS; j++) {
        for (int i = 100; i <= MAX_S; i += STEP) {
            switch (Case) {
            case BEST:
            case AVEREGE:
                FillRandomArray(v, i, 10, 50000, false, UNSORTED);
                break;
            case WORST:
                FillRandomArray(v, i, 10, 50000, false, ASCENDING);
                break;
            default:
                cout << "Err\n";
                return;
            }
            CopyArray(u, v, i);
            quick_sort_helper(u, i, Case);

            CopyArray(u, v, i);

            if (Case == BEST) {
                std::sort(u, u + i, greater<int>());
            }
            heap_sort(u, i);
        }

        if (Case != AVEREGE)
            break;
    }
    if (Case == AVEREGE) {
        profiler.divideValues("heapsort_op", NUMBER_OF_TESTS);
        profiler.divideValues("QS_op", NUMBER_OF_TESTS);
    }

    profiler.createGroup("op", "QS_op", "heapsort_op");
}

int main() {

    demo(QUICKSORT);
    demo(HEAPSORT);
    demo(QUICKSELECT);

    /*
    perf(AVEREGE);
    profiler.reset("WORST");
    perf(WORST);
    profiler.reset("BEST");
    perf(BEST);
    profiler.showReport();
    */
    return 0;
}
