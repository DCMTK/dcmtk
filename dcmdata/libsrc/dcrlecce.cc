/*
 *
 *  Copyright (C) 2002-2010, OFFIS e.V.
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
 *  Module:  dcmdata
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose: encoder codec class for RLE
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:09 $
 *  CVS/RCS Revision: $Revision: 1.16 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dcrlecce.h"

#include "dcmtk/dcmdata/dcrleenc.h"  /* for class DcmRLEEncoder */
#include "dcmtk/dcmdata/dcrlecp.h"   /* for class DcmRLECodecParameter */
#include "dcmtk/dcmdata/dcdeftag.h"  /* for tag constants */
#include "dcmtk/dcmdata/dcpixseq.h"  /* for class DcmPixelSequence */
#include "dcmtk/dcmdata/dcpxitem.h"  /* for class DcmPixelItem */
#include "dcmtk/dcmdata/dcswap.h"    /* for swapIfNecessary */
#include "dcmtk/dcmdata/dcitem.h"
#include "dcmtk/ofstd/ofstd.h"

#define INCLUDE_CSTDIO
#include "dcmtk/ofstd/ofstdinc.h"


typedef OFList<DcmRLEEncoder *> DcmRLEEncoderList;
typedef OFListIterator(DcmRLEEncoder *) DcmRLEEncoderListIterator;


// =======================================================================

DcmRLECodecEncoder::DcmRLECodecEncoder()
: DcmCodec()
{
}


DcmRLECodecEncoder::~DcmRLECodecEncoder()
{
}


OFBool DcmRLECodecEncoder::canChangeCoding(
    const E_TransferSyntax oldRepType,
    const E_TransferSyntax newRepType) const
{
  E_TransferSyntax myXfer = EXS_RLELossless;
  DcmXfer newRep(newRepType);
  DcmXfer oldRep(oldRepType);
  if (oldRep.isNotEncapsulated() && (newRepType == myXfer)) return OFTrue; // compress requested

  // we don't support re-coding for now.
  return OFFalse;
}


OFCondition DcmRLECodecEncoder::decode(
    const DcmRepresentationParameter * /* fromRepParam */,
    DcmPixelSequence * /* pixSeq */,
    DcmPolymorphOBOW& /* uncompressedPixelData */,
    const DcmCodecParameter * /* cp */,
    const DcmStack& /* objStack */) const
{
  // we are an encoder only
  return EC_IllegalCall;
}


OFCondition DcmRLECodecEncoder::decodeFrame(
    const DcmRepresentationParameter * /* fromParam */ ,
    DcmPixelSequence * /* fromPixSeq */ ,
    const DcmCodecParameter * /* cp */ ,
    DcmItem * /* dataset */ ,
    Uint32 /* frameNo */ ,
    Uint32& /* startFragment */ ,
    void * /* buffer */ ,
    Uint32 /* bufSize */ ,
    OFString& /* decompressedColorModel */ ) const
{
  // we are an encoder only
  return EC_IllegalCall;
}    


OFCondition DcmRLECodecEncoder::encode(
    const E_TransferSyntax /* fromRepType */,
    const DcmRepresentationParameter * /* fromRepParam */,
    DcmPixelSequence * /* fromPixSeq */,
    const DcmRepresentationParameter * /* toRepParam */,
    DcmPixelSequence * & /* toPixSeq */,
    const DcmCodecParameter * /* cp */,
    DcmStack & /* objStack */) const
{
  // we don't support re-coding for now.
  return EC_IllegalCall;
}


