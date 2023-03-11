#include "Profiler.h"
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <stdio.h>

using namespace std;

Profiler profiler = Profiler("Rec vs Iter");

struct Node {
    int val;
    Node* left;
    Node* right;
    Node* parent;
};

enum Orders {
    PREORDER,
    INORDER,
    POSTORDER
};

enum Direction {
    LEFT = 1,
    RIGHT,
    UP,
};

bool is_sorted(int* arr, int size = 10000) {
    for (int i = 1; i < 10000; i++) {
        if (arr[i] < arr[i - 1])
            return false;
    }
    return true;
}

void print_array(int* arr, int size = 10000) {
    for (int i = 0; i < size; i++) {
        cout << arr[i] << ' ';
    }
    cout << '\n';
}

void rec(Node* root, Operation& op, Orders order = INORDER, bool perf = true,
         bool is_root = true) {
    if (!root)
        return;

    if (order == PREORDER) {
        if (!perf) {
            cout << root->val << ' ';
        }
        op.count();
    }

    rec(root->left, op, order, perf, false);
    if (order == INORDER) {
        if (!perf) {
            cout << root->val << ' ';
        }
        op.count();
    }

    rec(root->right, op, order, perf, false);
    if (order == POSTORDER) {
        if (!perf) {
            cout << root->val << ' ';
        }
        op.count();
    }

    if (is_root && !perf)
        cout << '\n';
}

void iter(Node* root, Operation& op, bool perf = true, Orders ord = INORDER) {
    int dir = LEFT;
    while (root->parent || dir != UP) {
        if (dir == LEFT) {
            if (ord == PREORDER) {
                if (!perf)
                    cout << root->val << ' ';
                op.count();
            }
            if (root->left)
                root = root->left;
            else {
                dir = RIGHT;
            }
        } else if (dir == RIGHT) {
            if (ord == INORDER) {
                if (!perf)
                    cout << root->val << ' ';
                op.count();
            }
            if (root->right) {
                root = root->right;
                dir = LEFT;
            } else
                dir = UP;
        } else if (dir == UP) {
            if (ord == POSTORDER) {
                if (!perf)
                    cout << root->val << ' ';
                op.count();
            }
            if (root == root->parent->left) {
                dir = RIGHT;
            }
            root = root->parent;
        }
    }

    if (ord == POSTORDER) {

        if (!perf)
            cout << root->val;
        op.count();
    }

    if (!perf)
        cout << '\n';
}

Node* build_tree_helper(int l, int r) {
    Node* root;
    int m = (l + r) / 2;

    root = new Node{m};

    if (l <= m - 1) {
        root->left = build_tree_helper(l, m - 1);
        root->left->parent = root;
    }
    if (m + 1 <= r) {
        root->right = build_tree_helper(m + 1, r);
        root->right->parent = root;
    }

    return root;
}

Node* build_tree(int n) {
    return build_tree_helper(1, n);
}

void clear_tree(Node* root) {
    if (!root)
        return;

    clear_tree(root->left);
    clear_tree(root->right);
    delete root;
}

void perf_tree() {
    for (int i = 100; i <= 10000; i += 100) {
        Operation op_rec = profiler.createOperation("rec_print", i);
        Operation op_iter = profiler.createOperation("iter_print", i);

        Node* root = build_tree(i);

        rec(root, op_rec);
        iter(root, op_iter);

        clear_tree(root);
    }

    profiler.createGroup("rec vs iter", "rec_print", "iter_print");

    profiler.reset("QuickSort Hybridization");
}

void insertion_sort(int* arr, int size, Operation& op, bool counting) {

    int buff;
    for (int i = 1; i < size; i++) {
        int j = i - 1;
        if (counting)
            op.count();
        buff = arr[i];

        while (j >= 0 && buff < arr[j]) {
            if (counting) {
                op.count(2);
            }
            arr[j + 1] = arr[j];
            j--;
        }

        if (j >= 0 && counting)
            op.count();

        if (j + 1 != i) {
            if (counting)
                op.count();
            arr[j + 1] = buff;
        }
    }
}

int partition(int* arr, int l, int r, Operation& op, bool counting) {

    if (counting)
        op.count();

    int x = arr[r];
    int i = l - 1;

    for (int j = l; j < r; j++) {
        if (counting)
            op.count();
        if (arr[j] <= x) {
            i++;
            if (counting)
                op.count(3);
            swap(arr[i], arr[j]);
        }
    }
    if (counting)
        op.count(3);

    swap(arr[i + 1], arr[r]);
    return i + 1;
}

