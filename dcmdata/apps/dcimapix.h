/*
 *
 * Author: Gerd Ehlers		Created:  11-07-94
 *                              Modified: 11-20-94
 *
 * Module: dcimapix.h
 *
 * Purpose:
 * Interface of an Image Pixel Module DcmImagePixelModule
 *
 *
 * Last Update:   $Author: andreas $
 * Revision:	  $Revision: 1.3 $
 * Status:	  $State: Exp $
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dctk.h"


typedef enum {
    EPI_unknown = 0,
    EPI_Monochrome1 = 1,
    EPI_Monochrome2 = 2,
    EPI_PaletteColor = 3,
    EPI_RGB = 4,
    EPI_HSV = 5,
    EPI_ARGB = 6,
    EPI_CMYK = 7
} E_PhotoInterp;


class DcmImagePixelModule {
    Uint16		   SamplesPerPixel;
    E_PhotoInterp	   PhotometricInterpretation;
    Uint16		   Rows;
    Uint16		   Columns;
    Uint16		   BitsAllocated;
    Uint16		   BitsStored;
    Uint16		   HighBit;
    Uint16		   PixelRepresentation;
    DcmOtherByteOtherWord* PixelData;
    BOOL                   pixelDataIsPartOfDatset;

    Uint32		   NumberOfFrames;
    Uint16		   PlanarConfiguration;
    Uint16		   PixelAspectRatio;
    Sint32		   SmallestImagePixelValue;
    Sint32		   LargestImagePixelValue;
    Uint16		   RedPalColLuTEntryNumber;
    Sint32		   RedPalColLuTMinValue;
    Uint16		   RedPalColLuTBitWidth;
    Sint32*		   RedPalColLuT;
    Uint16		   GreenPalColLuTEntryNumber;
    Sint32		   GreenPalColLuTMinValue;
    Uint16		   GreenPalColLuTBitWidth;
    Sint32*		   GreenPalColLuT;
    Uint16		   BluePalColLuTEntryNumber;
    Sint32		   BluePalColLuTMinValue;
    Uint16		   BluePalColLuTBitWidth;
    Sint32*		   BluePalColLuT;

    BOOL		   allDataValid;
protected:
    E_PhotoInterp       interpNameToEnum(      char *interpName );

public:
    DcmImagePixelModule();
    virtual E_Condition readImageFromDataset(  DcmDataset &dset );
    virtual E_Condition readImageFromPGM(FILE * inFile);
    virtual E_Condition writeImageIntoDataset( DcmDataset &dset );
    virtual E_Condition writeImageAsPGM(FILE * outFile);
};

