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
 *  Update Date:      $Date: 1999-01-11 09:35:56 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/include/Attic/dimopx.h,v $
 *  CVS/RCS Revision: $Revision: 1.4 $ 
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
#include "dimomod.h"
#include "diutils.h"


/*------------------------*
 *  forward declarations  *
 *------------------------*/

class DiInputPixel;


/*---------------------*
 *  class declaration  *
 *---------------------*/

class DiMonoPixel
  : public DiPixel
{

 public:

    DiMonoPixel(const unsigned long count);

    DiMonoPixel(const DiInputPixel *pixel,
                DiMonoModality *modality);

    virtual ~DiMonoPixel();
    
    inline int getPlanes() const
    {
        return 1;
    }

    virtual int getMinMaxValues(double &min,
                                double &max) const = 0;

    virtual int getMinMaxWindow(const int idx,
                                double &center,
                                double &width) = 0;

    virtual int getHistogramWindow(const double thresh,
                                   double &center,
                                   double &width) = 0;
                                   
    inline double getAbsMinimum() const
    {
        return (Modality != NULL) ? Modality->getAbsMinimum() : 0;
    }

    inline double getAbsMaximum() const
    {
        return (Modality != NULL) ? Modality->getAbsMaximum() : 0;
    }

    inline double getAbsMaxRange() const
    {
        return getAbsMaximum() - getAbsMinimum() + 1;
    }
    
    inline const char *getModalityLutExplanation() const
    {
        return (Modality != NULL) ? Modality->getExplanation() : NULL;
    }

    inline int isPotentiallySigned() const
    {
        return (getAbsMinimum() < 0);
    }


 protected:

    DiMonoPixel(const DiPixel *pixel);

    DiMonoPixel(const DiMonoPixel *pixel,
                const unsigned long count);
    
    DiMonoModality *Modality;


 private:

 // --- declarations to avoid compiler warnings
 
    DiMonoPixel(const DiMonoPixel &);
    DiMonoPixel &operator=(const DiMonoPixel &);
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: dimopx.h,v $
 * Revision 1.4  1999-01-11 09:35:56  joergr
 * Corrected some typos and formatting.
 *
 * Revision 1.3  1998/12/22 14:33:45  joergr
 * Added implementation of methods to return member variables AbsMinimum/
 * Maximum.
 *
 * Revision 1.2  1998/12/16 16:35:39  joergr
 * Added explanation string to LUT class (retrieved from dataset).
 *
 * Revision 1.1  1998/11/27 15:33:16  joergr
 * Added copyright message.
 * Introduced new pixel base class.
 *
 * Revision 1.4  1998/07/01 08:39:24  joergr
 * Minor changes to avoid compiler warnings (gcc 2.8.1 with additional
 * options), e.g. add copy constructors.
 *
 * Revision 1.3  1998/05/11 14:53:23  joergr
 * Added CVS/RCS header to each file.
 *
 *
 */
