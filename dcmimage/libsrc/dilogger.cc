/*
 *
 *  Copyright (C) 2009-2011, OFFIS e.V.
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
 *  Module:  dcmimage
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: Logger (Source)
 *
 */


#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmimage/dilogger.h"


/*--------------------*
 *  global variables  *
 *--------------------*/

OFLogger DCM_dcmimageLogger = OFLog::getLogger("dcmtk.dcmimage");
