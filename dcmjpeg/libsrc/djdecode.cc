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
 *  Purpose: singleton class that registers decoders for all supported JPEG processes.
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2001-11-13 15:58:26 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmjpeg/libsrc/djdecode.cc,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"
#include "djdecode.h"

#include "dccodec.h"  /* for DcmCodecStruct */
#include "djdecbas.h" 
#include "djdecext.h"
#include "djdecsps.h"
#include "djdecpro.h"
#include "djdecsv1.h"
#include "djdeclol.h"
#include "djcparam.h"

// initialization of static members
OFBool DJDecoderRegistration::registered                  = OFFalse;
DJCodecParameter *DJDecoderRegistration::cp               = NULL;
DJDecoderBaseline *DJDecoderRegistration::decbas          = NULL;
DJDecoderExtended *DJDecoderRegistration::decext          = NULL;
DJDecoderSpectralSelection *DJDecoderRegistration::decsps = NULL;
DJDecoderProgressive *DJDecoderRegistration::decpro       = NULL;
DJDecoderP14SV1 *DJDecoderRegistration::decsv1            = NULL;
DJDecoderLossless *DJDecoderRegistration::declol          = NULL;

void DJDecoderRegistration::registerCodecs(
    E_DecompressionColorSpaceConversion pDecompressionCSConversion,
    E_UIDCreation pCreateSOPInstanceUID,
    E_PlanarConfiguration pPlanarConfiguration)    
{
  if (! registered)
  {
    cp = new DJCodecParameter(
      ECC_lossyYCbCr, // ignored, compression only
      pDecompressionCSConversion, 
      pCreateSOPInstanceUID, 
      pPlanarConfiguration);
    if (cp)
    {
      // baseline JPEG
      decbas = new DJDecoderBaseline();
      if (decbas) DcmCodecList::registerCodec(decbas, NULL, cp);

      // extended JPEG
      decext = new DJDecoderExtended();
      if (decext) DcmCodecList::registerCodec(decext, NULL, cp);

      // spectral selection JPEG
      decsps = new DJDecoderSpectralSelection();
      if (decsps) DcmCodecList::registerCodec(decsps, NULL, cp);

      // progressive JPEG
      decpro = new DJDecoderProgressive();
      if (decpro) DcmCodecList::registerCodec(decpro, NULL, cp);

      // lossless SV1 JPEG
      decsv1 = new DJDecoderP14SV1();
      if (decsv1) DcmCodecList::registerCodec(decsv1, NULL, cp);

      // lossless JPEG
      declol = new DJDecoderLossless();
      if (declol) DcmCodecList::registerCodec(declol, NULL, cp);
    }
  }
}

void DJDecoderRegistration::cleanup()
{
  if (registered)
  {
    DcmCodecList::deregisterCodec(decbas);
    delete decbas;
    DcmCodecList::deregisterCodec(decext);
    delete decext;
    DcmCodecList::deregisterCodec(decsps);
    delete decsps;
    DcmCodecList::deregisterCodec(decpro);
    delete decpro;
    DcmCodecList::deregisterCodec(decsv1);
    delete decsv1;
    DcmCodecList::deregisterCodec(declol);
    delete declol;
    delete cp;
    registered = OFFalse;
#ifdef DEBUG
    // not needed but useful for debugging purposes
    decbas = NULL;
    decext = NULL;
    decsps = NULL;
    decpro = NULL;
    decsv1 = NULL;
    declol = NULL;
    cp     = NULL;
#endif

  }
}


/*
 * CVS/RCS Log
 * $Log: djdecode.cc,v $
 * Revision 1.1  2001-11-13 15:58:26  meichel
 * Initial release of module dcmjpeg
 *
 *
 */
