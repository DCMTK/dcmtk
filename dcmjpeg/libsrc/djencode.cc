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
 *  Author:  Marco Eichelberg
 *
 *  Purpose: singleton class that registers encoders for all supported JPEG processes.
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2001-11-19 15:13:32 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmjpeg/libsrc/djencode.cc,v $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"
#include "djencode.h"

#include "dccodec.h"  /* for DcmCodecStruct */
#include "djencbas.h" 
#include "djencext.h"
#include "djencsps.h"
#include "djencpro.h"
#include "djencsv1.h"
#include "djenclol.h"
#include "djcparam.h"

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
{
  if (! registered)
  {
    cp = new DJCodecParameter(
      pCompressionCSConversion,
      EDC_photometricInterpretation,  // not relevant, used for decompression only
      pCreateSOPInstanceUID,
      EPC_default, // not relevant, used for decompression only
      pVerbose,
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
      pUseModalityRescale);
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


/*
 * CVS/RCS Log
 * $Log: djencode.cc,v $
 * Revision 1.2  2001-11-19 15:13:32  meichel
 * Introduced verbose mode in module dcmjpeg. If enabled, warning
 *   messages from the IJG library are printed on ofConsole, otherwise
 *   the library remains quiet.
 *
 * Revision 1.1  2001/11/13 15:58:32  meichel
 * Initial release of module dcmjpeg
 *
 *
 */
