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
 *  Update Date:      $Date: 2010-10-14 13:15:42 $
 *  CVS/RCS Revision: $Revision: 1.7 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCRLEERG_H
#define DCRLEERG_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/oftypes.h"  /* for OFBool */
#include "dcmtk/ofstd/oftypes.h"  /* for Uint32 */

class DcmRLECodecParameter;
class DcmRLECodecEncoder;

/** singleton class that registers an RLE encoder.
 */
class DcmRLEEncoderRegistration 
{
public: 

  /** registers RLE encoder.
   *  If already registered, call is ignored unless cleanup() has
   *  been performed before.
   *  @param pCreateSOPInstanceUID flag indicating whether or not
   *    a new SOP Instance UID should be assigned upon compression.
   *  @param pFragmentSize maximum fragment size (in kbytes) for compression, 0 for unlimited.
   *  @param pCreateOffsetTable create offset table during image compression?
   *  @param pConvertToSC flag indicating whether image should be converted to 
   *    Secondary Capture upon compression
   */
  static void registerCodecs(
    OFBool pCreateSOPInstanceUID = OFFalse,
    Uint32 pFragmentSize = 0,
    OFBool pCreateOffsetTable = OFTrue,
    OFBool pConvertToSC = OFFalse);

  /** deregisters encoder.
   *  Attention: Must not be called while other threads might still use
   *  the registered codecs, e.g. because they are currently encoding
   *  DICOM data sets through dcmdata.
   */  
  static void cleanup();

private:

  /// private undefined copy constructor
  DcmRLEEncoderRegistration(const DcmRLEEncoderRegistration&);
  
  /// private undefined copy assignment operator
  DcmRLEEncoderRegistration& operator=(const DcmRLEEncoderRegistration&);

  /// flag indicating whether the encoder is already registered.
  static OFBool registered;

  /// pointer to codec parameter
  static DcmRLECodecParameter *cp;
  
  /// pointer to RLE encoder
  static DcmRLECodecEncoder *codec;

  // dummy friend declaration to prevent gcc from complaining
  // that this class only defines private constructors and has no friends.
  friend class DcmRLEEncoderRegistrationDummyFriend;
  
};

#endif

/*
 * CVS/RCS Log
 * $Log: dcrleerg.h,v $
 * Revision 1.7  2010-10-14 13:15:42  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.6  2010-03-01 09:08:44  uli
 * Removed some unnecessary include directives in the headers.
 *
 * Revision 1.5  2009-11-04 09:58:07  uli
 * Switched to logging mechanism provided by the "new" oflog module
 *
 * Revision 1.4  2005-12-08 16:28:39  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.3  2004/01/20 12:57:21  meichel
 * Added dummy friend class to avoid warning on certain gcc releases
 *
 * Revision 1.2  2003/03/21 13:06:46  meichel
 * Minor code purifications for warnings reported by MSVC in Level 4
 *
 * Revision 1.1  2002/06/06 14:52:37  meichel
 * Initial release of the new RLE codec classes
 *   and the dcmcrle/dcmdrle tools in module dcmdata
 *
 *
 */
