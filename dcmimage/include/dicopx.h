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
 *  Module:  dcmimage
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: DicomColorPixel (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1999-01-20 14:44:04 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/include/Attic/dicopx.h,v $
 *  CVS/RCS Revision: $Revision: 1.5 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __DICOPX_H
#define __DICOPX_H

#include "osconfig.h"

#include "dipixel.h"
#include "diutils.h"


/*------------------------*
 *  forward declarations  *
 *------------------------*/

class DiDocument;
class DiInputPixel;
class DiMonoPixel;
class DiColorPixel;


/*---------------------*
 *  class declaration  *
 *---------------------*/


class DiColorPixel
  : public DiPixel
{

 public:

    DiColorPixel(const DiDocument *,
                 const DiInputPixel *,
                 const Uint16,
                 EI_Status &,
                 const Uint16 = 0);

    virtual ~DiColorPixel();
    
    int getPlanes() const
    {
        return 3;
    }

    virtual void *createDIB(const Uint16,
                            const Uint16,
                            const unsigned long,
                            Sint16) const = 0;

    virtual void *createAWTBitmap(const Uint16,
                                  const Uint16,
                                  const unsigned long,
                                  Sint16) const = 0;
    

 protected:

    DiColorPixel(const DiMonoPixel *);

    DiColorPixel(const DiColorPixel *,
                 const unsigned long);

    int PlanarConfiguration;
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: dicopx.h,v $
 * Revision 1.5  1999-01-20 14:44:04  joergr
 * Corrected some typos and formatting.
 *
 * Revision 1.4  1998/11/27 13:47:54  joergr
 * Added copyright message. Added method to directly create java AWT bitmaps.
 *
 * Revision 1.3  1998/05/11 14:53:13  joergr
 * Added CVS/RCS header to each file.
 *
 *
 */
