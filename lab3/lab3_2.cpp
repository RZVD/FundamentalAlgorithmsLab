#include "Profiler.h"
#include <cstdio>
#include <iostream>

using namespace std;

/*
 *  Both implementations of insertionSort have relatively the same number of operations
 *  But the recursive implementation is less efficient because of having to utilize
 *  More function calls and use the Stack more often
 *
 * */

Profiler profiler = Profiler("Rec VS Iter");

enum Algos {
    ITERATIVE = 0,
    RECURSIVE
};

void print_array(int* arr, int size) {
    for (int i = 0; i < size; i++) {
        cout << arr[i] << ' ';
    }
    cout << '\n';
}

void insertion_sort_iterative(int* arr, int size, bool counting = true) {
    Operation op = profiler.createOperation("iter_op", size);

    int buff;

    for (int i = 1; i < size; i++) {
        int j = i - 1;
        if (counting)
            op.count();

        buff = arr[i];

        while (j >= 0 && buff < arr[j]) {
            if (counting) {
                op.count();
                op.count();
            }

            arr[j + 1] = arr[j];
            j--;
        }
        if (counting && j >= 0)
            op.count();

        if (j + 1 != i) {
            if (counting)
                op.count();

            arr[j + 1] = buff;
        }
    }
}

void insertion_sort_recusive(int* arr, int size, Operation& op,
                             bool counting = true) {
    if (size <= 1)
        return;

    insertion_sort_recusive(arr, size - 1, op);

    if (counting)
        op.count();

    int buff = arr[size - 1];

    int j = size - 2;

    while (j >= 0 && arr[j] > buff) {
        if (counting) {
            op.count();
            op.count();
        }
        arr[j + 1] = arr[j];
        j--;
    }
    if (counting && j >= 0)
        op.count();

    if (j + 1 != size) {
        if (counting)
            op.count();

        arr[j + 1] = buff;
    }
}

void insertion_sort_iterative_helper(int* arr, int size, bool counting = true) {
    if (!counting) {
        profiler.startTimer("iter", size);
        for (int i = 0; i < 3000; i++)
            insertion_sort_iterative(arr, size, false);

        profiler.stopTimer("iter", size);
    } else
        insertion_sort_iterative(arr, size, true);
}

void insertion_sort_recusive_helper(int* arr, int size, bool counting = true) {
    Operation op = profiler.createOperation("rec_op", size);

    if (!counting) {
        profiler.startTimer("rec", size);
        for (int i = 0; i < 3000; i++) {
            insertion_sort_recusive(arr, size, op, counting);
        }
        profiler.stopTimer("rec", size);
    } else {
        insertion_sort_recusive(arr, size, op, counting);
    }
}

void demo(Algos algo) {
    int arr[] = {4, 5, 7, 2, 3, -32, 145, 12, 6};
    int size = sizeof(arr) / sizeof(arr[0]);

    Operation _op = profiler.createOperation("unused", size);

    if (algo == ITERATIVE) {
        cout << "Iterative:\n";
        insertion_sort_iterative(arr, size);
    } else {
        cout << "Recursive:\n";
        insertion_sort_recusive(arr, size, _op);
    }

    print_array(arr, size);
}

void perf() {
    const int MAX_S = 10000;
    const int STEP = 100;

    int v[MAX_S], u[MAX_S];
    for (int i = 100; i <= MAX_S; i += STEP) {
        FillRandomArray(v, i, 10, 50000, false, UNSORTED);

        CopyArray(u, v, i);
        insertion_sort_iterative_helper(u, i, false);

        CopyArray(u, v, i);
        insertion_sort_recusive_helper(u, i, false);

        CopyArray(u, v, i);
        insertion_sort_iterative_helper(u, i);

        CopyArray(u, v, i);
        insertion_sort_recusive_helper(u, i);
    }

    profiler.createGroup("op", "iter_op", "rec_op");
    profiler.createGroup("time", "rec", "iter");
}

int main() {
    demo(ITERATIVE);
    demo(RECURSIVE);
    /*
    perf();
    profiler.showReport();
    */
}
