/*
 *
 *  Copyright (C) 2002-2016, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module:  dcmimage
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose: DcmQuant
 *
 */


#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmimage/diquant.h"
#include "dcmtk/dcmimage/diqtid.h"    /* for DcmQuantIdent */
#include "dcmtk/dcmimage/diqtcmap.h"  /* for DcmQuantColorMapping */
#include "dcmtk/dcmimage/diqtpix.h"   /* for DcmQuantPixel */
#include "dcmtk/dcmimage/diqthash.h"  /* for DcmQuantColorHashTable */
#include "dcmtk/dcmimage/diqtctab.h"  /* for DcmQuantColorTable */
#include "dcmtk/dcmimage/diqtfs.h"    /* for DcmQuantFloydSteinberg */
#include "dcmtk/dcmimage/dilogger.h"  /* for logging macros */
#include "dcmtk/dcmdata/dcswap.h"     /* for swapIfNecessary() */
#include "dcmtk/dcmdata/dcitem.h"     /* for DcmItem */
#include "dcmtk/dcmimgle/dcmimage.h"  /* for DicomImage */
#include "dcmtk/dcmdata/dcdeftag.h"   /* for tag constants */
#include "dcmtk/dcmdata/dcpixel.h"    /* for DcmPixelData */
#include "dcmtk/dcmdata/dcsequen.h"   /* for DcmSequenceOfItems */
#include "dcmtk/dcmdata/dcuid.h"      /* for dcmGenerateUniqueIdentifier() */


