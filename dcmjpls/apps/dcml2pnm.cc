/*
 *
 *  Copyright (C) 2001-2010, OFFIS e.V.
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
 *  Module:  dcmjpls
 *
 *  Author:  Uli Schlachter
 *
 *  Purpose: Convert DICOM Images to PPM or PGM using the dcmimage/dcmjpls library.
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:13:40 $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


// compile "dcm2pnm" with dcmjpls support
#define BUILD_DCM2PNM_AS_DCML2PNM

// include full implementation of "dcm2pnm"
#include "../../dcmimage/apps/dcm2pnm.cc"


/*
 * CVS/RCS Log:
 * $Log: dcml2pnm.cc,v $
 * Revision 1.2  2010-10-14 13:13:40  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.1  2010-10-12 13:12:58  uli
 * Added dcml2pnm which is a dcmjpls-enabled dcm2pnm.
 *
 *
 *
 */
