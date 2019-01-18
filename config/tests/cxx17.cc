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
 *  Purpose: Presents constructs that every C++17 compliant
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

// If the compiler admits that it is not ready for C++17, why torture it?
// Hopefully, this will speed up the test.

#ifndef __cplusplus

#error "This is not a C++ compiler"

#elif __cplusplus < 201703L

#error "This is not a C++17 compiler"

#else

int main()
{
  return 0;
}

#include <initializer_list>
#include <utility>
#include <type_traits>

namespace cxx17
{
  namespace test_constexpr_lambdas
  {
    constexpr int foo = [](){return 42;}();
  }

  namespace test::nested_namespace::definitions
  {

  }

  namespace test_fold_expression
  {
    template<typename... Args>
    int multiply(Args... args)
    {
      return (args * ... * 1);
    }

    template<typename... Args>
    bool all(Args... args)
    {
      return (args && ...);
    }
  }

  namespace test_extended_static_assert
  {
    static_assert (true);
  }

  namespace test_auto_brace_init_list
  {
    auto foo = {5};
    auto bar {5};

    static_assert(std::is_same<std::initializer_list<int>, decltype(foo)>::value);
    static_assert(std::is_same<int, decltype(bar)>::value);
  }

  namespace test_typename_in_template_template_parameter
  {
    template<template<typename> typename X> struct D;
  }

  namespace test_fallthrough_nodiscard_maybe_unused_attributes
  {
    int f1()
    {
      return 42;
    }

    [[nodiscard]] int f2()
    {
      [[maybe_unused]] auto unused = f1();

      switch (f1())
      {
      case 17:
        f1();
        [[fallthrough]];
      case 42:
        f1();
      }
      return f1();
    }
  }

  namespace test_extended_aggregate_initialization
  {
    struct base1
    {
      int b1, b2 = 42;
    };

    struct base2
    {
      base2() {
        b3 = 42;
      }
      int b3;
    };

    struct derived : base1, base2
    {
        int d;
    };

    derived d1 {{1, 2}, {}, 4};  // full initialization
    derived d2 {{}, {}, 4};      // value-initialized bases
  }

  namespace test_general_range_based_for_loop
  {
    struct iter
    {
      int i;

      int& operator* ()
      {
        return i;
      }

      const int& operator* () const
      {
        return i;
      }

      iter& operator++()
      {
        ++i;
        return *this;
      }
    };

    struct sentinel
    {
      int i;
    };

    bool operator== (const iter& i, const sentinel& s)
    {
      return i.i == s.i;
    }

    bool operator!= (const iter& i, const sentinel& s)
    {
      return !(i == s);
    }

    struct range
    {
      iter begin() const
      {
        return {0};
      }

      sentinel end() const
      {
        return {5};
      }
    };

    void f()
    {
      range r {};

      for (auto i : r)
      {
        [[maybe_unused]] auto v = i;
      }
    }
  }

  namespace test_lambda_capture_asterisk_this_by_value
  {
    struct t
    {
      int i;
      int foo()
      {
        return [*this]()
        {
          return i;
        }();
      }
    };
  }

  namespace test_enum_class_construction
  {
    enum class byte : unsigned char
    {};

    byte foo {42};
  }

  namespace test_constexpr_if
  {
    template <bool cond>
    int f ()
    {
      if constexpr(cond)
      {
        return 13;
      }
      else
      {
        return 42;
      }
    }
  }

  namespace test_selection_statement_with_initializer
  {
    int f()
    {
      return 13;
    }

    int f2()
    {
      if (auto i = f(); i > 0)
      {
        return 3;
      }

      switch (auto i = f(); i + 4)
      {
      case 17:
        return 2;

      default:
        return 1;
      }
    }
  }

  namespace test_template_argument_deduction_for_class_templates
  {
    template <typename T1, typename T2>
    struct pair
    {
      pair (T1 p1, T2 p2)
        : m1 {p1},
          m2 {p2}
      {}

      T1 m1;
      T2 m2;
    };

    void f()
    {
      [[maybe_unused]] auto p = pair{13, 42u};
    }
  }

  namespace test_non_type_auto_template_parameters
  {
    template <auto n>
    struct B
    {};

    B<5> b1;
    B<'a'> b2;
  }

  namespace test_structured_bindings
  {
    int arr[2] = { 1, 2 };
    std::pair<int, int> pr = { 1, 2 };

    auto f1() -> int(&)[2]
    {
      return arr;
    }

    auto f2() -> std::pair<int, int>&
    {
      return pr;
    }

    struct S
    {
      int x1 : 2;
      volatile double y1;
    };

    S f3()
    {
      return {};
    }

    auto [ x1, y1 ] = f1();
    auto& [ xr1, yr1 ] = f1();
    auto [ x2, y2 ] = f2();
    auto& [ xr2, yr2 ] = f2();
    const auto [ x3, y3 ] = f3();
  }

  namespace test_exception_spec_type_system
  {
    struct Good {};
    struct Bad {};

    void g1() noexcept;
    void g2();

    template<typename T>
    Bad
    f(T*, T*);

    template<typename T1, typename T2>
    Good
    f(T1*, T2*);

    static_assert (std::is_same_v<Good, decltype(f(g1, g2))>);
  }

  namespace test_inline_variables
  {
    template<class T> void f(T)
    {}

    template<class T> inline T g(T)
    {
      return T{};
    }

    template<> inline void f<>(int)
    {}

    template<> int g<>(int)
    {
      return 5;
    }
  }
}  // namespace cxx17

#endif  // __cplusplus <= 201703L
