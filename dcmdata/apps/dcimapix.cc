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
 * Last Update:   $Author: hewett $
 * Revision:	  $Revision: 1.1 $
 * Status:	  $State: Exp $
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

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

    NumberofFrames = 1;
    PlanarConfiguration = 0;
    PixelAspectRatio = 0;
    SmallestImagePixelValue = 0;
    LargestImagePixelValue = 0;
    RedPalColLuTEntryNumber = 0;
    RedPalColLuTMinValue = 0;
    RedPalColLuTBitWidth = 0;
    RedPalColLuT = (T_VR_SL*)NULL;
    GreenPalColLuTEntryNumber = 0;
    GreenPalColLuTMinValue = 0;
    GreenPalColLuTBitWidth = 0;
    GreenPalColLuT = (T_VR_SL*)NULL;
    BluePalColLuTEntryNumber = 0;
    BluePalColLuTMinValue = 0;
    BluePalColLuTBitWidth = 0;
    BluePalColLuT = (T_VR_SL*)NULL;

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

    getSingleValueUL( &dset, ET_NumberofFrames, NumberofFrames );
    cerr << "NumberofFrames=" << NumberofFrames << endl;

    failed |= getSingleValueUS( &dset, ET_SamplesperPixel, SamplesPerPixel );
    cerr << "SamplesPerPixel=" << SamplesPerPixel << endl;

    char *PhotometricString = "";
    failed |= getSingleValueByteString( &dset, ET_PhotometricInterpretation,
					PhotometricString );
    PhotometricInterpretation = interpNameToEnum( PhotometricString );
    if ( PhotometricInterpretation == EPI_unknown )
	failed |= TRUE;
    cerr << "PhotometricInterpretation=[" << PhotometricString << "]" << endl;

    fatal  |= getSingleValueUS( &dset, ET_Columns0028, Columns );
    cerr << "Columns=" << Columns << endl;

    fatal  |= getSingleValueUS( &dset, ET_Rows0028, Rows );
    cerr << "Rows=" << Rows << endl;

    failed |= getSingleValueUS( &dset, ET_BitsAllocated0028, BitsAllocated );
    cerr << "BitsAllocated=" << BitsAllocated << endl;

    failed |= getSingleValueUS( &dset, ET_BitsStored, BitsStored );
    cerr << "BitsStored=" << BitsStored << endl;

    failed |= getSingleValueUS( &dset, ET_HighBit, HighBit );
    cerr << "HighBit=" << HighBit << endl;

    failed |= getSingleValueUS( &dset, ET_PixelRepresentation,
				PixelRepresentation );
    cerr << "PixelRepresentation=" << PixelRepresentation << endl;

    DcmStack stack;
    if ( dset.search( ET_PixelData, stack, ESM_fromHere, FALSE ) == EC_Normal )
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
 *  get_str()
 */

void get_str( iDicomStream &iDs, char *outStr, int maxlen )
{
    char c = 0;
    int  p = 0;

    while ((c != EOLN) && (p < maxlen))
    {
	iDs.read( c );
	if ( iDs.gcount() != 1L )
	    break;
	if ( (p > 0) && (outStr[p-1] == 13) && (c == EOLN) )
	{
	    outStr[p-1] = c;
	}
	else
	{
	    outStr[p] = c;
	    p++;
	}
    }
    outStr[p] = EOS;
debug(( 1, "get_str: [%s]", outStr ));

}


/*
 *  get_int()
 */

void get_int( iDicomStream &iDs, int &outVal )
{
    char c = 0;
    int sol = 0;

    while (!isdigit(c))
    {
	iDs.read( c );
	if ( iDs.gcount() != 1L )
	{
	    outVal = EOF;
	    return;
	}
	if (sol && (c == '#'))
	{
	    while (c != EOLN)
	    {
		iDs.read( c );
		if ( iDs.gcount() != 1L )
		{
		    outVal = EOF;
		    return;
		}
	    }
	}
	sol = (c == EOLN);
    }
    outVal = 0;
    while (isdigit(c))
    {
	outVal *= 10;
	outVal += c-'0';
	iDs.read( c );
	if ( iDs.gcount() != 1L )
	{
	    outVal = EOF;
	    return;
	}
    }
debug(( 1, "get_int: [%ld]", outVal ));

}


