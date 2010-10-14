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
 *  Purpose: DiCubicSpline Function/Interpolation (Header/Implementation)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:16:27 $
 *  CVS/RCS Revision: $Revision: 1.21 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DISPLINT_H
#define DISPLINT_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofcast.h"

#define INCLUDE_CSTDDEF               /* For NULL */
#include "dcmtk/ofstd/ofstdinc.h"


/*--------------------*
 *  macro definition  *
 *--------------------*/

// SunCC 4.x does not support default values for template types :-/
#define T3_ double


/*------------------*
 *  template class  *
 *------------------*/

/** Template class for cubic spline interpolation
 */
template <class T1, class T2 /*, class T3 = double*/>
class DiCubicSpline
{

 public:

    /** calculate spline function for given points.
     *  T1  = type of x coordinates
     *  T2  = type of y coordinates
     *  T3_ = type of y coordinates of the spline function
     *
     ** @param  x    array with x coordinates of given points
     *  @param  y    array with y coordinates of given points
     *  @param  n    number of entries in array (= points)
     *  @param  y2   array used to store the resulting spline function (used for CubicSplineInterpolation)
     *  @param  yp1  first derivative of the interpolating function at point 1
     *  @param  ypn  first derivative of the interpolating function at point n
     *
     ** @return true if successful, false otherwise
     */
    static int Function(const T1 *x,
                        const T2 *y,
                        const unsigned int n,
                        T3_ *y2,
                        const T3_ yp1 = 1.0e30,
                        const T3_ ypn = 1.0e30)
    {
        if ((x != NULL) && (y != NULL) && (n > 0) && (y2 != NULL))
        {
            T3_ *u = new T3_[n];                            // temporary vector
            if (u != NULL)
            {
                register unsigned int i;
                T3_ p, qn, sig, un;
                if (yp1 > 0.99e30)                          // ignore value for first derivative at point 1
                    y2[0] = u[0] = 0.0;
                else
                {
                    y2[0] = -0.5;
                    u[0] = (3.0 / (OFstatic_cast(T3_, x[1]) - OFstatic_cast(T3_, x[0]))) *
                           ((OFstatic_cast(T3_, y[1]) - OFstatic_cast(T3_, y[0])) /
                           (OFstatic_cast(T3_, x[1]) - OFstatic_cast(T3_, x[0])) - yp1);
                }
                for (i = 1; i < n - 1; ++i)
                {
                    sig = (OFstatic_cast(T3_, x[i]) - OFstatic_cast(T3_, x[i - 1])) /
                          (OFstatic_cast(T3_, x[i + 1]) - OFstatic_cast(T3_, x[i - 1]));
                    p = sig * y2[i - 1] + 2.0;
                    y2[i] = (sig - 1.0) / p;
                    u[i] = (OFstatic_cast(T3_, y[i + 1]) - OFstatic_cast(T3_, y[i])) /
                           (OFstatic_cast(T3_, x[i + 1]) - OFstatic_cast(T3_, x[i])) -
                           (OFstatic_cast(T3_, y[i]) - OFstatic_cast(T3_, y[i - 1])) /
                           (OFstatic_cast(T3_, x[i]) - OFstatic_cast(T3_, x[i - 1]));
                    u[i] = (6.0 * u[i] / (OFstatic_cast(T3_, x[i + 1]) -
                            OFstatic_cast(T3_, x[i - 1])) - sig * u[i - 1]) / p;
                }
                if (ypn > 0.99e30)                          // ignore value for first derivative at point 1
                    qn = un = 0.0;
                else
                {
                    qn = 0.5;
                    un = (3.0 / (OFstatic_cast(T3_, x[n - 1]) - OFstatic_cast(T3_, x[n - 2]))) *
                         (ypn - (OFstatic_cast(T3_, y[n - 1]) - OFstatic_cast(T3_, y[n - 2])) /
                         (OFstatic_cast(T3_, x[n - 1]) - OFstatic_cast(T3_, x[n - 2])));
                }
                y2[n - 1] = (un - qn * u[n - 2]) / (qn * y2[n - 2] + 1.0);
                for (i = n - 1; i > 0; --i)
                    y2[i - 1] = y2[i - 1] * y2[i] + u[i - 1];
                delete[] u;
                return 1;
            }
        }
        return 0;
    }


