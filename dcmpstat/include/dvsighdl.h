/*
 *
 *  Copyright (C) 1998-2000, OFFIS
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
 *  Module: dcmpstat
 *
 *  Author: Marco Eichelberg
 *
 *  Purpose:
 *    classes: DVSignatureHandler
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2001-01-25 15:18:05 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef __DVSIGHDL_H__
#define __DVSIGHDL_H__

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dvpstyp.h"
#include "ofstring.h"
#include "sicertvf.h"

#include <iostream.h>

class DcmItem;
class DcmStack;
class DVConfiguration;

/** handler for all digital signature related functions in dcmpstat
 */  
class DVSignatureHandler
{
public:
  /** constructor
   *  @param cfg system configuration
   */
  DVSignatureHandler(DVConfiguration& cfg);
  
  /// copy constructor
  DVSignatureHandler(const DVSignatureHandler& copy);

  /** clone method.
   *  @return a pointer to a new DVSignatureHandler object containing
   *  a copy of this object.
   */
  DVSignatureHandler *clone() { return new DVSignatureHandler(*this); }

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

  /** returns the combined status flag for the current image and presentation state.
   *  @return digital signature status for image and presentation state
   */
  DVPSSignatureStatus getCombinedImagePStateSignatureStatus() const;
    
private:

  /// private undefined assignment operator
  DVSignatureHandler& operator=(const DVSignatureHandler&);

  /* print the location stack into the given stream.
   * It is assumed that the stack top is a DigitalSignatureSequence which is not printed
   * and that the stack bottom is the main dataset, which is also not printed.
   * @param stack search stack, as returned by DcmSignature::findFirstSignatureItem() etc.
   * @param os output stream
   */
  static void printSignatureItemPosition(DcmStack& stack, ostream& os);

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
  
  /// the certificate verifier
  SiCertificateVerifier certVerifier;  
};

#endif

/*
 *  $Log: dvsighdl.h,v $
 *  Revision 1.1  2001-01-25 15:18:05  meichel
 *  Added initial support for verification of digital signatures
 *    in presentation states, images and structured reports to module dcmpstat.
 *
 *
 */

