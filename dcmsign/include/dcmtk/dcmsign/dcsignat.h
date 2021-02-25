/*
 *
 *  Copyright (C) 1998-2021, OFFIS e.V.
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

#ifdef WITH_OPENSSL

#include "dcmtk/dcmsign/sitypes.h"
#include "dcmtk/dcmdata/dcxfer.h"    /* for E_TransferSyntax */
#include "dcmtk/dcmsign/sipurpos.h"  /* for E_SignaturePurposeType */

class DcmAttributeTag;
class DcmDateTime;
class DcmItem;
class DcmSequenceOfItems;
class DcmStack;
class DcmTagKey;
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
 *  @remark this class is only available if DCMTK is compiled with
 *  OpenSSL support enabled.
 */
class DCMTK_DCMSIGN_EXPORT DcmSignature
{
public:
  /** initializes the dcmsign library including the underlying OpenSSL library.
   *  this method should be called by main() before any object of the dcmsign
   *  library is created or used.
   */
  static void initializeLibrary();

  /** cleans up the dcmsign library including the underlying OpenSSL library.
   *  this method should be called by main() before program end to avoid memory leaks.
   */
  static void cleanupLibrary();

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
   *  @param sigPurpose digital signature purpose
   *  @return status code
   */
  OFCondition createSignature(
    SiPrivateKey& key,
    SiCertificate& cert,
    SiMAC& mac,
    SiSecurityProfile& profile,
    E_TransferSyntax xfer=EXS_LittleEndianExplicit,
    const DcmAttributeTag *tagList=NULL,
    SiTimeStamp *timeStamp=NULL,
    SiSignaturePurpose::E_SignaturePurposeType sigPurpose=SiSignaturePurpose::ESP_none);

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

  /** returns a pointer to the object representing Signature Date/Time
   *  of the current signature.
   *  Current signature must be selected with selectSignature().
   *  @return pointer to signature datetime if present, NULL otherwise
   */
  DcmDateTime *getCurrentSignatureDateTime();

  /** returns the Data Elements Signed attribute of the current signature if present.
   *  Current signature must be selected with selectSignature().
   *  If a valid signature is selected but the signature does not contain
   *  the Data Elements Signed element (i.e. all attributes are signed), this method
   *  returns an error code.
   *  @param desig data elements signed returned in this parameter upon success
   *  @return status code
   */
  OFCondition getCurrentDataElementsSigned(DcmAttributeTag& desig);

  /** returns the signature purpose code of the current signature if present.
   *  Current signature must be selected with selectSignature().
   *  If a valid signature is selected but the signature does not contain
   *  a valid SignaturePurposeCodeSequence, this method returns an error code.
   *  @param codeValue signature purpose code value returned in this parameter upon success
   *  @param codeMeaning signature purpose code meaning returned in this parameter upon success
   *  @param codingSchemeDesignator signature purpose coding scheme designator returned in this parameter upon success
   *  @return status code
   */
  OFCondition getCurrentSignaturePurpose(OFString& codeValue, OFString& codeMeaning, OFString& codingSchemeDesignator);

  /** verifies whether the currently selected signature within the
   *  currently attached dataset matches the requirements of the
   *  given signature profile.
   *  @param sprof security profile
   *  @return EC_Normal if signature matches, an error code otherwise
   */
  OFCondition verifySignatureProfile(SiSecurityProfile &sprof);

  /** returns the certificate of the current signature if present.
   *  Current signature must be selected with selectSignature().
   *  May return NULL if certificate is unavailable.
   *  @return pointer to current certificate, NULL if unavailable.
   */
  SiCertificate *getCurrentCertificate();

  /** returns the certified timestamp of the current signature if present.
   *  Current signature must be selected with selectSignature().
   *  May return NULL if timestamp is unavailable.
   *  @return pointer to current timestamp, NULL if unavailable.
   */
  SiTimeStamp *getCurrentTimestamp();

  /** returns the item of the DigitalSignaturesSequence selected by the last call
   *  to selectSignature(), or NULL if no signature has been selected.
   *  @return pointer to current signature item, may be NULL
   */
  DcmItem *getSelectedSignatureItem();

  /** dump all data that is fed into the MAC algorithm into the given file,
   *  which must be opened and closed by caller.
   *  @param f pointer to file already opened for writing; may be NULL.
   */
  void setDumpFile(FILE *f);

  /** recursively browses through the given dataset and searches the first
   *  occurrence of the DigitalSignaturesSequence. If found, returns
   *  a pointer to the Item in which the sequence is contained.
   *  @param item dataset to be browsed
   *  @param stack search stack, must be passed to findNextSignatureItem() later on.
   *  @return pointer to Item containing a DigitalSignatureSequence if found, NULL otherwise.
   */
  static DcmItem *findFirstSignatureItem(DcmItem& item, DcmStack& stack);

  /** recursively browses through the given dataset and searches the next
   *  occurrence of the DigitalSignaturesSequence. If found, returns
   *  a pointer to the Item in which the sequence is contained.
   *  @param item dataset to be browsed
   *  @param stack search stack as returned by findFirstSignatureItem() or the last call to this method.
   *  @return pointer to Item containing a DigitalSignatureSequence if found, NULL otherwise.
   */
  static DcmItem *findNextSignatureItem(DcmItem& item, DcmStack& stack);

  /** check a DER encoded ASN.1 SEQUENCE structure for a possible pad byte
   *  and, if a pad byte is detected, remove it by decreasing buflen.
   *  This will work for SEQUENCEs with one byte and two byte encoding
   *  (i.e. max 64 kBytes).
   *  @param buf pointer to DER encoded ASN.1 data
   *  @param buflen length of buffer pointed to, in bytes. The variable is
   *    decreased by one if a pad byte is detected.
   */
  static void adjustASN1SequenceLength(const unsigned char *buf, unsigned long& buflen);

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
   *  its value if present
   *  @param item item to be searched
   *  @param macid MAC ID returned in this parameter upon success
   *  @return EC_Normal if successful, an error code otherwise
   */
  static OFCondition getMACIDnumber(DcmItem &item, Uint16& macid);

  /** checks if all tags from tagList are present in tagListOut,
   *  which is the list of attribute tags actually included in a signature,
   *  including tags added due to a signature profile, and without tags
   *  that were absent in the dataset.
   *  @param tagList list of attribute tags that should be present in the signature, may be NULL
   *  @param tagListOut list of attribute tags actually present in the signature, must not be NULL
   *  @return EC_Normal if check succeeds, an error code otherwise
   */
  static OFCondition checkListOfSignedTags(const DcmAttributeTag *tagList, const DcmAttributeTag *tagListOut);

  /** checks if the given tag key is present in tagList
   *  @param tag tag key
   *  @param tagList list of tag keys
   *  @return OFTrue of tag is present in tagList, OFFalse otherwise
   */
  static OFBool inTagList(const DcmTagKey &tag, const DcmAttributeTag& tagList);

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

  /// pointer to certified timestamp for currently selected signature item
  SiTimeStamp *selectedTimestamp;

};

#endif
#endif
