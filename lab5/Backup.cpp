#include <random>
#include "Profiler.h"
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <limits.h>
#include <iostream>
#include <string.h>
using namespace std;

/**
 *
 */

struct Entry {
    int id;
    char* name;
};

void print_entry(Entry* e) {
    if (!e) {
        puts("NO");
        return;
    }
    if (e->id == -1) {
        puts("NO");
        return;
    }
    printf("%d\t%s\n", e->id, e->name);
}

int h(Entry* e, int i, int size = 10007) {
    const int c1 = 3;
    const int c2 = 5;
    return (e->id + c1 * i + c2 * i * i) % size;
}

int h(int e, int i, int size = 10007) {
    const int c1 = 3;
    const int c2 = 5;
    return (e + c1 * i + c2 * i * i) % size;
}

int hash_insert(Entry* T[], Entry* k, int* effort_counter, int size = 10007) {
    int i = 0;
    effort_counter = &i;
    do {
        int j = h(k, i);
        if (!T[j]) {
            T[j] = k;
            return j;
        } else if (T[j]->id == -1) {
            delete T[j];
            T[j] = k;
        } else {
            i++;
        }
    } while (i < size);
    return -1;
}

int hash_search(Entry* T[], Entry* k, int& effort_counter, int size = 10007) {
    int i = 0;
    int j;
    do {
        effort_counter = i + 1;
        j = h(k, i);
        if (T[j])
            if (T[j]->id == k->id) {
                return j;
            }
        i++;
    } while ((T[j] && i < size));
    return -1;
}

int hash_search(Entry* T[], int k, int& effort_counter, int size = 10007) {
    int i = 0;
    int j;
    do {
        effort_counter = i + 1;
        j = h(k, i);
        if (T[j]) {
            if (T[j]->id == k) {
                return j;
            }
        }
        i++;
    } while (T[j] && i < size);
    return -1;
}

void hash_delete(Entry* T[], int k, int size = 10007) {
    int i = 0;
    int j;
    do {
        j = h(i, j);

        if (T[j]) {
            if (T[j]->id == k) {
                T[j]->id = -1;
                return;
            }
        }
        i++;
    } while (T[j] && i < size);
}

void hash_delete(Entry* T[], Entry* k, int size = 10007) {
    int i = 0;
    int j;

    do {
        j = h(i, j);
        if (T[j]) {
            if (T[j]->id == k->id) {
                T[j]->id = -1;
                return;
            }
        }
        i++;
    } while (T[j] && i < size);
}
void hash_delete_(Entry* T[], Entry* k, int size = 10007) {
    int _unused;
    int idx = hash_search(T, k, _unused);
    T[idx]->id = -1;
}

Entry* hash_search_(Entry* T[], Entry* k, int& effort_counter,
                    int size = 10007) {
    if (!k)
        return NULL;

    int i = 0;
    int j;
    do {
        effort_counter = i + 1;
        j = h(k, i);
        if (T[j])
            if (T[j]->id == k->id) {
                return T[j];
            }
        i++;
    } while (T[j] && i < size);
    return NULL;
}

char* random_string() {
    char alpha[] = "abcdefghijklmnopqrstuvwxyz";

    char* result = (char*)malloc(sizeof(char) * 31);
    for (int i = 0; i < 30; i++)
        result[i] = alpha[rand() % 26];

    result[30] = '\0';
    return result;
}
void demo() {

    Entry* arr[10007] = {NULL};
    Entry* test[] = {
        new Entry{5, random_string()}, new Entry{5, random_string()},
        new Entry{10, random_string()}, new Entry{9, random_string()},
        new Entry{19, random_string()}};

    int _c;
    for (int i = 0; i < 5; i++)
        hash_insert(arr, test[i], &_c);

    for (int i = 0; i < 10007; i++) {
        if (arr[i]) {
            //      print_entry(arr[i]);
        }
    }

    for (int i = 0; i < 5; i++) {
        hash_search(arr, test[i], _c);
    }
    cout << _c;
}

