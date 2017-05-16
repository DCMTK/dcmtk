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
 *  Purpose: Rudimentary tests for a working <type_traits> implementation.
 */

#include <type_traits>

class A{};

class B{ virtual ~B(); };

int main()
{
    if (!std::is_void<void>::value) {
        return -1;
    }

    if (!std::is_pointer<void *>::value) {
        return -1;
    }

    if (!std::is_class<A>::value) {
        return -1;
    }

    if (!std::is_volatile<volatile int>::value) {
        return -1;
    }

    if (!std::is_empty<A>::value) {
        return -1;
    }

    if (!std::has_virtual_destructor<B>::value) {
        return -1;
    }

    if (std::is_empty<B>::value) {
        return -1;
    }

    if (std::is_const<std::remove_const<const int>>::value) {
        return -1;
    }

    if (!std::is_same<std::enable_if<std::is_empty<A>::value, int>::type, int>::value) {
        return -1;
    }

    return 0;
}
