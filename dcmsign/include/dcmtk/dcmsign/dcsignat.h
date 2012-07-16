/*
 *
 *  Copyright (C) 1998-2011, OFFIS e.V.
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
 *    classes: DcmSignature
 *
 */

#ifndef DCMSIGN_H
#define DCMSIGN_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmsign/sitypes.h"

#ifdef WITH_OPENSSL

#include "dcmtk/dcmdata/dcxfer.h"  /* for E_TransferSyntax */

#define INCLUDE_CSTDIO
#include "dcmtk/ofstd/ofstdinc.h"

class DcmItem;
class DcmStack;
class DcmSequenceOfItems;
class DcmAttributeTag;
class SiPrivateKey;
class SiCertificate;
class SiSecurityProfile;
class SiMAC;
class SiTimeStamp;

/** this class provides the main interface to the dcmsign module - it allows 
 *  to create, examine and verify digital signatures in DICOM datasets or 
 *  items. The methods in this class do not handle digital signatures 
 *  embedded in sequence items within the dataset, other than providing 
 *  helper functions that allow to locate and attach the sub-items 
 *  separately.
 */
class DCMTK_DCMSIGN_EXPORT DcmSignature
{
public:
  /** initializes the dcmsign library including the underlying OpenSSL library.
   *  this method should be called by main() before any object of the dcmsign
   *  library is created or used.
   */
  static void initializeLibrary();

  /// default constructor
  DcmSignature();
 
  /// destructor
  virtual ~DcmSignature();

  /** attaches a DICOM dataset or item to the signature object.
   *  The dataset is detached by a call to detach() or by destruction
   *  of the signature object.  This object may modify but never deletes 
   *  an attached dataset.
   *  @param dataset dataset or item to be attached
   */
  void attach(DcmItem *dataset);  
  
  /** detaches an attached DICOM dataset from the signature object.
   */
  void detach();

  /** creates a new digital signature in the current dataset.
   *  Checks whether private and public key match and whether
   *  all requirements of the given security profile are fulfilled.
   *  @param key private key for signature creation
   *  @param cert certificate with public key
   *  @param mac MAC algorithm to be used for signature creation
   *  @param profile security profile for signature creation
   *  @param xfer transfer syntax to use when serializing DICOM data
   *  @param tagList pointer to list of attribute tags to sign, may be NULL.
   *    If this parameter is nonzero, it contains a list of attribute sign.
   *    The real list of attributes signed is derived from this parameter plus the
   *    requirements of the security profile. If NULL, a universal match is assumed,
   *    i.e. all signable attributes in the data set are signed.
   *  @param timeStamp pointer to time stamp client used to create timestamps
   *    for the digital signature.
   *  @return status code
   */
  OFCondition createSignature(
    SiPrivateKey& key, 
    SiCertificate& cert, 
    SiMAC& mac,
    SiSecurityProfile& profile, 
    E_TransferSyntax xfer=EXS_LittleEndianExplicit,
    const DcmAttributeTag *tagList=NULL,
    SiTimeStamp *timeStamp=NULL);

  /** returns the number of signatures in the dataset. Does not count 
   *  signatures embedded in sequence items within the dataset.
   */
  unsigned long numberOfSignatures();

  /** removes a signature from the dataset.
   *  @param i index, must be < numberOfSignatures().
   *  @return status code
   */
  OFCondition removeSignature(unsigned long i);

  /** selects one of the digital signatures from the attached dataset for reading.
   *  @param i index, must be < numberOfSignatures()
   *  @return status code
   */
  OFCondition selectSignature(unsigned long i);
  
  /** verifies the current signature.
   *  Current signature must be selected with selectSignature().
   *  @return SI_EC_Normal if signature is complete and valid, an error code
   *    describing the type of verification failure otherwise.
   */
  OFCondition verifyCurrent();

  /** returns the MAC ID of the current signature.
   *  Current signature must be selected with selectSignature().
   *  @param macID MAC ID returned in this parameter upon success
   *  @return status code
   */
  OFCondition getCurrentMacID(Uint16& macID);

  /** returns the MAC Calculation Transfer Syntax of the current signature.
   *  If the transfer syntax is well-known, the UID is replaced by the
   *  transfer syntax name preceded by '='.
   *  Current signature must be selected with selectSignature().
   *  @param str transfer syntax name or UID returned in this parameter upon success
   *  @return status code
   */
  OFCondition getCurrentMacXferSyntaxName(OFString& str);

