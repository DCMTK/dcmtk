/*
 *
 *  Copyright (C) 1996-99, OFFIS
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
 *  Update Date:      $Date: 1999-04-28 13:20:13 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/libsrc/dicopx.cc,v $
 *  CVS/RCS Revision: $Revision: 1.5 $
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
                           const Uint16 sample_rate)
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
                if (DicomImageClass::DebugLevel & DicomImageClass::DL_Warnings)
                {
                    cerr << "WARNING: invalid value for 'SamplesPerPixel' (" << us;
                    cerr << ") ... assuming " << samples << " !" << endl;
                }
            }
            if (docu->getValue(DCM_PlanarConfiguration, us))
            {
                PlanarConfiguration = (us == 1);
                if ((us != 0) && (us != 1))
                {
                    if (DicomImageClass::DebugLevel & DicomImageClass::DL_Warnings)
                    {
                        cerr << "WARNING: invalid value for 'PlanarConfiguration' (" << us;
                        cerr << ") ... assuming 'color-by-pixel' (0) !" << endl;
                    }
                }
            }
            else if (samples > 1)
            {
                status = EIS_MissingAttribute;
                if (DicomImageClass::DebugLevel & DicomImageClass::DL_Errors)
                    cerr << "ERROR: mandatory attribute 'PlanarConfiguration' is missing !" << endl;
                return;
            }
            if (pixel != NULL)
                Count = pixel->getCount() / ((sample_rate == 0) ? samples : sample_rate);
        }
        else
        {
            status = EIS_MissingAttribute;
            if (DicomImageClass::DebugLevel & DicomImageClass::DL_Errors)
                cerr << "ERROR: mandatory attribute 'SamplesPerPixel' is missing !" << endl;
        }
    }
}


DiColorPixel::DiColorPixel(const DiMonoPixel *pixel)
  : DiPixel(pixel->getCount()),
    PlanarConfiguration(0)
{
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
 * Revision 1.5  1999-04-28 13:20:13  joergr
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
