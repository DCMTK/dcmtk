/*
 *  Copyright (C) 2019, OFFIS e.V.
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
 *  Author:  Jan Schlamelcher
 *
 *  Purpose: Presents constructs that every C++14 compliant
 *           compiler should be able to compile.
 *
 *
 *  This file is based on ax_cxx_compile_stdcxx from the GNU Autoconf project.
 *  The original license is as follows:
 *
 *  LICENSE
 *
 *   Copyright (c) 2008 Benjamin Kosnik <bkoz@redhat.com>
 *   Copyright (c) 2012 Zack Weinberg <zackw@panix.com>
 *   Copyright (c) 2013 Roy Stogner <roystgnr@ices.utexas.edu>
 *   Copyright (c) 2014, 2015 Google Inc.; contributed by Alexey Sokolov <sokolov@google.com>
 *   Copyright (c) 2015 Paul Norman <penorman@mac.com>
 *   Copyright (c) 2015 Moritz Klammler <moritz@klammler.eu>
 *   Copyright (c) 2016 Krzesimir Nowak <qdlacz@gmail.com>
 *
 *   Copying and distribution of this file, with or without modification, are
 *   permitted in any medium without royalty provided the copyright notice
 *   and this notice are preserved.  This file is offered as-is, without any
 *   warranty.
 */

// If the compiler admits that it is not ready for C++14, why torture it?
// Hopefully, this will speed up the test.

#ifndef __cplusplus

#error "This is not a C++ compiler"

#elif __cplusplus < 201402L

#error "This is not a C++14 compiler"

#else

int main()
{
  return 0;
}

namespace cxx14
{
  namespace test_polymorphic_lambdas
  {
    int
    test()
    {
      const auto lambda = [](auto&&... args){
        const auto istiny = [](auto x){
          return (sizeof(x) == 1UL) ? 1 : 0;
        };
        const int aretiny[] = { istiny(args)... };
        return aretiny[0];
      };
      return lambda(1, 1L, 1.0f, '1');
    }
  }

  namespace test_binary_literals
  {
    constexpr auto ivii = 0b0000000000101010;
    static_assert(ivii == 42, "wrong value");
  }

  namespace test_generalized_constexpr
  {
    template < typename CharT >
    constexpr unsigned long
    strlen_c(const CharT *const s) noexcept
    {
      auto length = 0UL;
      for (auto p = s; *p; ++p)
        ++length;
      return length;
    }

    static_assert(strlen_c("") == 0UL, "");
    static_assert(strlen_c("x") == 1UL, "");
    static_assert(strlen_c("test") == 4UL, "");
    static_assert(strlen_c("another\0test") == 7UL, "");
  }

  namespace test_lambda_init_capture
  {
    int
    test()
    {
      auto x = 0;
      const auto lambda1 = [a = x](int b){ return a + b; };
      const auto lambda2 = [a = lambda1(x)](){ return a; };
      return lambda2();
    }
  }

  namespace test_digit_separators
  {
    constexpr auto ten_million = 100'000'000;
    static_assert(ten_million == 100000000, "");
  }

  namespace test_return_type_deduction
  {
    auto f(int& x) { return x; }
    decltype(auto) g(int& x) { return x; }

    template < typename T1, typename T2 >
    struct is_same
    {
      static constexpr auto value = false;
    };

    template < typename T >
    struct is_same<T, T>
    {
      static constexpr auto value = true;
    };

    int
    test()
    {
      auto x = 0;
      static_assert(is_same<int, decltype(f(x))>::value, "");
      static_assert(is_same<int&, decltype(g(x))>::value, "");
      return x;
    }
  }
}  // namespace cxx14

#endif  // __cplusplus >= 201402L
