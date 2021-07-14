//
// Created by Vito Wu on 2021/7/4.
//

#ifndef LAMPPOSTAUTOCARDEMO_CTRLPATHSEARCH_HH
#define LAMPPOSTAUTOCARDEMO_CTRLPATHSEARCH_HH

#include "math.h"
#include <array>
#include <cstring>
#include <iostream>
#include <queue>
#include <set>
#include <stack>
#include <tuple>

using namespace std;

namespace Control {
// Creating a shortcut for int, int pair type
    typedef pair<int, int> Pair;
// Creating a shortcut for tuple<int, int, int> type
    typedef tuple<double, int, int> Tuple;

// A structure to hold the neccesary parameters
    struct cell {
        // Row and Column index of its parent
        Pair parent;
        // f = g + h
        double f, g, h;

        cell() : parent(-1, -1), f(-1), g(-1), h(-1) {}
    };

    bool aStarSearch(int **grid, size_t COL, size_t ROW,
                     const Pair &src, const Pair &dest,
                     vector<Pair> &path);
}
#endif //LAMPPOSTAUTOCARDEMO_CTRLPATHSEARCH_HH