void perf(int fill_factor) {
    srand(time(NULL));

    int size = 10007;
    float alphas[] = {0.8f, 0.85f, 0.9f, 0.99f};
    int lim = alphas[fill_factor] * size;
    int nums[size];
    FillRandomArray(nums, lim, INT_MAX / 2, INT_MAX, false);

    vector<Entry*> search;
    Entry* arr[10007] = {NULL};

    int _counter;

    int found_counter = 0;
    int max_effort_found = 0;
    float found_counter_avg = 0;

    int not_found_counter = 0;
    int max_effort_not_found = 0;
    float not_found_avg;

    int s = 0;
    for (int i = 0; i < lim; i++) {
        search.push_back(new Entry{nums[i], random_string()});
        hash_insert(arr, search[i], &_counter);
    }
    shuffle(search.begin(), search.end(), std::random_device());
    for (int i = 0; i < lim; i += 5) {
        hash_search_(arr, search[i], found_counter);
        max_effort_found = max(found_counter, max_effort_found);
        found_counter_avg += found_counter;
        s++;
    }

    found_counter_avg /= s;

    for (int i = 0; i < 1500; i++) {
        int num = INT_MAX / 3 + rand() % (INT_MAX / 2);
        hash_search(arr, num, not_found_counter);
        max_effort_not_found = max(not_found_counter, max_effort_not_found);
        not_found_avg += not_found_counter;
    }

    not_found_avg /= 1500;

    printf("%.2f\t\t|\t%f\t|\t%d\t\t|\t%f\t|\t%d\n", alphas[fill_factor],
           found_counter_avg, max_effort_found, not_found_avg,
           max_effort_not_found);
}

void perf_del() {
    srand(time(NULL));

    int size = 10007;
    int nums[size];
    int lim1 = 0.99f * size;
    int lim2 = 0.8f * size;
    FillRandomArray(nums, lim1, INT_MAX / 2, INT_MAX, false);

    vector<Entry*> search;
    Entry* arr[10007] = {NULL};

    int _counter;

    int found_counter = 0;
    int max_effort_found = 0;
    float found_counter_avg = 0;

    int not_found_counter = 0;
    int max_effort_not_found = 0;
    float not_found_avg;

    int s = 0;
    for (int i = 0; i < lim1; i++) {
        search.push_back(new Entry{nums[i], random_string()});
        hash_insert(arr, search[i], &_counter);
    }
    shuffle(search.begin(), search.end(), std::random_device());

    while (lim1 > lim2) {
        hash_delete_(arr, search.back());
        search.pop_back();
        lim1--;
    }

    for (int i = 0; i < lim2; i += 5) {
        hash_search_(arr, search[i], found_counter);
        max_effort_found = max(found_counter, max_effort_found);
        found_counter_avg += found_counter;
        s++;
    }

    found_counter_avg /= s;

    for (int i = 0; i < 1500; i++) {
        int num = INT_MAX / 3 + rand() % (INT_MAX / 2);
        hash_search(arr, num, not_found_counter);
        max_effort_not_found = max(not_found_counter, max_effort_not_found);
        not_found_avg += not_found_counter;
    }

    not_found_avg /= 1500;

    printf("%s\t|\t%f\t|\t%d\t\t|\t%f\t|\t%d\n", "deletion", found_counter_avg,
           max_effort_found, not_found_avg, max_effort_not_found);
}

void perf_helper() {

    printf("%s\t|%s\t| %s\t| %s\t%s\n", "Fill factor", "Avg effort found",
           "Max effort found", "Avg effort not-found", "Max effort not-found");
    cout << string(110, '-') << '\n';
    perf(0);
    perf(1);
    perf(2);
    perf(3);
    perf_del();
    cout << string(110, '-') << '\n';
}

int main() {
    perf_helper();
    return 0;
}
