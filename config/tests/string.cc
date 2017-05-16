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
 *  Purpose: Rudimentary tests for a working <string> implementation.
 */

#include <string>

int main()
{
    std::string s = "TEST";

    if (s[1] != 'E') {
        return -1;
    }

    s += "HAHA";

    if (s[5] != 'A') {
        return -1;
    }

    std::string sub = s.substr(4);

    if ("HAHA" != sub) {
        return -1;
    }

    s.replace(0, 4, "HAHA");

    if (s != "HAHAHAHA") {
        return -1;
    }

    return 0;
}
