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
 *  Module:  dcmjpls
 *
 *  Author:  Martin Willkomm, Uli Schlachter
 *
 *  Purpose: singleton class that registers encoders for all supported JPEG processes.
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:24 $
 *  CVS/RCS Revision: $Revision: 1.4 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmjpls/djencode.h"
#include "dcmtk/dcmdata/dccodec.h"  /* for DcmCodecStruct */
#include "dcmtk/dcmjpls/djcparam.h"
#include "dcmtk/dcmjpls/djcodece.h"

// initialization of static members
OFBool DJLSEncoderRegistration::registered_                             = OFFalse;
DJLSCodecParameter *DJLSEncoderRegistration::cp_                        = NULL;
DJLSLosslessEncoder *DJLSEncoderRegistration::losslessencoder_          = NULL;
DJLSNearLosslessEncoder *DJLSEncoderRegistration::nearlosslessencoder_  = NULL;


void DJLSEncoderRegistration::registerCodecs(
    OFBool jpls_optionsEnabled,
    Uint16 jpls_t1,
    Uint16 jpls_t2,
    Uint16 jpls_t3,
    Uint16 jpls_reset,
    Uint16 jpls_limit,
    OFBool preferCookedEncoding,
    Uint32 fragmentSize,
    OFBool createOffsetTable,
    JLS_UIDCreation uidCreation,
    OFBool convertToSC,
    DJLSCodecParameter::interleaveMode jplsInterleaveMode)
{
  if (! registered_)
  {
    cp_ = new DJLSCodecParameter(jpls_optionsEnabled, jpls_t1, jpls_t2, jpls_t3, jpls_reset,
      jpls_limit, preferCookedEncoding, fragmentSize, createOffsetTable, uidCreation, 
      convertToSC, EJLSPC_restore, OFFalse, jplsInterleaveMode);

    if (cp_)
    {
      losslessencoder_ = new DJLSLosslessEncoder();
      if (losslessencoder_) DcmCodecList::registerCodec(losslessencoder_, NULL, cp_);
      nearlosslessencoder_ = new DJLSNearLosslessEncoder();
      if (nearlosslessencoder_) DcmCodecList::registerCodec(nearlosslessencoder_, NULL, cp_);
      registered_ = OFTrue;
    }
  }
}

void DJLSEncoderRegistration::cleanup()
{
  if (registered_)
  {
    DcmCodecList::deregisterCodec(losslessencoder_);
    DcmCodecList::deregisterCodec(nearlosslessencoder_);
    delete losslessencoder_;
    delete nearlosslessencoder_;
    delete cp_;
    registered_ = OFFalse;
#ifdef DEBUG
    // not needed but useful for debugging purposes
    losslessencoder_ = NULL;
    nearlosslessencoder_ = NULL;
    cp_     = NULL;
#endif
  }
}

/*
 * CVS/RCS Log:
 * $Log: djencode.cc,v $
 * Revision 1.4  2010-10-14 13:14:24  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.3  2009-10-07 13:16:47  uli
 * Switched to logging mechanism provided by the "new" oflog module.
 *
 * Revision 1.2  2009-07-31 09:14:53  meichel
 * Added codec parameter and command line options that allow to control
 *   the interleave mode used in the JPEG-LS bitstream when compressing
 *   color images.
 *
 * Revision 1.1  2009-07-29 14:46:48  meichel
 * Initial release of module dcmjpls, a JPEG-LS codec for DCMTK based on CharLS
 *
 * Revision 1.5  2007-06-15 14:35:45  meichel
 * Renamed CMake project and include directory from dcmjpgls to dcmjpls
 *
 * Revision 1.4  2007/06/15 10:39:15  meichel
 * Completed implementation of decoder, which now correctly processes all
 *   of the NEMA JPEG-LS sample images, including fragmented frames.
 *
 * Revision 1.3  2007/06/14 12:36:14  meichel
 * Further code clean-up. Updated doxygen comments.
 *
 * Revision 1.2  2007/06/13 16:41:07  meichel
 * Code clean-up and removal of dead code
 *
 *
 */
