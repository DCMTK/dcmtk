/*********************************************************************
** 
**  DicomMonochromeModality (Source)
**
**  author   : Joerg Riesmeier
**  created  : 29.12.96
**  modified : 15.02.97
**
*********************************************************************/


#include "osconfig.h"
#include "dcdeftag.h"

#include "dimomod.h"
#include "didocu.h"
#include "diinpx.h"


/*----------------*
 *  constructors  *
 *----------------*/

DiMonoModality::DiMonoModality(const DiDocument *docu, DiInputPixel *pixel)
  : Representation(EPR_MaxSigned),
	MinValue(0),
	MaxValue(0),
	RescaleIntercept(0),
	RescaleSlope(0),
	LookupTable(0),
	Rescaling(0),
	TableData(NULL)
{
	if ((docu != NULL) && (pixel != NULL))
	{
		pixel->determineMinMax();
		MinValue = pixel->getMinValue(); 
		MaxValue = pixel->getMaxValue();
		Uint16 us;
		if (docu->getValue(DCM_SamplesPerPixel, us) && (us != 1))
			cerr << "WARNING: invalid value for 'SamplesPerPixel' (" << us << ") ... assuming 1 !" << endl;
		TableData = new DiLookupTable(docu, DCM_ModalityLUTSequence, DCM_LUTDescriptor, DCM_LUTData);
		if (TableData != NULL)
		{
			LookupTable = TableData->isValid();
			if (LookupTable)
			{
				MinValue = TableData->getMinValue();
				MaxValue = TableData->getMaxValue();
			}
		}
		Rescaling = (docu->getValue(DCM_RescaleIntercept, RescaleIntercept) > 0);
		Rescaling &= (docu->getValue(DCM_RescaleSlope, RescaleSlope) > 0);
		if (!LookupTable && Rescaling)
		{
			if (RescaleSlope == 0)
			{
				cerr << "WARNING: invalid value for 'RescaleSlope' (" << RescaleSlope;
				cerr << ") ... ignoring modality transformation !" << endl;
				Rescaling = 0;
			}
			else
			{
				MinValue = MinValue * RescaleSlope + RescaleIntercept;
				MaxValue = MaxValue * RescaleSlope + RescaleIntercept;
			}
		}
		Representation = determineRepresentation(MinValue, MaxValue);
	}
}


/*--------------*
 *  destructor  *
 *--------------*/

DiMonoModality::~DiMonoModality()
{
	delete TableData;
}
