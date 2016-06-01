/*
 *
 *  Copyright (C) 1996-2016, OFFIS e.V.
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
 *  Module:  dcmimgle
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: DiCurveFitting (header/implementation)
 *
 */


#ifndef DICRVFIT_H
#define DICRVFIT_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/oftypes.h"
#include "dcmtk/ofstd/ofcast.h"

#define INCLUDE_CMATH
#define INCLUDE_CSTDDEF               /* For NULL */
#include "dcmtk/ofstd/ofstdinc.h"


/*---------------------*
 *  macro definitions  *
 *---------------------*/

// SunCC 4.x does not support default values for template types :-/
#define T3_ double


/*------------------*
 *  template class  *
 *------------------*/

/** Template class for polynomial curve fitting algorithm
 */
template <class T1, class T2 /*, class T3 = double*/>
class DiCurveFitting
{

 public:

    /** calculate coefficients for resulting polynomial function.
     *  T1  = type of x coordinates
     *  T2  = type of y coordinates
     *  T3_ = type of coefficients (and for internal calculations)
     *
     ** @param  x  array with x coordinates of given points
     *  @param  y  array with y coordinates of given points
     *  @param  n  number of entries in array (= points)
     *  @param  o  order of polynomial function
     *  @param  c  array to store the resulting coefficients (o+1 entries !)
     *
     ** @return true if successful, false otherwise
     */
    static int calculateCoefficients(const T1 *x,
                                     const T2 *y,
                                     const unsigned int n,
                                     const unsigned int o,
                                     T3_ *c)
    {
        int result = 0;
        if ((x != NULL) && (y != NULL) && (c !=NULL) && (n > 0))
        {
            const unsigned int order = o + 1;
            const unsigned int order2 = order * order;
            T3_ *basis = new T3_[order * n];
            T3_ *alpha = new T3_[order2];
            T3_ *beta = new T3_[order];
            if ((basis != NULL) && (alpha != NULL) && (beta != NULL))
            {
                unsigned int i;
                unsigned int j;
                unsigned int k;
                for (i = 0; i < order; ++i)
                {
                    for (j = 0; j < n; ++j)
                    {
                        k = i + j * order;
                        if (i == 0)
                            basis[k] = 1;
                        else
                            basis[k] = OFstatic_cast(T3_, x[j]) * basis[k - 1];
                     }
                }
                T3_ sum;
                for (i = 0; i < order; ++i)
                {
                    const unsigned int i_order = i * order;
                    for (j = 0; j <= i; ++j)
                    {
                        sum = 0;
                        for (k = 0; k < n; ++k)
                            sum += basis[i + k * order] * basis[j + k * order];
                        alpha[i + j * order] = sum;
                        if (i != j)
                            alpha[j + i_order] = sum;
                    }
                }
                for (i = 0; i < order; ++i)
                {
                    sum = 0;
                    for (j = 0; j < n; ++j)
                        sum += OFstatic_cast(T3_, y[j]) * basis[i + j * order];
                    beta[i] = sum;
                }
                if (solve(alpha, beta, order))
                {
                    for (i = 0; i < order; ++i)
                        c[i] = beta[i];
                    result = 1;
                }
            }
            delete[] basis;
            delete[] alpha;
            delete[] beta;
        }
        return result;
    }


    /** calculate y coordinates for the given range of x coordinates.
     *  The polynomial function is defined by the specified coefficients.
     *  T1  = type of x coordinates
     *  T2  = type of y coordinates
     *  T3_ = type of coefficients (and for internal calculations)
     *
     ** @param  xs  first x coordinate for computation
     *  @param  xe  last x coordinate for computation
     *  @param  y   array to store the resulting y coordinates (n entries !)
     *  @param  n   number of entries in array (= points)
     *  @param  o   order of polynomial function
     *  @param  c   array of coefficients computed by the above method (o+1 entries !)
     *
     ** @return true if successful, false otherwise
     */
    static int calculateValues(const T1 xs,
                               const T1 xe,
                               T2 *y,
                               const unsigned int n,
                               const unsigned int o,
                               const T3_ *c)
    {
        int result = 0;
        if ((y != NULL) && (c != NULL) && (n > 0) && (xe > xs))
        {
            unsigned int i;
            unsigned int j;
            T3_ x;
            T3_ x2;
            T3_ w;
            const T3_ xo = OFstatic_cast(T3_, xs);
            const T3_ xi = OFstatic_cast(T3_, (OFstatic_cast(T3_, xe) - OFstatic_cast(T3_, xs)) / (n - 1));
            for (i = 0; i < n; ++i)
            {
                x = xo + OFstatic_cast(T3_, i) * xi;
                x2 = 1;
                w = 0;
                for (j = 0; j <= o; ++j)
                {
                    w += c[j] * x2;
                    x2 *= x;
                }
                convertValue(w, y[i]);          // cut value if necessary
            }
            result = 1;
        }
        return result;
    }


