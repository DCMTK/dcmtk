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
 *  Purpose: Rudimentary tests for a working <tuple> implementation.
 */

#include <tuple>

int main()
{
    auto tuple = std::make_tuple(1, "TEST");

    if (std::get<0>(tuple) != 1) {
        return -1;
    }

    if (std::tuple_size<decltype(tuple)>::value != 2) {
        return -1;
    }

    int n;
    const char *p;
    std::tie(n, p) = tuple;

    if (n != 1) {
        return -1;
    }

    // ensure std::tuple can take at least 50 template arguments, otherwise
    // our own implementation is superior
    std::tuple<int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int> t;

    return 0;
}
