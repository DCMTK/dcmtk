/*
 *
 *  Copyright (C) 2007-2020, OFFIS e.V.
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
 *  Module:  dcmjpls
 *
 *  Author:  Martin Willkomm, Marco Eichelberg, Uli Schlachter
 *
 *  Purpose: codec classes for JPEG-LS encoders.
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmjpls/djcodece.h"

// ofstd includes
#include "dcmtk/ofstd/oflist.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/ofstd/offile.h"      /* for class OFFile */
#include "dcmtk/ofstd/ofbmanip.h"

#define INCLUDE_CMATH
#include "dcmtk/ofstd/ofstdinc.h"

// dcmdata includes
#include "dcmtk/dcmdata/dcdatset.h"  /* for class DcmDataset */
#include "dcmtk/dcmdata/dcdeftag.h"  /* for tag constants */
#include "dcmtk/dcmdata/dcovlay.h"   /* for class DcmOverlayData */
#include "dcmtk/dcmdata/dcpixseq.h"  /* for class DcmPixelSequence */
#include "dcmtk/dcmdata/dcpxitem.h"  /* for class DcmPixelItem */
#include "dcmtk/dcmdata/dcuid.h"     /* for dcmGenerateUniqueIdentifer()*/
#include "dcmtk/dcmdata/dcvrcs.h"    /* for class DcmCodeString */
#include "dcmtk/dcmdata/dcvrds.h"    /* for class DcmDecimalString */
#include "dcmtk/dcmdata/dcvrlt.h"    /* for class DcmLongText */
#include "dcmtk/dcmdata/dcvrst.h"    /* for class DcmShortText */
#include "dcmtk/dcmdata/dcvrus.h"    /* for class DcmUnsignedShort */
#include "dcmtk/dcmdata/dcswap.h"    /* for swapIfNecessary */

// dcmjpls includes
#include "dcmtk/dcmjpls/djcparam.h"  /* for class DJLSCodecParameter */
#include "dcmtk/dcmjpls/djrparam.h"  /* for class D2RepresentationParameter */
#include "djerror.h"                 /* for private class DJLSError */

// dcmimgle includes
#include "dcmtk/dcmimgle/dcmimage.h"  /* for class DicomImage */

// JPEG-LS library (CharLS) includes
#include "intrface.h"

BEGIN_EXTERN_C
#ifdef HAVE_FCNTL_H
#include <fcntl.h>       /* for O_RDONLY */
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>   /* required for sys/stat.h */
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>    /* for stat, fstat */
#endif
END_EXTERN_C


E_TransferSyntax DJLSLosslessEncoder::supportedTransferSyntax() const
{
  return EXS_JPEGLSLossless;
}

E_TransferSyntax DJLSNearLosslessEncoder::supportedTransferSyntax() const
{
  return EXS_JPEGLSLossy;
}

// --------------------------------------------------------------------------

DJLSEncoderBase::DJLSEncoderBase()
: DcmCodec()
{
}


DJLSEncoderBase::~DJLSEncoderBase()
{
}


OFBool DJLSEncoderBase::canChangeCoding(
    const E_TransferSyntax oldRepType,
    const E_TransferSyntax newRepType) const
{
  // this codec only handles conversion from uncompressed to JPEG-LS.
  DcmXfer oldRep(oldRepType);
  return (oldRep.isNotEncapsulated() && (newRepType == supportedTransferSyntax()));
}


OFCondition DJLSEncoderBase::decode(
    const DcmRepresentationParameter * /* fromRepParam */,
    DcmPixelSequence * /* pixSeq */,
    DcmPolymorphOBOW& /* uncompressedPixelData */,
    const DcmCodecParameter * /* cp */,
    const DcmStack& /* objStack */,
    OFBool& /* removeOldRep */ ) const
{
  // we are an encoder only
  return EC_IllegalCall;
}


