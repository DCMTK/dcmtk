/*
 *
 *  Copyright (C) 1996-2000, OFFIS
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
 *  Purpose: DicomOverlayData (Header)
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2000-03-08 16:24:22 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/include/Attic/diovdat.h,v $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __DIOVDAT_H
#define __DIOVDAT_H

#include "osconfig.h"
#include "dctypes.h"

#include "diobjcou.h"


/*------------------------*
 *  forward declarations  *
 *------------------------*/

class DiOverlay;
class DiOverlayPlane;


/*-------------------------------*
 *  declaration of helper class  *
 *-------------------------------*/

/** Class to handle overlay pixel data
 */
class DiOverlayData
  : public DiObjectCounter
{

    friend class DiOverlay;

 public:

    /** constructor
     *
     ** @param  entries  number of array entries
     *  @param  count    number of valid overlay planes
     */
    DiOverlayData(unsigned int entries,
                  unsigned int count = 0);

    /** destructor
     */
    virtual ~DiOverlayData();


 private:

    /// number of (valid) overlay planes
    unsigned int Count;
    /// number of array entries (allocated memory)
    unsigned int ArrayEntries;

    /// pointer to an array of planes
    DiOverlayPlane **Planes;
    /// pointer to overlay data (if scaled, flipped or rotated)
    Uint16 *DataBuffer;

 // --- declarations to avoid compiler warnings

    DiOverlayData(const DiOverlayData &);
    DiOverlayData &operator=(const DiOverlayData &);
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: diovdat.h,v $
 * Revision 1.2  2000-03-08 16:24:22  meichel
 * Updated copyright header.
 *
 * Revision 1.1  1999/09/17 12:46:22  joergr
 * Splitted file diovlay.h into two files (one for each class).
 *
 *
 *
 */
