/*
 *
 * Author: Gerd Ehlers		Created:  11-07-94
 *                              Modified: 11-20-94
 *
 * Module: dcimapix.cc
 *
 * Purpose:
 * Implementation of an Image Pixel Module DcmImagePixelModule
 *
 *
 * Last Update:   $Author: andreas $
 * Revision:	  $Revision: 1.4 $
 * Status:	  $State: Exp $
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strstream.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>

#include "dcdebug.h"

#include "dcmutils.h"
#include "dcimapix.h"


char* PhotoInterpNames[] =
{
    "unknown Photometric Interpretation",
    "MONOCHROME1",
    "MONOCHROME2",
    "PALETTE COLOR",
    "RGB",
    "HSV",
    "ARGB",
    "CMYK"
	};

short DIM_OF_PhotoInterpNames = 8;



// ********************************


E_PhotoInterp DcmImagePixelModule::interpNameToEnum( char *interpName )
{
	Bdebug((3, "dcimapix:DcmImagePixelModule::interpNameToEnum(char*)" ));

    E_PhotoInterp interp = EPI_unknown;
    if ( interpName != (char*)NULL )
    {
		short i = 0;
		while (    (i < DIM_OF_PhotoInterpNames)
			   && (strcmp(PhotoInterpNames[i], interpName) != 0)
			   )
			i++;
		if (	(i < DIM_OF_PhotoInterpNames)
			&& (strcmp(PhotoInterpNames[i], interpName) == 0)
			)
			interp = (E_PhotoInterp)i;
		debug(( 3, "input char*=\"%s\" output enum=%d", interpName, interp ));

    }
	Edebug(());

    return interp;
}


// ********************************


DcmImagePixelModule::DcmImagePixelModule()
{
	Bdebug((3, "dcimapix:DcmImagePixelModule::DcmImagePixelModule()" ));

    SamplesPerPixel = 1;
    PhotometricInterpretation = EPI_unknown;
    Rows = 0;
    Columns = 0;
    BitsAllocated = 0;
    BitsStored = 0;
    HighBit = 0;
    PixelRepresentation = 0;
    PixelData = (DcmOtherByteOtherWord*)NULL;
    pixelDataIsPartOfDatset = FALSE;

    NumberOfFrames = 1;
    PlanarConfiguration = 0;
    PixelAspectRatio = 0;
    SmallestImagePixelValue = 0;
    LargestImagePixelValue = 0;
    RedPalColLuTEntryNumber = 0;
    RedPalColLuTMinValue = 0;
    RedPalColLuTBitWidth = 0;
    RedPalColLuT = (Sint32*)NULL;
    GreenPalColLuTEntryNumber = 0;
    GreenPalColLuTMinValue = 0;
    GreenPalColLuTBitWidth = 0;
    GreenPalColLuT = (Sint32*)NULL;
    BluePalColLuTEntryNumber = 0;
    BluePalColLuTMinValue = 0;
    BluePalColLuTBitWidth = 0;
    BluePalColLuT = (Sint32*)NULL;

    allDataValid = FALSE;
	Edebug(());
}


// ********************************


E_Condition DcmImagePixelModule::readImageFromDataset( DcmDataset &dset )
{
	Bdebug((3, "dcimapix:DcmImagePixelModule::readImageFromDataset(DcmDataset&)" ));

    E_Condition l_error = EC_Normal;
    BOOL failed = FALSE;
    BOOL fatal	= FALSE;

    getSingleValueUL( &dset, DCM_NumberOfFrames, NumberOfFrames );
    cerr << "NumberOfFrames=" << NumberOfFrames << endl;

    failed |= getSingleValueUS( &dset, DCM_SamplesPerPixel, SamplesPerPixel );
    cerr << "SamplesPerPixel=" << SamplesPerPixel << endl;

    char *PhotometricString = "";
    failed |= getSingleValueByteString(&dset, DCM_PhotometricInterpretation,
									   PhotometricString );
    PhotometricInterpretation = interpNameToEnum( PhotometricString );
    if ( PhotometricInterpretation == EPI_unknown )
		failed |= TRUE;
    cerr << "PhotometricInterpretation=[" << PhotometricString << "]" << endl;

    fatal  |= getSingleValueUS( &dset, DCM_Columns, Columns );
    cerr << "Columns=" << Columns << endl;

    fatal  |= getSingleValueUS( &dset, DCM_Rows, Rows );
    cerr << "Rows=" << Rows << endl;

    failed |= getSingleValueUS( &dset, DCM_BitsAllocated, BitsAllocated );
    cerr << "BitsAllocated=" << BitsAllocated << endl;

    failed |= getSingleValueUS( &dset, DCM_BitsStored, BitsStored );
    cerr << "BitsStored=" << BitsStored << endl;

    failed |= getSingleValueUS( &dset, DCM_HighBit, HighBit );
    cerr << "HighBit=" << HighBit << endl;

    failed |= getSingleValueUS( &dset, DCM_PixelRepresentation,
							   PixelRepresentation );
    cerr << "PixelRepresentation=" << PixelRepresentation << endl;

    DcmStack stack;
    if ( dset.search( DCM_PixelData, stack, ESM_fromHere, FALSE ) == EC_Normal )
    {
        if (    PixelData != (DcmOtherByteOtherWord*)NULL
			&& pixelDataIsPartOfDatset == FALSE
			)
            delete PixelData;
		PixelData = (DcmOtherByteOtherWord*)stack.top();
        pixelDataIsPartOfDatset = TRUE;
    }
    else
		fatal |= TRUE;

    if ( failed || fatal )
    {
		cerr << "Mandatory attributes missing in Image Pixel module"
			<< " - cannot proceed\n" << flush;
		allDataValid = FALSE;
		l_error = EC_CorruptedData;
    }
    else
		allDataValid = TRUE;
	Edebug(());

    return l_error;
}


// ********************************

#define EOLN	    10
#define EOS	    0

/*
 *  return stats of a PBM file
 */

