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
 *  Purpose: DicomBartenLUT (Source)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1999-02-03 17:48:36 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/libsrc/Attic/dibarlut.cc,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"

#include "dibarlut.h"
#include "displine.h"

//BEGIN_EXTERN_C
 #include <math.h>
//END_EXTERN_C


/*----------------*
 *  constructors  *
 *----------------*/

DiBartenLUT::DiBartenLUT(const unsigned long count,
                         const Uint16 *ddl_tab,
                         const double *lum_tab,
                         const Uint16 ddl_cnt,
                         const double *gsdf_tab,
                         const double *gsdf_spl,
                         const unsigned int gsdf_cnt,
                         const double jnd_min,
                         const double jnd_max)
  : DiBaseLUT(count, DicomImageClass::tobits(ddl_cnt))
{
    if ((Count > 0) && (Bits > 0))
    {
        if (DicomImageClass::DebugLevel >= DicomImageClass::DL_Informationals)
            cerr << "INFO: new Barten LUT with " << Bits << " bits output and " << Count << " entries created !" << endl;
        Valid = createLUT(ddl_tab, lum_tab, ddl_cnt, gsdf_tab, gsdf_spl, gsdf_cnt, jnd_min, jnd_max);
    }
} 


/*--------------*
 *  destructor  *
 *--------------*/

DiBartenLUT::~DiBartenLUT()
{
}


/********************************************************************/


int DiBartenLUT::createLUT(const Uint16 *ddl_tab,
                           const double *lum_tab,
                           const Uint16 ddl_cnt,
                           const double *gsdf_tab,
                           const double *gsdf_spl,
                           const unsigned int gsdf_cnt,
                           const double jnd_min,
                           const double jnd_max)
{
    if ((ddl_tab != NULL) && (lum_tab != NULL) && (ddl_cnt > 0) && (gsdf_tab != NULL) && (gsdf_spl != NULL) && (gsdf_cnt > 0))
    {
        int status = 0;
        double *jidx = new double[Count];
        if (jidx != NULL)
        {
            const double dist = (jnd_max - jnd_min) / (Count - 1);      // distance between two entries
            register unsigned int i;
            register double *r = jidx;
            register double value = jnd_min;                            // first values is static !
            for (i = 1; i < Count; i++)                                 // initialize scaled JND index array
            {
                *(r++) = value;
                value += dist;                                          // add step by step ...
            }
            *r = jnd_max;                                               // last value is static !

            double *jnd_idx = new double[gsdf_cnt];
            if (jnd_idx != NULL)
            {
                r = jnd_idx;
                for (i = 0; i < gsdf_cnt; i++)                          // initialize JND index array
                    *(r++) = i + 1;

                double *gsdf = new double[Count];                       // interpolated GSDF
                if (gsdf != NULL)
                {     
                    if (CubicSplineInterpolation<double, double>(jnd_idx, gsdf_tab, gsdf_spl, gsdf_cnt, jidx, gsdf, Count))
                    {
                        DataBuffer = new Uint16[Count];
                        if (DataBuffer != NULL)
                        {
                            r = gsdf;
                            register Uint16 *q = DataBuffer;
                            register Uint16 j = 0;
                            for (i = 0; i < Count; i++, r++)
                            {
                                while ((j + 1 < ddl_cnt) && (lum_tab[j] < *r))      // search for closest index, assuming monotony
                                    j++;
                                if ((j > 0) && (fabs(lum_tab[j - 1] - *r) < fabs(lum_tab[j] - *r)))
                                    j--;
// cerr << "lut[" << i << "] = " << j << "  " << "lum: " << *r << "  " << lum_tab[j] << "  " << fabs(lum_tab[j] - *r) << endl;
                                *(q++) = j;
                            }
                            Data = DataBuffer;
                            status = 1;
                        }
                    }
                }
                delete[] gsdf;
            }
            delete[] jnd_idx;
        }
        delete[] jidx;
        return status;
    }
    return 0;
}


/*
 *
 * CVS/RCS Log:
 * $Log: dibarlut.cc,v $
 * Revision 1.1  1999-02-03 17:48:36  joergr
 * Added support for calibration according to Barten transformation (incl.
 * a DISPLAY file describing the monitor characteristic).
 *
 *
 *
 */
