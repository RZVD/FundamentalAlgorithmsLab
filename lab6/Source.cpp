#include "Profiler.h"
#include <cstdio>
#include <iostream>
using namespace std;

/*
 *
 */

struct R2 {
    int val;
    vector<R2*> children;
};

struct R3 {
    int val;
    R3* first_child;
    R3* next_sibling;
};

void pretty_print(int* parents, int size, int target = -1, int level = 0) {
    for (int i = 0; i < size; i++) {
        if (parents[i] == target) {
            cout << string(level * 4, ' ') << i + 1 << '\n';
            pretty_print(parents, size, i + 1, level + 1);
        }
    }
}

void pretty_print(R2* root, int level = 0) {
    if (!root) {
        return;
    }
    cout << string(level * 4, ' ') << root-> val << '\n';
    for (R2* child : root-> children) {
        pretty_print(child, level + 1);
    }
}

void pretty_print(R3* root, int level = 0) {
    while (root) {
        cout << string(level * 4, ' ') << root-> val << '\n';
        pretty_print(root-> first_child, level + 1);
        root = root-> next_sibling;
    }
}

auto T2(int* parents, int size) -> R2* {
    R2* root = NULL;
    R2* nodes[size];

    for (int i = 0; i < size; i++) {
        nodes[i] = new R2{i + 1};
    }

    for (int i = 0; i < size; i++) {
        if (parents[i] == -1)
            root = nodes[i];
        else
            nodes[parents[i] - 1]-> children.push_back(nodes[i]);
    }

    return root;
}

auto T3(R2* root) -> R3* {
    if (!root)
        return NULL;

    R3* ans = new R3{root-> val};

    if (root-> children.size()) {
        ans-> first_child = T3(root-> children[0]);
    }

    R3* aux = ans-> first_child;
    for (int i = 1; i < root-> children.size(); ++i) {
        aux-> next_sibling = T3(root-> children[i]);
        aux = aux-> next_sibling;
    }
    return ans;
}

void perf() {
    puts("R1:");
    int parents[] = {2, 7, 5, 2, 7, 7, -1, 5, 2};
    //int parents[] = {-1, 1, 3, 1, };
    int size = sizeof(parents) / sizeof(parents[0]);
    pretty_print(parents, size);

    puts("\n\nR2:");
    R2* multiway = T2(parents, size);
    pretty_print(multiway);

    multiway->children.push_back(NULL);


    puts("\n\nR3:");
    R3* binary = T3(multiway);
    pretty_print(binary);
}

void demo() {
    R3* one = new R3{1};
    R3* two = new R3{2};
    R3* three = new R3{3};
    R3* four = new R3{4};
    R3* five = new R3{5};
    R3* six = new R3{6};
    R3* seven = new R3{7};
    R3* eight = new R3{8};
    R3* nine = new R3{9};

    seven-> first_child = two;
    two-> next_sibling = five;
    five-> next_sibling = six;

    two-> first_child = one;
    one-> next_sibling = four;
    four-> next_sibling = nine;
    five-> first_child = three;
    three-> next_sibling = eight;

    pretty_print(seven);

    
}

int main() {
    perf();
    // demo();

    return 0;
}
