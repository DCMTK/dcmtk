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
 *  Purpose: codec parameter class for JPEG-LS codecs
 *
 *  Last Update:      $Author: uli $
 *  Update Date:      $Date: 2010-11-03 12:32:06 $
 *  CVS/RCS Revision: $Revision: 1.5 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmjpls/djcparam.h"
#include "dcmtk/ofstd/ofstd.h"

DJLSCodecParameter::DJLSCodecParameter(
     OFBool jpls_optionsEnabled,
     Uint16 jpls_t1, // these are the defaults for 8bpp in lossless mode
     Uint16 jpls_t2,
     Uint16 jpls_t3,
     Uint16 jpls_reset,
     Uint16 jpls_limit,
     OFBool preferCookedEncoding,
     Uint32 fragmentSize,
     OFBool createOffsetTable,
     JLS_UIDCreation uidCreation,
     OFBool convertToSC,
     JLS_PlanarConfiguration planarConfiguration,
     OFBool ignoreOffsetTble,
     interleaveMode jplsInterleaveMode)
: DcmCodecParameter()
, jpls_optionsEnabled_(jpls_optionsEnabled)
, jpls_t1_(jpls_t1)
, jpls_t2_(jpls_t2)
, jpls_t3_(jpls_t3)
, jpls_reset_(jpls_reset)
, jpls_limit_(jpls_limit)
, fragmentSize_(fragmentSize)
, createOffsetTable_(createOffsetTable)
, preferCookedEncoding_(preferCookedEncoding)
, uidCreation_(uidCreation)
, convertToSC_(convertToSC)
, jplsInterleaveMode_(jplsInterleaveMode)
, planarConfiguration_(planarConfiguration)
, ignoreOffsetTable_(ignoreOffsetTble)
{
}


DJLSCodecParameter::DJLSCodecParameter(
    JLS_UIDCreation uidCreation,
    JLS_PlanarConfiguration planarConfiguration,
    OFBool ignoreOffsetTble)
: DcmCodecParameter()
, jpls_optionsEnabled_(OFFalse)
, jpls_t1_(3)
, jpls_t2_(7)
, jpls_t3_(21)
, jpls_reset_(64)
, jpls_limit_(0)
, fragmentSize_(0)
, createOffsetTable_(OFTrue)
, preferCookedEncoding_(OFTrue)
, uidCreation_(uidCreation)
, convertToSC_(OFFalse)
, jplsInterleaveMode_(interleaveDefault)
, planarConfiguration_(planarConfiguration)
, ignoreOffsetTable_(ignoreOffsetTble)
{
}

DJLSCodecParameter::DJLSCodecParameter(const DJLSCodecParameter& arg)
: DcmCodecParameter(arg)

, jpls_optionsEnabled_(arg.jpls_optionsEnabled_)
, jpls_t1_(arg.jpls_t1_)
, jpls_t2_(arg.jpls_t2_)
, jpls_t3_(arg.jpls_t3_)
, jpls_reset_(arg.jpls_reset_)
, jpls_limit_(arg.jpls_limit_)
, fragmentSize_(arg.fragmentSize_)
, createOffsetTable_(arg.createOffsetTable_)
, preferCookedEncoding_(arg.preferCookedEncoding_)
, uidCreation_(arg.uidCreation_)
, convertToSC_(arg.convertToSC_)
, planarConfiguration_(arg.planarConfiguration_)
, ignoreOffsetTable_(arg.ignoreOffsetTable_)
{
}

DJLSCodecParameter::~DJLSCodecParameter()
{
}

DcmCodecParameter *DJLSCodecParameter::clone() const
{
  return new DJLSCodecParameter(*this);
}

const char *DJLSCodecParameter::className() const
{
  return "DJLSCodecParameter";
}


/*
 * CVS/RCS Log:
 * $Log: djcparam.cc,v $
 * Revision 1.5  2010-11-03 12:32:06  uli
 * Fixed some more warnings by gcc with by additional flags.
 *
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