BOOL readPGM(FILE * inFile, Uint16 &sizex, Uint16 &sizey, Uint16 &maxcol,
			 BYTE *&pixel )
{
    char line[80+1];
    int type;

	if (fscanf(inFile, "%s\n%hu %hu\n%hu\n", line, &sizex, &sizey, &maxcol) != 4)
	{
		cerr << "not a PBM file, cannot find parameters" << endl;
		return FALSE;
	}

    if (line[0] != 'P')
    {
		cerr << "not a PBM: magic number of PBM is: " << line << endl;
		return FALSE;
    }
    type = line[1];

    switch (type)
    {
	  case '2':
	{
	    pixel = new BYTE[sizex * sizey];
	    for ( int i=0; i < (int)(sizex * sizey); i++ )
	    {
			Uint16 val;
			fscanf(inFile,"%hu", &val);
			pixel[i] = (unsigned char)(val & 0xff);
	    }
	    break;
	}
	  case '5':
	{
	    pixel = new BYTE[sizex * sizey];
		fread(pixel, sizex *sizey, 1, inFile);
	    break;
	}
	  default:
	{
	    cerr << "unsupported PBM format" << endl;
	    return FALSE;
		//	    break;
	}
    }
    return TRUE;
}

// ********************************


E_Condition DcmImagePixelModule::readImageFromPGM(FILE * inFile )
{
	Bdebug((3, "dcimapix:DcmImagePixelModule::readImageFromPGM(FILE*)" ));

    E_Condition l_error = EC_Normal;
    BYTE *pixel = (BYTE*)NULL;
    Uint16 maxcol;

    if ( readPGM(inFile, Columns, Rows, maxcol, pixel ) == TRUE )
    {
		NumberOfFrames = 1;
		SamplesPerPixel = 1;
		PhotometricInterpretation = EPI_Monochrome2;
		HighBit = 7;
		BitsStored = 8;
		BitsAllocated = 8;
		PixelRepresentation = 0;

        if (    PixelData != (DcmOtherByteOtherWord*)NULL
			&& pixelDataIsPartOfDatset == FALSE
			)
            delete PixelData;
        DcmTag pixtag( DCM_PixelData );
        PixelData = new DcmOtherByteOtherWord( pixtag );
        pixelDataIsPartOfDatset = FALSE;

        PixelData->setVR( EVR_OW );
		PixelData->put( pixel, Columns * Rows );
        delete pixel;                    // wurde in readPGM() alloziiert

		allDataValid = TRUE;
    }
    else
		l_error = EC_CorruptedData;
	Edebug(());

    return l_error;
}


