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
 *  Purpose: CubicSplineFunction/Interpolation (Header/Implementation)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1999-02-11 16:36:29 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/include/Attic/displint.h,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 * 
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __DISPLINE_H
#define __DISPLINE_H

#include "osconfig.h"


/*------------------*
 *  template class  *
 *------------------*/

template <class T1, class T2>
class CubicSpline
{

 public:

    /** calculate spline function for given points
     *  T1 = type of x coordinates
     *  T2 = type of y coordinates
     *
     ** @param  x    array with x coordinates of given points
     *  @param  y    array with y coordinates of given points
     *  @param  n    number of entries in array (= points)
     *  @param  y2   array used to store the resulting spline function (used for CubicSplineInterpolation)
     *  @param  yp1  first derivative of the interpolating function at point 1
     *  @param  ypn  first derivative of the interpolating function at point n
     *
     ** @result true if successful, false otherwise
     */
    static int Function(const T1 *x,
                        const T2 *y,
                        const unsigned int n,
                        T2 *y2,
                        const T2 yp1 = 1.0e30,
                        const T2 ypn = 1.0e30)
    {
        if ((x != NULL) && (y != NULL) && (n > 0) && (y2 != NULL))
        {
            T2 *u = new T2[n];                              // temporary vector
            if (u != NULL)
            {
                register unsigned int i;
                T2 p, qn, sig, un;
                if (yp1 > 0.99e30)                          // ignore value for first derivative at point 1
                    y2[0] = u[0] = 0.0;
                else
                {
                    y2[0] = -0.5;
                    u[0] = (3.0 / (T2)(x[1] - x[0])) * ((y[1] - y[0]) / (T2)(x[1] - x[0]) - yp1);
                }
                for (i = 1; i < n - 1; i++)
                {
                    sig = (T2)(x[i] - x[i - 1]) / (x[i + 1] - x[i - 1]);
                    p = sig * y2[i - 1] + 2.0;
                    y2[i] = (sig - 1.0) / p;
                    u[i] = (y[i + 1] - y[i]) / (T2)(x[i + 1] - x[i]) - (y[i] - y[i - 1]) / (T2)(x[i] - x[i - 1]);
                    u[i] = (6.0 * u[i] / (T2)(x[i + 1] - x[i - 1]) - sig * u[i - 1]) / p;
                }
                if (ypn > 0.99e30)                          // ignore value for first derivative at point 1
                    qn = un = 0.0;
                else
                {
                    qn = 0.5;
                    un = (3.0 / (T2)(x[n - 1] - x[n - 2])) * (ypn - (y[n - 1] - y[n - 2]) / (T2)(x[n - 1] - x[n - 2]));
                }
                y2[n - 1] = (un - qn * u[n - 2]) / (qn * y2[n - 2] + 1.0);
                for (i = n - 1; i > 0; i--)
                    y2[i - 1] = y2[i - 1] * y2[i] + u[i - 1];
                delete u;
                return 1;
            }
        }
        return 0;
    }



    /** perform cubic spline interpolation for given points
     *  T1 = type of x coordinates
     *  T2 = type of y coordinates
     *
     ** @param  xa   array with x coordinates of given points
     *  @param  ya   array with y coordinates of given points
     *  @param  y2a  array used to store the resulting spline function (calculated by CubicSplineFunction)
     *  @param  na   number of entries in above arrays (xa, ya and y2a)
     *  @param  x    array with x coordinates of points to be interpolated
     *  @param  y    array used to store interpolated values
     *  @param  n    number of entries in above array (x and y)
     *
     ** @result true if successful, false otherwise
     */
    static int Interpolation(const T1 *xa,
                             const T2 *ya,
                             const T2 *y2a,
                             const unsigned int na,
                             const T1 *x,
                             T2 *y,             
                             const unsigned int n)
    {
        if ((xa != NULL) && (ya != NULL) && (y2a != NULL) && (na > 0) && (x != NULL) && (y != NULL) && (n > 0))
        {
            register unsigned int k, i, klo, khi;
            T2 h, b, a;
            for (i = 0; i < n; i++)
            {
                if ((i == 0) || (xa[klo] > x[i]) || (xa[khi] < x[i]))       // optimization
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
                    h = (T2)(xa[khi] - xa[klo]);
                    if (h == 0.0)                               // bad xa input, values must be distinct !
                        return 0;
                    a = (T2)(xa[khi] - x[i]) / h;
                    b = (T2)(x[i] - xa[klo]) / h;
                    y[i] = a * ya[klo] + b * ya[khi] + ((a * a * a - a) * y2a[klo] + (b * b * b - b) * y2a[khi]) * (h * h) / 6.0;
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
 * Revision 1.1  1999-02-11 16:36:29  joergr
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
