/*
**
** Author:  Joerg Riesmeier
** Created: 29.12.96
**
** Module:  dimomod.h
**
** Purpose: DicomMonochromeModality (Header)
**
** Last Update:      $Author: joergr $
** Update Date:      $Date: 1998-05-11 14:53:21 $
** Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/include/Attic/dimomod.h,v $
** CVS/RCS Revision: $Revision: 1.4 $
** Status:           $State: Exp $
**
** CVS/RCS Log at end of file
**
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
    DiMonoModality(const DiDocument *, DiInputPixel *);
    virtual ~DiMonoModality();
    
    inline EP_Representation getRepresentation() const
        { return Representation; }

    inline double getMinValue() const
        { return MinValue; }
    inline double getMaxValue() const
        { return MaxValue; }
    inline double getRescaleIntercept() const
        { return RescaleIntercept; }
    inline double getRescaleSlope() const
        { return RescaleSlope; }
    inline const DiLookupTable *getTableData() const
        { return TableData; }

    inline int hasLookupTable() const
        { return LookupTable; }
    inline int hasRescaling() const
        { return Rescaling; }

 private:
    EP_Representation Representation;
    
    double MinValue;
    double MaxValue;
    
    double RescaleIntercept;
    double RescaleSlope;
    
    int LookupTable;
    int Rescaling;
    
    DiLookupTable *TableData;
};


#endif


/*
**
** CVS/RCS Log:
** $Log: dimomod.h,v $
** Revision 1.4  1998-05-11 14:53:21  joergr
** Added CVS/RCS header to each file.
**
**
*/
