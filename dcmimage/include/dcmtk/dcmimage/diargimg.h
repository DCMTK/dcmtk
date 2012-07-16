/*
 *
 *  Copyright (C) 1996-2011, OFFIS e.V.
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
 *  Purpose: DiARGBImage (Header)
 *
 */


#ifndef DIARGIMG_H
#define DIARGIMG_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmimage/dicoimg.h"


/*------------------------*
 *  forward declarations  *
 *------------------------*/

class DiLookupTable;


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class for ARGB images
 */
class DCMTK_DCMIMAGE_EXPORT DiARGBImage
  : public DiColorImage
{

 public:

    /** constructor
     *
     ** @param  docu    pointer to dataset (encapsulated)
     *  @param  status  current image status
     */
    DiARGBImage(const DiDocument *docu,
                const EI_Status status);

    /** destructor
     */
    virtual ~DiARGBImage();

    /** process next couple of frames
     *
     ** @param  fcount  number of frames to be processed (0 = same number as before)
     *
     ** @return status, true if successful, false otherwise
     */
    virtual int processNextFrames(const unsigned long fcount);


 protected:

    /** initialize internal data structures and member variables
     */
    void Init();


 private:

    /// three color palettes for R, G and B
    DiLookupTable *Palette[3];

 // --- declarations to avoid compiler warnings

    DiARGBImage(const DiARGBImage &);
    DiARGBImage &operator=(const DiARGBImage &);
};


#endif
