#include <limits.h>
#include <algorithm>
#include <cstdint>
#include <iostream>
#include "Profiler.h"

using namespace std;

/*
 *  The overall complexity of the algorithm is O(nlogk)
 *
 *  To accurately graph the performance of the algorithm we need to
 *  fix a variable and graph the other
 *  
 *  The graphs should be:
 *      i)  linear O(n)
 *      ii) logarithmic O(log(k))
 *  
 */

Profiler profiler("fixed K");

enum PerfCases {
    K1 = 0,
    K2,
    K3,
    N
};
enum DemoOptions {
    GEN = 0,
    TRUE_RND_GEN,
    HARD_CODED,
    MERGE
};

struct Node {
    int key;
    Node* next;
};

void print_list(Node* head) {
    Node* dummy = head;
    while (dummy) {
        cout << dummy->key;
        if (dummy->next)
            cout << "-> ";
        dummy = dummy->next;
    }
    puts("");
}

void append(Node* &head, int key) {
    Node* new_node = new Node{key, NULL};

    if (!head) {
        head = new_node;
    } else {
        Node* dummy = head;
        while (dummy->next) {
            dummy = dummy->next;
        }
        dummy->next = new_node;
    }
}

void append(Node*& head, Node* new_node) {
    new_node->next = NULL;

    if (!head) {
        head = new_node;
    } else {
        Node* dummy = head;
        while (dummy->next) {
            dummy = dummy->next;
        }
        dummy->next = new_node;
    }
}

Node* merge(Node* a, Node* b, Operation& op) {
    op.count();
    if (!a)
        return b;
    op.count();
    if (!b)
        return a;

    Node* result = NULL;

    op.count(3); // comp
                 // assig of a or b
                 // assig of result-> next
    if (a->key < b->key) {
        result = a;
        result->next = merge(a->next, b, op);
    } else {
        result = b;
        result->next = merge(a, b->next, op);
    }
    return result;
}

void generate_lists(Node* v[], int n, int k, bool true_random = false) {
    int arr[10000] = {0};
    srand(time(NULL));
    int j = 0;

    while (k > 1) {
        int lim = 1 + rand() % (true_random ? (n - k + 1) : (n / k));
        FillRandomArray(arr, lim, 10, 50000, false, ASCENDING);
        for (int i = 0; i < lim; i++) {
            append(v[j], arr[i]);
        }
        j++;
        k--;
        n -= lim;
    }

    FillRandomArray(arr, n, 10, 50000, false, ASCENDING);
    for (int i = 0; i < n; i++) {
        append(v[j], arr[i]);
    }
}

void heapify(Node* arr[], int heap_size, int i, Operation& op) {
    int smallest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < heap_size) {
        op.count();
        if (arr[left]->key < arr[smallest]->key) {
            smallest = left;
        }
    }

    if (right < heap_size) {
        op.count();
        if (arr[right]->key < arr[smallest]->key) {
            smallest = right;
        }
    }

    if (smallest != i) {
        op.count(3);
        swap(arr[i], arr[smallest]);
        heapify(arr, heap_size, smallest, op);
    }
}

void build_heap(Node* arr[], int size, Operation& op) {

    for (int i = size / 2 - 1; i >= 0; i--) {
        heapify(arr, size, i, op);
    }
}

Node* pq_pop(Node* arr[], int& size, Operation& op) {
    op.count(2);
    Node* top = arr[0];
    arr[0] = arr[0]->next;

    op.count();
    if (!arr[0]) {
        op.count(3);
        swap(arr[0], arr[size - 1]);
        size--;
    }

    heapify(arr, size, 0, op);
    return top;
}

bool is_sorted(Node* head) {
    if (!head)
        return true;
    if (!head->next)
        return true;
    Node* curr = head->next;
    Node* prev = head;

    while (curr) {
        if (curr->key < prev->key)
            return false;
        curr = curr->next;
        prev = prev->next;
    }
    return true;
}

