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
 *  Update Date:      $Date: 1999-09-17 13:13:28 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/libsrc/dicielut.cc,v $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"

#include "dicielut.h"

//BEGIN_EXTERN_C
 #include <math.h>
//END_EXTERN_C


/*----------------*
 *  constructors  *
 *----------------*/

DiCIELABLUT::DiCIELABLUT(const unsigned long count,
                         const Uint16 max,
                         const Uint16 *ddl_tab,
                         const double *lum_tab,
                         const Uint16 ddl_cnt,
                         const double lum_min,
                         const double lum_max,
                         const double amb,
                         ostream *stream)
  : DiDisplayLUT(count, max, amb)
{
    if ((Count > 0) && (Bits > 0))
    {
        if (DicomImageClass::DebugLevel & DicomImageClass::DL_Informationals)
            cerr << "INFO: new CIELAB LUT with " << Bits << " bits output and " << Count << " entries created !" << endl;
        Valid = createLUT(ddl_tab, lum_tab, ddl_cnt, lum_min, lum_max, stream);
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
                           const Uint16 ddl_cnt,
                           const double lum_min,
                           const double lum_max,
                           ostream *stream)
{
    int status = 0;
    if ((ddl_tab != NULL) && (lum_tab != NULL) && (ddl_cnt > 0))
    {
        double *cielab = new double[Count];
        if (cielab != NULL)
        {
            register unsigned int i;
            register double llin = 0;
            register double lcub = 0;
            register double cub = 0;
            const double amb = getAmbientLightValue();
            const double min = lum_min + amb;
            const double max = lum_max + amb;
            const double lfac = 100 * max / ((double)(Count - 1) * 903.3);
            const double cfac = 100 / ((double)(Count - 1) * 116);
            const double tresh = max * 0.008856;
            const double fac = (max - min) / max;
            for (i = 0; i < Count; i++)
            {
                llin = (double)i * lfac;
                cub = (double)i * cfac + (16.0 / 116.0);
                lcub = max * cub * cub * cub;
                cielab[i] = ((llin < tresh) ? llin : lcub) * fac + min;
            }
            DataBuffer = new Uint16[Count];
            if (DataBuffer != NULL)
            {
                register double *r = cielab;
                register Uint16 *q = DataBuffer;
                register Uint16 j = 0;
                for (i = Count; i != 0; i--, r++)
                {
                    while (((Uint16)(j + 1) < ddl_cnt) && (lum_tab[j]  + amb < *r))  // search for closest index, assuming monotony
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
                            (*stream) << ddl_tab[i] << "\t"; 
                            stream->setf(ios::fixed, ios::floatfield);
                            (*stream) << lum_tab[i] + amb << "\t";
                            (*stream) << cielab[i] << "\t";
                            (*stream) << lum_tab[Data[i]] + amb << endl;
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
 * Revision 1.2  1999-09-17 13:13:28  joergr
 * Enhanced efficiency of some "for" loops.
 *
 * Revision 1.1  1999/09/10 08:54:48  joergr
 * Added support for CIELAB display function. Restructured class hierarchy
 * for display functions.
 *
 */