OFCondition DJLSEncoderBase::decodeFrame(
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


OFCondition DJLSEncoderBase::encode(
    const E_TransferSyntax /* fromRepType */,
    const DcmRepresentationParameter * /* fromRepParam */,
    DcmPixelSequence * /* fromPixSeq */,
    const DcmRepresentationParameter * /* toRepParam */,
    DcmPixelSequence * & /* toPixSeq */,
    const DcmCodecParameter * /* cp */,
    DcmStack& /* objStack */,
    OFBool& /* removeOldRep */ ) const
{
  // we don't support re-coding for now.
  return EC_IllegalCall;
}

OFCondition DJLSEncoderBase::encode(
    const Uint16 * pixelData,
    const Uint32 length,
    const DcmRepresentationParameter * toRepParam,
    DcmPixelSequence * & pixSeq,
    const DcmCodecParameter *cp,
    DcmStack& objStack,
    OFBool& removeOldRep) const
{
  OFCondition result = EC_Normal;
  DJLSRepresentationParameter defRep;

  // this codec may modify the DICOM header such that the previous pixel
  // representation is not valid anymore. Indicate this to the caller
  // to trigger removal.
  removeOldRep = OFTrue;

  // retrieve pointer to dataset from parameter stack
  DcmStack localStack(objStack);
  (void)localStack.pop();  // pop pixel data element from stack
  DcmObject *dobject = localStack.pop(); // this is the item in which the pixel data is located
  if ((!dobject)||((dobject->ident()!= EVR_dataset) && (dobject->ident()!= EVR_item))) return EC_InvalidTag;
  DcmItem *dataset = OFstatic_cast(DcmItem *, dobject);

  // assume we can cast the codec and representation parameters to what we need
  const DJLSCodecParameter *djcp = OFreinterpret_cast(const DJLSCodecParameter *, cp);
  const DJLSRepresentationParameter *djrp = OFreinterpret_cast(const DJLSRepresentationParameter *, toRepParam);
  double compressionRatio = 0.0;

  if (!djrp)
    djrp = &defRep;

  if (supportedTransferSyntax() == EXS_JPEGLSLossless || djrp->useLosslessProcess())
  {
    if (djcp->cookedEncodingPreferred())
      result = losslessCookedEncode(pixelData, length, dataset, djrp, pixSeq, djcp, compressionRatio, 0);
      else result = losslessRawEncode(pixelData, length, dataset, djrp, pixSeq, djcp, compressionRatio);
  }
  else
  {
      // near-lossless mode always uses the "cooked" encoder since this one is guaranteed not to "mix"
      // overlays and pixel data in one cell subjected to lossy compression.
      result = losslessCookedEncode(pixelData, length, dataset, djrp, pixSeq, djcp, compressionRatio, djrp->getnearlosslessDeviation());
  }

  // the following operations do not affect the Image Pixel Module
  // but other modules such as SOP Common.  We only perform these
  // changes if we're on the main level of the dataset,
  // which should always identify itself as dataset, not as item.
  if (result.good() && dataset->ident() == EVR_dataset)
  {
    if (result.good())
    {
      if (supportedTransferSyntax() == EXS_JPEGLSLossless || djrp->useLosslessProcess())
      {
        // lossless process - create new UID if mode is EUC_always or if we're converting to Secondary Capture
        if (djcp->getConvertToSC() || (djcp->getUIDCreation() == EJLSUC_always))
          result = DcmCodec::newInstance(dataset, "DCM", "121320", "Uncompressed predecessor");
      }
      else
      {
        // lossy process - create new UID unless mode is EUC_never and we're not converting to Secondary Capture
        if (djcp->getConvertToSC() || (djcp->getUIDCreation() != EJLSUC_never))
          result = DcmCodec::newInstance(dataset, "DCM", "121320", "Uncompressed predecessor");

        // update image type
        if (result.good()) result = DcmCodec::updateImageType(dataset);

        // update derivation description
        if (result.good()) result = updateDerivationDescription(dataset, djrp, compressionRatio);

        // update lossy compression ratio
        if (result.good()) result = updateLossyCompressionRatio(dataset, compressionRatio);
      }
    }

    // convert to Secondary Capture if requested by user.
    // This method creates a new SOP class UID, so it should be executed
    // after the call to newInstance() which creates a Source Image Sequence.
    if (result.good() && djcp->getConvertToSC()) result = DcmCodec::convertToSecondaryCapture(dataset);
  }

  return result;
}


OFCondition DJLSEncoderBase::determineDecompressedColorModel(
    const DcmRepresentationParameter * /* fromParam */,
    DcmPixelSequence * /* fromPixSeq */,
    const DcmCodecParameter * /* cp */,
    DcmItem * /* dataset */,
    OFString & /* decompressedColorModel */) const
{
    return EC_IllegalCall;
}


OFCondition DJLSEncoderBase::adjustOverlays(
  DcmItem *dataset,
  DicomImage& image) const
{
  if (dataset == NULL) return EC_IllegalCall;

  unsigned int overlayCount = image.getOverlayCount();
  if (overlayCount > 0)
  {
    Uint16 group = 0;
    DcmStack stack;
    unsigned long bytesAllocated = 0;
    Uint8 *buffer = NULL;
    unsigned int width = 0;
    unsigned int height = 0;
    unsigned long frames = 0;
    DcmElement *elem = NULL;
    OFCondition result = EC_Normal;

    // adjust overlays (prior to grayscale compression)
    for (unsigned int i=0; i < overlayCount; i++)
    {
      // check if current overlay is embedded in pixel data
      group = OFstatic_cast(Uint16, image.getOverlayGroupNumber(i));
      stack.clear();
      if ((dataset->search(DcmTagKey(group, 0x3000), stack, ESM_fromHere, OFFalse)).bad())
      {
        // separate Overlay Data not found. Assume overlay is embedded.
        bytesAllocated = image.create6xxx3000OverlayData(buffer, i, width, height, frames);
        if (bytesAllocated > 0)
        {
          elem = new DcmOverlayData(DcmTagKey(group, 0x3000)); // DCM_OverlayData
          if (elem)
          {
            result = elem->putUint8Array(buffer, bytesAllocated);
            delete[] buffer;
            if (result.good())
            {
              dataset->insert(elem, OFTrue /*replaceOld*/);
              // DCM_OverlayBitsAllocated
              result = dataset->putAndInsertUint16(DcmTagKey(group, 0x0100), 1);
              // DCM_OverlayBitPosition
              if (result.good()) result = dataset->putAndInsertUint16(DcmTagKey(group, 0x0102), 0);
            }
            else
            {
              delete elem;
              return result;
            }
          }
          else
          {
            delete[] buffer;
            return EC_MemoryExhausted;
          }
        }
        else return EC_IllegalCall;
      }
    }
  }
  return EC_Normal;
}


OFCondition DJLSEncoderBase::updateLossyCompressionRatio(
  DcmItem *dataset,
  double ratio) const
{
  if (dataset == NULL) return EC_IllegalCall;

  // set Lossy Image Compression to "01" (see DICOM part 3, C.7.6.1.1.5)
  OFCondition result = dataset->putAndInsertString(DCM_LossyImageCompression, "01");
  if (result.bad()) return result;

  // set Lossy Image Compression Ratio
  OFString s;
  const char *oldRatio = NULL;
  if ((dataset->findAndGetString(DCM_LossyImageCompressionRatio, oldRatio)).good() && oldRatio)
  {
    s = oldRatio;
    s += "\\";
  }

  // append lossy compression ratio
  char buf[64];
  OFStandard::ftoa(buf, sizeof(buf), ratio, OFStandard::ftoa_uppercase, 0, 5);
  s += buf;

  result = dataset->putAndInsertString(DCM_LossyImageCompressionRatio, s.c_str());
  if (result.bad()) return result;

  // count VM of lossy image compression ratio
  size_t i;
  size_t s_vm = 0;
  size_t s_sz = s.size();
  for (i = 0; i < s_sz; ++i)
    if (s[i] == '\\') ++s_vm;

  // set Lossy Image Compression Method
  const char *oldMethod = NULL;
  OFString m;
  if ((dataset->findAndGetString(DCM_LossyImageCompressionMethod, oldMethod)).good() && oldMethod)
  {
    m = oldMethod;
    m += "\\";
  }

  // count VM of lossy image compression method
  size_t m_vm = 0;
  size_t m_sz = m.size();
  for (i = 0; i < m_sz; ++i)
    if (m[i] == '\\') ++m_vm;

  // make sure that VM of Compression Method is not smaller than  VM of Compression Ratio
  while (m_vm++ < s_vm) m += "\\";

  m += "ISO_14495_1";
  return dataset->putAndInsertString(DCM_LossyImageCompressionMethod, m.c_str());
}


OFCondition DJLSEncoderBase::updateDerivationDescription(
  DcmItem *dataset,
  const DJLSRepresentationParameter *djrp,
  double ratio) const
{
  OFString derivationDescription;
  char buf[64];

  derivationDescription =  "near lossless JPEG-LS compression, factor ";
  OFStandard::ftoa(buf, sizeof(buf), ratio, OFStandard::ftoa_uppercase, 0, 5);
  derivationDescription += buf;
  sprintf(buf, " (NEAR=%lu)", OFstatic_cast(unsigned long, djrp->getnearlosslessDeviation()));
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

  OFCondition result = dataset->putAndInsertString(DCM_DerivationDescription, derivationDescription.c_str());
  if (result.good()) result = DcmCodec::insertCodeSequence(dataset, DCM_DerivationCodeSequence, "DCM", "113040", "Lossy Compression");
  return result;
}


OFCondition DJLSEncoderBase::losslessRawEncode(
    const Uint16 *pixelData,
    const Uint32 length,
    DcmItem *dataset,
    const DJLSRepresentationParameter *djrp,
    DcmPixelSequence * & pixSeq,
    const DJLSCodecParameter *djcp,
    double& compressionRatio) const
{
  compressionRatio = 0.0; // initialize if something goes wrong

  // determine image properties
  Uint16 bitsAllocated = 0;
  Uint16 bitsStored = 0;
  Uint16 bytesAllocated = 0;
  Uint16 samplesPerPixel = 0;
  Uint16 planarConfiguration = 0;
  Uint16 columns = 0;
  Uint16 rows = 0;
  Sint32 numberOfFrames = 1;
  OFBool byteSwapped = OFFalse;  // true if we have byte-swapped the original pixel data
  OFString photometricInterpretation;

  OFCondition result = dataset->findAndGetUint16(DCM_BitsAllocated, bitsAllocated);
  if (result.good()) result = dataset->findAndGetUint16(DCM_BitsStored, bitsStored);
  if (result.good()) result = dataset->findAndGetUint16(DCM_SamplesPerPixel, samplesPerPixel);
  if (result.good()) result = dataset->findAndGetUint16(DCM_Columns, columns);
  if (result.good()) result = dataset->findAndGetUint16(DCM_Rows, rows);
  if (result.good()) result = dataset->findAndGetOFString(DCM_PhotometricInterpretation, photometricInterpretation);
  if (result.good())
  {
    result = dataset->findAndGetSint32(DCM_NumberOfFrames, numberOfFrames);
    if (result.bad() || numberOfFrames < 1) numberOfFrames = 1;
    result = EC_Normal;
  }
  if (result.good() && (samplesPerPixel > 1))
  {
    result = dataset->findAndGetUint16(DCM_PlanarConfiguration, planarConfiguration);
  }

  if (result.good())
  {
    // check if bitsAllocated is 8 or 16 - we don't handle anything else
    if (bitsAllocated == 8)
    {
      bytesAllocated = 1;
    }
    else if (bitsAllocated == 16)
    {
      bytesAllocated = 2;
    }
    else
    {
      if (photometricInterpretation == "MONOCHROME1" ||
          photometricInterpretation == "MONOCHROME2" ||
          photometricInterpretation == "RGB" ||
          photometricInterpretation == "YBR_FULL")
      {
        // A bitsAllocated value that we don't handle, but a color model that indicates
        // that the cooked encoder could handle this case. Fall back to cooked encoder.
        return losslessCookedEncode(pixelData, length, dataset, djrp, pixSeq, djcp, compressionRatio, 0);
      }

      // an image that is not supported by either the raw or the cooked encoder.
      result = EC_JLSUnsupportedImageType;
    }

    // make sure that all the descriptive attributes have sensible values
    if ((columns < 1)||(rows < 1)||(samplesPerPixel < 1)) result = EC_JLSUnsupportedImageType;

    // make sure that we have at least as many bytes of pixel data as we expect
    if (bytesAllocated * samplesPerPixel * columns * rows *
      OFstatic_cast(unsigned long,numberOfFrames) > length)
      result = EC_JLSUncompressedBufferTooSmall;
  }

  DcmPixelSequence *pixelSequence = NULL;
  DcmPixelItem *offsetTable = NULL;

  // create initial pixel sequence
  if (result.good())
  {
    pixelSequence = new DcmPixelSequence(DCM_PixelSequenceTag);
    if (pixelSequence == NULL) result = EC_MemoryExhausted;
    else
    {
      // create empty offset table
      offsetTable = new DcmPixelItem(DCM_PixelItemTag);
      if (offsetTable == NULL) result = EC_MemoryExhausted;
      else pixelSequence->insert(offsetTable);
    }
  }

  DcmOffsetList offsetList;
  unsigned long compressedSize = 0;
  unsigned long compressedFrameSize = 0;
  double uncompressedSize = 0.0;

  // render and compress each frame
  if (result.good())
  {

    // byte swap pixel data to little endian if bits allocate is 8
    if ((gLocalByteOrder == EBO_BigEndian) && (bitsAllocated == 8))
    {
       swapIfNecessary(EBO_LittleEndian, gLocalByteOrder, OFstatic_cast(void *, OFconst_cast(Uint16 *, pixelData)), length, sizeof(Uint16));
       byteSwapped = OFTrue;
    }

    unsigned long frameCount = OFstatic_cast(unsigned long, numberOfFrames);
    unsigned long frameSize = columns * rows * samplesPerPixel * bytesAllocated;
    const Uint8 *framePointer = OFreinterpret_cast(const Uint8 *, pixelData);

    // compute original image size in bytes, ignoring any padding bits.
    uncompressedSize = columns * rows * samplesPerPixel * bitsStored * frameCount / 8.0;

    for (unsigned long i=0; (i<frameCount) && (result.good()); ++i)
    {
      // compress frame
      DCMJPLS_DEBUG("JPEG-LS encoder processes frame " << (i+1) << " of " << frameCount);
      result = compressRawFrame(framePointer, bitsAllocated, columns, rows,
          samplesPerPixel, planarConfiguration, photometricInterpretation,
          pixelSequence, offsetList, compressedFrameSize, djcp);

      compressedSize += compressedFrameSize;
      framePointer += frameSize;
    }
  }

  // store pixel sequence if everything went well.
  if (result.good()) pixSeq = pixelSequence;
  else
  {
    delete pixelSequence;
    pixSeq = NULL;
  }

  // create offset table
  if ((result.good()) && (djcp->getCreateOffsetTable()))
  {
    result = offsetTable->createOffsetTable(offsetList);
  }

  // adjust planar configuration
  if (result.good())
  {
    if (photometricInterpretation == "RGB" || photometricInterpretation == "YBR_FULL")
    {
      // CP 1843 requires a planar configuration value of 0 for these color models
      result = dataset->putAndInsertUint16(DCM_PlanarConfiguration, 0);
    }
    else if (samplesPerPixel == 1)
    {
      delete dataset->remove(DCM_PlanarConfiguration);
    }
  }

  if (compressedSize > 0) compressionRatio = uncompressedSize / compressedSize;

  // byte swap pixel data back to local endian if necessary
  if (byteSwapped)
  {
    swapIfNecessary(gLocalByteOrder, EBO_LittleEndian, OFstatic_cast(void *, OFconst_cast(Uint16 *, pixelData)), length, sizeof(Uint16));
  }

  return result;
}

// static helper functions for DJLSEncoderBase::setCustomParameters().
static long setcp_clamp(long i, long j, long MAXVAL)
{
    if (i > MAXVAL || i < j)
        return j;

    return i;
}

long setcp_min(long a, long b)
{
  return (((a) < (b)) ? (a) : (b));
}

void DJLSEncoderBase::setCustomParameters(
  JlsCustomParameters& custom,
  Uint16 bitsAllocated,
  Uint16 nearLosslessDeviation,
  const DJLSCodecParameter *djcp)
{
  // first check if all parameters are set to default (which will be the most common case).
  // In this case we will set everything in the custom struct to zero as well.
  if ((djcp->getT1() == 0) && (djcp->getT2() == 0) && (djcp->getT3() == 0) && (djcp->getReset() == 0))
  {
      custom.T1 = 0;
      custom.T2 = 0;
      custom.T3 = 0;
      custom.RESET = 0;
      custom.MAXVAL = 0;
      return;
  }

  // unfortunately, CharLS either takes all or none of the parameters
  // in the "custom" struct. So if we change any of them, we need to provide
  // legal values for all of them. The function in CharLS that computes these
  // values is not public, so we basically have to re-implement it here.

  const int BASIC_T1       = 3;
  const int BASIC_T2       = 7;
  const int BASIC_T3       = 21;
  const long BASIC_RESET   = 64;

  long MAXVAL = (1 << bitsAllocated) - 1;
  long FACTOR = (setcp_min(MAXVAL, 4095) + 128)/256;
  long NEAR = nearLosslessDeviation;

  custom.MAXVAL = MAXVAL;

  if (djcp->getT1() > 0) custom.T1 = djcp->getT1(); else
    custom.T1 = setcp_clamp(FACTOR * (BASIC_T1 - 2) + 2 + 3*NEAR, NEAR + 1, MAXVAL);

  if (djcp->getT2() > 0) custom.T2 = djcp->getT2(); else
    custom.T2 = setcp_clamp(FACTOR * (BASIC_T2 - 3) + 3 + 5*NEAR, custom.T1, MAXVAL);

  if (djcp->getT3() > 0) custom.T3 = djcp->getT3(); else
    custom.T3 = setcp_clamp(FACTOR * (BASIC_T3 - 4) + 4 + 7*NEAR, custom.T2, MAXVAL);

  if (djcp->getReset() > 0) custom.RESET = djcp->getReset();
    else custom.RESET = BASIC_RESET;

}

OFCondition DJLSEncoderBase::compressRawFrame(
  const Uint8 *framePointer,
  Uint16 bitsAllocated,
  Uint16 width,
  Uint16 height,
  Uint16 samplesPerPixel,
  Uint16 planarConfiguration,
  const OFString& /* photometricInterpretation */,
  DcmPixelSequence *pixelSequence,
  DcmOffsetList &offsetList,
  unsigned long &compressedSize,
  const DJLSCodecParameter *djcp) const
{
  OFCondition result = EC_Normal;
  Uint16 bytesAllocated = bitsAllocated / 8;
  Uint32 frameSize = width*height*bytesAllocated*samplesPerPixel;
  Uint32 fragmentSize = djcp->getFragmentSize();
  JlsParameters jls_params;
  Uint8 *frameBuffer = NULL;

  // Set up the information structure for CharLS
  OFBitmanipTemplate<char>::zeroMem((char *) &jls_params, sizeof(jls_params));
  jls_params.bitspersample = bitsAllocated;
  jls_params.height = height;
  jls_params.width = width;
  jls_params.allowedlossyerror = 0; // must be zero for raw mode
  jls_params.outputBgr = false;
  // No idea what this one does, but I don't think DICOM says anything about it
  jls_params.colorTransform = 0;
  // Unset: jls_params.jfif (thumbnail, dpi)

  // set parameters T1, T2, T3, MAXVAL and RESET.
  // compressRawFrame() is only used for true lossless mode, so the near-lossless deviation is always 0 here.
  setCustomParameters(jls_params.custom, bitsAllocated, 0, djcp);

  // Theoretically we could support any samplesPerPixel value, but for now we
  // only accept these (charls is a little picky for other values).
  if (samplesPerPixel == 1 || samplesPerPixel == 3)
    jls_params.components = samplesPerPixel;
  else
    return EC_IllegalCall;

  enum interleavemode ilv;
  switch (planarConfiguration)
  {
    // ILV_LINE is not supported by DICOM
    case 0:
      ilv = ILV_SAMPLE;
      break;
    case 1:
      ilv = ILV_NONE;
      break;
    default:
      return EC_IllegalCall;
  }

  switch (djcp->getJplsInterleaveMode())
  {
    case DJLSCodecParameter::interleaveSample:
      jls_params.ilv = ILV_SAMPLE;
      break;
    case DJLSCodecParameter::interleaveLine:
      jls_params.ilv = ILV_LINE;
      break;
#ifdef ENABLE_DCMJPLS_INTERLEAVE_NONE
    case DJLSCodecParameter::interleaveNone:
      jls_params.ilv = ILV_NONE;
      break;
#endif
    case DJLSCodecParameter::interleaveDefault:
    default:
      // In default mode we just never convert the image to another
      // interleave-mode. Instead, we use what is already there.
#ifdef ENABLE_DCMJPLS_INTERLEAVE_NONE
      jls_params.ilv = ilv;
#else
      jls_params.ilv = (ilv == ILV_NONE ? ILV_LINE : ilv);
#endif
      break;
  }

  // Special case: one component images are always ILV_NONE (Standard requires this)
  if (jls_params.components == 1)
  {
    jls_params.ilv = ILV_NONE;
    // Don't try to convert to another interleave mode, not necessary
    ilv = ILV_NONE;
  }

  // Do we have to convert the image to some other interleave mode?
  if ((jls_params.ilv == ILV_NONE && (ilv == ILV_SAMPLE || ilv == ILV_LINE)) ||
      (ilv == ILV_NONE && (jls_params.ilv == ILV_SAMPLE || jls_params.ilv == ILV_LINE)))
  {
    DCMJPLS_DEBUG("Converting image from " << (ilv == ILV_NONE ? "color-by-plane" : "color-by-pixel")
          << " to " << (jls_params.ilv == ILV_NONE ? "color-by-plane" : "color-by-pixel"));

    frameBuffer = new Uint8[frameSize];
    if (jls_params.ilv == ILV_NONE)
      result = convertToUninterleaved(frameBuffer, framePointer, samplesPerPixel, width, height, bitsAllocated);
    else
      /* For CharLS, sample-interleaved and line-interleaved is both expected to
       * be color-by-pixel.
       */
      result = convertToSampleInterleaved(frameBuffer, framePointer, samplesPerPixel, width, height, bitsAllocated);
    framePointer = frameBuffer;
  }

  if (result.good())
  {
    // The buffer is going to be dynamically reallocated if it's too small, so it doesn't matter that
    // much what initial size we use.
    size_t size = frameSize + 1024;
    BYTE *buffer = new BYTE[size];

    size_t bytesWritten = 0;

    JLS_ERROR err = JpegLsEncode(&buffer, &size, &bytesWritten, framePointer, frameSize, &jls_params);
    result = DJLSError::convert(err);

    if (result.good())
    {
      compressedSize = OFstatic_cast(unsigned long, bytesWritten);
      fixPaddingIfNecessary(OFstatic_cast(Uint8 *, buffer), size, compressedSize, djcp->getUseFFbitstreamPadding());
      result = pixelSequence->storeCompressedFrame(offsetList, buffer, compressedSize, fragmentSize);
    }

    delete[] buffer;
  }

  if (frameBuffer)
    delete[] frameBuffer;

  return result;
}


OFCondition DJLSEncoderBase::losslessCookedEncode(
    const Uint16 *pixelData,
    const Uint32 length,
    DcmItem *dataset,
    const DJLSRepresentationParameter *djrp,
    DcmPixelSequence * & pixSeq,
    const DJLSCodecParameter *djcp,
    double& compressionRatio,
    Uint16 nearLosslessDeviation) const
{
  compressionRatio = 0.0; // initialize if something goes wrong

  // determine a few image properties
  OFString photometricInterpretation;
  Uint16 bitsAllocated = 0;
  OFCondition result = dataset->findAndGetOFString(DCM_PhotometricInterpretation, photometricInterpretation);
  if (result.good()) result = dataset->findAndGetUint16(DCM_BitsAllocated, bitsAllocated);
  if (result.bad()) return result;

  // The cooked encoder only handles the following photometic interpretations
  if (photometricInterpretation != "MONOCHROME1" &&
      photometricInterpretation != "MONOCHROME2" &&
      photometricInterpretation != "RGB" &&
      photometricInterpretation != "YBR_FULL")
  {
    // a photometric interpretation that we don't handle. Fall back to raw encoder (unless in near-lossless mode)
     if (nearLosslessDeviation > 0) return EC_JLSUnsupportedPhotometricInterpretation;
     else return losslessRawEncode(pixelData, length, dataset, djrp, pixSeq, djcp, compressionRatio);
  }

  Uint16 pixelRepresentation = 0;
  result = dataset->findAndGetUint16(DCM_PixelRepresentation, pixelRepresentation);
  if (result.bad()) return result;

  if (pixelRepresentation > 0)
  {
    // in near lossless mode we cannot handle signed images because JPEG-LS assumes unsigned pixels
    if (nearLosslessDeviation > 0) return EC_JLSUnsupportedPixelRepresentation;

    // The cooked encoder only handles unsigned color images
    if (photometricInterpretation == "RGB" || photometricInterpretation == "YBR_FULL")
    {
        // prevent a loop - only call lossless raw encoder if bitsAllocated is OK for the raw encoder
        if ((bitsAllocated == 8) || (bitsAllocated == 16))
          return losslessRawEncode(pixelData, length, dataset, djrp, pixSeq, djcp, compressionRatio);
        else return EC_JLSUnsupportedPixelRepresentation;
    }
  }

  // Check if image is 2..16 bits/sample, bail out otherwise.
  // We check the value of BitsStored, which is not affected by any transformation such as MLUT.
  Uint16 bitsStored = 0;
  result = dataset->findAndGetUint16(DCM_BitsStored, bitsStored);
  if (result.bad()) return result;

  if (bitsStored > 16)
  {
    DCMJPLS_WARN("Cannot compress image with " << bitsStored << " bits/sample: JPEG-LS supports max. 16 bits.");
    return EC_JLSUnsupportedBitDepth;
  }
  if (bitsStored < 2)
  {
    DCMJPLS_WARN("Cannot compress image with " << bitsStored << " bit/sample: JPEG-LS requires at least 2 bits.");
    return EC_JLSUnsupportedBitDepth;
  }

  DcmPixelSequence *pixelSequence = NULL;
  DcmPixelItem *offsetTable = NULL;

  // ignore modality transformation (rescale slope/intercept or LUT) stored in the dataset
  unsigned long flags = CIF_IgnoreModalityTransformation;
  // don't convert YCbCr (Full and Full 4:2:2) color images to RGB
  flags |= CIF_KeepYCbCrColorModel;
  // Don't optimize memory usage, but keep using the same bitsAllocated.
  // Without this, the DICOM and the JPEG-LS value for bitsAllocated could
  // differ and the decoder would error out.
  flags |= CIF_UseAbsolutePixelRange;

  DicomImage *dimage = new DicomImage(dataset, EXS_LittleEndianImplicit, flags); // read all frames
  if (dimage == NULL) return EC_MemoryExhausted;
  if (dimage->getStatus() != EIS_Normal)
  {
    delete dimage;
    return EC_IllegalCall;
  }

  // create overlay data for embedded overlays
  result = adjustOverlays(dataset, *dimage);

  // determine number of bits per sample
  int bitsPerSample = dimage->getDepth();

  // create initial pixel sequence
  if (result.good())
  {
    pixelSequence = new DcmPixelSequence(DCM_PixelSequenceTag);
    if (pixelSequence == NULL) result = EC_MemoryExhausted;
    else
    {
      // create empty offset table
      offsetTable = new DcmPixelItem(DCM_PixelItemTag);
      if (offsetTable == NULL) result = EC_MemoryExhausted;
      else pixelSequence->insert(offsetTable);
    }
  }

  DcmOffsetList offsetList;
  unsigned long compressedSize = 0;
  unsigned long compressedFrameSize = 0;
  double uncompressedSize = 0.0;

  // render and compress each frame
  if (result.good())
  {
    unsigned long frameCount = dimage->getFrameCount();

    // compute original image size in bytes, ignoring any padding bits.
    Uint16 samplesPerPixel = 0;
    if ((dataset->findAndGetUint16(DCM_SamplesPerPixel, samplesPerPixel)).bad()) samplesPerPixel = 1;
    uncompressedSize = dimage->getWidth() * dimage->getHeight() *
      bitsPerSample * frameCount * samplesPerPixel / 8.0;

    for (unsigned long i=0; (i<frameCount) && (result.good()); ++i)
    {
      // compress frame
      DCMJPLS_DEBUG("JPEG-LS encoder processes frame " << (i+1) << " of " << frameCount);
      result = compressCookedFrame(pixelSequence, dimage,
          photometricInterpretation, offsetList, compressedFrameSize, djcp, i, nearLosslessDeviation);

      compressedSize += compressedFrameSize;
    }
  }

  // store pixel sequence if everything went well.
  if (result.good()) pixSeq = pixelSequence;
  else
  {
    delete pixelSequence;
    pixSeq = NULL;
  }

  // create offset table
  if ((result.good()) && (djcp->getCreateOffsetTable()))
  {
    result = offsetTable->createOffsetTable(offsetList);
  }

  // adapt attributes in image pixel module
  if (result.good())
  {
    // adjustments needed for both color and monochrome
    if (bitsPerSample > 8)
        result = dataset->putAndInsertUint16(DCM_BitsAllocated, 16);
      else
        result = dataset->putAndInsertUint16(DCM_BitsAllocated, 8);
    if (result.good()) result = dataset->putAndInsertUint16(DCM_BitsStored, bitsPerSample);
    if (result.good()) result = dataset->putAndInsertUint16(DCM_HighBit, bitsPerSample-1);
    if (result.good())
    {
      if (photometricInterpretation == "RGB" || photometricInterpretation == "YBR_FULL")
      {
        // CP 1843 requires a planar configuration value of 0 for these color models
        result = dataset->putAndInsertUint16(DCM_PlanarConfiguration, 0);
      }
      else
      {
        // this is monochrome since we have ruled out all other photometric interpretations
        // at the start of this method
        delete dataset->remove(DCM_PlanarConfiguration);
      }
    }
  }

  if (compressedSize > 0) compressionRatio = uncompressedSize / compressedSize;
  delete dimage;
  return result;
}


OFCondition DJLSEncoderBase::compressCookedFrame(
  DcmPixelSequence *pixelSequence,
  DicomImage *dimage,
  const OFString& /* photometricInterpretation */,
  DcmOffsetList &offsetList,
  unsigned long &compressedSize,
  const DJLSCodecParameter *djcp,
  Uint32 frame,
  Uint16 nearLosslessDeviation) const
{
  if (dimage == NULL) return EC_IllegalCall;

  // access essential image parameters
  int width = dimage->getWidth();
  int height = dimage->getHeight();
  int depth = dimage->getDepth();
  if ((depth < 1) || (depth > 16)) return EC_JLSUnsupportedBitDepth;

  Uint32 fragmentSize = djcp->getFragmentSize();

  const DiPixel *dinter = dimage->getInterData();
  if (dinter == NULL) return EC_IllegalCall;

  // There should be no other possibilities
  int samplesPerPixel = dinter->getPlanes();
  if (samplesPerPixel != 1 && samplesPerPixel != 3) return EC_IllegalCall;

  // get pointer to internal raw representation of image data
  const void *draw = dinter->getData();
  if (draw == NULL) return EC_IllegalCall;

  OFCondition result = EC_Normal;

  const void *planes[3] = {NULL, NULL, NULL};
  if (samplesPerPixel == 3)
  {
    // for color images, dinter->getData() returns a pointer to an array
    // of pointers pointing to the real plane data
    const void * const * draw_array = OFstatic_cast(const void * const *,draw);
    planes[0] = draw_array[0];
    planes[1] = draw_array[1];
    planes[2] = draw_array[2];
  }
  else
  {
    // for monochrome images, dinter->getData() directly returns a pointer
    // to the single monochrome plane.
    planes[0] = draw;
  }

  // This is the buffer with the uncompressed pixel data
  Uint8 *buffer;
  size_t buffer_size;

  Uint32 framesize = dimage->getWidth() * dimage->getHeight();
  switch(dinter->getRepresentation())
  {
    case EPR_Uint8:
    case EPR_Sint8:
      {
        // image representation is 8 bit signed or unsigned
        if (samplesPerPixel == 1)
        {
          const Uint8 *yv = OFreinterpret_cast(const Uint8 *, planes[0]) + framesize * frame;
          buffer_size = framesize;
          buffer = new Uint8[buffer_size];
          memcpy(buffer, yv, framesize);
        }
        else
        {
          const Uint8 *rv = OFreinterpret_cast(const Uint8 *, planes[0]) + framesize * frame;
          const Uint8 *gv = OFreinterpret_cast(const Uint8 *, planes[1]) + framesize * frame;
          const Uint8 *bv = OFreinterpret_cast(const Uint8 *, planes[2]) + framesize * frame;

          buffer_size = framesize * 3;
          buffer = new Uint8[buffer_size];

          size_t i = 0;
          for (int row=height; row; --row)
          {
            for (int col=width; col; --col)
            {
              buffer[i++] = *rv;
              buffer[i++] = *gv;
              buffer[i++] = *bv;

              rv++;
              gv++;
              bv++;
            }
          }
        }
      }
      break;
    case EPR_Uint16:
    case EPR_Sint16:
      {
        // image representation is 16 bit signed or unsigned
        if (samplesPerPixel == 1)
        {
          const Uint16 *yv = OFreinterpret_cast(const Uint16 *, planes[0]) + framesize * frame;
          buffer_size = framesize*sizeof(Uint16);
          buffer = new Uint8[buffer_size];
          memcpy(buffer, yv, buffer_size);
        }
        else
        {
          const Uint16 *rv = OFreinterpret_cast(const Uint16 *, planes[0]) + framesize * frame;
          const Uint16 *gv = OFreinterpret_cast(const Uint16 *, planes[1]) + framesize * frame;
          const Uint16 *bv = OFreinterpret_cast(const Uint16 *, planes[2]) + framesize * frame;

          buffer_size = framesize * 3;
          Uint16 *buffer16 = new Uint16[buffer_size];
          buffer = OFreinterpret_cast(Uint8 *, buffer16);

          // Convert to byte count
          buffer_size *= 2;

          size_t i = 0;
          for (int row=height; row; --row)
          {
            for (int col=width; col; --col)
            {
              buffer16[i++] = *rv;
              buffer16[i++] = *gv;
              buffer16[i++] = *bv;

              rv++;
              gv++;
              bv++;
            }
          }
        }
      }
      break;
    default:
      // we don't support images with > 16 bits/sample
      return EC_JLSUnsupportedBitDepth;
      break;
  }

  JlsParameters jls_params;

  // Set up the information structure for CharLS
  OFBitmanipTemplate<char>::zeroMem((char *) &jls_params, sizeof(jls_params));
  jls_params.height = height;
  jls_params.width = width;
  jls_params.allowedlossyerror = nearLosslessDeviation;
  jls_params.outputBgr = false;
  jls_params.bitspersample = depth;
  // No idea what this one does, but I don't think DICOM says anything about it
  jls_params.colorTransform = 0;

  // This was already checked for a sane value above
  jls_params.components = samplesPerPixel;

  // Unset: jls_params.jfif (thumbnail, dpi)

  // set parameters T1, T2, T3, MAXVAL and RESET
  setCustomParameters(jls_params.custom, depth, nearLosslessDeviation, djcp);

  switch (djcp->getJplsInterleaveMode())
  {
    case DJLSCodecParameter::interleaveSample:
      jls_params.ilv = ILV_SAMPLE;
      break;
    case DJLSCodecParameter::interleaveLine:
      jls_params.ilv = ILV_LINE;
      break;
#ifdef ENABLE_DCMJPLS_INTERLEAVE_NONE
    case DJLSCodecParameter::interleaveNone:
      jls_params.ilv = ILV_NONE;
      break;
#endif
    case DJLSCodecParameter::interleaveDefault:
    default:
      // Default for the cooked encoder is always ILV_LINE
      jls_params.ilv = ILV_LINE;
      break;
  }

  // Special case: one component images are always ILV_NONE (Standard requires this)
  if (jls_params.components == 1)
  {
    jls_params.ilv = ILV_NONE;
  }

  Uint8 *frameBuffer = NULL;
  Uint8 *framePointer = buffer;

#ifdef ENABLE_DCMJPLS_INTERLEAVE_NONE
  // Do we have to convert the image to color-by-plane now?
  if (jls_params.ilv == ILV_NONE && jls_params.components != 1)
  {
    DCMJPLS_DEBUG("Converting image from color-by-pixel to color-by-plane");

    frameBuffer = new Uint8[buffer_size];
    framePointer = frameBuffer;
    result = convertToUninterleaved(frameBuffer, buffer, samplesPerPixel, width, height, jls_params.bitspersample);
  }
#endif

  size_t compressed_buffer_size = buffer_size + 1024;
  BYTE *compressed_buffer = new BYTE[compressed_buffer_size];

  size_t bytesWritten = 0;

  JLS_ERROR err = JpegLsEncode(&compressed_buffer, &compressed_buffer_size, &bytesWritten, framePointer, buffer_size, &jls_params);
  result = DJLSError::convert(err);

  if (result.good())
  {
    // 'compressed_buffer_size' now contains the size of the compressed data in buffer
    compressedSize = OFstatic_cast(unsigned long, bytesWritten);
    fixPaddingIfNecessary(OFstatic_cast(Uint8 *, buffer), compressed_buffer_size, compressedSize, djcp->getUseFFbitstreamPadding());
    result = pixelSequence->storeCompressedFrame(offsetList, compressed_buffer, compressedSize, fragmentSize);
  }

  delete[] buffer;
  delete[] compressed_buffer;
  if (frameBuffer)
    delete[] frameBuffer;

  return result;
}

OFCondition DJLSEncoderBase::convertToUninterleaved(
    Uint8 *target,
    const Uint8 *source,
    Uint16 components,
    Uint32 width,
    Uint32 height,
    Uint16 bitsAllocated) const
{
  Uint8 bytesAllocated = bitsAllocated / 8;
  Uint32 planeSize = width * height * bytesAllocated;

  if (bitsAllocated % 8 != 0)
    return EC_IllegalCall;

  for (Uint32 pos = 0; pos < width * height; pos++)
  {
    for (int i = 0; i < components; i++)
    {
      memcpy(&target[i * planeSize + pos * bytesAllocated], source, bytesAllocated);
      source += bytesAllocated;
    }
  }
  return EC_Normal;
}

OFCondition DJLSEncoderBase::convertToSampleInterleaved(
    Uint8 *target,
    const Uint8 *source,
    Uint16 components,
    Uint32 width,
    Uint32 height,
    Uint16 bitsAllocated) const
{
  Uint8 bytesAllocated = bitsAllocated / 8;
  Uint32 planeSize = width * height * bytesAllocated;

  if (bitsAllocated % 8 != 0)
    return EC_IllegalCall;

  for (Uint32 pos = 0; pos < width * height; pos++)
  {
    for (int i = 0; i < components; i++)
    {
      memcpy(target, &source[i * planeSize + pos * bytesAllocated], bytesAllocated);
      target += bytesAllocated;
    }
  }
  return EC_Normal;
}

void DJLSEncoderBase::fixPaddingIfNecessary(
    Uint8 *buffer,
    size_t bufSize,
    unsigned long &bytesWritten,
    OFBool useFFpadding)
{
  // check if an odd number of bytes was written and the buffer
  // has space for the needed pad byte (which should in practice
  // always be the case because the buffer always has even length).
  if (buffer && ((bytesWritten % 2 )> 0) && (bufSize > bytesWritten))
  {
    // first write a zero pad byte after the end of the JPEG-LS bitstream
    buffer[bytesWritten++] = 0;

    // check if we are expected to use an extended EOI marker for padding
    if (useFFpadding)
    {
      // look for the EOI marker
      if ((bytesWritten > 2) && (buffer[bytesWritten-3] == 0xFF) && (buffer[bytesWritten-2] == 0xD9))
      {
        // we now have ff/d9/00 at the end of the JPEG bitstream,
        // i.e. an end of image (EOI) marker followed by a pad byte.
        // Replace this with ff/ff/d9, which is an "extended" EOI marker
        // ending on an even byte boundary.
        buffer[bytesWritten-2] = 0xFF;
        buffer[bytesWritten-1] = 0xD9;
      }
    }
  }
  return;
}
