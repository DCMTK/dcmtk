/*
 *
 *  Copyright (C) 1996-99, OFFIS
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
 *  Purpose: DicomColorOutputPixel (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1999-04-29 09:31:12 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/include/Attic/dicoopx.h,v $
 *  CVS/RCS Revision: $Revision: 1.8 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __DICOOPX_H
#define __DICOOPX_H

#include "osconfig.h"

#include "diutils.h"

BEGIN_EXTERN_C
 #include <stdlib.h>
 #include <stdio.h>
END_EXTERN_C

#include <iostream.h>
#include <fstream.h>


/*------------------------*
 *  forward declarations  *
 *------------------------*/

class DiPixel;


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Abstract base class to create color output data
 */
class DiColorOutputPixel 
{

 public:

    DiColorOutputPixel(const DiPixel *pixel,
                       const unsigned long frames);

    virtual ~DiColorOutputPixel();
    
    virtual EP_Representation getRepresentation() const = 0;

    virtual void *getData() const = 0;

    virtual void *getPlane(const int) const = 0;

    virtual size_t getItemSize() const = 0;

    virtual int writePPM(ostream &) const = 0;

    virtual int writePPM(FILE *) const = 0;
    
    inline unsigned long getCount() const
    {
        return Count;
    }


 protected:

    const unsigned long Count;
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: dicoopx.h,v $
 * Revision 1.8  1999-04-29 09:31:12  joergr
 * Moved color related image files back to non-public part.
 *
 * Revision 1.1  1999/04/28 14:57:31  joergr
 * Moved files from dcmimage module to dcmimgle to support new pastel color
 * output format.
 *
 * Revision 1.6  1999/02/03 16:53:42  joergr
 * Added BEGIN_EXTERN_C and END_EXTERN_C to some C includes.
 *
 * Revision 1.5  1999/01/20 14:42:06  joergr
 * Replaced invocation of getCount() by member variable Count where possible.
 *
 * Revision 1.4  1998/11/27 13:44:29  joergr
 * Added copyright message.
 *
 * Revision 1.3  1998/05/11 14:53:12  joergr
 * Added CVS/RCS header to each file.
 *
 *
 */
