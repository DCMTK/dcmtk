/*
 *
 *  Copyright (C) 1994-2002, OFFIS
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
 *  Module:  dcmdata
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose: singleton class that registers RLE decoder.
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2002-06-06 14:52:42 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcrledrg.cc,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"
#include "dcrledrg.h"

#include "dccodec.h"  /* for DcmCodecStruct */
#include "dcrleccd.h" /* for class DcmRLECodecDecoder */
#include "dcrlecp.h"  /* for class DcmRLECodecParameter */

// initialization of static members
OFBool DcmRLEDecoderRegistration::registered                  = OFFalse;
DcmRLECodecParameter *DcmRLEDecoderRegistration::cp           = NULL;
DcmRLECodecDecoder *DcmRLEDecoderRegistration::codec          = NULL;

void DcmRLEDecoderRegistration::registerCodecs(
    OFBool pCreateSOPInstanceUID,
    OFBool pVerbose)
{
  if (! registered)
  {
    cp = new DcmRLECodecParameter(
      pVerbose,
      pCreateSOPInstanceUID);
      
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
 * Revision 1.1  2002-06-06 14:52:42  meichel
 * Initial release of the new RLE codec classes
 *   and the dcmcrle/dcmdrle tools in module dcmdata
 *
 *
 */
