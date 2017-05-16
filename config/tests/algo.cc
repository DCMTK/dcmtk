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
 *  Purpose: Rudimentary tests for a working <algorithm> implementation
 */

#include <algorithm>
#include <vector>

struct X {
    int *n;

    void operator()(int x)
    {
        ++*n;
    }
};

int main()
{
    std::vector<int> vec;
    vec.push_back(1);
    vec.push_back(2);

    int n = 0;

    X x;
    x.n = &n;

    std::for_each(vec.begin(), vec.end(), x);

    if (n != 2) {
        return -1;
    }

    if (std::find(vec.begin(), vec.end(), 44) != vec.end()) {
        return -1;
    }

    if (std::adjacent_find(vec.begin(), vec.end()) != vec.end()) {
        return -1;
    }

    return 0;
}
