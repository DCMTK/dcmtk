/*
 *
 *  Copyright (C) 1997-2010, OFFIS e.V.
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
 *  Module:  dcmjpeg
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose: singleton class that registers encoders for all supported JPEG processes.
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmjpeg/djencode.h"

#include "dcmtk/dcmdata/dccodec.h"  /* for DcmCodecStruct */
#include "dcmtk/dcmjpeg/djencbas.h"
#include "dcmtk/dcmjpeg/djencext.h"
#include "dcmtk/dcmjpeg/djencsps.h"
#include "dcmtk/dcmjpeg/djencpro.h"
#include "dcmtk/dcmjpeg/djencsv1.h"
#include "dcmtk/dcmjpeg/djenclol.h"
#include "dcmtk/dcmjpeg/djcparam.h"

// initialization of static members
OFBool DJEncoderRegistration::registered                  = OFFalse;
DJCodecParameter *DJEncoderRegistration::cp               = NULL;
DJEncoderBaseline *DJEncoderRegistration::encbas          = NULL;
DJEncoderExtended *DJEncoderRegistration::encext          = NULL;
DJEncoderSpectralSelection *DJEncoderRegistration::encsps = NULL;
DJEncoderProgressive *DJEncoderRegistration::encpro       = NULL;
DJEncoderP14SV1 *DJEncoderRegistration::encsv1            = NULL;
DJEncoderLossless *DJEncoderRegistration::enclol          = NULL;

void DJEncoderRegistration::registerCodecs(
    E_CompressionColorSpaceConversion pCompressionCSConversion,
    E_UIDCreation pCreateSOPInstanceUID,
    OFBool pOptimizeHuffman,
    int pSmoothingFactor,
    int pForcedBitDepth,
    Uint32 pFragmentSize,
    OFBool pCreateOffsetTable,
    E_SubSampling pSampleFactors,
    OFBool pWriteYBR422,
    OFBool pConvertToSC,
    unsigned long pWindowType,
    unsigned long pWindowParameter,
    double pVoiCenter,
    double pVoiWidth,
    unsigned long pRoiLeft,
    unsigned long pRoiTop,
    unsigned long pRoiWidth,
    unsigned long pRoiHeight,
    OFBool pUsePixelValues,
    OFBool pUseModalityRescale,
    OFBool pAcceptWrongPaletteTags,
    OFBool pAcrNemaCompatibility,
    OFBool pRealLossless)
{
  if (! registered)
  {
    cp = new DJCodecParameter(
      pCompressionCSConversion,
      EDC_photometricInterpretation,  // not relevant, used for decompression only
      pCreateSOPInstanceUID,
      EPC_default, // not relevant, used for decompression only
      OFFalse, // not relevant, used for decompression only
      pOptimizeHuffman,
      pSmoothingFactor,
      pForcedBitDepth,
      pFragmentSize,
      pCreateOffsetTable,
      pSampleFactors,
      pWriteYBR422,
      pConvertToSC,
      pWindowType,
      pWindowParameter,
      pVoiCenter,
      pVoiWidth,
      pRoiLeft,
      pRoiTop,
      pRoiWidth,
      pRoiHeight,
      pUsePixelValues,
      pUseModalityRescale,
      pAcceptWrongPaletteTags,
      pAcrNemaCompatibility,
      pRealLossless);
    if (cp)
    {
      // baseline JPEG
      encbas = new DJEncoderBaseline();
      if (encbas) DcmCodecList::registerCodec(encbas, NULL, cp);

      // extended JPEG
      encext = new DJEncoderExtended();
      if (encext) DcmCodecList::registerCodec(encext, NULL, cp);

      // spectral selection JPEG
      encsps = new DJEncoderSpectralSelection();
      if (encsps) DcmCodecList::registerCodec(encsps, NULL, cp);

      // progressive JPEG
      encpro = new DJEncoderProgressive();
      if (encpro) DcmCodecList::registerCodec(encpro, NULL, cp);

      // lossless SV1 JPEG
      encsv1 = new DJEncoderP14SV1();
      if (encsv1) DcmCodecList::registerCodec(encsv1, NULL, cp);

      // lossless JPEG
      enclol = new DJEncoderLossless();
      if (enclol) DcmCodecList::registerCodec(enclol, NULL, cp);

      registered = OFTrue;
    }
  }
}

void DJEncoderRegistration::cleanup()
{
  if (registered)
  {
    DcmCodecList::deregisterCodec(encbas);
    delete encbas;
    DcmCodecList::deregisterCodec(encext);
    delete encext;
    DcmCodecList::deregisterCodec(encsps);
    delete encsps;
    DcmCodecList::deregisterCodec(encpro);
    delete encpro;
    DcmCodecList::deregisterCodec(encsv1);
    delete encsv1;
    DcmCodecList::deregisterCodec(enclol);
    delete enclol;
    delete cp;
    registered = OFFalse;
#ifdef DEBUG
    // not needed but useful for debugging purposes
    encbas = NULL;
    encext = NULL;
    encsps = NULL;
    encpro = NULL;
    encsv1 = NULL;
    enclol = NULL;
    cp     = NULL;
#endif

  }
}
