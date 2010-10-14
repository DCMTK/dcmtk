/*
 *
 *  Copyright (C) 1996-2010, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module:  dcmimgle
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: Utilities (Source)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:18 $
 *  CVS/RCS Revision: $Revision: 1.22 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmdata/dctypes.h"
#include "dcmtk/ofstd/ofstream.h"

#include "dcmtk/dcmimgle/diutils.h"

#define INCLUDE_CMATH
#include "dcmtk/ofstd/ofstdinc.h"


/*------------------------*
 *  function definitions  *
 *------------------------*/

OFLogger DCM_dcmimgleGetLogger()
{
    static OFLogger DCM_dcmimgleLogger = OFLog::getLogger("dcmtk.dcmimgle");
    return DCM_dcmimgleLogger;
}

unsigned int DicomImageClass::rangeToBits(double minvalue,
                                          double maxvalue)
{
    /* assertion: min < max ! */
    if (minvalue > maxvalue)
    {
        const double temp = minvalue;
        minvalue = maxvalue;
        maxvalue = temp;
    }
    /* signed data? */
    if (minvalue < 0)
    {
        if (fabs(minvalue) > fabs(maxvalue))
            return tobits(OFstatic_cast(unsigned long, fabs(minvalue)), 1) + 1;
        else /* 'minvalue' is negative, 'maxvalue' is positive */
            return tobits(OFstatic_cast(unsigned long, fabs(maxvalue)), 0) + 1;
    }
    return tobits(OFstatic_cast(unsigned long, maxvalue), 0);
}


int DicomImageClass::isRepresentationSigned(EP_Representation repres)
{
    /* determine whether integer representation is signed or unsigned */
    return (repres == EPR_Sint8) || (repres == EPR_Sint16) || (repres == EPR_Sint32);
}


unsigned int DicomImageClass::getRepresentationBits(EP_Representation repres)
{
    unsigned int bits = 0;
    /* determine number of bits for specified representation */
    switch (repres)
    {
        case EPR_Uint8:
        case EPR_Sint8:
            bits = 8;
            break;
        case EPR_Uint16:
        case EPR_Sint16:
            bits = 16;
            break;
        case EPR_Uint32:
        case EPR_Sint32:
            bits = 32;
            break;
    }
    return bits;
}


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
            DCMIMGLE_WARN("minimum pixel value (" << minvalue << ") exceeds signed " << MAX_BITS
                << " bit " << "representation after modality transformation");
        }
        if (maxvalue > maxval(MAX_BITS - 1))
        {
            DCMIMGLE_WARN("maximum pixel value (" << maxvalue << ") exceeds signed " << MAX_BITS
                << " bit " << "representation after modality transformation");
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
        DCMIMGLE_WARN("maximum pixel value (" << maxvalue << ") exceeds unsigned " << MAX_BITS
            << " bit " << "representation after modality transformation");
    }
#endif
    return EPR_Uint32;
}


/*
 *
 * CVS/RCS Log:
 * $Log: diutils.cc,v $
 * Revision 1.22  2010-10-14 13:14:18  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.21  2010-07-21 08:56:23  joergr
 * Fixed possibly wrong calculation of rangeToBits() for negative values.
 *
 * Revision 1.20  2010-02-23 16:42:15  joergr
 * Added new helper function which determines whether an integer representation
 * is signed or unsigned.
 *
 * Revision 1.19  2009-11-25 16:09:50  joergr
 * Removed inclusion of header file "ofconsol.h".
 *
 * Revision 1.18  2009-10-28 14:26:02  joergr
 * Fixed minor issues in log output.
 *
 * Revision 1.17  2009-10-28 09:53:41  uli
 * Switched to logging mechanism provided by the "new" oflog module.
 *
 * Revision 1.16  2009-04-20 12:20:34  joergr
 * Added new helper function getRepresentationBits().
 *
 * Revision 1.15  2006/08/15 16:30:11  meichel
 * Updated the code in module dcmimgle to correctly compile when
 *   all standard C++ classes remain in namespace std.
 *
 * Revision 1.14  2005/12/08 15:43:07  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.13  2005/03/09 17:30:13  joergr
 * Added new helper function rangeToBits().
 *
 * Revision 1.12  2004/01/05 14:58:42  joergr
 * Removed acknowledgements with e-mail addresses from CVS log.
 *
 * Revision 1.11  2003/12/08 17:43:04  joergr
 * Updated copyright header.
 *
 * Revision 1.10  2002/04/16 13:53:32  joergr
 * Added configurable support for C++ ANSI standard includes (e.g. streams).
 *
 * Revision 1.9  2001/06/01 15:50:00  meichel
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
