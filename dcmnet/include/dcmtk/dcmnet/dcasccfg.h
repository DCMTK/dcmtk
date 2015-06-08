/*
 *
 *  Copyright (C) 2003-2015, OFFIS e.V.
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
 *    class DcmAssociationConfiguration
 *
 */

#ifndef DCASCCFG_H
#define DCASCCFG_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofcond.h"    /* for class OFCondition */
#include "dcmtk/dcmnet/assoc.h"    /* for T_ASC_Parameters */
#include "dcmtk/dcmnet/dccftsmp.h" /* for class DcmTransferSyntaxMap */
#include "dcmtk/dcmnet/dccfpcmp.h" /* for class DcmPresentationContextMap */
#include "dcmtk/dcmnet/dccfrsmp.h" /* for class DcmRoleSelectionMap */
#include "dcmtk/dcmnet/dccfenmp.h" /* for class DcmExtendedNegotiationMap */
#include "dcmtk/dcmnet/dccfprmp.h" /* for class DcmProfileMap */


/** This class maintains a list of association negotiation configuration
 *  profiles. A profile is a combination of the following components:
 *  A list of presentation contexts, an optional list of SCP/SCU role
 *  selection items and an optional list of extended negotiation items.
 *  A presentation context itself consist of an abstract syntax and
 *  a list of transfer syntaxes, the latter each being separate components.
 *  Role selection and extended negotation items are atomic (i.e. they do not
 *  reference other components). All components are identified by a
 *  unique symbolic name.
 *  All components are re-usable since they are only referenced from a
 *  profile by their respective symbolic names. E.g. a list of transfer
 *  syntaxes can be referenced from various presentation contexts. This
 *  approach has been taken in order to save memory since, for instance,
 * in many presentation contexts, the list of supported transfer syntaxes
 *  will be the same.
 */
class DCMTK_DCMNET_EXPORT DcmAssociationConfiguration
{
public:
  /// default constructor
  DcmAssociationConfiguration();

  /// destructor
  ~DcmAssociationConfiguration();

  /// Copy constructor, performs deep copy
  DcmAssociationConfiguration(const DcmAssociationConfiguration& arg);

  /// Copy assignment operator, performs deep copy
  DcmAssociationConfiguration& operator=(const DcmAssociationConfiguration& arg);

  /// Reset configuration.
  void clear();

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

  /** returns profile identified by given name
   *  @param profileName the name of the profile to look for
   *  @return the profile if existant, otherwise NULL
   */
  const DcmProfileEntry* getProfileEntry(const OFString& profileName);

  /** checks if the profile is suitable for use by an SCP.
   *  A profile is suitable for use by an SCP if each SOP class in the
   *  list of presentation contexts appears at most once.
   *  @param key profile name, must not be NULL
   *  @return true if profile is suitable for use by an SCP, false otherwise
   */
  OFBool isValidSCPProfile(const char *key) const;

  /** find a list of transfer syntaxes that matches the given list and return
   *  its name. A match is only found if the number of transfer syntaxes is
   *  the same, each transfer syntax exists in the other list and the order
   *  is preserved.
   *  @param tslist the list of transfer syntaxes to look for
   *  @return the symbolic name of the list found. If nomatch is found,
   *     an empty string is returned.
   */
  OFString findTSKey(const OFList<OFString>& tslist);

  /** dumps all profiles or a selected profile to the given output stream.
   *  Mainly useful for debugging.
   *  @param out output stream to be used
   *  @param profileName if not empty, only the profile defined by the given is
   *     dumped. Otherwise, all profiles are dumped.
   */
  void dumpProfiles(STD_NAMESPACE ostream &out,
                    const OFString& profileName = "");

protected:

  /** dump a single association profile, mainly interesting for
   *  debugging purposes.
   *  @param out output stream to be used
   *  @param profile profile to dump
   *  @param profileName name of the profile to dump (used for debug output)
   */
  void dumpProfile(STD_NAMESPACE ostream &out,
                   const DcmProfileEntry* profile,
                   const OFString& profileName);

private:

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