 private:

    /** helper routine: convert to unsigned 8 bit value
     *
     ** @param  input   input value to be converted
     *  @param  output  output value (range: 0..255)
     *
     ** @return output value
     */
    static void convertValue(const T3_ input, Uint8 &output)
    {
        output = (input <= 0) ? 0 : ((input >= 255) ? 255 : OFstatic_cast(Uint8, input));
    }

    /** helper routine: convert to signed 8 bit value
     *
     ** @param  input   input value to be converted
     *  @param  output  output value (range: -128..127)
     *
     ** @return output value
     */
    static void convertValue(const T3_ input, Sint8 &output)
    {
        output = (input <= -128) ? -128 : ((input >= 127) ? 127 : OFstatic_cast(Sint8, input));
    }

    /** helper routine: convert to unsigned 16 bit value
     *
     ** @param  input   input value to be converted
     *  @param  output  output value (range: 0..65535)
     *
     ** @return output value
     */
    static void convertValue(const T3_ input, Uint16 &output)
    {
        output = (input <= 0) ? 0 : ((input >= 65535) ? 65535 : OFstatic_cast(Uint16, input));
    }

    /** helper routine: convert to signed 16 bit value
     *
     ** @param  input   input value to be converted
     *  @param  output  output value (range: -32768..32767)
     *
     ** @return output value
     */
    static void convertValue(const T3_ input, Sint16 &output)
    {
        output = (input <= -32768) ? -32768 : ((input >= 32767) ? 32767 : OFstatic_cast(Sint16, input));
    }

    /** helper routine: convert to floating point value (double precision)
     *
     ** @param  input   input value to be converted
     *  @param  output  output value (double)
     *
     ** @return output value
     */
    static inline void convertValue(const T3_ input, double &output)
    {
        output = OFstatic_cast(double, input);
    }

    /** solve the equation given by the two matrixes.
     *  T3_ = type of coefficients (and for internal calculations)
     *
     ** @param  a  first matrix (array of values)
     *  @param  b  second matrix (array of values)
     *  @param  n  number of entries in array
     *
     ** @return true if successful, false otherwise
     */
    static int solve(T3_ *a,
                     T3_ *b,
                     const unsigned int n)
    {
        int result = 0;
        if ((a != NULL) && (b != NULL) && (n > 0))
        {
            unsigned int i;
            unsigned int j;
            unsigned int k;
            signed int pivot;
            T3_ mag;
            T3_ mag2;
            T3_ temp;
            for (i = 0; i < n; ++i)
            {
                mag = 0;
                pivot = -1;
                for (j = i; j < n; ++j)
                {
                    mag2 = fabs(a[i + j * n]);
                    if (mag2 > mag)
                    {
                        mag = mag2;
                        pivot = j;
                    }
                }
                if ((pivot == -1) || (mag == 0))
                    break;
                else
                {
                    const unsigned int piv = OFstatic_cast(unsigned int, pivot);
                    const unsigned int i_n = i * n;
                    if (piv != i)
                    {
                        const unsigned int piv_n = piv * n;
                        for (j = i; j < n; ++j)
                        {
                            temp = a[j + i_n];
                            a[j + i_n] = a[j + piv_n];
                            a[j + piv_n] = temp;
                        }
                        temp = b[i];
                        b[i] = b[piv];
                        b[piv] = temp;
                    }
                    mag = a[i + i_n];
                    for (j = i; j < n; ++j)
                        a[j + i_n] /= mag;
                    b[i] /= mag;
                    for (j = 0; j < n; ++j)
                    {
                        if (i == j)
                            continue;
                        const unsigned int j_n = j * n;
                        mag2 = a[i + j_n];
                        for (k = i; k < n; ++k)
                            a[k + j_n] -= mag2 * a[k + i_n];
                        b[j] -= mag2 * b[i];
                    }
                    result = 1;
                }

            }
        }
        return result;
    }
};


#endif
