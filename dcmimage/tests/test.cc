/*********************************************************************
** 
**  TestBench (Source)
**
**  author   : Joerg Riesmeier
**  created  : 09.12.96
**  modified : 15.02.97
**
*********************************************************************/


#include "osconfig.h"

#include "dcmimage.h"


/*----------------------------*
 *  constant initializations  *
 *----------------------------*/

#ifdef OFFIS
  const char *input  = "/share/dicom/images/philips/mr-abdo.dcm";
#else
  const char *input = "/Users/joerg/Diplom/DICOM/Data/dicom1.dcm.signed";
#endif


/*----------------*
 *  main program  *
 *----------------*/

int main(int argc, char *argv[])
{
    if (argc > 1)
        input = argv[1];
		
    DicomImage di = input;
COUT << di.getWidth() << " " << di.getHeight() << endl;
	
	if (di.getFrameCount() > 1)
	{
		unsigned long i = 0;
		int ok = 1;
		COUT << "Frame" << flush;
		while ((i < di.getFrameCount()) && (ok = di.writeRawPPM("temp/temp.%d.ppm", 8, i)))
		{
			COUT << " " << i << flush;
			i++;
		}
		COUT << " -> (" << ok << ", " << (int)di.getStatus() << ")" << endl;
	}
	else if (di.getOverlayCount() > 0)
	{
		di.showOverlay(4, EMO_Complement, 0.5, 0.5); 
		di.writeRawPPM("temp/overlay.ppm", 8);
	}
	else
	{
		di.setHistogramWindow();
//		di.setMinMaxWindow(1);
		di.writeRawPPM("temp/convert.ppm", 8);
		DicomImage *ds = di.createScaledImage((unsigned long)320, 0);
		if (ds != NULL)
			ds->writeRawPPM("temp/scaled.ppm", 8);
		delete ds;
	}
	
    return 0;
}
