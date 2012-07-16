/*
 *
 *  Copyright (C) 2001-2012, OFFIS e.V.
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
 *  Module: dcmpstat
 *
 *  Author: Marco Eichelberg
 *
 *  Purpose:
 *    classes: DVSignatureHandler
 *
 */

#ifndef DVSIGHDL_H
#define DVSIGHDL_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmpstat/dvpstyp.h"
#include "dcmtk/dcmpstat/dpdefine.h"
#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/dcmsign/sicertvf.h"
#include "dcmtk/dcmdata/dcerror.h"



class DcmAttributeTag;
class DcmItem;
class DcmStack;
class DVConfiguration;
class DcmAttributeTag;
class DcmTagKey;

/** handler for all digital signature related functions in dcmpstat
 */  
class DCMTK_DCMPSTAT_EXPORT DVSignatureHandler
{
public:
  /** constructor
   *  @param cfg system configuration
   */
  DVSignatureHandler(DVConfiguration& cfg);

  /// destructor
  virtual ~DVSignatureHandler();

  /** returns a string containing a complete HTML page with the
   *  signature validation results for the last object of the 
   *  given type. Never returns NULL.
   *  @param objtype object type
   *  @return pointer to string with HTML page
   */
  const char *getCurrentSignatureValidationHTML(DVPSObjectType objtype) const;

  /** returns a string containing a complete HTML page with the
   *  signature validation overview. Never returns NULL.
   *  @return pointer to string with HTML page
   */
  const char *getCurrentSignatureValidationOverview() const;

  /** updates the digital signature information for the given object type.
   *  If compiled without WITH_OPENSSL, always reports that no signatures
   *  were found in the object.
   *  @param dataset dataset to be checked for digital signatures
   *  @param objtype object type of dataset
   *  @param onRead true if the dataset is just being read, false if it is being written
   */
  void updateDigitalSignatureInformation(DcmItem& dataset, DVPSObjectType objtype, OFBool onRead);

  /** disables the digital signature information for the given object type.
   *  @param objtype object type
   */
  void disableDigitalSignatureInformation(DVPSObjectType objtype);
  
  /** returns the status flag for the current object of given type.
   *  @param objtype object type
   *  @return digital signature status for object
   */
  DVPSSignatureStatus getCurrentSignatureStatus(DVPSObjectType objtype) const;

  /** returns number of correct signatures for given object type.
   *  @param objtype object type
   *  @return number of digital signatures
   */
  unsigned long getNumberOfCorrectSignatures(DVPSObjectType objtype) const;

  /** returns number of untrustworthy signatures for given object type.
   *  @param objtype object type
   *  @return number of digital signatures
   */
  unsigned long getNumberOfUntrustworthySignatures(DVPSObjectType objtype) const;

  /** returns number of corrupt signatures for given object type.
   *  @param objtype object type
   *  @return number of digital signatures
   */
  unsigned long getNumberOfCorruptSignatures(DVPSObjectType objtype) const;
  
  /** returns the combined status flag for the current image and presentation state.
   *  @return digital signature status for image and presentation state
   */
  DVPSSignatureStatus getCombinedImagePStateSignatureStatus() const;

  /** disables internal settings for image and presentation state.
   *  Called when a new SR object is loaded and the current
   *  image/presentation state are hidden consequently.
   */
  void disableImageAndPState();

  /** checks whether any attribute mentioned in the tag list is affected
   *  by digital signatures.  Returns true if either any of the attributes
   *  is signed by a digital signature on the dataset level, or if any
   *  of the attributes is a sequence that contains one or more signatures
   *  in its items.  Otherwise returns false.
   *  If compiled without WITH_OPENSSL, always returns false.
   *  @param item item or dataset to be tested
   *  @param tagList list of attributes (tags) to be looked up inside the dataset
   *  @return true if any of the given attributes is affected by a digital signature,
   *    false otherwise.
   */
  OFBool attributesSigned(DcmItem& item, DcmAttributeTag& tagList) const;

  /** adds one or more new digital signatures to the given dataset.
   *  If compiled without WITH_OPENSSL, always returns EC_IllegalCall.
   *  @param mainDataset reference to main dataset in which signature(s) are to be added
   *  @param itemStack stack of items within the main dataset that are to be signed
   *    separately.  If main dataset is to be signed, it must be included in this stack.
   *  @param attributesNotToSignInMainDataset list of attribute tags that should be
   *    omitted from the signature on the main dataset level
   *  @param usedID user ID in configuration file, must not be NULL
   *  @param passwd passwd password for private key, may be NULL
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition createSignature(
    DcmItem& mainDataset,
    const DcmStack& itemStack,
    DcmAttributeTag& attributesNotToSignInMainDataset,
    const char *userID,
    const char *passwd);
    
private:

  /// private undefined copy constructor
  DVSignatureHandler(const DVSignatureHandler& copy);

  /// private undefined assignment operator
  DVSignatureHandler& operator=(const DVSignatureHandler&);

  /* print the location stack into the given stream.
   * It is assumed that the stack top is a DigitalSignatureSequence which is not printed
   * and that the stack bottom is the main dataset, which is also not printed.
   * @param stack search stack, as returned by DcmSignature::findFirstSignatureItem() etc.
   * @param os output stream
   */
  static void printSignatureItemPosition(DcmStack& stack, STD_NAMESPACE ostream& os);

  /** replaces the current HTML page for the given object type by the new string.
   *  @param objtype object type
   *  @param str string, must not be NULL
   */
  void replaceString(DVPSObjectType objtype, const char *str);

  /** updates the overview HTML page.
   */
  void updateSignatureValidationOverview();

  /// current SR html page
  OFString htmlSR;

  /// current image html page
  OFString htmlImage;

  /// current presentation state html page
  OFString htmlPState;

  /// current overview
  OFString htmlOverview;

  /// number of correct signatures in current SR 
  unsigned long correctSignaturesSR;

  /// number of corrupt signatures in current SR 
  unsigned long corruptSignaturesSR;

  /// number of untrustworthy signatures in current SR 
  unsigned long untrustSignaturesSR;

  /// number of correct signatures in current Image 
  unsigned long correctSignaturesImage;

  /// number of corrupt signatures in current Image 
  unsigned long corruptSignaturesImage;

  /// number of untrustworthy signatures in current Image 
  unsigned long untrustSignaturesImage;

  /// number of correct signatures in current PState 
  unsigned long correctSignaturesPState;

  /// number of corrupt signatures in current PState 
  unsigned long corruptSignaturesPState;

  /// number of untrustworthy signatures in current PState 
  unsigned long untrustSignaturesPState;

#ifdef WITH_OPENSSL  
  /// the certificate verifier, available only if compiled with OpenSSL support
  SiCertificateVerifier certVerifier;  
#endif

  /// reference to object maintaining the system configuration
  DVConfiguration& config;

};

#endif
