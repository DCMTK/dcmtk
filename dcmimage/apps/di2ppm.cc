/*********************************************************************
** 
**  Convert DicomImage (Source)
**
**	command-line program to test converting to ppm facilitiy
**
**  author   : Joerg Riesmeier
**  created  : 01.01.97
**  modified : 21.04.97
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
    if (argc < 2)
		cerr << "usage: di2ppm dcmfile-in [ppmfile-out [depth]]" << endl;
	else
	{
        char *input = argv[1];
		char *output = "convert.ppm";
		int bits = 8;
		
		if (argc > 2)
			output = argv[2];
		if (argc > 3)
			bits = atoi(argv[3]);
		
    	DicomImage *di = new DicomImage(input);
		if ((di != NULL) && (di->getStatus() == EIS_Normal))
		{
			if (bits <= MAX_RAWPPM_BITS)
				di->writeRawPPM(output, bits);
			else
				di->writePPM(output, bits);
		}
		delete di;
	}
	return 0;
}
