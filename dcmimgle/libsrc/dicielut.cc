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
 *  Purpose: DicomCIELABLUT (Source)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1999-10-18 15:06:23 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/libsrc/dicielut.cc,v $
 *  CVS/RCS Revision: $Revision: 1.4 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"

#include "dicielut.h"

#include <math.h>


/*----------------*
 *  constructors  *
 *----------------*/

DiCIELABLUT::DiCIELABLUT(const unsigned long count,
                         const Uint16 max,
                         const Uint16 *ddl_tab,
                         const double *lum_tab,
                         const unsigned long ddl_cnt,
                         const double lum_min,
                         const double lum_max,
                         const double amb,
                         ostream *stream,
                         const OFBool mode)
  : DiDisplayLUT(count, max, amb)
{
    if ((Count > 0) && (Bits > 0))
    {
        if (DicomImageClass::DebugLevel & DicomImageClass::DL_Informationals)
            cerr << "INFO: new CIELAB LUT with " << Bits << " bits output and " << Count << " entries created !" << endl;
        Valid = createLUT(ddl_tab, lum_tab, ddl_cnt, lum_min, lum_max, stream, mode);
    }
}


/*--------------*
 *  destructor  *
 *--------------*/

DiCIELABLUT::~DiCIELABLUT()
{
}


/********************************************************************/


int DiCIELABLUT::createLUT(const Uint16 *ddl_tab,
                           const double *lum_tab,
                           const unsigned long ddl_cnt,
                           const double lum_min,
                           const double lum_max,
                           ostream *stream,
                           const OFBool mode)
{
    int status = 0;
    if ((ddl_tab != NULL) && (lum_tab != NULL) && (ddl_cnt > 0))
    {
        double *cielab = new double[Count];
        if (cielab != NULL)
        {
            register unsigned long i;
            register double llin = 0;
            register double cub = 0;
            const double amb = getAmbientLightValue();
            const double min = lum_min + amb;
            const double max = lum_max + amb;
            const double lfac = 100.0 / ((double)(Count - 1) * 903.3);
            const double cfac = 100.0 / ((double)(Count - 1) * 116.0);
            const double fac = max - min;
            for (i = 0; i < Count; i++)                     // compute CIELAB function
            {
                llin = (double)i * lfac;
                cub = (double)i * cfac + (16.0 / 116.0);
                cielab[i] = ((llin > 0.008856) ? cub * cub * cub : llin) * fac + min;
            }
            DataBuffer = new Uint16[Count];
            if (DataBuffer != NULL)                         // create look-up table
            {
                register double *r = cielab;
                register Uint16 *q = DataBuffer;
                register unsigned long j = 0;
                for (i = Count; i != 0; i--, r++)
                {
                    while ((j + 1 < ddl_cnt) && (lum_tab[j]  + amb < *r))  // search for closest index, assuming monotony
                        j++;
                    if ((j > 0) && (fabs(lum_tab[j - 1] + amb - *r) < fabs(lum_tab[j] + amb - *r)))
                        j--;
                    *(q++) = ddl_tab[j];
                }
                Data = DataBuffer;
                if (stream != NULL)                         // write curve data to file
                {
                    if (Count == ddl_cnt)                   // check whether CIELAB LUT fits exactly to DISPLAY file
                    {
                        for (i = 0; i < ddl_cnt; i++)
                        {
                            (*stream) << ddl_tab[i];
                            stream->setf(ios::fixed, ios::floatfield);
                            if (mode)
                                (*stream) << "\t" << lum_tab[i] + amb;
                            (*stream) << "\t" << cielab[i];
                            if (mode)
                                (*stream) << "\t" << lum_tab[Data[i]] + amb;
                            (*stream) << endl;
                        }
                    } else {
                        if (DicomImageClass::DebugLevel & DicomImageClass::DL_Warnings)
                            cerr << "WARNING: can't write curve data, wrong DISPLAY file or CIELAB LUT !" << endl;
                    }
                }
                status = 1;
            }
        }
        delete[] cielab;
    }
    return status;
}


/*
 *
 * CVS/RCS Log:
 * $Log: dicielut.cc,v $
 * Revision 1.4  1999-10-18 15:06:23  joergr
 * Enhanced command line tool dcmdspfn (added new options).
 *
 * Revision 1.3  1999/10/18 10:14:01  joergr
 * Simplified calculation of CIELAB function (now fully percentage based).
 *
 * Revision 1.2  1999/09/17 13:13:28  joergr
 * Enhanced efficiency of some "for" loops.
 *
 * Revision 1.1  1999/09/10 08:54:48  joergr
 * Added support for CIELAB display function. Restructured class hierarchy
 * for display functions.
 *
 */
