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
 * Revision:	  $Revision: 1.5 $
 * Status:	  $State: Exp $
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */


#include <stdlib.h>
#include <stdio.h>

#ifdef HAVE_LIBC_H
#include <libc.h>
#endif

#if defined(HAVE_MKTEMP) && !defined(HAVE_PROTOTYPE_MKTEMP)
extern "C" {
char * mktemp(char *);
}
#endif

#include <string.h>

#ifdef HAVE_UNIX_H
#include <unix.h>	/* for unlink() under Metrowerks C++ (Macintosh) */
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "dctk.h"
#include "dcdebug.h"
#include "dcmutils.h"
#include "dcimapix.h"

#include "cmdlnarg.h"

int main(int argc, char *argv[])
{
  SetDebugLevel(( 0 ));

  prepareCmdLineArgs(argc, argv);

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

  FILE *ppmFile = fopen( ifname, "r" );
  if (!ppmFile ) {
    cerr << argv[0] << ": cannot open inputfile: " << ifname  << endl;
    return 1;
  }
  DcmImagePixelModule image;
  image.readImageFromPGM( ppmFile );

  DcmFileStream inputStream( dcmfname, DCM_ReadMode );
  if ( inputStream.Fail() ) {
    cerr << argv[0] << ": Creating new DICOM-File: " << dcmfname  << endl;
    mustCreateNewDcmFile = TRUE;
    dfile = new DcmFileFormat;
  }
  else
  {
    cerr << argv[0] << ": Opening DICOM-File: " << dcmfname  << endl;
    dfile = new DcmFileFormat;
    dfile->transferInit();
    dfile->read(inputStream);
    dfile->transferEnd();
  }
  DcmDataset *dset = dfile->getDataset();
  if ( image.writeImageIntoDataset( *dset ) == EC_Normal )
  {
    char *newname = new char[ strlen( TEMPNAME_TEMPLATE ) + 1 ];
    strcpy( newname, TEMPNAME_TEMPLATE );
#ifdef HAVE_MKTEMP
    mktemp( newname );
    cerr << "use tempory filename " << newname << endl;
#else
    /* DANGER - just use TEMPNAME_TEMPLATE - DANGER */
#endif

    DcmFileStream oDS( newname, DCM_WriteMode );
    dfile->transferInit();
    dfile->write( oDS,
		 EXS_LittleEndianImplicit,
		 EET_UndefinedLength,
		 EGL_withoutGL );
    dfile->transferEnd();

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

