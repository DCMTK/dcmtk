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
 *  Module:  dcmimgle
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: Utilities (Source)
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2001-06-01 15:50:00 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/libsrc/diutils.cc,v $
 *  CVS/RCS Revision: $Revision: 1.9 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"
#include "dctypes.h"
#include "ofconsol.h"

#include "diutils.h"

#include <iostream.h>


/*-------------------*
 *  initializations  *
 *-------------------*/

const int DicomImageClass::DL_NoMessages     = 0x0;
const int DicomImageClass::DL_Errors         = 0x1;
const int DicomImageClass::DL_Warnings       = 0x2;
const int DicomImageClass::DL_Informationals = 0x4;
const int DicomImageClass::DL_DebugMessages  = 0x8;

#ifdef DEBUG
 OFGlobal<int> DicomImageClass::DebugLevel(DicomImageClass::DL_DebugMessages);
#else
 OFGlobal<int> DicomImageClass::DebugLevel(DicomImageClass::DL_NoMessages);
#endif


/*------------------------*
 *  function definitions  *
 *------------------------*/

EP_Representation DicomImageClass::determineRepresentation(double minvalue,
                                                           double maxvalue)
{
    if (minvalue > maxvalue)                        /* assertion: min < max ! */
    {
        const double temp = minvalue;
        minvalue = maxvalue;
        maxvalue = temp;
    }
    if (minvalue < 0)                               /* signed */
    {
        if ((-minvalue <= maxval(7, 0)) && (maxvalue <= maxval(7)))
            return EPR_Sint8;
        if ((-minvalue <= maxval(15, 0)) && (maxvalue <= maxval(15)))
            return EPR_Sint16;
#ifdef DEBUG
        if (-minvalue > maxval(MAX_BITS - 1, 0))
        {
            if (checkDebugLevel(DL_Warnings))
            {
                ofConsole.lockCerr() << "WARNING: minimum pixel value (" << minvalue << ") exceeds signed " << MAX_BITS
                                     << " bit " << "representation after modality transformation !" << endl;
                ofConsole.unlockCerr();
            }
        }
        if (maxvalue > maxval(MAX_BITS - 1))
        {
            if (checkDebugLevel(DL_Warnings))
            {
                ofConsole.lockCerr() << "WARNING: maximum pixel value (" << maxvalue << ") exceeds signed " << MAX_BITS
                                     << " bit " << "representation after modality transformation !" << endl;
                ofConsole.unlockCerr();
            }
        }
#endif
        return EPR_Sint32;
    }
    if (maxvalue <= maxval(8))
        return EPR_Uint8;
    if (maxvalue <= maxval(16))
        return EPR_Uint16;
#ifdef DEBUG
    if (maxvalue > maxval(MAX_BITS))
    {
        if (checkDebugLevel(DL_Warnings))
        {
            ofConsole.lockCerr() << "WARNING: maximum pixel value (" << maxvalue << ") exceeds unsigned " << MAX_BITS
                                 << " bit " << "representation after modality transformation !" << endl;
            ofConsole.unlockCerr();
        }
    }
#endif
    return EPR_Uint32;
}


/*
 *
 * CVS/RCS Log:
 * $Log: diutils.cc,v $
 * Revision 1.9  2001-06-01 15:50:00  meichel
 * Updated copyright header
 *
 * Revision 1.8  2000/05/03 09:47:24  joergr
 * Removed most informational and some warning messages from release built
 * (#ifndef DEBUG).
 *
 * Revision 1.7  2000/04/28 12:33:48  joergr
 * DebugLevel - global for the module - now derived from OFGlobal (MF-safe).
 *
 * Revision 1.6  2000/04/27 13:10:33  joergr
 * Dcmimgle library code now consistently uses ofConsole for error output.
 *
 * Revision 1.5  2000/03/08 16:24:33  meichel
 * Updated copyright header.
 *
 * Revision 1.4  2000/03/03 14:09:22  meichel
 * Implemented library support for redirecting error messages into memory
 *   instead of printing them to stdout/stderr for GUI applications.
 *
 * Revision 1.3  1999/04/28 15:04:50  joergr
 * Introduced new scheme for the debug level variable: now each level can be
 * set separately (there is no "include" relationship).
 *
 * Revision 1.2  1999/02/03 17:44:33  joergr
 * Moved global functions maxval() and determineRepresentation() to class
 * DicomImageClass (as static methods).
 *
 * Revision 1.1  1998/11/27 16:23:32  joergr
 * Added copyright message.
 * Introduced global debug level for dcmimage module to control error output.
 * Introduced configuration flags to adjust behaviour in different cases.
 *
 * Revision 1.4  1998/05/11 14:52:37  joergr
 * Added CVS/RCS header to each file.
 *
 *
 */