void quick_sort(int* arr, int l, int r, Operation* op, bool counting) {
    if (l < r) {
        int p = partition(arr, l, r, *op, counting);
        quick_sort(arr, l, p - 1, op, counting);
        quick_sort(arr, p + 1, r, op, counting);
    }
}

void hybridized_quick_sort(int* arr, int l, int r, Operation* op, bool counting,
                           int treshold = 10) {
    if (l >= r)
        return;

    int size = r - l + 1;

    if (size <= treshold) {
        insertion_sort(arr + l, size, *op, counting);
    } else {
        int p = partition(arr, l, r, *op, counting);
        hybridized_quick_sort(arr, l, p - 1, op, counting, treshold);
        hybridized_quick_sort(arr, p + 1, r, op, counting, treshold);
    }
}

void quick_sort_helper(int* arr, int size, bool hybridized, bool counting) {
    const int NUMBER_OF_TESTS = 3000;
    int u[10000];

    if (counting) {
        if (hybridized) {
            Operation op = profiler.createOperation("Hyb", size);
            hybridized_quick_sort(arr, 0, size - 1, &op, counting, 10);
        } else {
            Operation op = profiler.createOperation("Classic", size);
            quick_sort(arr, 0, size - 1, &op, counting);
        }
    } else {
        if (hybridized) {
            profiler.startTimer("Hyb_time", size);
            for (int i = 0; i < NUMBER_OF_TESTS; i++) {
                CopyArray(u, arr, size);
                hybridized_quick_sort(u, 0, size - 1, NULL, counting, 10);
            }
            profiler.stopTimer("Hyb_time", size);
        } else {
            profiler.startTimer("Classic_time", size);
            for (int i = 0; i < NUMBER_OF_TESTS; i++) {
                CopyArray(u, arr, size);
                quick_sort(u, 0, size - 1, NULL, counting);
            }
            profiler.stopTimer("Classic_time", size);
        }
    }
}

void perf_hyb() {

    int v[10000], u[10000];

    const int NUMBER_OF_TESTS = 100;
    for (int j = 0; j < NUMBER_OF_TESTS; j++) {
        for (int i = 100; i <= 10000; i += 100) {
            FillRandomArray(v, i);
            CopyArray(u, v, i);

            quick_sort_helper(u, i, true, true);

            FillRandomArray(v, i);
            CopyArray(u, v, i);

            quick_sort_helper(u, i, false, true);
        }
    }
    profiler.divideValues("Hyb", NUMBER_OF_TESTS);
    profiler.divideValues("Classic", NUMBER_OF_TESTS);

    profiler.createGroup("OP classic vs Hyb", "Hyb", "Classic");
    profiler.reset("treshold");
}

void perf_time() {
    int v[10000], u[10000];

    for (int i = 100; i <= 10000; i += 100) {
        FillRandomArray(v, i);
        CopyArray(u, v, i);

        quick_sort_helper(u, i, true, false);

        FillRandomArray(v, i);
        CopyArray(u, v, i);

        quick_sort_helper(u, i, false, false);
    }

    profiler.createGroup("OP classic vs Hyb", "Hyb_time", "Classic_time");

    profiler.showReport();
}

void perf_treshhold() {
    int v[10000], u[10000];
    const int NUMBER_OF_TESTS = 5;
    FillRandomArray(v, 10000);
    CopyArray(u, v, 10000);

    for (int i = 1; i <= 100; i++) {
        Operation op = profiler.createOperation("treshold", i);

        for (int j = 0; j < NUMBER_OF_TESTS; j++) {
            CopyArray(u, v, 10000);
            hybridized_quick_sort(u, 0, 9999, &op, true, i);
        }
    }

    profiler.reset("time");
}

void demo() {
    Node* root = build_tree(10);

    Operation op_rec = profiler.createOperation("unused", 0);
    Operation op_iter = profiler.createOperation("unused", 0);

    rec(root, op_rec, INORDER, false);
    root = build_tree(10);
    iter(root, op_iter, false, INORDER);
    clear_tree(root);

    int v[100];
    int u[100];

    FillRandomArray(v, 10);
    CopyArray(u, v, 10);

    quick_sort_helper(u, 10, true, false);

    print_array(u, 10);

    CopyArray(u, v, 10);

    quick_sort_helper(u, 10, false, false);

    print_array(u, 10);
}

void perf() {
    perf_tree();
    perf_hyb();
    perf_treshhold();
    perf_time();
}

int main() {

    demo();
    // perf();
}
