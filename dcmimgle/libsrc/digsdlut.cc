/*
 *
 *  Copyright (C) 1996-2000, OFFIS
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
 *  Purpose: DicomGSDFLUT (Source)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2000-05-03 09:47:23 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/libsrc/digsdlut.cc,v $
 *  CVS/RCS Revision: $Revision: 1.8 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"

#include "ofconsol.h"
#include "digsdlut.h"
#include "displint.h"

#include <math.h>


/*----------------*
 *  constructors  *
 *----------------*/

DiGSDFLUT::DiGSDFLUT(const unsigned long count,
                     const Uint16 max,
                     const Uint16 *ddl_tab,
                     const double *lum_tab,
                     const unsigned long ddl_cnt,
                     const double *gsdf_tab,
                     const double *gsdf_spl,
                     const unsigned int gsdf_cnt,
                     const double jnd_min,
                     const double jnd_max,
                     const double amb,
                     ostream *stream,
                     const OFBool mode)
  : DiDisplayLUT(count, max, amb)
{
    if ((Count > 0) && (Bits > 0))
    {
#ifdef DEBUG
        if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Informationals))
        {
            ofConsole.lockCerr() << "INFO: new GSDF LUT with " << Bits << " bits output and "
                                 << Count << " entries created !" << endl;
            ofConsole.unlockCerr();
        }
#endif
        Valid = createLUT(ddl_tab, lum_tab, ddl_cnt, gsdf_tab, gsdf_spl, gsdf_cnt, jnd_min, jnd_max, stream, mode);
    }
} 


/*--------------*
 *  destructor  *
 *--------------*/

DiGSDFLUT::~DiGSDFLUT()
{
}


/********************************************************************/


int DiGSDFLUT::createLUT(const Uint16 *ddl_tab,
                         const double *lum_tab,
                         const unsigned long ddl_cnt,
                         const double *gsdf_tab,
                         const double *gsdf_spl,
                         const unsigned int gsdf_cnt,
                         const double jnd_min,
                         const double jnd_max,
                         ostream *stream,
                         const OFBool mode)
{
    if ((ddl_tab != NULL) && (lum_tab != NULL) && (ddl_cnt > 0) && (gsdf_tab != NULL) && (gsdf_spl != NULL) && (gsdf_cnt > 0))
    {
        int status = 0;
        double *jidx = new double[Count];
        if (jidx != NULL)
        {
            const double dist = (jnd_max - jnd_min) / (Count - 1);      // distance between two entries
            register unsigned long i;
            register double *r = jidx;
            register double value = jnd_min;                            // first value is static !
            for (i = Count; i > 1; i--)                                 // initialize scaled JND index array
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
                            register unsigned long j = 0;
                            const double amb = getAmbientLightValue();
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
                                if (Count == ddl_cnt)                   // check whether GSDF LUT fits exactly to DISPLAY file
                                {
                                    for (i = 0; i < ddl_cnt; i++)
                                    {
                                        (*stream) << ddl_tab[i]; 
                                        stream->setf(ios::fixed, ios::floatfield);
                                        if (mode)
                                            (*stream) << "\t" << lum_tab[i] + amb;
                                        (*stream) << "\t" << gsdf[i];
                                        if (mode)
                                            (*stream) << "\t" << lum_tab[Data[i]] + amb;
                                        (*stream) << endl;
                                    }
                                } else {
                                    if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Warnings))
                                    {
                                        ofConsole.lockCerr() << "WARNING: can't write curve data, "
                                                             << "wrong DISPLAY file or GSDF LUT !" << endl;
                                        ofConsole.unlockCerr();
                                    }
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
 * $Log: digsdlut.cc,v $
 * Revision 1.8  2000-05-03 09:47:23  joergr
 * Removed most informational and some warning messages from release built
 * (#ifndef DEBUG).
 *
 * Revision 1.7  2000/04/28 12:33:43  joergr
 * DebugLevel - global for the module - now derived from OFGlobal (MF-safe).
 *
 * Revision 1.6  2000/04/27 13:10:27  joergr
 * Dcmimgle library code now consistently uses ofConsole for error output.
 *
 * Revision 1.5  2000/03/08 16:24:28  meichel
 * Updated copyright header.
 *
 * Revision 1.4  2000/03/03 14:09:18  meichel
 * Implemented library support for redirecting error messages into memory
 *   instead of printing them to stdout/stderr for GUI applications.
 *
 * Revision 1.3  1999/10/18 15:06:25  joergr
 * Enhanced command line tool dcmdspfn (added new options).
 *
 * Revision 1.2  1999/09/17 13:13:30  joergr
 * Enhanced efficiency of some "for" loops.
 *
 * Revision 1.1  1999/09/10 08:54:50  joergr
 * Added support for CIELAB display function. Restructured class hierarchy
 * for display functions.
 *
 *
 *
 */