Node* merge_k(Node* arr[], int n, int k, Operation& op) {

    Node* result = NULL;
    build_heap(arr, k, op);

    while (k > 0) {
        op.count(2);
        Node* top = pq_pop(arr, k, op);
        if (top) {
            op.count();
            append(result, top);
        }
    }

    return result;
}

void clean(Node* arr[], int size) {
    for (int i = 0; i < size; i++) {
        arr[i] = NULL;
    }
}
void clean(Node* head) {
    Node* curr = head;
    while (curr) {
        Node* aux = curr;
        curr = curr->next;
        delete aux;
    }
}

void demo(DemoOptions option) {
    Operation _op = profiler.createOperation("unused", 0);
    switch (option) {
    case TRUE_RND_GEN: {
        Node* v[5] = {NULL};

        generate_lists(v, 15, 3, true);
        for (int i = 0; i < 3; i++) {
            print_list(v[i]);
        }
        puts("");

        int k = 3;
        Node* result = merge_k(v, 15, k, _op);
        puts((is_sorted(result)) ? "List is sorted" : "List is not sorted");
        print_list(result);
    } break;
    case GEN: {
        Node* v[5] = {NULL};

        generate_lists(v, 15, 3);
        for (int i = 0; i < 3; i++) {
            print_list(v[i]);
        }

        puts("");

        int k = 3;
        Node* result = merge_k(v, 15, k, _op);
        puts((is_sorted(result)) ? "List is sorted" : "List is not sorted");
        print_list(result);
    } break;
    case HARD_CODED: {

        Node* arr[3] = {NULL};
        int k = 3;
        arr[0] = new Node{
            0, new Node{5, new Node{7, new Node{9, new Node{42, NULL}}}}};
        arr[1] = new Node{2, new Node{3, new Node{12, new Node{18, NULL}}}};
        arr[2] = new Node{4, new Node{6, new Node{24, NULL}}};
        Node* result = merge_k(arr, 12, k, _op);

        print_list(result);
        puts("");
        puts((is_sorted(result)) ? "List is sorted" : "List is not sorted");
    } break;
    case MERGE: {
                    
        Node* l1 = new Node{0, new Node{5, new Node{7, new Node{9, new Node{42, NULL}}}}};
        Node* l2 = new Node{2, new Node{3, new Node{12, new Node{18, NULL}}}};

        print_list(merge(l1, l2, _op));
    } break;
    }
}

void perf(PerfCases Case) {
    const int MAX_N = 10000;
    const int MAX_K = 500;
    const int N_STEP = 100;
    const int K_STEP = 10;
    const int NUMBER_OF_TESTS = 10;
    int k_arr[] = {5, 10, 100};
    Node* v[MAX_K] = {NULL};

    if (Case >= K1 && Case <= K3) {
        for (int j = 0; j < NUMBER_OF_TESTS; j++) {
            for (int i = 100; i <= MAX_N; i += N_STEP) {
                Operation op[] = {
                    profiler.createOperation("k1", i),
                    profiler.createOperation("k2", i),
                    profiler.createOperation("k3", i)
                };
                generate_lists(v, i, k_arr[Case]);
                Node* result = merge_k(v, i, k_arr[Case], op[Case]);
                clean(v, k_arr[Case]);
            }
        }
    } else {
        for (int j = 0; j < NUMBER_OF_TESTS; j++) {
            for (int i = 10; i <= MAX_K; i += K_STEP) {
                int k = i;
                generate_lists(v, 10000, k);
                Operation op = profiler.createOperation("N", i);
                Node* result = merge_k(v, 10000, k, op);
                clean(v, i);
            }
        }
    }
    profiler.createGroup("Fixed k", "k1", "k2", "k3");

    if (Case == N)
        profiler.divideValues("op", NUMBER_OF_TESTS);
}

int main() {
    perf(K1);
    perf(K2);
    perf(K3);
    profiler.reset("fixed N");
    perf(N);
    profiler.showReport();
    /*
    demo(GEN);
    cout << std::string(16, '-') << '\n';
    demo(TRUE_RND_GEN);
    cout << std::string(16, '-') << '\n';
    demo(HARD_CODED);
    cout << std::string(16, '-') << '\n';
    demo(MERGE);
     */
}