    /** perform cubic spline interpolation for given points.
     *  T1  = type of x coordinates
     *  T2  = type of y coordinates
     *  T3_ = type of y coordinates of the spline function
     *
     ** @param  xa   array with x coordinates of given points
     *  @param  ya   array with y coordinates of given points
     *  @param  y2a  array used to store the resulting spline function (calculated by CubicSplineFunction)
     *  @param  na   number of entries in above arrays (xa, ya and y2a)
     *  @param  x    array with x coordinates of points to be interpolated
     *  @param  y    array used to store interpolated values
     *  @param  n    number of entries in above array (x and y)
     *
     ** @return true if successful, false otherwise
     */
    static int Interpolation(const T1 *xa,
                             const T2 *ya,
                             const T3_ *y2a,
                             const unsigned int na,
                             const T1 *x,
                             T2 *y,
                             const unsigned int n)
    {
        if ((xa != NULL) && (ya != NULL) && (y2a != NULL) && (na > 0) && (x != NULL) && (y != NULL) && (n > 0))
        {
            register unsigned int k, i;
            register unsigned int klo = 0;
            register unsigned int khi = na - 1;
            T3_ h, b, a;
            for (i = 0; i < n; ++i)
            {
                if ((xa[klo] > x[i]) || (xa[khi] < x[i]))       // optimization
                {
                    klo = 0;
                    khi = na - 1;
                }
                while (khi - klo > 1)                           // search right place in the table, if necessary
                {
                    k = (khi + klo) >> 1;
                    if (xa[k] > x[i])
                        khi = k;
                    else
                        klo = k;
                }
                if (xa[khi] == x[i])                            // optimization: use known values
                    y[i] = ya[khi];
                else
                {
                    h = OFstatic_cast(T3_, xa[khi]) - OFstatic_cast(T3_, xa[klo]);
                    if (h == 0.0)                               // bad xa input, values must be distinct !
                        return 0;
                    a = (OFstatic_cast(T3_, xa[khi]) - OFstatic_cast(T3_, x[i])) / h;
                    b = (OFstatic_cast(T3_, x[i]) - OFstatic_cast(T3_, xa[klo])) / h;
                    y[i] = OFstatic_cast(T2, a * OFstatic_cast(T3_, ya[klo]) + b * OFstatic_cast(T3_, ya[khi]) +
                           ((a * a * a - a) * y2a[klo] + (b * b * b - b) * y2a[khi]) * (h * h) / 6.0);
                }
            }
            return 1;
        }
        return 0;
    }
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: displint.h,v $
 * Revision 1.21  2010-10-14 13:16:27  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.20  2010-03-01 09:08:47  uli
 * Removed some unnecessary include directives in the headers.
 *
 * Revision 1.19  2005-12-08 16:48:10  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.18  2003/12/23 15:53:22  joergr
 * Replaced post-increment/decrement operators by pre-increment/decrement
 * operators where appropriate (e.g. 'i++' by '++i').
 *
 * Revision 1.17  2003/12/08 19:20:47  joergr
 * Adapted type casts to new-style typecast operators defined in ofcast.h.
 * Removed leading underscore characters from preprocessor symbols (reserved
 * symbols). Updated copyright header.
 *
 * Revision 1.16  2002/07/18 12:30:59  joergr
 * Removed unused code.
 *
 * Revision 1.15  2001/06/01 15:49:51  meichel
 * Updated copyright header
 *
 * Revision 1.14  2000/03/08 16:24:24  meichel
 * Updated copyright header.
 *
 * Revision 1.13  2000/02/02 14:33:54  joergr
 * Replaced 'delete' statements by 'delete[]' for objects created with 'new[]'.
 *
 * Revision 1.12  1999/10/21 08:29:42  joergr
 * Renamed template type definition from 'T3' to 'T3_' to avoid naming
 * conflicts.
 *
 * Revision 1.11  1999/10/20 18:38:50  joergr
 * Eliminated default values for template types since this features is not
 * supported by SunCC 4.x (temporarily introduced '#define' instead).
 *
 * Revision 1.10  1999/10/15 09:38:31  joergr
 * Fixed typos.
 *
 * Revision 1.9  1999/10/14 19:05:17  joergr
 * Fixed typo.
 *
 * Revision 1.8  1999/10/01 13:25:35  joergr
 * Enhanced template class for cubic spline interpolation to support
 * non-floating point classes/types as y-coordinates.
 *
 * Revision 1.7  1999/07/23 14:11:25  joergr
 * Added preliminary support for 2D bi-cubic spline interpolation (currently
 * not used).
 *
 * Revision 1.6  1999/05/03 11:09:31  joergr
 * Minor code purifications to keep Sun CC 2.0.1 quiet.
 *
 * Revision 1.5  1999/04/29 13:49:08  joergr
 * Renamed class CubicSpline to DiCubicSpline.
 *
 * Revision 1.4  1999/03/24 17:20:26  joergr
 * Added/Modified comments and formatting.
 *
 * Revision 1.3  1999/03/22 08:52:43  joergr
 * Added/Changed comments.
 *
 * Revision 1.2  1999/02/25 16:17:16  joergr
 * Initialize local variables to avoid compiler warnings (reported by gcc
 * 2.7.2.1 on Linux).
 *
 * Revision 1.1  1999/02/11 16:36:29  joergr
 * Renamed file to indicate the use of templates.
 *
 * Revision 1.2  1999/02/09 14:21:54  meichel
 * Removed default parameters from template functions, required for Sun CC 4.2
 *
 * Revision 1.1  1999/02/04 17:59:23  joergr
 * Added support for calibration according to Barten transformation (incl.
 * a DISPLAY file describing the monitor characteristic).
 *
 *
 */
