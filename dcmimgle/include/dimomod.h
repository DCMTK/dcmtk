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
 *  Update Date:      $Date: 1999-03-24 17:20:12 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/include/Attic/dimomod.h,v $
 *  CVS/RCS Revision: $Revision: 1.6 $
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

/** Class to handle modality specific transformations (incl. modality LUT)
 */
class DiMonoModality
  : public DiObjectCounter
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
                   const DcmUnsignedShort &descriptor,
                   const DcmLongString *explanation);

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
    
    inline unsigned int getBits() const
    {
        return Bits;
    }
    
    inline double getAbsMinimum() const
    {
        return AbsMinimum;
    }
    
    inline double getAbsMaximum() const
    {
        return AbsMaximum;
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
    
    inline const char *getExplanation() const
    {
        return (TableData != NULL) ? TableData->getExplanation() : NULL;
    }

    inline int hasLookupTable() const
    {
        return LookupTable;
    }
    
    inline int hasRescaling() const
    {
        return Rescaling;
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

    unsigned int Bits;
    double AbsMinimum;
    double AbsMaximum;
    
    double RescaleIntercept;
    double RescaleSlope;
    
    int LookupTable;
    int Rescaling;
    
    DiLookupTable *TableData;

 // --- declarations to avoid compiler warnings
 
    DiMonoModality(const DiMonoModality &);
    DiMonoModality &operator=(const DiMonoModality &);
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: dimomod.h,v $
 * Revision 1.6  1999-03-24 17:20:12  joergr
 * Added/Modified comments and formatting.
 *
 * Revision 1.5  1999/02/03 17:30:00  joergr
 * Added member variable and related methods to store number of bits used for
 * pixel data.
 *
 * Revision 1.4  1998/12/22 14:31:01  joergr
 * Changed calculation of AbsMinimum/Maximum.
 * Removed member variable and method for isPotentiallySigned.
 *
 * Revision 1.3  1998/12/16 16:34:37  joergr
 * Added explanation string to LUT class (retrieved from dataset).
 *
 * Revision 1.2  1998/12/14 17:23:52  joergr
 * Added support for correct scaling of input/output values for grayscale
 * transformations.
 *
 * Revision 1.1  1998/11/27 15:25:15  joergr
 * Added copyright message.
 * Added constructors to use external modality transformations.
 *
 * Revision 1.5  1998/07/01 08:39:23  joergr
 * Minor changes to avoid compiler warnings (gcc 2.8.1 with additional
 * options), e.g. add copy constructors.
 *
 * Revision 1.4  1998/05/11 14:53:21  joergr
 * Added CVS/RCS header to each file.
 *
 * 
 */
