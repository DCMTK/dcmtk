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
 *  Module:  dcmimgle
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: Utilities (Source)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1999-02-03 17:44:33 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/libsrc/diutils.cc,v $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"
#include "dctypes.h"

#include "diutils.h"

#include <iostream.h>


/*----------------------------*
 *  constant initializations  *
 *----------------------------*/

const int DicomImageClass::DL_NoMessages     = 0;
const int DicomImageClass::DL_Errors         = 1;
const int DicomImageClass::DL_Warnings       = 2;
const int DicomImageClass::DL_Informationals = 3;
const int DicomImageClass::DL_DebugMessages  = 4;

#ifdef DEBUG
 int DicomImageClass::DebugLevel = DicomImageClass::DL_DebugMessages;
#else
 int DicomImageClass::DebugLevel = DicomImageClass::DL_NoMessages;
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
        if (-minvalue > maxval(MAX_BITS - 1, 0))
        {
            if (DebugLevel >= DL_Warnings)
            {
                cerr << "WARNING: minimum pixel value (" << minvalue << ") exceeds signed " << MAX_BITS << " bit ";
                cerr << "representation after modality transformation !" << endl;
            }
        }
        if (maxvalue > maxval(MAX_BITS - 1))
        {
            if (DebugLevel >= DL_Warnings)
            {
                cerr << "WARNING: maximum pixel value (" << maxvalue << ") exceeds signed " << MAX_BITS << " bit ";
                cerr << "representation after modality transformation !" << endl;
            }
        }
        return EPR_Sint32;
    }
    if (maxvalue <= maxval(8))
        return EPR_Uint8;
    if (maxvalue <= maxval(16))
        return EPR_Uint16;
    if (maxvalue > maxval(MAX_BITS))
    {
        if (DebugLevel >= DL_Warnings)
        {
            cerr << "WARNING: maximum pixel value (" << maxvalue << ") exceeds unsigned " << MAX_BITS << " bit ";
            cerr << "representation after modality transformation !" << endl;
        }
    }
    return EPR_Uint32;
}


/*
 *
 * CVS/RCS Log:
 * $Log: diutils.cc,v $
 * Revision 1.2  1999-02-03 17:44:33  joergr
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
