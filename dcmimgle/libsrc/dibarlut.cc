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
 *  Update Date:      $Date: 1999-05-03 11:05:27 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/libsrc/Attic/dibarlut.cc,v $
 *  CVS/RCS Revision: $Revision: 1.10 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"

#include "dibarlut.h"
#include "displint.h"

//BEGIN_EXTERN_C
 #include <math.h>
//END_EXTERN_C


/*----------------*
 *  constructors  *
 *----------------*/

DiBartenLUT::DiBartenLUT(const unsigned long count,
                         const Uint16 max,
                         const Uint16 *ddl_tab,
                         const double *lum_tab,
                         const Uint16 ddl_cnt,
                         const double *gsdf_tab,
                         const double *gsdf_spl,
                         const unsigned int gsdf_cnt,
                         const double jnd_min,
                         const double jnd_max,
                         const double amb,
                         ostream *stream)
  : DiBaseLUT(count, DicomImageClass::tobits(max, 0)),
    AmbientLight(amb)
{
    if ((Count > 0) && (Bits > 0))
    {
        if (DicomImageClass::DebugLevel & DicomImageClass::DL_Informationals)
            cerr << "INFO: new Barten LUT with " << Bits << " bits output and " << Count << " entries created !" << endl;
        Valid = createLUT(ddl_tab, lum_tab, ddl_cnt, gsdf_tab, gsdf_spl, gsdf_cnt, jnd_min, jnd_max, stream);
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
                           const double jnd_max,
                           ostream *stream)
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
            register double value = jnd_min;                            // first value is static !
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
                    if (DiCubicSpline<double, double>::Interpolation(jnd_idx, gsdf_tab, gsdf_spl, gsdf_cnt, jidx, gsdf, (unsigned int)Count))
                    {
                        DataBuffer = new Uint16[Count];
                        if (DataBuffer != NULL)
                        {
                            r = gsdf;
                            register Uint16 *q = DataBuffer;
                            register Uint16 j = 0;
                            for (i = 0; i < Count; i++, r++)
                            {
                                while (((Uint16)(j + 1) < ddl_cnt) && (lum_tab[j]  + AmbientLight < *r))  // search for closest index, assuming monotony
                                    j++;
                                if ((j > 0) && (fabs(lum_tab[j - 1] + AmbientLight - *r) < fabs(lum_tab[j] + AmbientLight - *r)))
                                    j--;
                                *(q++) = ddl_tab[j];
                            }
                            Data = DataBuffer;
                            if (stream != NULL)                         // write curve data to file
                            {
                                if (Count == ddl_cnt)                   // check whether Barten LUT fits exactly to DISPLAY file
                                {
                                    for (i = 0; i < ddl_cnt; i++)
                                    {
                                        (*stream) << ddl_tab[i] << "\t"; 
                                        stream->setf(ios::fixed, ios::floatfield);
                                        (*stream) << lum_tab[i] + AmbientLight << "\t";
                                        (*stream) << gsdf[i] << "\t";
                                        (*stream) << lum_tab[Data[i]] + AmbientLight << endl;
                                    }
                                } else {
                                    if (DicomImageClass::DebugLevel & DicomImageClass::DL_Warnings)
                                        cerr << "WARNING: can't write CurveData, wrong DISPLAY file or Barten LUT !" << endl;
                                }
                            }
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
 * Revision 1.10  1999-05-03 11:05:27  joergr
 * Minor code purifications to keep Sun CC 2.0.1 quiet.
 *
 * Revision 1.9  1999/04/29 13:49:36  joergr
 * Renamed class CubicSpline to DiCubicSpline.
 *
 * Revision 1.8  1999/04/28 15:01:42  joergr
 * Introduced new scheme for the debug level variable: now each level can be
 * set separately (there is no "include" relationship).
 *
 * Revision 1.7  1999/03/04 09:43:28  joergr
 * Barten LUT is now be re-created when ambient light value has changed.
 *
 * Revision 1.6  1999/03/03 12:05:14  joergr
 * Added support to specify ambient light value (re: Barten transformation).
 *
 * Revision 1.5  1999/02/23 16:56:06  joergr
 * Added tool to export display curves to a text file.
 *
 * Revision 1.4  1999/02/11 16:46:40  joergr
 * Removed unused parameter / member variable.
 * Renamed file to indicate the use of templates. Moved global functions for
 * cubic spline interpolation to static methods of a separate template class.
 *
 * Revision 1.3  1999/02/09 14:22:30  meichel
 * Removed explicit template parameters from template function calls,
 *   required for Sun CC 4.2
 *
 * Revision 1.2  1999/02/08 13:07:30  joergr
 * Corrected some typos and formatting.
 *
 * Revision 1.1  1999/02/03 17:48:36  joergr
 * Added support for calibration according to Barten transformation (incl.
 * a DISPLAY file describing the monitor characteristic).
 *
 *
 */
