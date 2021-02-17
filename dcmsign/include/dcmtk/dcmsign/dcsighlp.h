/*
 *
 *  Copyright (C) 1998-2019, OFFIS e.V.
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
 *    classes: DcmSignatureHelper
 *
 */

#ifndef DCMSIGHLP_H
#define DCMSIGHLP_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/oftypes.h"     /* for Uint32 */
#include "dcmtk/ofstd/ofstring.h"    /* for class OFString */
#include "dcmtk/dcmdata/dcxfer.h"    /* for E_TransferSyntax */
#include "dcmtk/dcmsign/sidefine.h"  /* for DCMTK_DCMSIGN_EXPORT */
#include "dcmtk/dcmsign/sipurpos.h"  /* for class SiSignaturePurpose */
#include "dcmtk/dcmsign/sitypes.h"   /* for dcmsign enums */

#ifdef WITH_OPENSSL

class DcmAttributeTag;
class DcmItem;
class DcmStack;
class DcmTagKey;
class SiCertificate;
class SiMAC;
class SiPrivateKey;
class SiSecurityProfile;
class SiTimeStamp;
class SiTimeStampFS;
class SiCertificateVerifier;
class DcmSignature;

/** this class provides helper functions for creating and verifying
 *  digital signatures. It encapsulates most of the code that was part
 *  of the main command line program "dcmsign" in prior DCMTK releases.
 *  @remark this class is only available if DCMTK is compiled with
 *  OpenSSL support enabled.
 */
class DCMTK_DCMSIGN_EXPORT DcmSignatureHelper
{
public:

  /// default constructor
  DcmSignatureHelper();

  /// destructor
  virtual ~DcmSignatureHelper();

  /** locate a specific item within the given dataset.
   * @param dataset dataset to be searched
   * @param location location string. Format is "sequence[item]{.sequence[item]}*"
   *   Where sequence can be (gggg,eeee) or a dictionary name and items
   *   within sequences are counted from zero.
   * @return pointer to the item searched if found, NULL otherwise
   */
  static DcmItem *locateItemforSignatureCreation(DcmItem& dataset, const char *location);

  /** read a list of attributes from a text file.
   * The attributes can be in the form (gggg,eeee) or can be dictionary names,
   * separated by arbitrary whitespace.
   * @param filename file to be read from
   * @param tagList attribute tags are added to this list
   * @return 0 if successful, a program exit code otherwise
   */
  static int parseTextFile(const char *filename, DcmAttributeTag& tagList);

  /** read an attribute tag in the form "gggg,eeee" and adds it
   * to the given attribute tag list
   * @param c input string
   * @param tagList list to be added to
   * @return true if successful, false otherwise
   */
  static OFBool addTag(const char *c, DcmAttributeTag& tagList);

  /** print the location stack into the given stack.
   * It is assumed that the stack top is a DigitalSignatureSequence which is not printed
   * and that the stack bottom is the main dataset, which is also not printed.
   * @param stack search stack, as returned by DcmSignature::findFirstSignatureItem() etc.
   * @param str printable text returned in this string.
   */
  static void printSignatureItemPosition(DcmStack& stack, OFString& str);

  /** perform a signature operation on a given dataset
   * @param dataset to sign
   * @param key private key for signature
   * @param cert certificate for signature
   * @param opt_mac MAC for signature
   * @param opt_profile security profile for signature
   * @param opt_tagList list of attribute tags, may be NULL
   * @param opt_signatureXfer signature transfer syntax
   * @param dumpFile file to dump the byte stream to
   * @param opt_sigPurpose signature purpose
   * @param timeStamp pointer to timestamp client, may be NULL
   * @return 0 if successful, a program exit code otherwise
   */
  static int do_sign(
    DcmItem *dataset,
    SiPrivateKey& key,
    SiCertificate& cert,
    SiMAC *opt_mac,
    SiSecurityProfile *opt_profile,
    DcmAttributeTag *opt_tagList,
    E_TransferSyntax opt_signatureXfer,
    FILE *dumpFile,
    SiSignaturePurpose::E_SignaturePurposeType opt_sigPurpose,
    SiTimeStamp *timeStamp = NULL);