  /** returns the MAC Algorithm Name of the current signature.
   *  Current signature must be selected with selectSignature().
   *  @param str MAC algorithm name returned in this parameter upon success
   *  @return status code
   */
  OFCondition getCurrentMacName(OFString& str);

  /** returns the Digital Signature UID of the current signature.
   *  Current signature must be selected with selectSignature().
   *  @param str signature UID returned in this parameter upon success
   *  @return status code
   */
  OFCondition getCurrentSignatureUID(OFString& str);

  /** returns the Signature Date/Time of the current signature.
   *  Current signature must be selected with selectSignature().
   *  @param str signature date/time returned in this parameter upon success
   *  @return status code
   */
  OFCondition getCurrentSignatureDateTime(OFString& str);

  /** returns the Data Elements Signed attribute of the current signature if present.
   *  Current signature must be selected with selectSignature().
   *  If a valid signature is selected but the signature does not contain
   *  the Data Elements Signed element (i.e. all attributes are signed), this method
   *  returns an error code.
   *  @param desig data elements signed returned in this parameter upon success
   *  @return status code
   */
  OFCondition getCurrentDataElementsSigned(DcmAttributeTag& desig);

  /** returns the certificate of the current signature if present.
   *  Current signature must be selected with selectSignature().
   *  May return NULL if certificate is unavailable.
   *  @return pointer to current certificate, NULL if unavailable.
   */
  SiCertificate *getCurrentCertificate();
  
  /** dump all data that is fed into the MAC algorithm into the given file,
   *  which must be opened and closed by caller.
   *  @param f pointer to file already opened for writing; may be NULL.
   */
  void setDumpFile(FILE *f);

  /** recursively browses through the given dataset and searches the first
   *  occurence of the DigitalSignaturesSequence. If found, returns
   *  a pointer to the Item in which the sequence is contained.
   *  @param item dataset to be browsed
   *  @param stack search stack, must be passed to findNextSignatureItem() later on.
   *  @return pointer to Item containing a DigitalSignatureSequence if found, NULL otherwise.
   */
  static DcmItem *findFirstSignatureItem(DcmItem& item, DcmStack& stack);

  /** recursively browses through the given dataset and searches the next
   *  occurence of the DigitalSignaturesSequence. If found, returns
   *  a pointer to the Item in which the sequence is contained.
   *  @param item dataset to be browsed
   *  @param stack search stack as returned by findFirstSignatureItem() or the last call to this method.
   *  @return pointer to Item containing a DigitalSignatureSequence if found, NULL otherwise.
   */
  static DcmItem *findNextSignatureItem(DcmItem& item, DcmStack& stack);

private:

  /// private undefined copy constructor
  DcmSignature(DcmSignature& arg);

  /// private undefined copy assignment operator
  DcmSignature& operator=(DcmSignature& arg);

  /// removes the selection of a current signature if present
  void deselect();

  /** allocates a new mac ID number for a new signature.
   *  examines all mac ID numbers in the digital signatures sequence
   *  and in the mac parameters sequence and returns an unused number.
   *  @param newID upon successful return, new number is passed in this parameter
   *  @return status code
   */
  OFCondition allocateMACID(Uint16& newID);
  
  /** searches a given item for the DCM_MACIDnumber element and returns
   *  its value if present, otherwise returns 0.
   *  @param item item to be searched
   *  @return MAC ID number in item or zero if absent.
   */
  static Uint16 getMACIDnumber(DcmItem &item);

  /** returns the current date and time as a DICOM DT string.
   *  @param str date/time returned in this string.
   */
  static void currentDateTime(OFString &str);

  /// pointer to current item if attached, NULL otherwise
  DcmItem *currentItem;

  /// pointer to mac parameters sequence of attached item, may be NULL if not attached or not yet present
  DcmSequenceOfItems *macParametersSq;

  /// pointer to digital signatures sequence of attached item, may be NULL if not attached or not yet present
  DcmSequenceOfItems *signatureSq;

  /// if nonzero, the data fed to the MAC algorithm is also stored in this file.
  FILE *dumpFile;

  /// pointer to currently selected signature item
  DcmItem *selectedSignatureItem;

  /// pointer to currently selected mac parameters item
  DcmItem *selectedMacParametersItem;

  /// pointer to certificate for currently selected signature item
  SiCertificate *selectedCertificate;  
  
};

#endif
#endif
