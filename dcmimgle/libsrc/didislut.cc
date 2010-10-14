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
 *  Purpose: DicomDisplayLUT (Source)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:17 $
 *  CVS/RCS Revision: $Revision: 1.8 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmimgle/didislut.h"


/*----------------*
 *  constructors  *
 *----------------*/

DiDisplayLUT::DiDisplayLUT(const unsigned long count,
                           const Uint16 max,
                           const double amb,
                           const double illum)
  : DiBaseLUT(count, DicomImageClass::tobits(max, 0)),
    AmbientLight((amb > 0) ? amb : 0),
    Illumination((illum > 0) ? illum : 0)
{
}


/*--------------*
 *  destructor  *
 *--------------*/

DiDisplayLUT::~DiDisplayLUT()
{
}


/*
 *
 * CVS/RCS Log:
 * $Log: didislut.cc,v $
 * Revision 1.8  2010-10-14 13:14:17  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.7  2005/12/08 15:42:46  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.6  2003/12/08 17:43:04  joergr
 * Updated copyright header.
 *
 * Revision 1.5  2002/07/02 16:24:37  joergr
 * Added support for hardcopy devices to the calibrated output routines.
 *
 * Revision 1.4  2001/06/01 15:49:54  meichel
 * Updated copyright header
 *
 * Revision 1.3  2000/03/08 16:24:26  meichel
 * Updated copyright header.
 *
 * Revision 1.2  1999/10/21 17:47:13  joergr
 * Added underflow check to ambient light value.
 *
 * Revision 1.1  1999/09/10 08:54:48  joergr
 * Added support for CIELAB display function. Restructured class hierarchy
 * for display functions.
 *
 *
 *
 */