OFCondition DcmRLECodecEncoder::encode(
    const Uint16 *pixelData,
    const Uint32 length,
    const DcmRepresentationParameter * /* toRepParam */ ,
    DcmPixelSequence * & pixSeq,
    const DcmCodecParameter *cp,
    DcmStack & objStack) const
{
  OFCondition result = EC_Normal;

  // assume we can cast the codec parameter to what we need
  const DcmRLECodecParameter *djcp = OFstatic_cast(const DcmRLECodecParameter *, cp);
  DcmStack localStack(objStack);
  (void)localStack.pop();             // pop pixel data element from stack
  DcmObject *dataset = localStack.pop(); // this is the item in which the pixel data is located
  Uint8 *pixelData8 = OFreinterpret_cast(Uint8 *, OFconst_cast(Uint16 *, pixelData));
  Uint8 *pixelPointer = NULL;
  DcmOffsetList offsetList;
  DcmRLEEncoderList rleEncoderList;
  DcmRLEEncoderListIterator first = rleEncoderList.begin();
  DcmRLEEncoderListIterator last = rleEncoderList.end();
  Uint32 rleHeader[16];
  Uint32 i;
  OFBool byteSwapped = OFFalse;  // true if we have byte-swapped the original pixel data

  if ((!dataset)||((dataset->ident()!= EVR_dataset) && (dataset->ident()!= EVR_item))) result = EC_InvalidTag;
  else
  {
    DcmItem *ditem = OFstatic_cast(DcmItem *, dataset);
    Uint16 bitsAllocated = 0;
    Uint16 bytesAllocated = 0;
    Uint16 samplesPerPixel = 0;
    Uint16 planarConfiguration = 0;
    Uint16 columns = 0;
    Uint16 rows = 0;
    Sint32 numberOfFrames = 1;
    Uint32 numberOfStripes = 0;
    Uint32 compressedSize = 0;

    result = ditem->findAndGetUint16(DCM_BitsAllocated, bitsAllocated);
    if (result.good()) result = ditem->findAndGetUint16(DCM_SamplesPerPixel, samplesPerPixel);
    if (result.good()) result = ditem->findAndGetUint16(DCM_Columns, columns);
    if (result.good()) result = ditem->findAndGetUint16(DCM_Rows, rows);
    if (result.good())
    {
      result = ditem->findAndGetSint32(DCM_NumberOfFrames, numberOfFrames);
      if (result.bad() || numberOfFrames < 1) numberOfFrames = 1;
      result = EC_Normal;
    }
    if (result.good() && (samplesPerPixel > 1))
    {
      result = ditem->findAndGetUint16(DCM_PlanarConfiguration, planarConfiguration);
    }

    if (result.good())
    {
      // check if bitsAllocated is a multiple of 8 - we don't handle anything else
      bytesAllocated = OFstatic_cast(Uint16, bitsAllocated / 8);
      if ((bitsAllocated < 8)||(bitsAllocated % 8 != 0)) result = EC_CannotChangeRepresentation;

      // make sure that all the descriptive attributes have sensible values
      if ((columns < 1)||(rows < 1)||(samplesPerPixel < 1)) result = EC_CannotChangeRepresentation;

      // an RLE stripe set can have at most 15 stripes, i.e. 15 bytes allocated per pixel
      numberOfStripes = bytesAllocated * samplesPerPixel;
      if (numberOfStripes > 15) result = EC_CannotChangeRepresentation;

      // make sure that we have at least as many bytes of pixel data as we expect
      if (numberOfStripes * columns * rows * numberOfFrames > length) result = EC_CannotChangeRepresentation;
    }

    DcmPixelSequence *pixelSequence = NULL;
    DcmPixelItem *offsetTable = NULL;

    // create initial pixel sequence
    if (result.good())
    {
      pixelSequence = new DcmPixelSequence(DcmTag(DCM_PixelData,EVR_OB));
      if (pixelSequence == NULL) result = EC_MemoryExhausted;
      else
      {
        // create empty offset table
        offsetTable = new DcmPixelItem(DcmTag(DCM_Item,EVR_OB));
        if (offsetTable == NULL) result = EC_MemoryExhausted;
        else pixelSequence->insert(offsetTable);
      }
    }

    // byte swap pixel data to little endian
    if (gLocalByteOrder == EBO_BigEndian)
    {
       swapIfNecessary(EBO_LittleEndian, gLocalByteOrder, OFstatic_cast(void *, OFconst_cast(Uint16 *, pixelData)), length, sizeof(Uint16));
    }

    // create RLE stripe sets
    if (result.good())
    {
      const Uint32 bytesPerStripe = columns * rows;
      const Uint32 frameSize = columns * rows * samplesPerPixel * bytesAllocated;
      Uint32 frameOffset = 0;
      Uint32 sampleOffset = 0;
      Uint32 offsetBetweenSamples = 0;
      Uint32 sample = 0;
      Uint32 byte = 0;
      register Uint32 pixel = 0;
      register Uint32 columnCounter = 0;

      DcmRLEEncoder *rleEncoder = NULL;
      Uint32 rleSize = 0;
      Uint8 *rleData = NULL;
      Uint8 *rleData2 = NULL;

      // compute byte offset between samples
      if (planarConfiguration == 0)
         offsetBetweenSamples = samplesPerPixel * bytesAllocated;
         else offsetBetweenSamples = bytesAllocated;

      // loop through all frames of the image
      for (Uint32 currentFrame = 0; ((currentFrame < OFstatic_cast(Uint32, numberOfFrames)) && result.good()); currentFrame++)
      {
        // offset to start of frame, in bytes
        frameOffset = frameSize * currentFrame;

        // loop through all samples of one frame
        for (sample = 0; sample < samplesPerPixel; sample++)
        {
          // compute byte offset for first sample in frame
          if (planarConfiguration == 0)
             sampleOffset = sample * bytesAllocated;
             else sampleOffset = sample * bytesAllocated * columns * rows;

          // loop through the bytes of one sample
          for (byte = 0; byte < bytesAllocated; byte++)
          {
            pixelPointer = pixelData8 + frameOffset + sampleOffset + bytesAllocated - byte - 1;

            // initialize new RLE codec for this stripe
            rleEncoder = new DcmRLEEncoder(1 /* DICOM padding required */);
            if (rleEncoder)
            {
              rleEncoderList.push_back(rleEncoder);
              columnCounter = columns;

              // loop through all pixels of the frame
              for (pixel = 0; pixel < bytesPerStripe; ++pixel)
              {
                rleEncoder->add(*pixelPointer);

                // enforce DICOM rule that "Each row of the image shall be encoded
                // separately and not cross a row boundary."
                // (see DICOM part 5 section G.3.1)
                if (--columnCounter == 0)
                {
                  rleEncoder->flush();
                  columnCounter = columns;
                }
                pixelPointer += offsetBetweenSamples;
              }

              rleEncoder->flush();
              if (rleEncoder->fail()) result = EC_MemoryExhausted;
            } else result = EC_MemoryExhausted;
          }
        }

        // store frame and erase RLE codec list
        if (result.good() && (rleEncoderList.size() > 0) && (rleEncoderList.size() < 16))
        {
          // compute size of compressed frame including RLE header
          // and populate RLE header
          for (i=0; i<16; i++) rleHeader[i] = 0;
          rleHeader[0] = rleEncoderList.size();
          rleSize = 64;
          i = 1;
          first = rleEncoderList.begin();
          while (first != last)
          {
            rleHeader[i++] = rleSize;
            rleSize += (*first)->size();
            ++first;
          }

          // allocate buffer for compressed frame
          rleData = new Uint8[rleSize];

          if (rleData)
          {
            // copy RLE header to compressed frame buffer
            swapIfNecessary(EBO_LittleEndian, gLocalByteOrder, rleHeader, 16*sizeof(Uint32), sizeof(Uint32));
            memcpy(rleData, rleHeader, 64);

            // store RLE stripe sets in compressed frame buffer
            rleData2 = rleData + 64;
            first = rleEncoderList.begin();
            while (first != last)
            {
              (*first)->write(rleData2);
              rleData2 += (*first)->size();
              delete *first;
              first = rleEncoderList.erase(first);
            }

            // store compressed frame, breaking into segments if necessary
            result = pixelSequence->storeCompressedFrame(offsetList, rleData, rleSize, djcp->getFragmentSize());
            compressedSize += rleSize;

            // erase buffer for compressed frame
            delete[] rleData;
          } else result = EC_MemoryExhausted;
        }
        else
        {
          // erase RLE codec list
          first = rleEncoderList.begin();
          while (first != last)
          {
            delete *first;
            first = rleEncoderList.erase(first);
          }
          if (result.good()) result = EC_CannotChangeRepresentation;
        }

      }

    }

    // store pixel sequence if everything went well.
    if (result.good()) pixSeq = pixelSequence;
    else
    {
      delete pixelSequence;
      pixSeq = NULL;
    }

    if ((result.good()) && (djcp->getCreateOffsetTable()))
    {
      // create offset table
      result = offsetTable->createOffsetTable(offsetList);
    }

    // the following operations do not affect the Image Pixel Module
    // but other modules such as SOP Common.  We only perform these
    // changes if we're on the main level of the dataset,
    // which should always identify itself as dataset, not as item.
    if (dataset->ident() == EVR_dataset)
    {
        if (result.good())
        {
            // create new UID if mode is true or if we're converting to Secondary Capture
            if (djcp->getConvertToSC() || djcp->getUIDCreation())
            {
                result = DcmCodec::newInstance(OFstatic_cast(DcmItem *, dataset), "DCM", "121320", "Uncompressed predecessor");
                
                // set image type to DERIVED
                if (result.good()) result = updateImageType(OFstatic_cast(DcmItem *, dataset));

                // update derivation description
                if (result.good())
                {
                  // compute original image size in bytes, ignoring any padding bits.
                  double compressionRatio = 0.0;
                  if (compressedSize > 0) compressionRatio = (OFstatic_cast(double, columns * rows * bitsAllocated * OFstatic_cast(Uint32, numberOfFrames) * samplesPerPixel) / 8.0) / compressedSize;
                  result = updateDerivationDescription(OFstatic_cast(DcmItem *, dataset), compressionRatio);
                }
            }
        }

        // convert to Secondary Capture if requested by user.
        // This method creates a new SOP class UID, so it should be executed
        // after the call to newInstance() which creates a Source Image Sequence.
        if (result.good() && djcp->getConvertToSC()) result = DcmCodec::convertToSecondaryCapture(OFstatic_cast(DcmItem *, dataset));
    }
  }

  // byte swap pixel data back to local endian if necessary
  if (byteSwapped)
  {
    swapIfNecessary(gLocalByteOrder, EBO_LittleEndian, OFstatic_cast(void *, OFconst_cast(Uint16 *, pixelData)), length, sizeof(Uint16));
  }
  return result;
}


