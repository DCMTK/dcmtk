/*
 *
 *  Copyright (C) 2001-2010, OFFIS
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
 *  Module:  dcmjpls
 *
 *  Author:  Uli Schlachter
 *
 *  Purpose: Convert DICOM Images to PPM or PGM using the dcmimage/dcmjpls library.
 *
 *  Last Update:      $Author: uli $
 *  Update Date:      $Date: 2010-10-12 13:12:58 $
 *  CVS/RCS Revision: $Revision: 1.1 $
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
 * Revision 1.1  2010-10-12 13:12:58  uli
 * Added dcml2pnm which is a dcmjpls-enabled dcm2pnm.
 *
 *
 *
 */
