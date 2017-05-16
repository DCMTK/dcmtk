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
 *  Purpose: Rudimentary tests for a working <system_error> implementation.
 */

#include <system_error>

#include <cstring>

#ifdef _WIN32
#include <Windows.h>
#endif

int main()
{
    // Not much to test, really.

    if (strcmp(std::generic_category().name(), "generic") != 0) {
        return -1;
    }

    if (strcmp(std::system_category().name(), "system") != 0) {
        return -1;
    }

#ifdef _WIN32
    // Visual Studio might be using strerror for both categories.

    // Test with the highest numbered WINAPI error code; highly unlikely that there exists an equal-numbered errno.
    std::system_error system_err(ERROR_API_UNAVAILABLE, std::system_category());
    std::system_error generic_err(ERROR_API_UNAVAILABLE, std::generic_category());

    // The system error message should be retrieved using FormatMessage(),
    // while the generic error message should be retrieved with strerror and be something like "unknown error",
    // since it isn't a valid errno.
    // The point is, these two should result in different messages.

    if (strcmp(system_err.what(), generic_err.what()) == 0) {
        // Visual Studio messed up, what a surprise...
        return -1;
    }
#endif
    return 0;
}
