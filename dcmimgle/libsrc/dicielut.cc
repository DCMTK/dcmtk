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
 *  Purpose: DicomCIELABLUT (Source)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2000-04-28 12:33:41 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/libsrc/dicielut.cc,v $
 *  CVS/RCS Revision: $Revision: 1.9 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"

#include "ofconsol.h"
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
        if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Informationals))
        {
            ofConsole.lockCerr() << "INFO: new CIELAB LUT with " << Bits << " bits output and " << Count << " entries created !" << endl;
            ofConsole.unlockCerr();
        }
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
    if ((ddl_tab != NULL) && (lum_tab != NULL) && (ddl_cnt > 0) && (lum_max > 0))
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
            const double lmin = min / max;
            const double hmin = (lmin > 0.008856) ? 116.0 * pow(lmin, 1.0 / 3.0) - 16 : 903.3 * lmin;
            const double lfac = (100.0 - hmin) / ((double)(Count - 1) * 903.3);
            const double loff = hmin / 903.3;
            const double cfac = (100.0 - hmin) / ((double)(Count - 1) * 116.0);
            const double coff = (16.0  + hmin) / 116.0;
            for (i = 0; i < Count; i++)                     // compute CIELAB function
            {
                llin = (double)i * lfac + loff;
                cub = (double)i * cfac + coff;
                cielab[i] = ((llin > 0.008856) ? cub * cub * cub : llin) * max;
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
                        if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Warnings))
                        {
                            ofConsole.lockCerr() << "WARNING: can't write curve data, wrong DISPLAY file or CIELAB LUT !" << endl;
                            ofConsole.unlockCerr();
                        }
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
 * Revision 1.9  2000-04-28 12:33:41  joergr
 * DebugLevel - global for the module - now derived from OFGlobal (MF-safe).
 *
 * Revision 1.8  2000/04/27 13:10:25  joergr
 * Dcmimgle library code now consistently uses ofConsole for error output.
 *
 * Revision 1.7  2000/03/08 16:24:26  meichel
 * Updated copyright header.
 *
 * Revision 1.6  2000/03/03 14:09:17  meichel
 * Implemented library support for redirecting error messages into memory
 *   instead of printing them to stdout/stderr for GUI applications.
 *
 * Revision 1.5  1999/10/21 17:46:46  joergr
 * Corrected calculation of CIELAB display curve (thanks to Mr. Mertelmeier
 * from Siemens).
 *
 * Revision 1.4  1999/10/18 15:06:23  joergr
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
