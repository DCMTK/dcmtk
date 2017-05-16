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
 *  Purpose: Rudimentary tests for a working <stack> implementation.
 */

#include <stack>

int main()
{
    std::stack<int> s;

    if (!s.empty()) {
        return -1;
    }

    s.push(1);
    s.push(2);
    s.push(3);

    s.pop();
    s.pop();

    s.push(42);

    if (s.top() != 42) {
        return -1;
    }

    return 0;
}
