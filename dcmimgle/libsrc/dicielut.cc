/*
 *
 *  Copyright (C) 1996-2002, OFFIS
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
 *  Update Date:      $Date: 2002-07-18 12:33:07 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/libsrc/dicielut.cc,v $
 *  CVS/RCS Revision: $Revision: 1.14 $
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
                         const double *val_tab,
                         const unsigned long ddl_cnt,
                         const double val_min,
                         const double val_max,
                         const double amb,
                         const OFBool inverse,
                         ostream *stream,
                         const OFBool printMode)
  : DiDisplayLUT(count, max, amb /*, 'illum' not used*/)
{
    if ((Count > 0) && (Bits > 0))
    {
#ifdef DEBUG
        if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Informationals))
        {
            ofConsole.lockCerr() << "INFO: new CIELAB LUT with " << Bits << " bits output and "
                                 << Count << " entries created !" << endl;
            ofConsole.unlockCerr();
        }
#endif
        Valid = createLUT(ddl_tab, val_tab, ddl_cnt, val_min, val_max, inverse, stream, printMode);
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
                           const double *val_tab,
                           const unsigned long ddl_cnt,
                           const double val_min,
                           const double val_max,
                           const OFBool inverse,
                           ostream *stream,
                           const OFBool printMode)
{
    int status = 0;
    if ((ddl_tab != NULL) && (val_tab != NULL) && (ddl_cnt > 0) && (val_max > 0))
    {
        double *cielab = new double[Count];
        if (cielab != NULL)
        {
            register unsigned long i;
            register double llin = 0;
            register double cub = 0;
            const double amb = getAmbientLightValue();
            const double min = val_min + amb;
            const double max = val_max + amb;
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
                register Uint16 *q = DataBuffer;
                register unsigned long j = 0;
                if (inverse)
                {
                    register const double *r = val_tab;
                    register double v;
                    /* convert from DDL */
                    for (i = Count; i != 0; i--, r++)
                    {
                        v = *r + amb;
                        while ((j + 1 < ddl_cnt) && (cielab[j] < v))          // search for closest index, assuming monotony
                            j++;
                        if ((j > 0) && (fabs(cielab[j - 1] - v) < fabs(cielab[j] - v)))
                            j--;
                        *(q++) = ddl_tab[j];
                    }
                } else {
                    register const double *r = cielab;
                    /* convert to DDL */
                    for (i = Count; i != 0; i--, r++)
                    {
                        while ((j + 1 < ddl_cnt) && (val_tab[j] + amb < *r))  // search for closest index, assuming monotony
                            j++;
                        if ((j > 0) && (fabs(val_tab[j - 1] + amb - *r) < fabs(val_tab[j] + amb - *r)))
                            j--;
                        *(q++) = ddl_tab[j];
                    }
                }
                Data = DataBuffer;
                if (stream != NULL)                         // write curve data to file
                {
                    if (Count == ddl_cnt)                   // check whether CIELAB LUT fits exactly to DISPLAY file
                    {
                        for (i = 0; i < ddl_cnt; i++)
                        {
                            (*stream) << ddl_tab[i];                               // DDL
                            stream->setf(ios::fixed, ios::floatfield);
                            if (printMode)
                                (*stream) << "\t" << val_tab[i] + amb;             // CC
                            (*stream) << "\t" << cielab[i];                        // CIELAB
                            if (printMode)
                            {
                                if (inverse)
                                    (*stream) << "\t" << cielab[Data[i]];          // PSC'
                                else
                                    (*stream) << "\t" << val_tab[Data[i]] + amb;   // PSC
                            }
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
 * Revision 1.14  2002-07-18 12:33:07  joergr
 * Added support for hardcopy and softcopy input devices (camera and scanner).
 *
 * Revision 1.13  2002/07/03 13:50:59  joergr
 * Fixed inconsistencies regarding the handling of ambient light.
 *
 * Revision 1.12  2002/07/02 16:24:36  joergr
 * Added support for hardcopy devices to the calibrated output routines.
 *
 * Revision 1.11  2001/06/01 15:49:53  meichel
 * Updated copyright header
 *
 * Revision 1.10  2000/05/03 09:47:22  joergr
 * Removed most informational and some warning messages from release built
 * (#ifndef DEBUG).
 *
 * Revision 1.9  2000/04/28 12:33:41  joergr
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
