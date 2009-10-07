/*
 *
 *  Copyright (C) 1997-2007, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    OFFIS e.V.
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
 *  Module:  dcmjpls
 *
 *  Author:  Martin Willkomm
 *
 *  Purpose: singleton class that registers decoders for all supported JPEG-LS processes.
 *
 *  Last Update:      $Author: uli $
 *  Update Date:      $Date: 2009-10-07 13:16:47 $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmjpls/djdecode.h"
#include "dcmtk/dcmdata/dccodec.h"  /* for DcmCodecStruct */
#include "dcmtk/dcmjpls/djcparam.h"
#include "dcmtk/dcmjpls/djcodecd.h"

// initialization of static members
OFBool DJLSDecoderRegistration::registered_                            = OFFalse;
DJLSCodecParameter *DJLSDecoderRegistration::cp_                       = NULL;
DJLSLosslessDecoder *DJLSDecoderRegistration::losslessdecoder_         = NULL;
DJLSNearLosslessDecoder *DJLSDecoderRegistration::nearlosslessdecoder_ = NULL;

void DJLSDecoderRegistration::registerCodecs(
    JLS_UIDCreation uidcreation,
    JLS_PlanarConfiguration planarconfig,
    OFBool ignoreOffsetTable)
{
  if (! registered_)
  {
    cp_ = new DJLSCodecParameter(uidcreation, planarconfig, ignoreOffsetTable);
    if (cp_)
    {
      losslessdecoder_ = new DJLSLosslessDecoder();
      if (losslessdecoder_) DcmCodecList::registerCodec(losslessdecoder_, NULL, cp_);

      nearlosslessdecoder_ = new DJLSNearLosslessDecoder();
      if (nearlosslessdecoder_) DcmCodecList::registerCodec(nearlosslessdecoder_, NULL, cp_);
      registered_ = OFTrue;
    }
  }
}

void DJLSDecoderRegistration::cleanup()
{
  if (registered_)
  {
    DcmCodecList::deregisterCodec(losslessdecoder_);
    DcmCodecList::deregisterCodec(nearlosslessdecoder_);
    delete losslessdecoder_;
    delete nearlosslessdecoder_;
    delete cp_;
    registered_ = OFFalse;
#ifdef DEBUG
    // not needed but useful for debugging purposes
    losslessdecoder_ = NULL;
    nearlosslessdecoder_ = NULL;
    cp_      = NULL;
#endif
  }
}


/*
 * CVS/RCS Log:
 * $Log: djdecode.cc,v $
 * Revision 1.2  2009-10-07 13:16:47  uli
 * Switched to logging mechanism provided by the "new" oflog module.
 *
 * Revision 1.1  2009-07-29 14:46:47  meichel
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
