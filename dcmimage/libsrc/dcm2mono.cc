/*********************************************************************
** 
**  DicomImage2Monochrome (Source)
**
**	command-line program to test converting to monochrome facilitiy
**
**  author   : Joerg Riesmeier
**  created  : 05.01.97
**  modified : 13.01.97
**
*********************************************************************/


#include "osconfig.h"

#include "dcmimage.h"


/*----------------*
 *  main program  *
 *----------------*/

int main(int argc, char *argv[])
{
    if (argc < 2)
		cerr << "usage: dcm2mono dcmfile-in [ppmfile-out]" << endl;
	else
	{
        char *input = argv[1];
		char *output = "temp/mono.ppm";
		
		if (argc > 2)
        	output = argv[2];
		
    	DicomImage *di = new DicomImage(input);
		if (di != NULL)
		{
			DicomImage *ds = di->createMonochromeImage(0.3, 0.5, 0.2);
			if ((ds != NULL) && (ds->getStatus() == EIS_Normal))
				ds->writeRawPPM(output, 8);
			delete ds;
		}
		delete di;
	}
	return 0;
}
