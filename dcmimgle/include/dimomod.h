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
 *  Purpose: DicomMonochromeModality (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1998-12-14 17:23:52 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/include/Attic/dimomod.h,v $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __DIMOMOD_H
#define __DIMOMOD_H

#include "osconfig.h"
#include "dctypes.h"

#include "diluptab.h"
#include "diobjcou.h"
#include "diutils.h"


/*------------------------*
 *  forward declarations  *
 *------------------------*/

class DiDocument;
class DiInputPixel;


/*---------------------*
 *  class declaration  *
 *---------------------*/

class DiMonoModality : public DiObjectCounter
{

 public:

    DiMonoModality(const DiDocument *docu,
                   DiInputPixel *pixel);

    DiMonoModality(const DiDocument *docu,
                   DiInputPixel *pixel,
                   const double slope,
                   const double intercept);

    DiMonoModality(const DiDocument *docu,
                   DiInputPixel *pixel,
                   const DcmUnsignedShort &data,
                   const DcmUnsignedShort &descriptor);

    virtual ~DiMonoModality();
    
    inline EP_Representation getRepresentation() const
    {
        return Representation;
    }

    inline double getMinValue() const
    {
        return MinValue;
    }
    
    inline double getMaxValue() const
    {
        return MaxValue;
    }
    
    inline double getRescaleIntercept() const
    {
        return RescaleIntercept;
    }
    
    inline double getRescaleSlope() const
    {
        return RescaleSlope;
    }
    
    inline const DiLookupTable *getTableData() const
    {
        return TableData;
    }

    inline int hasLookupTable() const
    {
        return LookupTable;
    }
    
    inline int hasRescaling() const
    {
        return Rescaling;
    }

    inline int isPotentiallySigned() const
    {
        return PotentialSignedRange;
    }


 protected:

    int Init(const DiDocument *docu,
             DiInputPixel *pixel);
 
    void checkTable();

    void checkRescaling(const DiInputPixel *pixel);

 
 private:

    EP_Representation Representation;
    
    double MinValue;
    double MaxValue;
    
    double RescaleIntercept;
    double RescaleSlope;
    
    int LookupTable;
    int Rescaling;
    int PotentialSignedRange;
    
    DiLookupTable *TableData;

 // --- declarations to avoid compiler warnings
 
    DiMonoModality(const DiMonoModality &);
    DiMonoModality &operator=(const DiMonoModality &);
};


#endif


/*
**
** CVS/RCS Log:
** $Log: dimomod.h,v $
** Revision 1.2  1998-12-14 17:23:52  joergr
** Added support for correct scaling of input/output values for grayscale
** transformations.
**
** Revision 1.1  1998/11/27 15:25:15  joergr
** Added copyright message.
** Added constructors to use external modality transformations.
**
** Revision 1.5  1998/07/01 08:39:23  joergr
** Minor changes to avoid compiler warnings (gcc 2.8.1 with additional
** options), e.g. add copy constructors.
**
** Revision 1.4  1998/05/11 14:53:21  joergr
** Added CVS/RCS header to each file.
**
**
*/