OFCondition DcmRLECodecEncoder::updateDerivationDescription(
  DcmItem *dataset,
  double ratio)
{
  char buf[32];

  // create new Derivation Description
  OFString derivationDescription = "Lossless RLE compression, compression ratio ";
  OFStandard::ftoa(buf, sizeof(buf), ratio, OFStandard::ftoa_uppercase, 0, 5);
  derivationDescription += buf;

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


OFCondition DcmRLECodecEncoder::determineDecompressedColorModel(
    const DcmRepresentationParameter * /* fromParam */,
    DcmPixelSequence * /* fromPixSeq */,
    const DcmCodecParameter * /* cp */,
    DcmItem * /* dataset */,
    OFString & /* decompressedColorModel */) const
{
    return EC_IllegalCall;
}


/*
 * CVS/RCS Log
 * $Log: dcrlecce.cc,v $
 * Revision 1.16  2010-10-14 13:14:09  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.15  2010-03-01 09:08:45  uli
 * Removed some unnecessary include directives in the headers.
 *
 * Revision 1.14  2009-11-17 16:41:26  joergr
 * Added new method that allows for determining the color model of the
 * decompressed image.
 *
 * Revision 1.13  2009-11-04 09:58:10  uli
 * Switched to logging mechanism provided by the "new" oflog module
 *
 * Revision 1.12  2008-05-29 10:46:16  meichel
 * Implemented new method DcmPixelData::getUncompressedFrame
 *   that permits frame-wise access to compressed and uncompressed
 *   objects without ever loading the complete object into main memory.
 *   For this new method to work with compressed images, all classes derived from
 *   DcmCodec need to implement a new method decodeFrame(). For now, only
 *   dummy implementations returning an error code have been defined.
 *
 * Revision 1.11  2005/12/08 15:41:30  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.10  2004/08/24 14:54:20  meichel
 *  Updated compression helper methods. Image type is not set to SECONDARY
 *   any more, support for the purpose of reference code sequence added.
 *
 * Revision 1.9  2004/02/04 16:43:42  joergr
 * Adapted type casts to new-style typecast operators defined in ofcast.h.
 * Removed acknowledgements with e-mail addresses from CVS log.
 *
 * Revision 1.8  2003/08/14 09:01:06  meichel
 * Adapted type casts to new-style typecast operators defined in ofcast.h
 *
 * Revision 1.7  2003/03/21 13:08:04  meichel
 * Minor code purifications for warnings reported by MSVC in Level 4
 *
 * Revision 1.6  2002/12/04 10:41:01  meichel
 * Changed toolkit to use OFStandard::ftoa instead of sprintf for all
 *   double to string conversions that are supposed to be locale independent
 *
 * Revision 1.5  2002/11/27 12:06:51  meichel
 * Adapted module dcmdata to use of new header file ofstdinc.h
 *
 * Revision 1.4  2002/08/27 16:55:56  meichel
 * Initial release of new DICOM I/O stream classes that add support for stream
 *   compression (deflated little endian explicit VR transfer syntax)
 *
 * Revision 1.3  2002/07/18 12:15:40  joergr
 * Added explicit type casts to keep Sun CC 2.0.1 quiet.
 *
 * Revision 1.2  2002/06/27 15:15:43  meichel
 * Modified RLE encoder to make it usable for other purposes than
 *   DICOM encoding as well (e.g. PostScript, TIFF)
 *
 * Revision 1.1  2002/06/06 14:52:41  meichel
 * Initial release of the new RLE codec classes
 *   and the dcmcrle/dcmdrle tools in module dcmdata
 *
 *
 */
