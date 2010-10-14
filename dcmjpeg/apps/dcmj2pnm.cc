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
 *  Module:  dcmjpeg
 *
 *  Authors: Joerg Riesmeier
 *
 *  Purpose: Convert DICOM Images to PPM or PGM using the dcmimage/dcmjpeg library.
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:13:38 $
 *  CVS/RCS Revision: $Revision: 1.5 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


// compile "dcm2pnm" with dcmjpeg support
#define BUILD_DCM2PNM_AS_DCMJ2PNM

// include full implementation of "dcm2pnm"
#include "../../dcmimage/apps/dcm2pnm.cc"


/*
 * CVS/RCS Log:
 * $Log: dcmj2pnm.cc,v $
 * Revision 1.5  2010-10-14 13:13:38  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.4  2006-07-27 14:04:37  joergr
 * Updated copyright date.
 *
 * Revision 1.3  2005/12/08 15:43:22  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.2  2004/01/16 14:28:01  joergr
 * Updated copyright header.
 *
 * Revision 1.1  2001/11/19 13:08:37  joergr
 * Added new tool dcmj2pnm which supports all features of dcm2pnm and in
 * addition also JPEG compressed images.
 *
 *
 */
