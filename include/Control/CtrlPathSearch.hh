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
#include <bits/stdc++.h>
#include <tuple>

using namespace std;

namespace Control {
// Creating a shortcut for int, int pair type
    typedef pair<int, int> Pair;

// Creating a shortcut for pair<int, pair<int, int>> type
    typedef pair<double, pair<int, int> > pPair;

// A structure to hold the necessary parameters
    struct cell {
        // Row and Column index of its parent
        // Note that 0 <= i <= ROW-1 & 0 <= j <= COL-1
        int parent_i, parent_j;
        // f = g + h
        double f, g, h;
    };

    bool aStarSearch(int **grid, int COL, int ROW, Pair src, Pair dest, std::vector<std::pair<int, int>>& path);
}
#endif //LAMPPOSTAUTOCARDEMO_CTRLPATHSEARCH_HH
