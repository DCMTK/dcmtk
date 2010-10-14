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
 *  Purpose: singleton class that registers RLE encoder.
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:09 $
 *  CVS/RCS Revision: $Revision: 1.4 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dcrleerg.h"

#include "dcmtk/dcmdata/dccodec.h"  /* for DcmCodecStruct */
#include "dcmtk/dcmdata/dcrlecce.h" /* for class DcmRLECodecEncoder */
#include "dcmtk/dcmdata/dcrlecp.h"  /* for class DcmRLECodecParameter */

// initialization of static members
OFBool DcmRLEEncoderRegistration::registered                  = OFFalse;
DcmRLECodecParameter *DcmRLEEncoderRegistration::cp           = NULL;
DcmRLECodecEncoder *DcmRLEEncoderRegistration::codec          = NULL;

void DcmRLEEncoderRegistration::registerCodecs(
    OFBool pCreateSOPInstanceUID,
    Uint32 pFragmentSize,
    OFBool pCreateOffsetTable,
    OFBool pConvertToSC)
{
  if (! registered)
  {
    cp = new DcmRLECodecParameter(
      pCreateSOPInstanceUID,
      pFragmentSize,
      pCreateOffsetTable,
      pConvertToSC);

    if (cp)
    {
      codec = new DcmRLECodecEncoder();
      if (codec) DcmCodecList::registerCodec(codec, NULL, cp);
      registered = OFTrue;
    }
  }
}

void DcmRLEEncoderRegistration::cleanup()
{
  if (registered)
  {
    DcmCodecList::deregisterCodec(codec);
    delete codec;
    delete cp;
    registered = OFFalse;
#ifdef DEBUG
    // not needed but useful for debugging purposes
    codec = NULL;
    cp     = NULL;
#endif

  }
}


/*
 * CVS/RCS Log
 * $Log: dcrleerg.cc,v $
 * Revision 1.4  2010-10-14 13:14:09  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.3  2009-11-04 09:58:10  uli
 * Switched to logging mechanism provided by the "new" oflog module
 *
 * Revision 1.2  2005-12-08 15:41:34  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.1  2002/06/06 14:52:42  meichel
 * Initial release of the new RLE codec classes
 *   and the dcmcrle/dcmdrle tools in module dcmdata
 *
 *
 */
