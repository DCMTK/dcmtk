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
 *  Purpose: DicomMonochromePixel (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1998-11-27 15:33:16 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/include/Attic/dimopx.h,v $
 *  CVS/RCS Revision: $Revision: 1.1 $ 
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __DIMOPX_H
#define __DIMOPX_H

#include "osconfig.h"
#include "dctypes.h"

#include "dipixel.h"
#include "diutils.h"


/*------------------------*
 *  forward declarations  *
 *------------------------*/

class DiInputPixel;
class DiMonoModality;


/*---------------------*
 *  class declaration  *
 *---------------------*/

class DiMonoPixel : public DiPixel
{
 public:
    DiMonoPixel(const unsigned long);
    DiMonoPixel(const DiInputPixel *, DiMonoModality *);
    virtual ~DiMonoPixel();
    
    inline int getPlanes() const
        { return 1; }

    virtual int getMinMaxValues(double &, double &) const = 0;

    virtual int getMinMaxWindow(const int, double &, double &) = 0;
    virtual int getHistogramWindow(const double, double &, double &) = 0;

 protected:
    DiMonoPixel(const DiPixel *);
    DiMonoPixel(const DiMonoPixel *, const unsigned long);
    
    DiMonoModality *Modality;

 private:

 // --- declarations to avoid compiler warnings
 
    DiMonoPixel(const DiMonoPixel &);
    DiMonoPixel &operator=(const DiMonoPixel &);
};


#endif


/*
**
** CVS/RCS Log:
** $Log: dimopx.h,v $
** Revision 1.1  1998-11-27 15:33:16  joergr
** Added copyright message.
** Introduced new pixel base class.
**
** Revision 1.4  1998/07/01 08:39:24  joergr
** Minor changes to avoid compiler warnings (gcc 2.8.1 with additional
** options), e.g. add copy constructors.
**
** Revision 1.3  1998/05/11 14:53:23  joergr
** Added CVS/RCS header to each file.
**
**
*/

