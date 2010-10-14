/*
 *
 *  Copyright (C) 2002-2010, OFFIS e.V.
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
 *  Update Date:      $Date: 2010-10-14 13:15:42 $
 *  CVS/RCS Revision: $Revision: 1.8 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCRLEDRG_H
#define DCRLEDRG_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/oftypes.h"  /* for OFBool */

class DcmRLECodecParameter;
class DcmRLECodecDecoder;

/** singleton class that registers an RLE decoder.
 */
class DcmRLEDecoderRegistration 
{
public: 
  /** registers RLE decoder.
   *  If already registered, call is ignored unless cleanup() has
   *  been performed before.
   *  @param pCreateSOPInstanceUID flag indicating whether or not
   *    a new SOP Instance UID should be assigned upon decompression.
   *  @param pReverseDecompressionByteOrder flag indicating whether the byte order should
   *    be reversed upon decompression. Needed to correctly decode some incorrectly encoded
   *    images with more than one byte per sample.
   */   
  static void registerCodecs(
    OFBool pCreateSOPInstanceUID = OFFalse,
    OFBool pReverseDecompressionByteOrder = OFFalse);

  /** deregisters decoder.
   *  Attention: Must not be called while other threads might still use
   *  the registered codecs, e.g. because they are currently decoding
   *  DICOM data sets through dcmdata.
   */  
  static void cleanup();

private:

  /// private undefined copy constructor
  DcmRLEDecoderRegistration(const DcmRLEDecoderRegistration&);
  
  /// private undefined copy assignment operator
  DcmRLEDecoderRegistration& operator=(const DcmRLEDecoderRegistration&);

  /// flag indicating whether the decoder is already registered.
  static OFBool registered;

  /// pointer to codec parameter
  static DcmRLECodecParameter *cp;
  
  /// pointer to RLE decoder
  static DcmRLECodecDecoder *codec;

  // dummy friend declaration to prevent gcc from complaining
  // that this class only defines private constructors and has no friends.
  friend class DcmRLEDecoderRegistrationDummyFriend;

};

#endif

/*
 * CVS/RCS Log
 * $Log: dcrledrg.h,v $
 * Revision 1.8  2010-10-14 13:15:42  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.7  2009-11-04 09:58:07  uli
 * Switched to logging mechanism provided by the "new" oflog module
 *
 * Revision 1.6  2005-12-08 16:28:37  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.5  2005/07/26 17:08:33  meichel
 * Added option to RLE decoder that allows to correctly decode images with
 *   incorrect byte order of byte segments (LSB instead of MSB).
 *
 * Revision 1.4  2004/02/04 16:00:22  joergr
 * Added CVS log entry at the end of the file.
 *
 *
 */