  /** performs a signature operation on a sub-item within a dataset
   * @param dataset in which to sign
   * @param key private key for signature
   * @param cert certificate for signature
   * @param opt_mac MAC for signature
   * @param opt_profile security profile for signature
   * @param opt_tagList list of attribute tags, may be NULL
   * @param location location string. Format is "sequence[item]{.sequence[item]}*"
   *   Where sequence can be (gggg,eeee) or a dictionary name and items
   *   within sequences are counted from zero.
   * @param opt_signatureXfer signature transfer syntax
   * @param dumpFile file to dump the byte stream to
   * @param opt_sigPurpose signature purpose
   * @param timeStamp pointer to timestamp client, may be NULL
   * @return 0 if successful, a program exit code otherwise
   */
  static int do_sign_item(
    DcmItem *dataset,
    SiPrivateKey& key,
    SiCertificate& cert,
    SiMAC *opt_mac,
    SiSecurityProfile *opt_profile,
    DcmAttributeTag *opt_tagList,
    const char *opt_location,
    E_TransferSyntax opt_signatureXfer,
    FILE *dumpFile,
    SiSignaturePurpose::E_SignaturePurposeType opt_sigPurpose,
    SiTimeStamp *timeStamp = NULL);

  /** verify all signatures in the given dataset and print results to stdout.
   * @param dataset dataset to verify
   * @param certVerifier certification verifier helper object
   * @param verificationPolicy signature verification policy
   * @param timstampPolicy timestamp verification policy
   * @return 0 if successful, a program exit code otherwise
   */
  static int do_verify(
    DcmItem *dataset,
    SiCertificateVerifier& certVerifier,
    E_SignatureVerificationPolicy verificationPolicy,
    E_TimestampVerificationPolicy timstampPolicy);

  /** insert certified timestamp from file.
   *  @param dataset in which to add timestamp
   *  @param timeStamp handler, must not be NULL
   */
  static int do_insert_ts(DcmItem *dataset, SiTimeStampFS *timeStamp);

  /** remove all signatures from the given dataset, print action details.
   * @param dataset dataset to modify
   * @return 0 if successful, a program exit code otherwise
   */
  static int do_remove_all(DcmItem *dataset);

  /** remove the signature with the given UID from the dataset, print action details.
   * @param dataset dataset to modify
   * @param opt_location Digital Signature UID of the signature to remove
   * @return 0 if successful, a program exit code otherwise
   */
  static int do_remove(
    DcmItem *dataset,
    const char *opt_location);

private:

  /** scans a token from the given string and returns it. Ignores leading whitespace.
   * @param c string to parse
   * @param pos position within string, modified after successful scan
   * @param key tag key returned in this parameter if return value is "tag key".
   * @param idx index returned in this parameter if return value is "index".
   * @return -1 for "EOF", 0 for "parse error", 1 for "tag key", 2 for "index", 3 for "period"
   */
  static int readNextToken(const char *c, int& pos, DcmTagKey& key, Uint32& idx);

  /** reads a complete text file (max 64K) into a memory block
   * and returns a pointer to the memory block.
   * memory must be freed by caller.
   * @param filename file to be read
   * @return pointer to memory block if successful, NULL otherwise.
   */
  static char *readTextFile(const char *filename);

  /** print the details of the current signature to the logger
   *  @param sig signature object
   *  @param stack position of the signature object in the dataset
   *  @param count number of the signature (counter)
   */
  static void printSignatureDetails(DcmSignature& sig, DcmStack& stack, int count);

  /** print the details of the timestamp for the current signature to the logger
   *  @param sig signature object
   *  @param tsPolicy timestamp verification policy
   */
  static void printTimestampDetails(DcmSignature& sig, E_TimestampVerificationPolicy tsPolicy);

};

#endif
#endif
