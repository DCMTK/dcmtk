/*
 *
 *  Copyright (C) 1993-2005, OFFIS
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
 *  Module:  dcmqrdb
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose: class DcmQueryRetrieveOptions
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2005-03-30 13:34:53 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmqrdb/libsrc/dcmqropt.cc,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmqropt.h"

#define INCLUDE_CSTDARG
#include "ofstdinc.h"

const OFConditionConst APPE_INVALIDPEER(1024, 1, OF_error, "dcmqrdb: invalid peer for move operation");
const OFCondition APP_INVALIDPEER(APPE_INVALIDPEER);

DcmQueryRetrieveOptions::DcmQueryRetrieveOptions()
: allowShutdown_(OFFalse)
, bitPreserving_(OFFalse)
, correctUIDPadding_(OFFalse)
, debug_(OFFalse)
, disableGetSupport_(OFFalse)
, filepad_(0)
, groupLength_(EGL_recalcGL)
, ignoreStoreData_(OFFalse)
, itempad_(0) 
, maxAssociations_(20)
, maxPDU_(ASC_DEFAULTMAXPDU)
, net_(NULL)
, networkTransferSyntax_(EXS_Unknown)
, paddingType_(EPD_withoutPadding)
, refuseMultipleStorageAssociations_(OFFalse)
, refuse_(OFFalse)
, rejectWhenNoImplementationClassUID_(OFFalse)
, requireFindForMove_(OFFalse)
, restrictMoveToSameAE_(OFFalse)
, restrictMoveToSameHost_(OFFalse)
, restrictMoveToSameVendor_(OFFalse)
, sequenceType_(EET_ExplicitLength)
#ifdef HAVE_FORK
, singleProcess_(OFFalse)
#else
, singleProcess_(OFTrue)
#endif
, supportPatientRoot_(OFTrue)
#ifdef NO_PATIENTSTUDYONLY_SUPPORT
, supportPatientStudyOnly_(OFFalse)
#else
, supportPatientStudyOnly_(OFTrue)
#endif
, supportStudyRoot_(OFTrue)
, useMetaheader_(OFTrue)
, verbose_(0)
, writeTransferSyntax_(EXS_Unknown)
{
}

DcmQueryRetrieveOptions::~DcmQueryRetrieveOptions()
{
}

void DcmQueryRetrieveOptions::errmsg(const char* msg, ...)
{
    va_list args;

    fprintf(stderr, "DcmQueryRetrieve: ");
    va_start(args, msg);
    vfprintf(stderr, msg, args);
    va_end(args);
    fprintf(stderr, "\n");
}

/*
 * CVS Log
 * $Log: dcmqropt.cc,v $
 * Revision 1.1  2005-03-30 13:34:53  meichel
 * Initial release of module dcmqrdb that will replace module imagectn.
 *   It provides a clear interface between the Q/R DICOM front-end and the
 *   database back-end. The imagectn code has been re-factored into a minimal
 *   class structure.
 *
 *
 */
