/*********************************************************************
** 
**  Clip DicomImage (Source)
**
**	command-line program to test clipping facilitiy
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
    if (argc < 4)
		cerr << "SYNTAX: dcmclip dcmfile-in left top [width] [height] [ppmfile-out]" << endl;
	else
	{
        char *input = argv[1];
		char *output = "temp/clipped.ppm";
		unsigned long left = atoi(argv[2]);
		unsigned long top = atoi(argv[3]);
		unsigned long width = 0;
		unsigned long height = 0;
		
		if (argc > 4)
			width = atoi(argv[4]);
		if (argc > 5)
			height = atoi(argv[5]);
		if (argc > 6)
        	output = argv[6];
		
    	DicomImage *di = new DicomImage(input);
		if (di != NULL)
		{
			DicomImage *dc = di->createClippedImage(left, top, width, height);
			if (dc != NULL)
				dc->writeRawPPM(output, 8);
			delete dc;
		}
		delete di;
	}
	return 0;
}
