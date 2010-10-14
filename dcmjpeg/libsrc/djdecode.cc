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
 *  Purpose: singleton class that registers decoders for all supported JPEG processes.
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:21 $
 *  CVS/RCS Revision: $Revision: 1.7 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmjpeg/djdecode.h"

#include "dcmtk/dcmdata/dccodec.h"  /* for DcmCodecStruct */
#include "dcmtk/dcmjpeg/djdecbas.h" 
#include "dcmtk/dcmjpeg/djdecext.h"
#include "dcmtk/dcmjpeg/djdecsps.h"
#include "dcmtk/dcmjpeg/djdecpro.h"
#include "dcmtk/dcmjpeg/djdecsv1.h"
#include "dcmtk/dcmjpeg/djdeclol.h"
#include "dcmtk/dcmjpeg/djcparam.h"

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
    E_PlanarConfiguration pPlanarConfiguration,
    OFBool predictor6WorkaroundEnable)
{
  if (! registered)
  {
    cp = new DJCodecParameter(
      ECC_lossyYCbCr, // ignored, compression only
      pDecompressionCSConversion, 
      pCreateSOPInstanceUID, 
      pPlanarConfiguration,
      predictor6WorkaroundEnable);
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

      registered = OFTrue;
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
 * Revision 1.7  2010-10-14 13:14:21  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.6  2009-10-07 12:44:33  uli
 * Switched to logging mechanism provided by the "new" oflog module.
 *
 * Revision 1.5  2006-03-29 15:58:52  meichel
 * Added support for decompressing images with 16 bits/pixel compressed with
 *   a faulty lossless JPEG encoder that produces integer overflows in predictor 6.
 *
 * Revision 1.4  2005/12/08 15:43:32  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.3  2001/12/04 17:10:20  meichel
 * Fixed codec registration: flag registered was never set to true
 *
 * Revision 1.2  2001/11/19 15:13:30  meichel
 * Introduced verbose mode in module dcmjpeg. If enabled, warning
 *   messages from the IJG library are printed on ofConsole, otherwise
 *   the library remains quiet.
 *
 * Revision 1.1  2001/11/13 15:58:26  meichel
 * Initial release of module dcmjpeg
 *
 *
 */
