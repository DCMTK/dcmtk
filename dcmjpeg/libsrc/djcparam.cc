/*
 *
 *  Copyright (C) 1997-2014, OFFIS e.V.
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
 *  Author:  Norbert Olges, Marco Eichelberg
 *
 *  Purpose: codec parameter class for dcmjpeg codecs
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmjpeg/djcparam.h"

DJCodecParameter::DJCodecParameter(
    E_CompressionColorSpaceConversion pCompressionCSConversion,
    E_DecompressionColorSpaceConversion pDecompressionCSConversion,
    E_UIDCreation pCreateSOPInstanceUID,
    E_PlanarConfiguration pPlanarConfiguration,
    OFBool predictor6WorkaroundEnable,
    OFBool pOptimizeHuffman,
    int pSmoothingFactor,
    int pForcedBitDepth,
    Uint32 pFragmentSize,
    OFBool pCreateOffsetTable,
    E_SubSampling pSampleFactors,
    OFBool pWriteYBR422,
    OFBool pConvertToSC,
    size_t pWindowType,
    size_t pWindowParameter,
    double pVoiCenter,
    double pVoiWidth,
    size_t pRoiLeft,
    size_t pRoiTop,
    size_t pRoiWidth,
    size_t pRoiHeight,
    OFBool pUsePixelValues,
    OFBool pUseModalityRescale,
    OFBool pAcceptWrongPaletteTags,
    OFBool pAcrNemaCompatibility,
    OFBool pTrueLosslessMode)
: DcmCodecParameter()
, compressionCSConversion(pCompressionCSConversion)
, decompressionCSConversion(pDecompressionCSConversion)
, planarConfiguration(pPlanarConfiguration)
, optimizeHuffman(pOptimizeHuffman)
, smoothingFactor(pSmoothingFactor)
, forcedBitDepth(pForcedBitDepth)
, fragmentSize(pFragmentSize)
, createOffsetTable(pCreateOffsetTable)
, sampleFactors(pSampleFactors)
, writeYBR422(pWriteYBR422)
, convertToSC(pConvertToSC)
, uidCreation(pCreateSOPInstanceUID)
, windowType(pWindowType)
, windowParameter(pWindowParameter)
, voiCenter(pVoiCenter)
, voiWidth(pVoiWidth)
, roiLeft(pRoiLeft)
, roiTop(pRoiTop)
, roiWidth(pRoiWidth)
, roiHeight(pRoiHeight)
, usePixelValues(pUsePixelValues)
, useModalityRescale(pUseModalityRescale)
, acceptWrongPaletteTags(pAcceptWrongPaletteTags)
, acrNemaCompatibility(pAcrNemaCompatibility)
, trueLosslessMode(pTrueLosslessMode)
, predictor6WorkaroundEnabled_(predictor6WorkaroundEnable)
{
}


DJCodecParameter::DJCodecParameter(const DJCodecParameter& arg)
: DcmCodecParameter(arg)
, compressionCSConversion(arg.compressionCSConversion)
, decompressionCSConversion(arg.decompressionCSConversion)
, planarConfiguration(arg.planarConfiguration)
, optimizeHuffman(arg.optimizeHuffman)
, smoothingFactor(arg.smoothingFactor)
, forcedBitDepth(arg.forcedBitDepth)
, fragmentSize(arg.fragmentSize)
, createOffsetTable(arg.createOffsetTable)
, sampleFactors(arg.sampleFactors)
, writeYBR422(arg.writeYBR422)
, convertToSC(arg.convertToSC)
, uidCreation(arg.uidCreation)
, windowType(arg.windowType)
, windowParameter(arg.windowParameter)
, voiCenter(arg.voiCenter)
, voiWidth(arg.voiWidth)
, roiLeft(arg.roiLeft)
, roiTop(arg.roiTop)
, roiWidth(arg.roiWidth)
, roiHeight(arg.roiHeight)
, usePixelValues(arg.usePixelValues)
, useModalityRescale(arg.useModalityRescale)
, acceptWrongPaletteTags(arg.acceptWrongPaletteTags)
, acrNemaCompatibility(arg.acrNemaCompatibility)
, trueLosslessMode(arg.trueLosslessMode)
, predictor6WorkaroundEnabled_(arg.predictor6WorkaroundEnabled_)
{
}

DJCodecParameter::~DJCodecParameter()
{
}

DcmCodecParameter *DJCodecParameter::clone() const
{
  return new DJCodecParameter(*this);
}

const char *DJCodecParameter::className() const
{
  return "DJCodecParameter";
}
