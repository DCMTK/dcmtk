/*
 *
 *  Copyright (C) 1999, OFFIS
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
 *  Purpose: DicomDisplayLUT (Source)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1999-09-10 08:54:48 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/libsrc/didislut.cc,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"

#include "didislut.h"


/*----------------*
 *  constructors  *
 *----------------*/

DiDisplayLUT::DiDisplayLUT(const unsigned long count,
                           const Uint16 max,
                           const double amb)
  : DiBaseLUT(count, DicomImageClass::tobits(max, 0)),
    AmbientLight(amb)
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
 * Revision 1.1  1999-09-10 08:54:48  joergr
 * Added support for CIELAB display function. Restructured class hierarchy
 * for display functions.
 *
 *
 *
 */
