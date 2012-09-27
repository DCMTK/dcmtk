/*
 *
 *  Copyright (C) 1997-2012, OFFIS e.V.
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
 *  Module:  ofstd
 *
 *  Author:  Andreas Barth
 *
 *  Purpose:
 *      Defines template algorithms for contaimer classes with
 *      interfaces similar to the C++ Standard
 *
 */

#ifndef OFALGO_H
#define OFALGO_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/oftypes.h"

// Usage:
//  Function_type OFForEach(InputIterator_type, Function_type, first, last ,f)
//     Applies function f from type Function_type to the result of
//     derferencing every iterator in the range [first, last) starting
//     with first and proceeding to last -1 (first, last are of type
//     InputIterator_type).  Returns f.
//
//  InputIterator_type OFFind(InputIterator_type, T_type, first, last, value)
//     Returns the first Iterator i of type InputIterator_type in the range
//     [first, last) for which *i == value (of type T_type). Returns last,
//     if no such iterator is found
//
//  InputIterator_type OFFindIf(InputIterator_type, Predicate_type,
//                              first, last, pred)
//     Returns the first iterator i of type InputIterator_type in the range
//     [first, last) for which pred(*i) != false. The function pred is of
//     type Predicate_type. Returns last, if no such iterator is found
//
//  ForwardIterator OFAdjacentFind(ForwardIterator_type, first, last)
//     Returns the first iterator i of type ForwardIterator_type such
//     that both i and i+1 are in the range [first, last) and *i == *(i+1)
//     returns last, if no such iterator is found. i+1 means the successor
//     of i.
//
// ForwardIterator OFAdjacentFindPred(ForwardIterator_type,
//                                    BinaryPredicate_type,
//                                    first, last, pred)
//     Returns the first iterator i of type ForwardIterator_type such
//     that both i and i+1 are in the range [first, last) and
//     pred (*i, *(i+1)) != false.
//     Returns last, if no such iterator is found. i+1 means the successor
//     of i.
//
// Additional Remarks:
//   In some template functions one template parameter is another function.
//   Some compilers  cannot determine automatically the type of template
//   function parameters, so you must give  them a hint casting
//   the parameter function to the correct type (e.g. NeXT gcc 2.5.8)


#if defined(HAVE_STL) || defined(HAVE_STL_ALGORITHMS)
// It is possible to use the standard template library list class since the
// interface is nearly identical.
// Important: If you want to use the standard template library (STL), no
// variable within a namespace using a class of the STL shall have a name
// of one class of the STL
#include <algorithm>
#define OFForEach(InputIterator_type, Function_type, first, last, f) STD_NAMESPACE for_each((first), (last), (f))
#define OFFind(InputIterator_type, T_type, first, last, value) STD_NAMESPACE find((first), (last), (value))
#define OFFindIf(InputIterator_type, Predicate_type, first, last, pred) STD_NAMESPACE find_if((first), (last), (pred))
#define OFAdjacentFind(ForwardIterator_type, first, last) STD_NAMESPACE adjacent_find((first), (last))
#define OFAdjacentFindPred(ForwardIterator_type, BinaryPredicate_type, first, last, pred) STD_NAMESPACE adjacent_find((first), (last), (pred))
#else

#ifdef HAVE_FUNCTION_TEMPLATE

#define OFForEach(InputIterator_type, Function_type, first, last, f) OF_ForEach((first), (last), (f))

#define OFFind(InputIterator_type, T_type, first, last, value) OF_Find((first), (last), (value))

#define OFFindIf(InputIterator_type, Predicate_type, first, last, pred) OF_FindIf((first), (last), (pred))

#define OFAdjacentFind(ForwardIterator_type, first, last) OF_AdjacentFind((first), (last))

#define OFAdjacentFindPred(ForwardIterator_type, BinaryPredicate_type, first, last, pred) OF_AdjacentFind((first), (last), (pred))

#elif defined(HAVE_STATIC_TEMPLATE_METHOD)

