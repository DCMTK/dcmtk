/*
 *
 *  Copyright (C) 1996-2001, OFFIS
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
 *  Module:  dcmimage
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: DicomColorPixel (Source)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2001-11-09 16:49:38 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/libsrc/dicopx.cc,v $
 *  CVS/RCS Revision: $Revision: 1.11 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"
#include "dctypes.h"
#include "dcdeftag.h"

#include "dicopx.h"
#include "dimopx.h"
#include "diinpx.h"
#include "didocu.h"


/*----------------*
 *  constructors  *
 *----------------*/

DiColorPixel::DiColorPixel(const DiDocument *docu,
                           const DiInputPixel *pixel,
                           const Uint16 samples,
                           EI_Status &status,
                           const Uint16 /*sample_rate*/)
  : DiPixel(0),
    PlanarConfiguration(0)
{
    if (docu != NULL)
    {
        Uint16 us = 0;
        if (docu->getValue(DCM_SamplesPerPixel, us))
        {
            if (us != samples)
            {
                if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Warnings))
                {
                    ofConsole.lockCerr() << "WARNING: invalid value for 'SamplesPerPixel' (" << us
                                         << ") ... assuming " << samples << " !" << endl;
                    ofConsole.unlockCerr();
                }
            }
            if (docu->getValue(DCM_PlanarConfiguration, us))
            {
                PlanarConfiguration = (us == 1);
                if ((us != 0) && (us != 1))
                {
                    if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Warnings))
                    {
                        ofConsole.lockCerr() << "WARNING: invalid value for 'PlanarConfiguration' (" << us
                                             << ") ... assuming 'color-by-pixel' (0) !" << endl;
                        ofConsole.unlockCerr();
                    }
                }
            }
            else if (samples > 1)
            {
                status = EIS_MissingAttribute;
                if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Errors))
                {
                    ofConsole.lockCerr() << "ERROR: mandatory attribute 'PlanarConfiguration' is missing !" << endl;
                    ofConsole.unlockCerr();
                }
                return;
            }
            if (pixel != NULL)
//                Count = pixel->getCount() / ((sample_rate == 0) ? samples : sample_rate);
                Count = pixel->getPixelCount();
        }
        else
        {
            status = EIS_MissingAttribute;
            if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Errors))
            {
                ofConsole.lockCerr() << "ERROR: mandatory attribute 'SamplesPerPixel' is missing !" << endl;
                ofConsole.unlockCerr();
            }
        }
    }
}

DiColorPixel::DiColorPixel(const DiColorPixel *pixel,
                           const unsigned long count)
  : DiPixel(count),
    PlanarConfiguration(pixel->PlanarConfiguration)
{
}


/*--------------*
 *  destructor  *
 *--------------*/

DiColorPixel::~DiColorPixel()
{
}


/*
 *
 * CVS/RCS Log:
 * $Log: dicopx.cc,v $
 * Revision 1.11  2001-11-09 16:49:38  joergr
 * Removed unused constructor.
 *
 * Revision 1.10  2001/06/01 15:49:35  meichel
 * Updated copyright header
 *
 * Revision 1.9  2000/04/28 12:40:03  joergr
 * DebugLevel - global for the module - now derived from OFGlobal (MF-safe).
 *
 * Revision 1.8  2000/04/27 13:15:57  joergr
 * Dcmimage library code now consistently uses ofConsole for error output.
 *
 * Revision 1.7  2000/03/08 16:21:56  meichel
 * Updated copyright header.
 *
 * Revision 1.6  2000/03/03 14:07:55  meichel
 * Implemented library support for redirecting error messages into memory
 *   instead of printing them to stdout/stderr for GUI applications.
 *
 * Revision 1.5  1999/04/28 13:20:13  joergr
 * Introduced new scheme for the debug level variable: now each level can be
 * set separately (there is no "include" relationship).
 *
 * Revision 1.4  1998/11/27 14:29:32  joergr
 * Added copyright message.
 * Introduced global debug level for dcmimage module to control error output.
 *
 * Revision 1.3  1998/05/11 14:52:27  joergr
 * Added CVS/RCS header to each file.
 *
 *
 */