// ********************************


E_Condition DcmImagePixelModule::writeImageIntoDataset( DcmDataset &dset )
{
	Bdebug((3, "dcimapix:DcmImagePixelModule::writeImageIntoDataset(DcmDataset&)" ));

    E_Condition l_error = EC_Normal;
    BOOL	fatal	= FALSE;

    if ( allDataValid == TRUE )
    {
		if ( NumberOfFrames > 1 )
		{
			fatal |= putSingleValue( &dset, DCM_NumberOfFrames, NumberOfFrames );
			cerr << "NumberOfFrames=" << NumberOfFrames << endl;
		}
		else
		{
			deleteAttribute( &dset, DCM_NumberOfFrames );
			deleteAttribute( &dset, DCM_FrameIncrementPointer );
		}

		fatal |= putSingleValue( &dset, DCM_SamplesPerPixel, SamplesPerPixel );
		cerr << "SamplesPerPixel=" << SamplesPerPixel << endl;

		if ( PhotometricInterpretation == EPI_unknown )
			fatal |= TRUE;
		char *PhotoStr = PhotoInterpNames[ (int)PhotometricInterpretation ];
		fatal |= putSingleValue( &dset, DCM_PhotometricInterpretation, PhotoStr );

		fatal |= putSingleValue( &dset, DCM_Columns, Columns );
		cerr << "Columns=" << Columns << endl;

		fatal |= putSingleValue( &dset, DCM_Rows, Rows );
		cerr << "Rows=" << Rows << endl;

		fatal |= putSingleValue( &dset, DCM_BitsAllocated, BitsAllocated );
		cerr << "BitsAllocated=" << BitsAllocated << endl;

		fatal |= putSingleValue( &dset, DCM_BitsStored, BitsStored );
		cerr << "BitsStored=" << BitsStored << endl;

		fatal |= putSingleValue( &dset, DCM_HighBit, HighBit );
		cerr << "HighBit=" << HighBit << endl;

		fatal |= putSingleValue( &dset, DCM_PixelRepresentation,
								PixelRepresentation );
		cerr << "PixelRepresentation=" << PixelRepresentation << endl;

		if ( PixelData == (DcmOtherByteOtherWord*)NULL )
		{
			DcmTag pixtag( DCM_PixelData );
			PixelData = new DcmOtherByteOtherWord( pixtag );
		}
		dset.insert( PixelData, TRUE );
        pixelDataIsPartOfDatset = TRUE;
    }
    else
		fatal = TRUE;

    if ( fatal )
    {
		cerr << "Mandatory attributes missing in Image Pixel module"
			<< " - cannot proceed\n" << flush;
		allDataValid = FALSE;
		l_error = EC_CorruptedData;
    }
	Edebug(());

    return l_error;
}


// ********************************


E_Condition DcmImagePixelModule::writeImageAsPGM(FILE * outFile )
{
    Bdebug((3, "dcimapix:DcmImagePixelModule::writeImageAsPGM()" ));

    E_Condition l_error = EC_Normal;
    if ( allDataValid == TRUE )
    {
	if (SamplesPerPixel != 1)
	{
	    cerr << "Unsupported SamplesPerPixel = " << SamplesPerPixel
		 << endl;
	    l_error = EC_IllegalCall;
	}
	else
	{
	    if (   PhotometricInterpretation == EPI_Monochrome1
		   || PhotometricInterpretation == EPI_Monochrome2
		)
	    {
		if (HighBit+1-BitsStored != 0)
		{
		    cerr << "Unsupported - data not packed in low part of word"
			 << endl;
		    l_error = EC_IllegalCall;
		}
		else
		{
		    Uint32 maxval = ( 1 << BitsStored ) - 1;
		    fprintf(outFile, "P5\n%u %u\n%lu\n", Columns, Rows, (unsigned long)maxval);
		    fwrite(PixelData->getBytes(), 
			   (size_t)PixelData->getLength(), 1, outFile);
		}
	    }
	    else
	    {
		cerr << "Unsupported PhotometricInterpretation = "
		     << PhotometricInterpretation
		     << endl;
		l_error = EC_IllegalCall;
	    }
	}
    }
    Edebug(());

    return l_error;
}


// ********************************


