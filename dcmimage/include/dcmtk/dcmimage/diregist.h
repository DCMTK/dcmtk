/*
 *
 *  Copyright (C) 1998-2010, OFFIS e.V.
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
 *  Purpose: DicomRegister (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:16:30 $
 *  CVS/RCS Revision: $Revision: 1.11 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DIREGIST_H
#define DIREGIST_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmimgle/diregbas.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class to register color support to dcmimgle library
 */
class DiRegister
  : public DiRegisterBase
{

 public:

    /** constructor
     */
    DiRegister();

    /** destructor
     */
    virtual ~DiRegister();

    /** create color image representation
     *
     ** @param  docu    pointer to dataset (encapsulated)
     *  @param  status  image status
     *  @param  photo   photometric interpretation
     *
     ** @return pointer to new DiImage object (NULL if an error occurred)
     */
    virtual DiImage *createImage(const DiDocument *docu,
                                 const EI_Status status,
                                 const EP_Interpretation photo);

    /** create monochrome pixel data from color image
     *
     ** @param  image  pointer to color image
     *  @param  red    coefficient by which the red component is weighted
     *  @param  green  coefficient by which the green component is weighted
     *  @param  blue   coefficient by which the blue component is weighted
     *
     ** @return pointer to new DiImage object (NULL if an error occurred)
     */
    virtual DiMonoPixel *createMonoImageData(const DiColorImage *image,
                                             const double red,
                                             const double green,
                                             const double blue);
};


/*------------------*
 *  initialization  *
 *------------------*/

static DiRegister Init_dcmimage_Module;        // registration pointer is initialized in class constructor


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: diregist.h,v $
 * Revision 1.11  2010-10-14 13:16:30  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.10  2005-12-08 16:01:57  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.9  2003/12/23 12:22:02  joergr
 * Added missing API documentation.
 *
 * Revision 1.8  2003/12/17 18:18:08  joergr
 * Removed leading underscore characters from preprocessor symbols (reserved
 * symbols).
 *
 * Revision 1.7  2001/06/20 15:11:39  joergr
 * Removed old dcmimage license information.
 *
 * Revision 1.6  2001/06/01 15:49:31  meichel
 * Updated copyright header
 *
 * Revision 1.5  2000/03/10 13:15:08  meichel
 * Release 1.1.1 (Release date - 2000-03-10)
 *
 * Revision 1.4  2000/03/08 16:21:53  meichel
 * Updated copyright header.
 *
 * Revision 1.3  1999/05/31 13:01:03  joergr
 * Changed Release version and date.
 *
 * Revision 1.2  1999/05/10 09:32:36  joergr
 * Moved dcm2pnm version definition from module dcmimgle to dcmimage.
 *
 * Revision 1.1  1998/11/27 13:09:54  joergr
 * Added registration class to allow easy combination of both modules.
 * Added copyright message to all source files.
 *
 *
 */

