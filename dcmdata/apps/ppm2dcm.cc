/*
 *
 * Author: Gerd Ehlers          Created:  11-14-94
 *                              Modified: 11-14-94
 *
 * Module: ppm2dcm.cc
 *
 * Purpose:
 * Implementation of a ppm to DICOM-Image file format converter
 *
 *
 * Last Update:   $Author: hewett $
 * Revision:	  $Revision: 1.2 $
 * Status:	  $State: Exp $
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libc.h>
#include <unistd.h>
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
        cerr << "usage: " << argv[0] << " PPM-infile DICOM-inoutfile" ;
	cerr << endl;
	return 0;
    }

    char *ifname = argv[1];
    char *dcmfname = argv[2];
    BOOL mustCreateNewDcmFile = FALSE;
    DcmFileFormat *dfile = NULL;

    iDicomStream ppmStream( ifname );
    if ( ppmStream.fail() ) {
        cerr << argv[0] << ": cannot open inputfile: " << ifname  << endl;
	return 1;
    }
    DcmImagePixelModule image;
    image.readImageFromPGM( ppmStream );

    iDicomStream inputStream( dcmfname );
    if ( inputStream.fail() ) {
        cerr << argv[0] << ": Creating new DICOM-File: " << dcmfname  << endl;
        mustCreateNewDcmFile = TRUE;
        dfile = new DcmFileFormat();
    }
    else
    {
        cerr << argv[0] << ": Opening DICOM-File: " << dcmfname  << endl;
        dfile = new DcmFileFormat( &inputStream );
        dfile->read();
    }
    DcmDataset *dset = dfile->getDataset();
    if ( image.writeImageIntoDataset( *dset ) == EC_Normal )
    {
        char *newname = new char[ strlen( TEMPNAME_TEMPLATE ) + 1 ];
        strcpy( newname, TEMPNAME_TEMPLATE );
        mktemp( newname );
        cerr << "use tempory filename " << newname << endl;

        oDicomStream oDS( newname );
        dfile->write( oDS,
                      EXS_LittleEndianImplicit,
                      EET_UndefinedLength,
                      EGL_withoutGL );
        oDS.close();

        if ( !mustCreateNewDcmFile )
            unlink( dcmfname );
        rename( newname, dcmfname );
        delete newname;
    }
    else
        cerr << "an error has occured while creating new image" << endl;

    delete dfile;
    return 0;
}

