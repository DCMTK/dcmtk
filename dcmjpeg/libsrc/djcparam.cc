/*
 *
 *  Copyright (C) 1997-2001, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module:  dcmjpeg
 *
 *  Author:  Norbert Olges, Marco Eichelberg
 *
 *  Purpose: codec parameter class for dcmjpeg codecs
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2001-11-19 15:13:30 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmjpeg/libsrc/djcparam.cc,v $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"
#include "djcparam.h"

DJCodecParameter::DJCodecParameter(
    E_CompressionColorSpaceConversion pCompressionCSConversion,
    E_DecompressionColorSpaceConversion pDecompressionCSConversion,
    E_UIDCreation pCreateSOPInstanceUID,
    E_PlanarConfiguration pPlanarConfiguration,
    OFBool pVerbose,
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
    OFBool pUseModalityRescale)
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
, verboseMode(pVerbose)
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


/*
 * CVS/RCS Log
 * $Log: djcparam.cc,v $
 * Revision 1.2  2001-11-19 15:13:30  meichel
 * Introduced verbose mode in module dcmjpeg. If enabled, warning
 *   messages from the IJG library are printed on ofConsole, otherwise
 *   the library remains quiet.
 *
 * Revision 1.1  2001/11/13 15:58:25  meichel
 * Initial release of module dcmjpeg
 *
 *
 */
