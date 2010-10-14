/*
 *
 *  Copyright (C) 1998-2010, OFFIS e.V.
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
 *  Module: dcmsign
 *
 *  Author: Marco Eichelberg
 *
 *  Purpose:
 *    classes: SiMACConstructor
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:17:25 $
 *  CVS/RCS Revision: $Revision: 1.10 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef SIMACCON_H
#define SIMACCON_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmsign/sitypes.h"

#ifdef WITH_OPENSSL

#include "dcmtk/dcmdata/dcostrmb.h"  /* for DcmOutputBufferStream */
#include "dcmtk/dcmdata/dcxfer.h"    /* for E_TransferSyntax */
#include "dcmtk/dcmdata/dcdeftag.h"

#define INCLUDE_CSTDIO
#include "dcmtk/ofstd/ofstdinc.h"

class SiMAC;
class DcmItem;
class DcmElement;
class DcmAttributeTag;

/** a class that allows to feed selected parts of a DICOM dataset into the MAC generation code
 */
class SiMACConstructor
{
public:

  /// constructor
  SiMACConstructor();
  
  /// destructor
  virtual ~SiMACConstructor();

  /** encodes a DICOM dataset (or parts of it) as a byte stream in the format
   *  required for DICOM digital signatures and feeds the byte stream into
   *  the given MAC codec.
   *  If a dump file was set with setDumpFile(), the byte stream is written 
   *  to file as well.
   *  @param item the DICOM dataset to be encoded
   *  @param mac the MAC codec into which the resulting byte stream is fed
   *  @param oxfer the transfer syntax to be used when encoding the dataset.
   *     The caller might wish to use DcmItem::canWriteXfer() to check beforehand
   *     whether this transfer syntax can be used.
   *  @param tagListOut upon return this parameter contains the list of attribute
   *     tags which were fed into the MAC codec. For sequences, only the sequence
   *     attribute tag is contained in this list; the items and elements within the items
   *     are not encoded.
   *  @param tagListIn optional parameter restricting the parts of the dataset
   *     to be encoded. Only elements which are present in this list of tags, 
   *     which are signable (see DcmTagKey::isSignable()) and are present in the dataset
   *     are encoded.  Upon verification of a signature the caller might wish to compare
   *     tagListIn and tagListOut after successful return to see whether the lists of 
   *     attributes are the same.
   *     If parameter is absent or NULL, a global match is assumed, i.e. all elements
   *     of the dataset which are present and signable are encoded.
   *  @return status code
   */
  OFCondition encodeDataset(
    DcmItem& item, 
    SiMAC& mac, 
    E_TransferSyntax oxfer,
    DcmAttributeTag &tagListOut,
    DcmAttributeTag *tagListIn = NULL);

  /** encodes the contents of the digital signature sequence
   *  except CertificateOfSigner, Signature, CertifiedTimestampType
   *  and CertifiedTimestamp as a byte stream in the format
   *  required for DICOM digital signatures and feeds the byte stream into
   *  the given MAC codec.
   *  If a dump file was set with setDumpFile(), the byte stream is written 
   *  to file as well.
   *  @param signatureItem the DICOM digital signature item to be encoded
   *  @param mac the MAC codec into which the resulting byte stream is fed
   *  @param oxfer the transfer syntax to be used when encoding the dataset.
   *     The caller might wish to use DcmItem::canWriteXfer() to check beforehand
   *     whether this transfer syntax can be used.
   *  @return status code
   */
  OFCondition encodeDigitalSignatureItem(
    DcmItem& signatureItem, 
    SiMAC& mac, 
    E_TransferSyntax oxfer);

  /** flushes all buffers inside this object, finalizing the MAC code
   *  @param mac the MAC codec into which the resulting byte stream is fed
   *  @return status code
   */
  OFCondition flush(SiMAC& mac);

  /** dump all data that is fed into the MAC algorithm into the given file,
   *  which must be opened and closed by caller.
   *  @param f pointer to file already opened for writing; may be NULL.
   */
  void setDumpFile(FILE *f);

private:

  /// private undefined copy constructor
  SiMACConstructor(SiMACConstructor& arg);

  /// private undefined copy assignment operator
  SiMACConstructor& operator=(SiMACConstructor& arg);

  /** flushes the internal buffer to the given MAC and to dumpFile if open
   *  @param mac MAC to which the buffer content is added
   *  @return error code from MAC
   */
  OFCondition flushBuffer(SiMAC& mac);

  /** feeds a DcmElement into the MAC data stream if is signable. 
   *  If the element is a sequence, all signable elements from all items are added. 
   *  @param element pointer to element, must not be NULL
   *  @param mac MAC to use
   *  @param oxfer transfer syntax in which data is encoded
   *  @return status code
   */
  OFCondition encodeElement(DcmElement *element, SiMAC& mac, E_TransferSyntax oxfer);

  /** checks whether the attribute tag of the given DcmElement is contained
   *  in the given list of tags.  If the list is absent (NULL), a universal match
   *  is assumed, i.e. always returns true if element is nonzero.
   *  Does not check whether an element is signable.
   *  @param element pointer to element to check
   *  @param tagList pointer to list of attribute tags, may be NULL
   *  @return true if attribute is in tag list, false otherwise
   */
  static OFBool inTagList(const DcmElement *element, DcmAttributeTag *tagList);

  /// the buffer to which data is written
  unsigned char *buf;

  /// the internal buffer stream
  DcmOutputBufferStream stream;
  
  /** if nonzero, the data fed to the MAC algorithm
   *  is also stored in this file.
   */
  FILE *dumpFile;
};


#endif
#endif

/*
 *  $Log: simaccon.h,v $
 *  Revision 1.10  2010-10-14 13:17:25  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.9  2005-12-08 16:04:38  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.8  2003/06/04 14:21:03  meichel
 *  Simplified include structure to avoid preprocessor limitation
 *    (max 32 #if levels) on MSVC5 with STL.
 *
 *  Revision 1.7  2002/11/27 14:53:40  meichel
 *  Adapted module dcmsign to use of new header file ofstdinc.h
 *
 *  Revision 1.6  2002/08/27 17:21:00  meichel
 *  Initial release of new DICOM I/O stream classes that add support for stream
 *    compression (deflated little endian explicit VR transfer syntax)
 *
 *  Revision 1.5  2001/12/10 16:40:46  meichel
 *  Fixed warnings from Sun CC 4.2
 *
 *  Revision 1.4  2001/11/16 15:50:50  meichel
 *  Adapted digital signature code to final text of supplement 41.
 *
 *  Revision 1.3  2001/09/26 14:30:20  meichel
 *  Adapted dcmsign to class OFCondition
 *
 *  Revision 1.2  2001/06/01 15:50:49  meichel
 *  Updated copyright header
 *
 *  Revision 1.1  2000/11/07 16:48:55  meichel
 *  Initial release of dcmsign module for DICOM Digital Signatures
 *
 *
 */

