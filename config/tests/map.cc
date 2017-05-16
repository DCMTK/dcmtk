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
 *  Purpose: Rudimentary tests for a working <map> implementation.
 */

#include <map>

int main()
{
    std::map<int, int> m;

    if (!m.empty()) {
        return -1;
    }

    m[3] = 555;
    m[66] = 2;
    m[42] = 42;

    if (m.size() != 3) {
        return -1;
    }

    if (m[3] != 555) {
        return -1;
    }

    if (m[42] != 42) {
        return -1;
    }

    m.erase(m.find(3));

    if (m.find(3) != m.end()) {
        return -1;
    }

    m.clear();

    if (!m.empty()) {
        return -1;
    }

    return 0;
}
