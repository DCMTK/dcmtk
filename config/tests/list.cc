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
 *  Purpose: Rudimentary tests for a working <list> implementation.
 */

#include <list>

int main()
{
    std::list<int> l;

    if (!l.empty()) {
        return -1;
    }

    l.push_back(18);
    l.push_back(22);
    l.push_front(2);

    if (*l.begin() != 2) {
        return -1;
    }

    l.reverse();

    if (*l.begin() != 22) {
        return -1;
    }

    int sum = 0;
    for (std::list<int>::iterator it = l.begin(); it != l.end(); ++it) {
        sum += *it;
    }

    if (sum != 42) {
        return -1;
    }

    // ensure iterators are NOT invalidated by swap()
    std::list<int>::iterator it = l.begin();
    std::list<int> m;
    m.push_back(23);
    l.swap(m);
    if (it != m.begin()) {
        return -1;
    }

    return 0;
}
