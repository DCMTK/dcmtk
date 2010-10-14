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
 *  Module:  dcmnet
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose: 
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:17:22 $
 *  CVS/RCS Revision: $Revision: 1.4 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCASCCFG_H
#define DCASCCFG_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofcond.h"   /* for class OFCondition */
#include "dcmtk/dcmnet/assoc.h"    /* for T_ASC_Parameters */
#include "dcmtk/dcmnet/dccftsmp.h" /* for class DcmTransferSyntaxMap */
#include "dcmtk/dcmnet/dccfpcmp.h" /* for class DcmPresentationContextMap */
#include "dcmtk/dcmnet/dccfrsmp.h" /* for class DcmRoleSelectionMap */
#include "dcmtk/dcmnet/dccfenmp.h" /* for class DcmExtendedNegotiationMap */
#include "dcmtk/dcmnet/dccfprmp.h" /* for class DcmProfileMap */


/** this class maintains a list of association negotiation profiles
 *  that can be addressed by symbolic keys.
 */
class DcmAssociationConfiguration
{
public:
  /// default constructor
  DcmAssociationConfiguration();

  /// destructor
  ~DcmAssociationConfiguration();

  /** this method prepares a T_ASC_Parameters structure according to the settings
   *  of a profile maintained by this object. It is used by an association initiator.
   *  @param symbolic profile name, must not be NULL
   *  @param params T_ASC_Parameters structure to be filled
   *  @return EC_Normal if successful, an error code otherwise
   */
  OFCondition setAssociationParameters(
    const char *profile,
    T_ASC_Parameters& params) const;

  /** this method evaluates an incoming association request according to the settings
   *  of a profile maintained by this object. It is used by an association acceptor.
   *  @param symbolic profile name, must not be NULL
   *  @param assoc T_ASC_Association structure to be evaluated
   *  @return EC_Normal if successful, an error code otherwise
   */
  OFCondition evaluateAssociationParameters(
    const char *profile,
    T_ASC_Association &assoc) const;

  /** adds the given transfer syntax UID to the list of transfer syntaxes
   *  maintained under the given key.  If key is not yet known, a new list
   *  is created under this key.
   *  @param key transfer syntax list key, must not be NULL
   *  @param transferSyntaxUID transfer syntax UID in symbolic or numeric format
   *  @return EC_Normal if successful, an error code otherwise
   */
  OFCondition addTransferSyntax(
    const char *key,
    const char *transferSyntaxUID);

  /** adds the given abstract syntax UID and transfer syntax list key to 
   *  the list of presentation contexts maintained under the given key. 
   *  If key is not yet known, a new list is created under this key.
   *  One list of presentation contexts may not have more than 128 entries
   *  because of the limitations of the DICOM ACSE protocol.
   *  @param key presentation context list key, must not be NULL
   *  @param abstractSyntaxUID abstract syntax UID in symbolic or numeric format
   *  @param transferSyntaxKey symbolic key that has been used in a call
   *     to addTransferSyntax() prior to this call.
   *  @return EC_Normal if successful, an error code otherwise
   */
  OFCondition addPresentationContext(
    const char *key,
    const char *abstractSyntaxUID,
    const char *transferSyntaxKey);

  /** adds the given abstract syntax UID and role to 
   *  the list of SCP/SCU role selection items maintained under the given key. 
   *  If key is not yet known, a new list is created under this key.
   *  One abstract syntax UID must not appear twice within one list.
   *  @param key role list key, must not be NULL
   *  @param abstractSyntaxUID abstract syntax UID in symbolic or numeric format
   *  @param role role enum
   *  @return EC_Normal if successful, an error code otherwise
   */  
  OFCondition addRole(
    const char *key,
    const char *abstractSyntaxUID,
    T_ASC_SC_ROLE role);

  /** adds the given abstract syntax UID and extended negotiation data to 
   *  the list of extended negotiation items maintained under the given key. 
   *  If key is not yet known, a new list is created under this key.
   *  One abstract syntax UID must not appear twice within one list.
   *  @param key role list key, must not be NULL
   *  @param abstractSyntaxUID abstract syntax UID in symbolic or numeric format
   *  @param rawData pointer to raw data, must not be NULL
   *  @param length length of data block pointed to by data, in bytes
   *  @return EC_Normal if successful, an error code otherwise
   */  
  OFCondition addExtendedNegotiation(
    const char *key,
    const char *abstractSyntaxUID,
    const unsigned char *rawData,
    Uint32 length);

  /** creates a new association negotiation profile under the given key.
   *  A profile consists of a list of presentation contexts and may optionally
   *  also include a list of SCP/SCU role selection items and a list of
   *  extended negotiation items. This method checks the consistency of the
   *  three lists, i.e. makes sure that all abstract syntaxes mentioned either
   *  in the list of role selection items or the list of extended negotiation
   *  items are also contained in at least one presentation context.
   *  @param key profile key, must not be NULL
   *  @param presentationContextKey presentation context list key, must not be NULL
   *  @param roleSelectionKey role selection list key, may be NULL
   *  @param extendedNegotiationKey extended negotiation list key, may be NULL
   *  @return EC_Normal if successful, an error code otherwise
   */  
  OFCondition addProfile(
    const char *key,
    const char *presentationContextKey,
    const char *roleSelectionKey=NULL,
    const char *extendedNegotiationKey=NULL);

  /** checks if the profile is known
   *  @param key profile name, must not be NULL
   *  @return true if profile is known, false otherwise
   */
  OFBool isKnownProfile(const char *key) const;

  /** checks if the profile is suitable for use by an SCP.
   *  A profile is suitable for use by an SCP if each SOP class in the
   *  list of presentation contexts appears at most once.
   *  @param key profile name, must not be NULL
   *  @return true if profile is suitable for use by an SCP, false otherwise
   */
  OFBool isValidSCPProfile(const char *key) const;

private:
  /// private undefined copy constructor
  DcmAssociationConfiguration(const DcmAssociationConfiguration& arg);

  /// private undefined copy assignment operator
  DcmAssociationConfiguration& operator=(const DcmAssociationConfiguration& arg);

  /// map of transfer syntax lists
  DcmTransferSyntaxMap xferSyntaxes_;

  /// map of presentation context lists
  DcmPresentationContextMap contexts_;

  /// map of role selection lists
  DcmRoleSelectionMap roleselection_;

  /// map of extended negotiation lists
  DcmExtendedNegotiationMap extneg_;

  /// map of profiles
  DcmProfileMap profiles_;
};

#endif

/*
 * CVS/RCS Log
 * $Log: dcasccfg.h,v $
 * Revision 1.4  2010-10-14 13:17:22  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.3  2005/12/08 16:02:09  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.2  2003/08/14 10:58:49  meichel
 * Added check if association configuration profile is valid for SCP use
 *
 * Revision 1.1  2003/06/10 14:27:33  meichel
 * Initial release of class DcmAssociationConfiguration and support
 *   classes. This class maintains a list of association negotiation
 *   profiles that can be addressed by symbolic keys. The profiles may
 *   be read from a configuration file.
 *
 *
 */
