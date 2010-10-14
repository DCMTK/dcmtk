/*
 *
 *  Copyright (C) 1994-2010, OFFIS e.V.
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
 *  Purpose: singleton class that registers RLE decoder.
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:09 $
 *  CVS/RCS Revision: $Revision: 1.5 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dcrledrg.h"

#include "dcmtk/dcmdata/dccodec.h"  /* for DcmCodecStruct */
#include "dcmtk/dcmdata/dcrleccd.h" /* for class DcmRLECodecDecoder */
#include "dcmtk/dcmdata/dcrlecp.h"  /* for class DcmRLECodecParameter */

// initialization of static members
OFBool DcmRLEDecoderRegistration::registered                  = OFFalse;
DcmRLECodecParameter *DcmRLEDecoderRegistration::cp           = NULL;
DcmRLECodecDecoder *DcmRLEDecoderRegistration::codec          = NULL;

void DcmRLEDecoderRegistration::registerCodecs(
    OFBool pCreateSOPInstanceUID,
    OFBool pReverseDecompressionByteOrder)
{
  if (! registered)
  {
    cp = new DcmRLECodecParameter(
      pCreateSOPInstanceUID,
      0, OFTrue, OFFalse,
      pReverseDecompressionByteOrder);
      
    if (cp)
    {
      codec = new DcmRLECodecDecoder();
      if (codec) DcmCodecList::registerCodec(codec, NULL, cp);
      registered = OFTrue;
    }
  }
}

void DcmRLEDecoderRegistration::cleanup()
{
  if (registered)
  {
    DcmCodecList::deregisterCodec(codec);
    delete codec;
    delete cp;
    registered = OFFalse;
#ifdef DEBUG
    // not needed but useful for debugging purposes
    codec  = NULL;
    cp     = NULL;
#endif
  }
}


/*
 * CVS/RCS Log
 * $Log: dcrledrg.cc,v $
 * Revision 1.5  2010-10-14 13:14:09  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.4  2009-11-04 09:58:10  uli
 * Switched to logging mechanism provided by the "new" oflog module
 *
 * Revision 1.3  2005-12-08 15:41:33  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.2  2005/07/26 17:08:35  meichel
 * Added option to RLE decoder that allows to correctly decode images with
 *   incorrect byte order of byte segments (LSB instead of MSB).
 *
 * Revision 1.1  2002/06/06 14:52:42  meichel
 * Initial release of the new RLE codec classes
 *   and the dcmcrle/dcmdrle tools in module dcmdata
 *
 *
 */