/*
 *  return stats of a PBM file
 */

BOOL readPGM( iDicomStream &iDs,
	      T_VR_US &sizex,
	      T_VR_US &sizey,
	      T_VR_US &maxcol,
	      U_CHAR *&pixel )
{
    char line[80+1];
    int type;

    get_str ( iDs, line, 80);
    if (line[0] != 'P')
    {
	cerr << "not a PBM: magic number of PBM is: " << line << endl;
	return FALSE;
    }
    type = line[1];
    int l_sizex, l_sizey, l_maxcol;
    get_int( iDs, l_sizex );
    get_int( iDs, l_sizey );
    get_int( iDs, l_maxcol );
    sizex = l_sizex;
    sizey = l_sizey;
    maxcol = l_maxcol;
    switch (type)
    {
	case '2':
	{
	    pixel = new U_CHAR[sizex * sizey];
	    for ( int i=0; i < (int)(sizex * sizey); i++ )
	    {
		int val;
		get_int( iDs, val );
		pixel[i] = (unsigned char)(val & 0xff);
	    }
	    break;
	}
	case '5':
	{
	    pixel = new U_CHAR[sizex * sizey];
	    iDs.read( pixel, sizex * sizey );
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


E_Condition DcmImagePixelModule::readImageFromPGM( iDicomStream &iDs )
{
Bdebug((3, "dcimapix:DcmImagePixelModule::readImageFromPGM(iDicomStream&)" ));

    E_Condition l_error = EC_Normal;
    U_CHAR *pixel = (U_CHAR*)NULL;
    T_VR_US maxcol;

    if ( readPGM( iDs, Columns, Rows, maxcol, pixel ) == TRUE )
    {
	NumberofFrames = 1;
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
        DcmTag pixtag( ET_PixelData );
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
	if ( NumberofFrames > 1 )
	{
	    fatal |= putSingleValue( &dset, ET_NumberofFrames, NumberofFrames );
	    cerr << "NumberofFrames=" << NumberofFrames << endl;
	}
	else
	{
	    deleteAttribute( &dset, ET_NumberofFrames );
	    deleteAttribute( &dset, ET_FrameIncrementPointer );
	}

	fatal |= putSingleValue( &dset, ET_SamplesperPixel, SamplesPerPixel );
	cerr << "SamplesPerPixel=" << SamplesPerPixel << endl;

	if ( PhotometricInterpretation == EPI_unknown )
	    fatal |= TRUE;
	char *PhotoStr = PhotoInterpNames[ (int)PhotometricInterpretation ];
	fatal |= putSingleValue( &dset, ET_PhotometricInterpretation, PhotoStr );

	fatal |= putSingleValue( &dset, ET_Columns0028, Columns );
	cerr << "Columns=" << Columns << endl;

	fatal |= putSingleValue( &dset, ET_Rows0028, Rows );
	cerr << "Rows=" << Rows << endl;

	fatal |= putSingleValue( &dset, ET_BitsAllocated0028, BitsAllocated );
	cerr << "BitsAllocated=" << BitsAllocated << endl;

	fatal |= putSingleValue( &dset, ET_BitsStored, BitsStored );
	cerr << "BitsStored=" << BitsStored << endl;

	fatal |= putSingleValue( &dset, ET_HighBit, HighBit );
	cerr << "HighBit=" << HighBit << endl;

	fatal |= putSingleValue( &dset, ET_PixelRepresentation,
					PixelRepresentation );
	cerr << "PixelRepresentation=" << PixelRepresentation << endl;

	if ( PixelData == (DcmOtherByteOtherWord*)NULL )
	{
	    DcmTag pixtag( ET_PixelData );
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


E_Condition DcmImagePixelModule::writeImageAsPGM( oDicomStream &oDs )
{
Bdebug((3, "dcimapix:DcmImagePixelModule::writeImageAsPGM(oDicomStream&)" ));

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
		    T_VR_UL maxval = ( 1 << BitsStored ) - 1;
		    ostrstream ost;
		    ost << "P5\n" << Columns << " " << Rows << "\n"
			<< maxval << "\n" << '\0';
		    char *str=ost.str();
		    oDs.write( str, strlen(str) );
		    delete str;

		    oDs.write( PixelData->getBytes(),
			       (int)PixelData->getLength() );
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


