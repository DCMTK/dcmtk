/*
 *
 * Author: Gerd Ehlers		Created:  11-06-94
 *                              Modified: 02-15-95
 *
 * Module: dcm2ppm.cc
 *
 * Purpose:
 * Implementation of a DICOM-Image to ppm file format converter
 *
 *
 * Last Update:   $Author: hewett $
 * Revision:	  $Revision: 1.5 $
 * Status:	  $State: Exp $
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <stdio.h>
#include <stdlib.h>
#include <strstream.h>

#ifdef HAVE_GUSI_H
#include <GUSI.h>
#endif

#include "dctk.h"
#include "dcdebug.h"
#include "dcmutils.h"
#include "dcimapix.h"
#include "cmdlnarg.h"

int main(int argc, char *argv[])
{
#ifdef HAVE_GUSI_H
    GUSISetup(GUSIwithSIOUXSockets);
    GUSISetup(GUSIwithInternetSockets);
#endif

    SetDebugLevel(( 0 ));

    prepareCmdLineArgs(argc, argv, "dcm2ppm");

    /* parse cmd line */
    if (argc != 3) {
	cerr << "usage: " << argv[0] << " DICOM-infile PPM-outfile" ;
	cerr << endl;
	return 0;
    }

    char *ifname = argv[1];
    char *ofname = argv[2];

    DcmFileStream inputStream(ifname, DCM_ReadMode);
    if (inputStream.GetError() != EC_Normal ) 
	{
		cerr << argv[0] << ": cannot open infile: " << ifname  << endl;
		return 1;
    }

	FILE * outFile = fopen(ofname, "wb");
    if (!outFile)
	{
		cerr << argv[0] << ": cannot write outfile: " << ofname  << endl;
		return 1;
    }

    DcmFileFormat dfile;
	dfile.transferInit();
    dfile.read(inputStream);
	dfile.transferEnd();

    DcmDataset *dset = dfile.getDataset();
    DcmImagePixelModule image;

    image.readImageFromDataset( *dset );
    image.writeImageAsPGM( outFile );
	fclose(outFile);

    return 0;
}

