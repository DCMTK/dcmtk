/*********************************************************************
** 
**  Size DicomImage (Source)
**
**	command-line program to test scaling (size) facilitiy
**
**  author   : Joerg Riesmeier
**  created  : 01.01.97
**  modified : 13.01.97
**
*********************************************************************/


#include "osconfig.h"

#include "dcmimage.h"

#include <stdlib.h>


/*----------------*
 *  main program  *
 *----------------*/

int main(int argc, char *argv[])
{
    if (argc < 3)
		cerr << "usage: dcmsize dcmfile-in width [height [interpolate [aspect [ppmfile-out]]]]" << endl;
	else
	{
        char *input = argv[1];
		char *output = "temp/scaled.ppm";
		unsigned long width = atoi(argv[2]);
		unsigned long height = 0;
		int interpolate = 0;
		int aspect = 0;
		
		if (argc > 3)
			height = atoi(argv[3]);
		if (argc > 4)
        	interpolate = atoi(argv[4]);
		if (argc > 5)
        	aspect = atoi(argv[5]);
		if (argc > 6)
        	output = argv[6];
		
    	DicomImage *di = new DicomImage(input);
		if ((di != NULL) && (di->getStatus() == EIS_Normal))
		{
			DicomImage *ds = di->createScaledImage(width, height, interpolate, aspect);
			if ((ds != NULL) && (ds->getStatus() == EIS_Normal))
				ds->writeRawPPM(output, 8);
			else
				cerr << "ERROR: image can't be scaled !" << endl;
			delete ds;
		}
		delete di;
	}
	return 0;
}
