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
 * Last Update:   $Author: hewett $
 * Revision:	  $Revision: 1.1 $
 * Status:	  $State: Exp $
 *
 */


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

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
    T_VR_US		   SamplesPerPixel;
    E_PhotoInterp	   PhotometricInterpretation;
    T_VR_US		   Rows;
    T_VR_US		   Columns;
    T_VR_US		   BitsAllocated;
    T_VR_US		   BitsStored;
    T_VR_US		   HighBit;
    T_VR_US		   PixelRepresentation;
    DcmOtherByteOtherWord* PixelData;
    BOOL                   pixelDataIsPartOfDatset;

    T_VR_UL		   NumberofFrames;
    T_VR_US		   PlanarConfiguration;
    T_VR_US		   PixelAspectRatio;
    T_VR_SL		   SmallestImagePixelValue;
    T_VR_SL		   LargestImagePixelValue;
    T_VR_US		   RedPalColLuTEntryNumber;
    T_VR_SL		   RedPalColLuTMinValue;
    T_VR_US		   RedPalColLuTBitWidth;
    T_VR_SL*		   RedPalColLuT;
    T_VR_US		   GreenPalColLuTEntryNumber;
    T_VR_SL		   GreenPalColLuTMinValue;
    T_VR_US		   GreenPalColLuTBitWidth;
    T_VR_SL*		   GreenPalColLuT;
    T_VR_US		   BluePalColLuTEntryNumber;
    T_VR_SL		   BluePalColLuTMinValue;
    T_VR_US		   BluePalColLuTBitWidth;
    T_VR_SL*		   BluePalColLuT;

    BOOL		   allDataValid;
protected:
    E_PhotoInterp       interpNameToEnum(      char *interpName );

public:
    DcmImagePixelModule();
    virtual E_Condition readImageFromDataset(  DcmDataset &dset );
    virtual E_Condition readImageFromPGM(      iDicomStream &iDs );
    virtual E_Condition writeImageIntoDataset( DcmDataset &dset );
    virtual E_Condition writeImageAsPGM(       oDicomStream &oDs );
};

