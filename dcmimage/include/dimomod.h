/*********************************************************************
** 
**  DicomMonochromeModality (Header)
**
**  author   : Joerg Riesmeier
**  created  : 29.12.96
**  modified : 15.02.97
**
*********************************************************************/


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