OFCondition DcmQuant::createPaletteColorImage(
    DicomImage& sourceImage,
    DcmItem& target,
    OFBool writeAsOW,
    OFBool write16BitEntries,
    OFBool floydSteinberg,
    Uint32 numberOfColors,
    OFString& description,
    DcmLargestDimensionType largeType,
    DcmRepresentativeColorType repType)
{
    // make sure we're operating on a color image
    if (sourceImage.isMonochrome()) return EC_IllegalCall;

    // make sure numberOfColors is in range
    if ((numberOfColors > 65536)||(numberOfColors < 2)) return EC_IllegalCall;

    OFCondition result = EC_Normal;

    // Create histogram of the colors, clustered if necessary
    DCMIMAGE_DEBUG("computing image histogram");

    DcmQuantColorTable chv;
    result = chv.computeHistogram(sourceImage, DcmQuantMaxColors);
    if (result.bad()) return result;

    unsigned long maxval = chv.getMaxVal();
    DCMIMAGE_DEBUG("image histogram: found " << chv.getColors() << " colors (at maxval=" << maxval << ")");

    // apply median-cut to histogram, making the new colormap.
    unsigned long cols = sourceImage.getWidth();
    unsigned long rows = sourceImage.getHeight();
    unsigned long frames = sourceImage.getFrameCount();

    DCMIMAGE_DEBUG("computing color map using Heckbert's median cut algorithm");

    DcmQuantColorTable colormap;
    result = colormap.medianCut(chv, cols * rows * frames, maxval, numberOfColors, largeType, repType);
    if (result.bad()) return result;
    chv.clear(); // frees most memory used by chv.

    // map the colors in the image to their closest match in the
    // new colormap, and write 'em out.
    DcmQuantColorHashTable cht;
    DCMIMAGE_DEBUG("mapping image data to color table");

    DcmQuantFloydSteinberg fs;
    if (floydSteinberg)
    {
      result = fs.initialize(cols);
      if (result.bad()) return result;
    }
    DcmQuantIdent id(cols);

    OFBool isByteData = (numberOfColors <= 256);

    // compute size requirement for palette color pixel data in bytes
    unsigned long totalSize = cols * rows * frames;
    if (! isByteData) totalSize *= 2;
    if (totalSize & 1) totalSize++;

    Uint16 *imageData16 = NULL;
    Uint8  *imageData8  = NULL;
    DcmPixelData *pixelData = new DcmPixelData(DCM_PixelData);
    if (pixelData)
    {
       result = pixelData->createUint16Array(totalSize/sizeof(Uint16), imageData16);
       if (result.good())
       {
         imageData16[(totalSize/sizeof(Uint16)) -1] = 0; // make sure pad byte is zero
         imageData8 = OFreinterpret_cast(Uint8 *, imageData16);
         result = target.insert(pixelData, OFTrue);
         if (result.good())
         {
            for (unsigned long ff=0; ff<frames; ff++)
            {
              if (isByteData)
              {
                if (floydSteinberg)
                  DcmQuantColorMapping<DcmQuantFloydSteinberg,Uint8>::create(sourceImage, ff, maxval, cht, colormap, fs, imageData8  + cols*rows*ff);
                  else DcmQuantColorMapping<DcmQuantIdent,    Uint8>::create(sourceImage, ff, maxval, cht, colormap, id, imageData8  + cols*rows*ff);
              }
              else
              {
                if (floydSteinberg)
                  DcmQuantColorMapping<DcmQuantFloydSteinberg,Uint16>::create(sourceImage, ff, maxval, cht, colormap, fs, imageData16 + cols*rows*ff);
                  else DcmQuantColorMapping<DcmQuantIdent,    Uint16>::create(sourceImage, ff, maxval, cht, colormap, id, imageData16 + cols*rows*ff);
             }
            } // for all frames

            // image creation is complete, finally adjust byte order if necessary
            if (isByteData)
            {
              result = swapIfNecessary(gLocalByteOrder, EBO_LittleEndian, imageData16, totalSize, sizeof(Uint16));
            }

         }
       }
    }

    DCMIMAGE_DEBUG("creating DICOM image pixel module");

    // create target image pixel module
    if (result.good()) result = target.putAndInsertUint16(DCM_SamplesPerPixel, 1);
    if (result.good()) result = target.putAndInsertUint16(DCM_PixelRepresentation, 0);
    if (result.good()) result = target.putAndInsertString(DCM_PhotometricInterpretation, "PALETTE COLOR");
    if (result.good()) result = target.putAndInsertUint16(DCM_Rows, OFstatic_cast(Uint16, rows));
    if (result.good()) result = target.putAndInsertUint16(DCM_Columns, OFstatic_cast(Uint16, cols));

    // determine bits allocated, stored, high bit
    Uint16 bitsAllocated = 8;
    Uint16 bitsStored = 8;
    Uint16 highBit = 7;
    if (! isByteData)
    {
      bitsAllocated = 16;
      bitsStored = 8;
      while ((1UL << bitsStored) < OFstatic_cast(unsigned long, numberOfColors)) bitsStored++;
      highBit = bitsStored - 1;
    }
    if (result.good()) result = target.putAndInsertUint16(DCM_BitsAllocated, bitsAllocated);
    if (result.good()) result = target.putAndInsertUint16(DCM_BitsStored, bitsStored);
    if (result.good()) result = target.putAndInsertUint16(DCM_HighBit, highBit);

    // make sure these attributes are not present in the target image
    delete target.remove(DCM_SmallestImagePixelValue);
    delete target.remove(DCM_LargestImagePixelValue);
    delete target.remove(DCM_PixelPaddingValue);
    delete target.remove(DCM_PixelPaddingRangeLimit);
    delete target.remove(DCM_SmallestPixelValueInSeries);
    delete target.remove(DCM_LargestPixelValueInSeries);
    delete target.remove(DCM_PaletteColorLookupTableUID);
    delete target.remove(DCM_SegmentedRedPaletteColorLookupTableData);
    delete target.remove(DCM_SegmentedGreenPaletteColorLookupTableData);
    delete target.remove(DCM_SegmentedBluePaletteColorLookupTableData);
    delete target.remove(DCM_PlanarConfiguration);

    // UNIMPLEMENTED: frame increment pointer
    // UNIMPLEMENTED: pixel aspect ratio/pixel spacing/imager pixel spacing

    if (frames > 1)
    {
      char buf[20];
      sprintf(buf, "%lu", frames);
      if (result.good()) result = target.putAndInsertString(DCM_NumberOfFrames, buf);
    }

    // create palette color LUT descriptor and data
    if (result.good()) result = colormap.write(target, writeAsOW, write16BitEntries);

    // create derivation description string
    if (result.good()) colormap.setDescriptionString(description);

    return result;
}


OFCondition DcmQuant::updateDerivationDescription(DcmItem *dataset, const char *description)
{
  if (description == NULL) return EC_IllegalCall;

  OFString derivationDescription(description);

  // append old Derivation Description, if any
  const char *oldDerivation = NULL;
  if ((dataset->findAndGetString(DCM_DerivationDescription, oldDerivation)).good() && oldDerivation)
  {
    derivationDescription += " [";
    derivationDescription += oldDerivation;
    derivationDescription += "]";
    if (derivationDescription.length() > 1024)
    {
      // ST is limited to 1024 characters, cut off tail
      derivationDescription.erase(1020);
      derivationDescription += "...]";
    }
  }

  return dataset->putAndInsertString(DCM_DerivationDescription, derivationDescription.c_str());
}
