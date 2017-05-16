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
 *  Purpose: Rudimentary tests for a working <limits> implementation.
 */

#include <limits>
#include <climits>

int main()
{
    if (!std::numeric_limits<signed char>::is_signed) {
        return -1;
    }

    if (std::numeric_limits<float>::is_iec559) {
        if (!std::numeric_limits<float>::has_infinity) {
            return -1;
        }
    }

    if (std::numeric_limits<int>::max() != INT_MAX) {
        return -1;
    }

    if (std::numeric_limits<int>::digits != CHAR_BIT * sizeof(int) - 1) {
        return -1;
    }

    return 0;
}
