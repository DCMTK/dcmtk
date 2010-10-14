/*
 *
 *  Copyright (C) 2010, OFFIS e.V.
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
 *  Module:  dcmdata
 *
 *  Author:  Uli Schlachter
 *
 *  Purpose: Interface to the VR scanner.
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:15:43 $
 *  CVS/RCS Revision: $Revision: 1.5 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef VRSCAN_H
#define VRSCAN_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/ofstring.h"

class vrscan
{
public:
    static int scan(const OFString& vr, const OFString& value);
private:
    static char* makeBuffer(const OFString& vr, const OFString& value, size_t& size);
};

#endif // VRSCAN_H


/*
** CVS/RCS Log:
** $Log: vrscan.h,v $
** Revision 1.5  2010-10-14 13:15:43  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.4  2010-09-02 10:16:02  uli
** The VR scanner now only copies the input data once, not twice.
**
** Revision 1.3  2010-09-02 09:49:38  uli
** Add the VR prefix into the scanner instead of adding it in the caller.
**
** Revision 1.2  2010-08-26 12:29:47  uli
** Ported vrscan from ancient flex++ to current flex version.
**
**
*/
