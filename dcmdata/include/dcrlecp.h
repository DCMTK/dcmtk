/*
 *
 *  Copyright (C) 1994-2005, OFFIS
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
 *  Purpose: codec parameter for RLE
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2005-07-26 17:08:33 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcrlecp.h,v $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCRLECP_H
#define DCRLECP_H

#include "osconfig.h"
#include "dccodec.h" /* for DcmCodecParameter */

/** codec parameter for RLE codec
 */
class DcmRLECodecParameter: public DcmCodecParameter
{
public:

  /** constructor.
   *  @param pVerbose verbose mode flag
   *  @param pCreateSOPInstanceUID true if a new SOP instance UID should be assigned
   *    upon compression/decompression
   *  @param pFragmentSize maximum fragment size (in kbytes) for compression, 0 for unlimited.
   *  @param pCreateOffsetTable create offset table during image compression?
   *  @param pConvertToSC flag indicating whether image should be converted to 
   *    Secondary Capture upon compression
   *  @param pReverseDecompressionByteOrder flag indicating whether the byte order should
   *    be reversed upon decompression. Needed to correctly decode some incorrectly encoded
   *    images with more than one byte per sample.
   */
  DcmRLECodecParameter(
    OFBool pVerbose = OFFalse,
    OFBool pCreateSOPInstanceUID = OFFalse,
    Uint32 pFragmentSize = 0,
    OFBool pCreateOffsetTable = OFTrue,
    OFBool pConvertToSC = OFFalse,
    OFBool pReverseDecompressionByteOrder = OFFalse);

  /// copy constructor
  DcmRLECodecParameter(const DcmRLECodecParameter& arg);

  /// destructor
  virtual ~DcmRLECodecParameter();

  /** this methods creates a copy of type DcmCodecParameter *
   *  it must be overweritten in every subclass.
   *  @return copy of this object
   */
  virtual DcmCodecParameter *clone() const;

  /** returns the class name as string.
   *  can be used as poor man's RTTI replacement.
   */
  virtual const char *className() const;

  /** returns maximum fragment size (in kbytes) for compression, 0 for unlimited.
   *  @returnmaximum fragment size for compression
   */
  Uint32 getFragmentSize() const
  {
    return fragmentSize;
  }

  /** returns offset table creation flag
   *  @return offset table creation flag
   */
  OFBool getCreateOffsetTable() const
  {
    return createOffsetTable;
  }

  /** returns secondary capture conversion flag
   *  @return secondary capture conversion flag
   */
  OFBool getConvertToSC() const
  {
    return convertToSC;
  }

  /** returns mode for SOP Instance UID creation
   *  @return mode for SOP Instance UID creation
   */
  OFBool getUIDCreation() const
  {
    return createInstanceUID;
  }

  /** returns verbose mode flag
   *  @return verbose mode flag
   */
  OFBool isVerbose() const
  {
    return verboseMode;
  }

  /** returns reverse decompression byte order mode
   *  @return reverse decompression byte order mode
   */
  OFBool getReverseDecompressionByteOrder() const
  {
    return reverseDecompressionByteOrder;
  }


private:

  /// private undefined copy assignment operator
  DcmRLECodecParameter& operator=(const DcmRLECodecParameter&);

  /// maximum fragment size (in kbytes) for compression, 0 for unlimited.
  Uint32 fragmentSize;

  /// create offset table during image compression
  OFBool createOffsetTable;

  /// flag indicating whether image should be converted to Secondary Capture upon compression
  OFBool convertToSC;

  /// create new Instance UID during compression/decompression?
  OFBool createInstanceUID;

  /** enable reverse byte order of RLE segments during decompression, needed to
   *  decompress certain incorrectly encoded RLE images
   */
  OFBool reverseDecompressionByteOrder;
  
  /// verbose mode flag. If true, warning messages are printed to console
  OFBool verboseMode;
};


#endif

/*
 * CVS/RCS Log
 * $Log: dcrlecp.h,v $
 * Revision 1.2  2005-07-26 17:08:33  meichel
 * Added option to RLE decoder that allows to correctly decode images with
 *   incorrect byte order of byte segments (LSB instead of MSB).
 *
 * Revision 1.1  2002/06/06 14:52:35  meichel
 * Initial release of the new RLE codec classes
 *   and the dcmcrle/dcmdrle tools in module dcmdata
 *
 *
 */
