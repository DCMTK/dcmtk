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
 *  Purpose: DicomMonoOutputPixel (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1999-01-20 15:07:02 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/include/Attic/dimoopx.h,v $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __DIMOOPX_H
#define __DIMOOPX_H

#include "osconfig.h"

#include "diutils.h"

#include <stdlib.h>
#include <stdio.h>
#include <iostream.h>
#include <fstream.h>


/*------------------------*
 *  forward declarations  *
 *------------------------*/

class DiMonoPixel;


/*---------------------*
 *  class declaration  *
 *---------------------*/

class DiMonoOutputPixel 
{

 public:

    DiMonoOutputPixel(const DiMonoPixel *pixel,
                      const unsigned long frames);

    virtual ~DiMonoOutputPixel();
    
    virtual EP_Representation getRepresentation() const = 0;

    virtual void *getData() const = 0;

    virtual size_t getItemSize() const = 0;

    virtual int writePPM(ostream &stream) const = 0;

    virtual int writePPM(FILE *stream) const = 0;

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
 * $Log: dimoopx.h,v $
 * Revision 1.2  1999-01-20 15:07:02  joergr
 * Replaced invocation of getCount() by member variable Count where possible.
 *
 * Revision 1.1  1998/11/27 15:25:48  joergr
 * Added copyright message.
 *
 * Revision 1.3  1998/05/11 14:53:22  joergr
 * Added CVS/RCS header to each file.
 *
 *
 */