#define OFForEach(InputIterator_type, Function_type, first, last, f) OF_ForEachClass<InputIterator_type, Function_type>::OF_ForEach((first), (last), (f))

#define OFFind(InputIterator_type, T_type, first, last, value) OF_FindClass<InputIterator_type, T_type>::OF_Find((first), (last), (value))

#define OFFindIf(InputIterator_type, Predicate_type, first, last, pred) OF_FindIfClass<InputIterator_type, Predicate_type>::OF_FindIf((first), (last), (pred))

#define OFAdjacentFind(ForwardIterator_type, first, last) OF_AdjacentFindClass<ForwardIterator_type, int>::OF_AdjacentFind((first), (last))

#define OFAdjacentFindPred(ForwardIterator_type, BinaryPredicate_type, first, last, pred) OF_AdjacentFindPredClass<ForwardIterator_type, BinaryPredicate_type>::OF_AdjacentFind((first), (last), (pred))
#else
#error Your C++ Compiler is not capable of compiling this code
#endif


template <class InputIterator, class Function>
#if defined(HAVE_STATIC_TEMPLATE_METHOD) && !defined(HAVE_FUNCTION_TEMPLATE)
class OF_ForEachClass
{
public:
static
#endif
Function OF_ForEach(InputIterator first, InputIterator last, Function f)
{
    while (first != last)
    {
        f(*first);
        ++first;
    }
    return f;
}
#if defined(HAVE_STATIC_TEMPLATE_METHOD) && !defined(HAVE_FUNCTION_TEMPLATE)
};
#endif

template <class InputIterator, class T>
#if defined(HAVE_STATIC_TEMPLATE_METHOD) && !defined(HAVE_FUNCTION_TEMPLATE)
class OF_FindClass
{
public:
static
#endif
InputIterator OF_Find(InputIterator first, InputIterator last, const T & value)
{
    while (first != last && *first != value) ++ first;
    return first;
}
#if defined(HAVE_STATIC_TEMPLATE_METHOD) && !defined(HAVE_FUNCTION_TEMPLATE)
};
#endif


template <class InputIterator, class Predicate>
#if defined(HAVE_STATIC_TEMPLATE_METHOD) && !defined(HAVE_FUNCTION_TEMPLATE)
class OF_FindIfClass
{
public:
static
#endif
InputIterator OF_FindIf(InputIterator first, InputIterator last,
                        Predicate pred)
{
    while (first != last && !pred(*first)) ++first;
    return first;
}
#if defined(HAVE_STATIC_TEMPLATE_METHOD) && !defined(HAVE_FUNCTION_TEMPLATE)
};
#endif

template <class ForwardIterator>
#if defined(HAVE_STATIC_TEMPLATE_METHOD) && !defined(HAVE_FUNCTION_TEMPLATE)
class OF_AdjacentFindClass
{
public:
static
#endif
ForwardIterator OF_AdjacentFind(ForwardIterator first, ForwardIterator last)
{
    if (first == last) return last;
    ForwardIterator next(first);
    while (++next != last)
    {
        if (*first == *next) return *first;
        ++first;
    }
    return last;
}
#if defined(HAVE_STATIC_TEMPLATE_METHOD) && !defined(HAVE_FUNCTION_TEMPLATE)
};
#endif

template <class ForwardIterator, class BinaryPredicate>
#if defined(HAVE_STATIC_TEMPLATE_METHOD) && !defined(HAVE_FUNCTION_TEMPLATE)
class OF_AdjacentFindPredClass
{
public:
static
#endif
ForwardIterator OF_AdjacentFind(ForwardIterator first, ForwardIterator last,
                                BinaryPredicate pred)
{
    if (first == last) return last;
    ForwardIterator next = first;
    while(++next != last)
    {
        if (pred(*first, *last)) return first;
        ++first;
    }
    return last;
}

#if defined(HAVE_STATIC_TEMPLATE_METHOD) && !defined(HAVE_FUNCTION_TEMPLATE)
};
#endif

#endif

#endif /* OFALGO_H */
