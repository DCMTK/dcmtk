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
 *  Purpose: Rudimentary tests for a working <memory> implementation.
 */

#include <memory>

int main()
{
    int *p = new int(55);

    std::unique_ptr<int> pp(p);

    if (*p != *pp) {
        return -1;
    }

    if (p != pp.get()) {
        return -1;
    }

    pp.release();

    std::shared_ptr<int> s_p(p);
    std::shared_ptr<int> s_p2(s_p);
    std::shared_ptr<int> s_p3(s_p2);

    if (s_p.get() != s_p3.get()) {
        return -1;
    }

    if (s_p.use_count() != 3) {
        return -1;
    }

    if (*s_p != *s_p3) {
        return -1;
    }

    return 0;
}
