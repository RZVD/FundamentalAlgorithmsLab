/**
 * @author Fuca Razvan-Ionut
 * @group 30422
 *
 * Averege, best and worst case for all 4 algorithms (except selection) means
 *Unsorted, Sorted ascendingly and Sorted descendingly (respectively)
 *
 * Worst case for selection means sorted ascendingly with the minimum element at
 *the end
 *
 * In the averege case bubble sort performs the worst, being followed (in that
 *order) by selection, insertion and binary insertion as being the most
 *efficient
 *
 * In the best case Selection sort peforms the worst, followed by
 * Binary insertion, insertion and bubble as the most efficient
 *
 * In the worst case Bubble Sort performs the worst, followed by insertion,
 *binary insertion and selection
 *
 * All the algorithms have a time complexity of O(n^2)
 *
 **/

#include "Profiler.h"
#include <iostream>

using namespace std;

enum Cases { AVEREGE = 0, BEST, WORST };

enum Algorithms {
    BUBBLE_SORT = 0,
    LINEAR_INSERTION_SORT,
    BINARY_INSERTION_SORT,
    SELECTION_SORT
};

Profiler profiler("AVEREGE");

void print_array(int* arr, int size) {
    for (int i = 0; i < size; i++) {
        cout << arr[i] << ' ';
    }
    cout << '\n';
}

void bubble_sort_do_while(int* arr, int size) {
    Operation comp = profiler.createOperation("bubble_comp", size);
    Operation assig = profiler.createOperation("bubble_assig", size);

    bool sorted = true;
    do {
        sorted = true;
        for (int i = 0; i < size - 1; i++) {
            comp.count();
            if (arr[i] > arr[i + 1]) {
                assig.count(3);
                swap(arr[i], arr[i + 1]);
                sorted = false;
            }
        }
        size--;
    } while (!sorted);
}

void bubble_sort_classic(int* arr, int size) {
    for (int i = 0; i < size - 1; i++) {
        bool swapped = false;
        for (int j = 0; j < size - 1 - i; j++) {
            if (arr[j] > arr[j + 1]) {
                swap(arr[j], arr[j + 1]);
                swapped = true;
            }
        }
        if (!swapped)
            break;
    }
}

void selection_sort(int* arr, int size) {
    Operation comp = profiler.createOperation("selection_comp", size);
    Operation assig = profiler.createOperation("selection_assig", size);

    for (int i = 0; i < size - 1; i++) {
        int min_idx = i;
        for (int j = i + 1; j < size; j++) {
            comp.count();
            if (arr[min_idx] > arr[j]) {
                min_idx = j;
            }
        }

        if (i != min_idx) {
            assig.count(3);
            swap(arr[i], arr[min_idx]);
        }
    }
}

void insertion_sort(int* arr, int size) {
    Operation comp = profiler.createOperation("ins_comp", size);
    Operation assig = profiler.createOperation("ins_assig", size);

    int buff;
    for (int i = 1; i < size; i++) {
        int j = i - 1;
        assig.count();
        buff = arr[i];

        while (j >= 0 && buff < arr[j]) {
            comp.count();

            assig.count();
            arr[j + 1] = arr[j];
            j--;
        }

        if (j >= 0)
            comp.count();

        if (j + 1 != i) {
            assig.count();
            arr[j + 1] = buff;
        }
    }
}

int binary_search(int* arr, int left, int right, int target, Operation& comp,
                  Operation& assig) {
    while (right > left) {
        int mid = (left + right) / 2;

        if (target == arr[mid]) {
            comp.count();
            while (arr[mid] == target) { // stability
                comp.count();
                mid++;
            }
            return mid;
        } else if (target > arr[mid]) {
            comp.count(2);
            left = mid + 1;
        } else {
            comp.count(2);
            right = mid - 1;
        }
    }

    if (arr[left] < target) {
        return left + 1;
    }

    return left;
}

