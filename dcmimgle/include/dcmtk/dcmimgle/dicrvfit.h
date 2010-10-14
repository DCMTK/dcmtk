/*
 *
 *  Copyright (C) 1996-2010, OFFIS e.V.
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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:16:25 $
 *  CVS/RCS Revision: $Revision: 1.19 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
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
                register unsigned int i;
                register unsigned int j;
                register unsigned int k;
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
            register unsigned int i;
            register unsigned int j;
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
            register unsigned int i;
            register unsigned int j;
            register unsigned int k;
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


/*
 *
 * CVS/RCS Log:
 * $Log: dicrvfit.h,v $
 * Revision 1.19  2010-10-14 13:16:25  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.18  2010-03-01 09:08:46  uli
 * Removed some unnecessary include directives in the headers.
 *
 * Revision 1.17  2005-12-08 16:47:35  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.16  2003/12/23 15:53:22  joergr
 * Replaced post-increment/decrement operators by pre-increment/decrement
 * operators where appropriate (e.g. 'i++' by '++i').
 *
 * Revision 1.15  2003/12/08 18:54:16  joergr
 * Adapted type casts to new-style typecast operators defined in ofcast.h.
 * Removed leading underscore characters from preprocessor symbols (reserved
 * symbols). Updated copyright header.
 *
 * Revision 1.14  2002/11/27 14:08:03  meichel
 * Adapted module dcmimgle to use of new header file ofstdinc.h
 *
 * Revision 1.13  2002/11/26 18:18:35  joergr
 * Replaced include for "math.h" with <math.h> to avoid inclusion of math.h in
 * the makefile dependencies.
 *
 * Revision 1.12  2002/10/31 10:10:45  meichel
 * Added workaround for a bug in the Sparc optimizer in gcc 3.2
 *
 * Revision 1.11  2002/07/19 08:23:12  joergr
 * Added missing doc++ comments.
 *
 * Revision 1.10  2002/07/18 12:28:11  joergr
 * Added explicit type casts to keep Sun CC 2.0.1 quiet.
 *
 * Revision 1.9  2001/06/01 15:49:40  meichel
 * Updated copyright header
 *
 * Revision 1.8  2000/03/08 16:24:14  meichel
 * Updated copyright header.
 *
 * Revision 1.7  2000/03/06 15:58:39  meichel
 * Changed static template functions to methods. Required for xlC 1.0 on AIX 3.2.
 *
 * Revision 1.6  1999/10/21 08:29:41  joergr
 * Renamed template type definition from 'T3' to 'T3_' to avoid naming
 * conflicts.
 *
 * Revision 1.5  1999/10/20 18:38:49  joergr
 * Eliminated default values for template types since this features is not
 * supported by SunCC 4.x (temporarily introduced '#define' instead).
 *
 * Revision 1.4  1999/10/20 10:32:44  joergr
 * Added generic specification for template function convertValue to avoid
 * compiler warnings reported by MSVC (with additional options?).
 *
 * Revision 1.3  1999/10/18 10:15:15  joergr
 * Fixed typos.
 *
 * Revision 1.2  1999/10/15 09:38:31  joergr
 * Fixed typos.
 *
 * Revision 1.1  1999/10/14 19:04:09  joergr
 * Added new template class that supports polynomial curve fitting algorithm.
 *
 *
 */
