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
 * Revision:	  $Revision: 1.2 $
 * Status:	  $State: Exp $
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <stdio.h>
#include <stdlib.h>
#include <strstream.h>

#include "dctk.h"
#include "dcdebug.h"

#include "dcmutils.h"
#include "dcimapix.h"


int main(int argc, char *argv[])
{
    SetDebugLevel(( 0 ));

    /* parse cmd line */
    if (argc != 3) {
	cerr << "usage: " << argv[0] << " DICOM-infile PPM-outfile" ;
	cerr << endl;
	return 0;
    }

    char *ifname = argv[1];
    char *ofname = argv[2];

    iDicomStream inputStream( ifname );
    if ( inputStream.fail() ) {
	cerr << argv[0] << ": cannot open infile: " << ifname  << endl;
	return 1;
    }

    oDicomStream outputStream( ofname );
    if ( outputStream.fail() ) {
	cerr << argv[0] << ": cannot write outfile: " << ofname  << endl;
	return 1;
    }

    DcmFileFormat dfile( &inputStream );
    dfile.read();

    DcmDataset *dset = dfile.getDataset();
    DcmImagePixelModule image;

    image.readImageFromDataset( *dset );
    image.writeImageAsPGM( outputStream );

    return 0;
}