void binary_insertion_sort(int* arr, int size) {
    Operation comp = profiler.createOperation("bin_ins_comp", size);
    Operation assig = profiler.createOperation("bin_ins_assig", size);

    int buff;

    for (int i = 0; i < size; i++) {
        int j = i - 1;

        assig.count();
        buff = arr[i];

        assig.count();
        int location = binary_search(arr, 0, j, buff, comp, assig);

        while (j >= location) {
            assig.count();
            arr[j + 1] = arr[j];
            j--;
        }

        assig.count();
        arr[j + 1] = buff;
    }
}

void demo(Algorithms algorithm) {
    int arr[] = {78, 59, 69, 52, 58, 62, 88, 89, 92, 12};
    int size = sizeof(arr) / sizeof(arr[0]);

    switch (algorithm) {
    case BUBBLE_SORT:
        bubble_sort_do_while(arr, size);
        break;
    case LINEAR_INSERTION_SORT:
        insertion_sort(arr, size);
        break;
    case BINARY_INSERTION_SORT:
        binary_insertion_sort(arr, size);
        break;
    case SELECTION_SORT:
        selection_sort(arr, size);
        break;
    default:
        exit(1);
    }

    print_array(arr, size);
}

void perf(Cases Case) {
    const int MAX_S = 1000;
    const int STEP = 100;
    const int NUMBER_OF_TESTS = 5;
    int v[MAX_S], u[MAX_S];

    for (int j = 1; j <= NUMBER_OF_TESTS; j++) {
        for (int i = 100; i <= MAX_S; i += STEP) {
            switch (Case) {
            case AVEREGE:
                FillRandomArray(v, i, 10, 100000, false, UNSORTED);
                break;
            case BEST:
                FillRandomArray(v, i, 10, 100000, false, ASCENDING);
                break;
            case WORST:
                FillRandomArray(v, i, 10, 100000, false, DESCENDING);
                break;
            default:
                cout << "Error\n";
                exit(1);
            }

            CopyArray(u, v, i);
            insertion_sort(u, i);

            CopyArray(u, v, i);
            bubble_sort_do_while(u, i);

            CopyArray(u, v, i);
            if (Case == WORST) {
                std::sort(u, u + i);
                swap(u[0], u[i - 1]);
                u[0] = u[1];
            }

            selection_sort(u, i);

            CopyArray(u, v, i);
            binary_insertion_sort(u, i);
        }
        if (Case != AVEREGE)
            break;
    }
    if (Case == AVEREGE) {
        profiler.divideValues("ins_assig", NUMBER_OF_TESTS);
        profiler.divideValues("ins_comp", NUMBER_OF_TESTS);
        profiler.divideValues("bubble_assig", NUMBER_OF_TESTS);
        profiler.divideValues("bubble_comp", NUMBER_OF_TESTS);
        profiler.divideValues("selection_assig", NUMBER_OF_TESTS);
        profiler.divideValues("selection_comp", NUMBER_OF_TESTS);
        profiler.divideValues("bin_ins_comp", NUMBER_OF_TESTS);
        profiler.divideValues("bin_ins_assig", NUMBER_OF_TESTS);
    }

    profiler.addSeries("ins_op", "ins_assig", "ins_comp");
    profiler.addSeries("bubble_op", "bubble_assig", "bubble_comp");
    profiler.addSeries("selection_op", "selection_assig", "selection_comp");
    profiler.addSeries("bin_ins_op", "bin_ins_assig", "bin_ins_comp");

    profiler.createGroup("op", "ins_op", "selection_op", "bubble_op",
                         "bin_ins_op");
    profiler.createGroup("comp", "ins_comp", "selection_comp", "bubble_comp",
                         "bin_ins_comp");
    profiler.createGroup("assig", "ins_assig", "selection_assig",
                         "bubble_assig", "bin_ins_assig");
}

int main() {
    perf(AVEREGE);
    profiler.reset("BEST");
    perf(BEST);
    profiler.reset("WORST");
    perf(WORST);
    profiler.showReport();

    /*
    demo(BUBBLE_SORT);
    demo(SELECTION_SORT);
    demo(LINEAR_INSERTION_SORT);
    demo(BINARY_INSERTION_SORT);
    */

    return 0;
}
