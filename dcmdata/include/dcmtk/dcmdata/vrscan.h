/*
 *
 *  Copyright (C) 2010-2011, OFFIS e.V.
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
 */


#ifndef VRSCAN_H
#define VRSCAN_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/ofstring.h"

#include "dcmtk/dcmdata/dcdefine.h"

class DCMTK_DCMDATA_EXPORT vrscan
{
public:
    static int scan(const OFString& vr, const OFString& value);
private:
    static char* makeBuffer(const OFString& vr, const OFString& value, size_t& size);
};

#endif // VRSCAN_H
