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
 *  Purpose: singleton class that registers RLE encoder.
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2002-06-06 14:52:37 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcrleerg.h,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCRLEERG_H
#define DCRLEERG_H

#include "osconfig.h"
#include "oftypes.h"  /* for OFBool */
#include "dctypes.h"  /* for Uint32 */

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
   *  @param pVerbose verbose mode flag
   *  @param pFragmentSize maximum fragment size (in kbytes) for compression, 0 for unlimited.
   *  @param pCreateOffsetTable create offset table during image compression?
   *  @param pConvertToSC flag indicating whether image should be converted to 
   *    Secondary Capture upon compression
   */
  static void registerCodecs(
    OFBool pCreateSOPInstanceUID = OFFalse,
    OFBool pVerbose = OFFalse,
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

  /// flag indicating whether the encoder is already registered.
  static OFBool registered;

  /// pointer to codec parameter
  static DcmRLECodecParameter *cp;
  
  /// pointer to RLE encoder
  static DcmRLECodecEncoder *codec;
  
};

#endif

/*
 * CVS/RCS Log
 * $Log: dcrleerg.h,v $
 * Revision 1.1  2002-06-06 14:52:37  meichel
 * Initial release of the new RLE codec classes
 *   and the dcmcrle/dcmdrle tools in module dcmdata
 *
 *
 */
