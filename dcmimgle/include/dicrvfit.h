/*
 *
 *  Copyright (C) 1999, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module:  dcmimgle
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: DiCurveFitting (Header/Implementation)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1999-10-14 19:04:09 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/include/Attic/dicrvfit.h,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 * 
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __DICRVFIT_H
#define __DICRVFIT_H

#include "osconfig.h"

#include "math.h"


/********************************************************************/


static inline void convertValue(const double input,
                                Uint8 &output)
{
    output = (input <= 0) ? 0 : ((input >= 255) ? 255 : (Uint8)input);
}

static inline void convertValue(const double input,
                                Sint8 &output)
{
    output = (input <= -128) ? -128 : ((input >= 127) ? 127 : (Sint8)input);
}

static inline void convertValue(const double input,
                                Uint16 &output)
{
    output = (input <= 0) ? 0 : ((input >= 65535) ? 65535 : (Uint16)input);
}

static inline void convertValue(const double input,
                                Sint16 &output)
{
    output = (input <= -32768) ? -32768 : ((input >= 32767) ? 32767 : (Sint16)input);
}


/*------------------*
 *  template class  *
 *------------------*/

/** Template class for polynomial curve fitting algorithm
 */
template <class T1, class T2, class T3 = double>
class DiCurveFitting
{

 public:

    /** calculate coefficients for resulting polynomial function
     *  T1 = type of x coordinates
     *  T2 = type of y coordinates
     *  T3 = type of coefficients (and for internal calculations)
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
                                     T3 *c)
    {
        int result = 0;
        if ((x != NULL) && (y != NULL) && (c !=NULL) && (n > 0))
        {
            const unsigned int order = o + 1;
            const unsigned int order2 = order * order;
            T3 *basis = new T3[order * n];
            T3 *alpha = new T3[order2];
            T3 *beta = new T3[order];
            if ((basis != NULL) && (alpha != NULL) && (beta != NULL))
            {
                register unsigned int i;
                register unsigned int j;
                register unsigned int k;
                for (i = 0; i < order; i++)
                {
                    for (j = 0; j < n; j++)
                    {
                        k = i + j * order;
                        if (i == 0)
                            basis[k] = 1;
                        else
                            basis[k] = (T3)x[j] * basis[k - 1];
                     }
                }
                T3 sum;
                for (i = 0; i < order; i++)
                {
                    const unsigned int i_order = i * order;
                    for (j = 0; j <= i; j++)
                    {
                        sum = 0;
                        for (k = 0; k < n; k++)
                            sum += basis[i + k * order] * basis[j + k * order];
                        alpha[i + j * order] = sum;
                        if (i != j)
                            alpha[j + i_order] = sum;
                    }
                }
                for (i = 0; i < order; i++)
                {
                    sum = 0;
                    for (j = 0; j < n; j++)
                        sum += (T3)y[j] * basis[i + j * order];
                    beta[i] = sum;
                }
                if (solve(alpha, beta, order))
                {
                    for (i = 0; i < order; i++)
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
     *  T1 = type of x coordinates
     *  T2 = type of y coordinates
     *  T3 = type of coefficients (and for internal calculations)
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
                               const T3 *c)
    {
        int result = 0;
        if ((y != NULL) && (c != NULL) && (n > 0) && (xs < xe))
        {
            register unsigned int i;
            register unsigned int j;
            T3 x;
            T3 x2;
            T3 w;
            const T3 xo = (T3)xs;
            const T3 xi = (T3)(xe - xs) / (T3)(n - 1);
            for (i = 0; i < n; i++)
            {
                x = xo + (T3)i * xi;
                x2 = 1;
                w = 0;
                for (j = 0; j <= o; j++)
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
 
    /** solve the equation given by the two matrixes.
     *  T3 = type of coefficients (and for internal calculations)
     *
     ** @param  a  first matrix (array of values)
     *  @param  b  second matrix (array of values)
     *  @param  n  number of entries in array
     *
     ** @return true if successful, false otherwise
     */
    static int solve(T3 *a,
                     T3 *b,
                     const unsigned int n)
    {
        int result = 0;
        if ((a != NULL) && (b != NULL) && (n > 0))
        {
            register unsigned int i;
            register unsigned int j;
            register unsigned int k;
            signed int pivot;
            T3 mag;
            T3 mag2;
            T3 temp;
            for (i = 0; i < n; i++)
            {
                mag = 0;
                pivot = -1;
                for (j = i; j < n; j++)
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
                    const unsigned int piv = (unsigned int)pivot;
                    const unsigned int i_n = i * n;
                    if (piv != i)
                    {
                        const unsigned int piv_n = piv * n;
                        for (j = i; j < n; j++)
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
                    for (j = i; j < n; j++)
                        a[j + i_n] /= mag;
                    b[i] /= mag;
                    for (j = 0; j < n; j++)
                    {
                        if (i == j)
                            continue;
                        const unsigned int j_n = j * n;
                        mag2 = a[i + j_n];
                        for (k = i; k < n; k++)
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
 * Revision 1.1  1999-10-14 19:04:09  joergr
 * Added new template class that supports polynomial curve fitting algorithm.
 *
 *
 *
 */
