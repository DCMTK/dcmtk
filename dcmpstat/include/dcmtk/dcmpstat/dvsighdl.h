/*
 *
 *  Copyright (C) 2001-2010, OFFIS e.V.
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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:16:37 $
 *  CVS/RCS Revision: $Revision: 1.11 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DVSIGHDL_H
#define DVSIGHDL_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/dcmpstat/dvpstyp.h"
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
class DVSignatureHandler
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

/*
 *  $Log: dvsighdl.h,v $
 *  Revision 1.11  2010-10-14 13:16:37  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.10  2006/08/15 16:57:01  meichel
 *  Updated the code in module dcmpstat to correctly compile when
 *    all standard C++ classes remain in namespace std.
 *
 *  Revision 1.9  2005/12/08 16:04:15  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.8  2004/02/04 15:49:09  joergr
 *  Removed acknowledgements with e-mail addresses from CVS log. Removed leading
 *  underscore characters from preprocessor symbols (reserved symbols).
 *
 *  Revision 1.7  2002/04/16 14:02:04  joergr
 *  Added configurable support for C++ ANSI standard includes (e.g. streams).
 *
 *  Revision 1.6  2001/09/26 15:36:19  meichel
 *  Adapted dcmpstat to class OFCondition
 *
 *  Revision 1.5  2001/06/01 15:50:26  meichel
 *  Updated copyright header
 *
 *  Revision 1.4  2001/05/25 10:07:34  meichel
 *  Modified dcmpstat signature handler to also compile without OpenSSL
 *
 *  Revision 1.3  2001/01/29 14:55:43  meichel
 *  Added new methods for creating signatures and checking the signature
 *    status in module dcmpstat.
 *
 *  Revision 1.2  2001/01/25 17:37:43  meichel
 *  Fixed problem with undefined copy constructor
 *
 *  Revision 1.1  2001/01/25 15:18:05  meichel
 *  Added initial support for verification of digital signatures
 *    in presentation states, images and structured reports to module dcmpstat.
 *
 *
 */
