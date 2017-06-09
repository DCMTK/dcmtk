/*
 *  Copyright (C) 2017, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module:  config
 *
 *  Author:  Thorben Hasenpusch
 *
 *  Purpose: Rudimentary tests for a working <vector> implementation.
 */

#include <vector>

struct Recursive : std::vector<Recursive>
{
    int i;
};

int main()
{
    std::vector<int> v;

    v.push_back(1);
    v.push_back(2);
    v.push_back(3);

    if (v.size() != 3) {
        return -1;
    }

    if (v[2] != 3) {
        return -1;
    }

    v.erase(v.begin());

    if (v[0] != 2) {
        return -1;
    }

    v.pop_back();

    if (v.size() != 1) {
        return -1;
    }

    v.clear();

    v.push_back(42);
    v.push_back(13);

    v.resize(1);

    if (v.size() != 1 || v[0] != 42) {
        return -1;
    }

    if (v.front() != v[0]) {
        return -1;
    }

    if (v.back() != v[v.size() - 1]) {
        return -1;
    }

    // ensure iterators are NOT invalidated by swap()
    std::vector<int>::iterator it = v.begin();
    std::vector<int> w;
    w.push_back(23);
    v.swap(w);
    if (it != w.begin()) {
        return -1;
    }

    // test if recursive vector structures can be constructed without causing
    // a stack overflow
    if (std::vector<Recursive>().size() != 0) {
        return -1;
    }

    return 0;
}
