/*********************************************************************
** 
**  DicomColorPixel (Source)
**
**  author   : Joerg Riesmeier
**  created  : 20.12.96
**  modified : 13.01.97
**
*********************************************************************/


#include "osconfig.h"
#include "dctypes.h"
#include "dcdeftag.h"

#include "dicopx.h"
#include "dimopx.h"
#include "diinpx.h"
#include "didocu.h"


/*----------------*
 *  constructors  *
 *----------------*/

DiColorPixel::DiColorPixel(const DiDocument *docu, const DiInputPixel *pixel, const Uint16 samples, EI_Status &status)
  : PlanarConfiguration(0),
	Count(0)
{
	if (docu != NULL)
	{
		Uint16 us = 0;
		if (docu->getValue(DCM_SamplesPerPixel, us))
		{
			if (us != samples)
			{
				cerr << "WARNING: invalid value for 'SamplesPerPixel' (" << us;
				cerr << ") ... assuming " << samples << " !" << endl;
			}
			if (docu->getValue(DCM_PlanarConfiguration, us))
			{
				PlanarConfiguration = (us == 1);
				if ((us != 0) && (us != 1))
				{
					cerr << "WARNING: invalid value for 'PlanarConfiguration' (" << us;
					cerr << ") ... assuming 'color-by-pixel' (0) !" << endl;
				}
			}
			else if (samples > 1)
			{
				status = EIS_MissingAttribute;
	    		cerr << "ERROR: mandatory attribute 'PlanarConfiguration' is missing !" << endl;
				return;
			}
			if (pixel != NULL)
				Count = pixel->getCount() / samples;
		}
		else
		{
			status = EIS_MissingAttribute;
	    	cerr << "ERROR: mandatory attribute 'SamplesPerPixel' is missing !" << endl;
		}
	}
}


DiColorPixel::DiColorPixel(const DiMonoPixel *pixel)
  : PlanarConfiguration(0),
	Count(pixel->getCount())
{
}


DiColorPixel::DiColorPixel(const DiColorPixel *pixel, const unsigned long count)
  : PlanarConfiguration(pixel->PlanarConfiguration),
	Count(count)
{
}


/*--------------*
 *  destructor  *
 *--------------*/

DiColorPixel::~DiColorPixel()
{
}

