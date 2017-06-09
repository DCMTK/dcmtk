/*
 *
 *  Copyright (C) 2003-2017, OFFIS e.V.
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

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmnet/dcasccfg.h"
#include "dcmtk/dcmdata/dcerror.h"  /* for EC_IllegalCall */

DcmAssociationConfiguration::DcmAssociationConfiguration()
: xferSyntaxes_()
, contexts_()
, roleselection_()
, extneg_()
, profiles_()
, alwaysAcceptDefaultRole_(OFFalse)
{
}

DcmAssociationConfiguration::~DcmAssociationConfiguration()
{
}

// Copy constructor, performs deep copy
DcmAssociationConfiguration::DcmAssociationConfiguration(const DcmAssociationConfiguration& arg) :
  xferSyntaxes_(arg.xferSyntaxes_),
  contexts_(arg.contexts_),
  roleselection_(arg.roleselection_),
  extneg_(arg.extneg_),
  profiles_(arg.profiles_)
{
}

/// Copy assignment operator, performs deep copy
DcmAssociationConfiguration& DcmAssociationConfiguration::operator=(const DcmAssociationConfiguration& arg)
{
  /* No self assignment */
  if (this != &arg)
  {
    this->clear();
    xferSyntaxes_ = arg.xferSyntaxes_;
    contexts_ = arg.contexts_;
    roleselection_ = arg.roleselection_;
    extneg_ = arg.extneg_;
    profiles_ = arg.profiles_;
  }
  return *this;
}

void DcmAssociationConfiguration::clear()
{
  xferSyntaxes_.clear();
  contexts_.clear();
  roleselection_.clear();
  extneg_.clear();
  profiles_.clear();
  alwaysAcceptDefaultRole_ = OFFalse;
}

OFCondition DcmAssociationConfiguration::addTransferSyntax(
  const char *key,
  const char *transferSyntaxUID)
{
  return xferSyntaxes_.add(key, transferSyntaxUID);
}

OFCondition DcmAssociationConfiguration::addPresentationContext(
  const char *key,
  const char *abstractSyntaxUID,
  const char *transferSyntaxKey)
{
  if ((!key)||(!abstractSyntaxUID)||(!transferSyntaxKey)) return EC_IllegalCall;

  if (! xferSyntaxes_.isKnownKey(transferSyntaxKey))
  {
    // error: key undefined
    OFString s("transfer syntax key undefined: ");
    s += transferSyntaxKey;
    return makeOFCondition(OFM_dcmnet, 1031, OF_error, s.c_str());
  }

  return contexts_.add(key, abstractSyntaxUID, transferSyntaxKey);
}

OFCondition DcmAssociationConfiguration::addRole(
  const char *key,
  const char *abstractSyntaxUID,
  T_ASC_SC_ROLE role)
{
  return roleselection_.add(key, abstractSyntaxUID, role);
}

OFCondition DcmAssociationConfiguration::createEmptyRoleList(
  const char* key)
{
  return roleselection_.addEmpty(key);
}

OFCondition DcmAssociationConfiguration::addExtendedNegotiation(
  const char *key,
  const char *abstractSyntaxUID,
  const unsigned char *rawData,
  Uint32 length)
{
  return extneg_.add(key, abstractSyntaxUID, rawData, length);
}

OFCondition DcmAssociationConfiguration::addProfile(
  const char *key,
  const char *presentationContextKey,
  const char *roleSelectionKey,
  const char *extendedNegotiationKey)
{
  if ((!key)||(!presentationContextKey)) return EC_IllegalCall;

  if (!contexts_.isKnownKey(presentationContextKey))
  {
    // error: key undefined
    OFString s("presentation context key undefined: ");
    s += presentationContextKey;
    return makeOFCondition(OFM_dcmnet, 1033, OF_error, s.c_str());
  }

  OFCondition status = EC_Normal;
  if (roleSelectionKey)
  {
    // check that role selection list exists and matches presentation context list
    status = roleselection_.checkConsistency(roleSelectionKey, contexts_, presentationContextKey);
    if (status.bad()) return status;
  }

  if (extendedNegotiationKey)
  {
    // check that extended negotiation list exists and matches presentation context list
    status = extneg_.checkConsistency(extendedNegotiationKey, contexts_, presentationContextKey);
    if (status.bad()) return status;
  }

  return profiles_.add(key, presentationContextKey, roleSelectionKey, extendedNegotiationKey);
}


OFBool DcmAssociationConfiguration::isKnownProfile(const char *key) const
{
  return profiles_.isKnownKey(key);
}


const DcmProfileEntry* DcmAssociationConfiguration::getProfileEntry(const OFString& profileName)
{
  OFMap<OFString, DcmProfileEntry*>::const_iterator it =  profiles_.begin();
  while ( it != profiles_.end() )
  {
    if ( (*it).first == profileName )
    {
      return (*it).second;
    }
    it++;
  }
  return NULL;
}


OFBool DcmAssociationConfiguration::isValidSCPProfile(const char *key) const
{
  const char *contextKey = profiles_.getPresentationContextKey(key);
  const DcmPresentationContextList *contextList = contexts_.getPresentationContextList(contextKey);
  if (contextList)
  {
    // now loop through all presentation contexts
    OFListConstIterator(DcmPresentationContextItem) outerfirst = contextList->begin();
    OFListConstIterator(DcmPresentationContextItem) innerfirst;
    OFListConstIterator(DcmPresentationContextItem) last = contextList->end();
    OFString outerAbstractSyntax;
    while (outerfirst != last)
    {
      innerfirst = outerfirst;
      ++innerfirst;
      outerAbstractSyntax = (*outerfirst).getAbstractSyntax();
      while (innerfirst != last)
      {
        if (outerAbstractSyntax == (*innerfirst).getAbstractSyntax())
        {
          // we have found two instances of the same abstract syntax in this list,
          // so this is not a valid SCP profile.
          return OFFalse;
        }
        ++innerfirst;
      }
      ++outerfirst;
    }
    // there are no duplicate abstract syntaxes, so this is a valid SCP profile.
    return OFTrue;
  }
  return OFFalse;
}


OFCondition DcmAssociationConfiguration::setAssociationParameters(
  const char *profile,
  T_ASC_Parameters& params) const
{
  if (!profile) return EC_IllegalCall;

  const char *contextKey = profiles_.getPresentationContextKey(profile);
  if (! contextKey)
  {
    // error: key undefined
    OFString s("profile key undefined: ");
    s += profile;
    return makeOFCondition(OFM_dcmnet, 1062, OF_error, s.c_str());
  }

  // these two keys may be NULL
  const char *roleKey = profiles_.getRoleSelectionKey(profile);
  const char *extnegKey = profiles_.getExtendedNegotiationKey(profile);

  // pointer to list of presentation contexts
  const DcmPresentationContextList *contextList = contexts_.getPresentationContextList(contextKey);
  if (! contextList)
  {
    // error: key undefined
    OFString s("presentation context key undefined: ");
    s += contextKey;
    return makeOFCondition(OFM_dcmnet, 1063, OF_error, s.c_str());
  }

  // pointer to list of SCP/SCU role selection items
  const DcmRoleSelectionList *roleSelectionList = NULL;
  if (roleKey)
  {
    roleSelectionList = roleselection_.getRoleSelectionList(roleKey);
    if (! roleSelectionList)
    {
      // error: key undefined
      OFString s("role selection key undefined: ");
      s += roleKey;
      return makeOFCondition(OFM_dcmnet, 1064, OF_error, s.c_str());
    }
  }

  // pointer to list of SCP/SCU role selection items
  const DcmExtendedNegotiationList *extendedNegotiationList = NULL;
  if (extnegKey)
  {
    extendedNegotiationList = extneg_.getExtendedNegotiationList(extnegKey);
    if (! extendedNegotiationList)
    {
      // error: key undefined
      OFString s("extended negotiation key undefined: ");
      s += extnegKey;
      return makeOFCondition(OFM_dcmnet, 1065, OF_error, s.c_str());
    }
  }

  // initialize presentation context ID counter
  T_ASC_PresentationContextID cid = 1;
  OFCondition result = EC_Normal;
  const char *abstractSyntax = NULL;
  const char *transferSyntaxKey = NULL;
  const DcmTransferSyntaxList *transferSyntaxList = NULL;
  const char *xferSyntaxes[128];
  OFListConstIterator(DcmUIDHandler) xffirst;
  OFListConstIterator(DcmUIDHandler) xflast;
  int numXferSyntaxes = 0;
  T_ASC_SC_ROLE proposedRole;
  OFListConstIterator(DcmRoleSelectionItem) rsfirst;
  OFListConstIterator(DcmRoleSelectionItem) rslast;
  DcmUIDHandler uid;

  // now loop through all presentation contexts
  OFListConstIterator(DcmPresentationContextItem) first = contextList->begin();
  OFListConstIterator(DcmPresentationContextItem) last = contextList->end();
  while (first != last)
  {
    // determine abstract syntax
    abstractSyntax = (*first).getAbstractSyntax();
    transferSyntaxKey = (*first).getTransferSyntaxKey();
    transferSyntaxList = xferSyntaxes_.getTransferSyntaxList(transferSyntaxKey);

    if (! transferSyntaxList)
    {
      // error: key undefined
      OFString s("transfer syntax key undefined: ");
      s += transferSyntaxKey;
      return makeOFCondition(OFM_dcmnet, 1066, OF_error, s.c_str());
    }

    if (transferSyntaxList->size() > 128)
    {
      // error: key undefined
      OFString s("transfer syntax list too long, cannot handle more than 128 entries: ");
      s += transferSyntaxKey;
      return makeOFCondition(OFM_dcmnet, 1067, OF_error, s.c_str());
    }

    if (transferSyntaxList->size() < 1)
    {
      // error: key undefined
      OFString s("transfer syntax list empty: ");
      s += transferSyntaxKey;
      return makeOFCondition(OFM_dcmnet, 1068, OF_error, s.c_str());
    }

    // copy list of transfer syntaxes into array
    numXferSyntaxes = 0;
    xffirst = transferSyntaxList->begin();
    xflast  = transferSyntaxList->end();
    while (xffirst != xflast)
    {
      xferSyntaxes[numXferSyntaxes++] = (*xffirst).c_str();
      ++xffirst;
    }

    // look up SCP/SCU role for this abstract syntax
    proposedRole = ASC_SC_ROLE_DEFAULT;
    if (roleSelectionList)
    {
      // look-up abstract syntax in role selection list
      rsfirst = roleSelectionList->begin();
      rslast = roleSelectionList->end();
      uid = abstractSyntax;
      while (rsfirst != rslast)
      {
        if ((*rsfirst).matches(uid))
        {
          // found abstract syntax, set role accordingly
          proposedRole = (*rsfirst).getRole();
          break; /* out of while loop */
        }
        ++rsfirst;
      }
    }

    // finally add presentation context to params
    result = ASC_addPresentationContext(&params, cid, abstractSyntax, xferSyntaxes, numXferSyntaxes, proposedRole);
    if (result.bad()) return result;

    // increase index by 2
    cid += 2;
    ++first;
  }

  // create extended negotiation items if present
  if (extendedNegotiationList)
  {
    // create extended negotiation item list as required by dcmnet code
    SOPClassExtendedNegotiationSubItemList *enlist = new SOPClassExtendedNegotiationSubItemList();
    ASC_setRequestedExtNegList(&params, enlist);
    SOPClassExtendedNegotiationSubItem *currentItem = NULL;

    // for each entry in our list
    OFListConstIterator(DcmExtendedNegotiationItem) enfirst = extendedNegotiationList->begin();
    OFListConstIterator(DcmExtendedNegotiationItem) enlast = extendedNegotiationList->end();
    while (enfirst != enlast)
    {
      // create and populate SOPClassExtendedNegotiationSubItem object
      currentItem = new SOPClassExtendedNegotiationSubItem();
      currentItem->sopClassUID = (*enfirst).getAbstractSyntaxC();
      currentItem->serviceClassAppInfoLength = OFstatic_cast(unsigned short, (*enfirst).getLength());
      currentItem->serviceClassAppInfo = new unsigned char[currentItem->serviceClassAppInfoLength];
      memcpy(currentItem->serviceClassAppInfo, (*enfirst).getRaw() , currentItem->serviceClassAppInfoLength);
      enlist->push_back(currentItem);
      ++enfirst;
    }
  }

  return result;
}

OFCondition DcmAssociationConfiguration::evaluateAssociationParameters(
  const char *profile,
  T_ASC_Association &assoc) const
{
  if (!profile) return EC_IllegalCall;

  const char *contextKey = profiles_.getPresentationContextKey(profile);
  if (! contextKey)
  {
    // error: key undefined
    OFString s("profile key undefined: ");
    s += profile;
    return makeOFCondition(OFM_dcmnet, 1069, OF_error, s.c_str());
  }

  // these two keys may be NULL
  const char *roleKey = profiles_.getRoleSelectionKey(profile);
  const char *extnegKey = profiles_.getExtendedNegotiationKey(profile);

  // pointer to list of presentation contexts
  const DcmPresentationContextList *contextList = contexts_.getPresentationContextList(contextKey);
  if (! contextList)
  {
    // error: key undefined
    OFString s("presentation context key undefined: ");
    s += contextKey;
    return makeOFCondition(OFM_dcmnet, 1070, OF_error, s.c_str());
  }

  // pointer to list of SCP/SCU role selection items
  const DcmRoleSelectionList *roleSelectionList = NULL;
  if (roleKey)
  {
    roleSelectionList = roleselection_.getRoleSelectionList(roleKey);
    if (! roleSelectionList)
    {
      // error: key undefined
      OFString s("role selection key undefined: ");
      s += roleKey;
      return makeOFCondition(OFM_dcmnet, 1071, OF_error, s.c_str());
    }
  }

  // pointer to list of SCP/SCU role selection items
  const DcmExtendedNegotiationList *extendedNegotiationList = NULL;
  if (extnegKey)
  {
    extendedNegotiationList = extneg_.getExtendedNegotiationList(extnegKey);
    if (! extendedNegotiationList)
    {
      // error: key undefined
      OFString s("extended negotiation key undefined: ");
      s += extnegKey;
      return makeOFCondition(OFM_dcmnet, 1072, OF_error, s.c_str());
    }
  }

  // declare a few variables we need later
  OFCondition result = EC_Normal;
  T_ASC_PresentationContext pc;
  const char *transferSyntaxKey = NULL;
  OFBool found;
  T_ASC_SC_ROLE acceptedRole;
  OFListConstIterator(DcmRoleSelectionItem) rsfirst;
  OFListConstIterator(DcmRoleSelectionItem) rslast;
  DcmUIDHandler uid;
  const DcmTransferSyntaxList *transferSyntaxList = NULL;
  OFListConstIterator(DcmPresentationContextItem) first;
  OFListConstIterator(DcmPresentationContextItem) last = contextList->end();
  OFListConstIterator(DcmUIDHandler) tsfirst;
  OFListConstIterator(DcmUIDHandler) tslast;
  int numContexts = ASC_countPresentationContexts(assoc.params);
  unsigned char j = 0;

  // traverse list of presentation contexts
  for (int i = 0; i < numContexts; ++i)
  {
    // retrieve presentation context
    result = ASC_getPresentationContext(assoc.params, i, &pc);
    if (result.bad()) return result;

    // check if the abstract syntax is in our list of abstract syntaxes
    first = contextList->begin();
    found = OFFalse;
    while (first != last)
    {
      if (0 == strcmp((*first).getAbstractSyntax(), pc.abstractSyntax))
      {
        found = OFTrue;
        break; /* out of while loop */
      }
      ++first;
    }

    if (!found)
    {
      // abstract syntax not supported, reject presentation context
      result = ASC_refusePresentationContext(assoc.params, pc.presentationContextID,
        ASC_P_ABSTRACTSYNTAXNOTSUPPORTED);
      if (result.bad()) return result;
    }
    else
    {
      // abstract syntax is supported, get transfer syntax list and SCP/SCU role
      transferSyntaxKey = (*first).getTransferSyntaxKey();
      transferSyntaxList = xferSyntaxes_.getTransferSyntaxList(transferSyntaxKey);
      if (! transferSyntaxList)
      {
        // error: key undefined
        OFString s("transfer syntax key undefined: ");
        s += transferSyntaxKey;
        return makeOFCondition(OFM_dcmnet, 1073, OF_error, s.c_str());
      }

      // look up SCP/SCU role for this abstract syntax in configuration.
      // default is no role selection, thus, default role applies, i.e.
      // no role selection item would be sent back
      acceptedRole = ASC_SC_ROLE_DEFAULT;
      if (roleSelectionList)
      {
        // look-up abstract syntax in role selection list
        rsfirst = roleSelectionList->begin();
        rslast = roleSelectionList->end();
        uid = pc.abstractSyntax;
        while (rsfirst != rslast)
        {
          if ((*rsfirst).matches(uid))
          {
            // found explicit role for this abstract syntax in configuration, set role accordingly
            acceptedRole = (*rsfirst).getRole();
            break; /* out of while loop */
          }
          ++rsfirst;
        }
      }

      // try to accept the presentation context with the most preferred transfer syntax
      tsfirst = transferSyntaxList->begin();
      tslast = transferSyntaxList->end();
      found = OFFalse;

      // loop through our list of preferred transfer syntaxes for this abstract syntax,
      // starting with the most preferred one
      while ((tsfirst != tslast) && !found)
      {
        // loop through list of transfer syntaxes in presentation context
        for (j=0; (!found && (j < pc.transferSyntaxCount)); ++j)
        {
          if ((*tsfirst).str() == pc.proposedTransferSyntaxes[j])
          {
            // matching transfer syntax found, accept context
            found = OFTrue;
            result = ASC_acceptPresentationContext(
                assoc.params, pc.presentationContextID,
                pc.proposedTransferSyntaxes[j], acceptedRole, alwaysAcceptDefaultRole_);
            // SCP/SCU role selection failed, reject presentation context
            if (result == ASC_SCPSCUROLESELECTIONFAILED)
            {
              result = ASC_refusePresentationContext(assoc.params,
                pc.presentationContextID, ASC_P_NOREASON);
            }
            if (result.bad()) return result;
          }
        }
        ++tsfirst;
      }

      // no matching transfer syntax found, reject context
      if (!found)
      {
        // transfer syntax not supported, reject presentation context
        result = ASC_refusePresentationContext(assoc.params, pc.presentationContextID,
          ASC_P_TRANSFERSYNTAXESNOTSUPPORTED);
        if (result.bad()) return result;
      }
    }
  }

  // create extended negotiation items if present
  if (extendedNegotiationList)
  {
    // create extended negotiation item list as required by dcmnet code
    SOPClassExtendedNegotiationSubItemList *enlist = new SOPClassExtendedNegotiationSubItemList();
    SOPClassExtendedNegotiationSubItem *currentItem = NULL;

    // for each entry in our list
    OFListConstIterator(DcmExtendedNegotiationItem) enfirst = extendedNegotiationList->begin();
    OFListConstIterator(DcmExtendedNegotiationItem) enlast = extendedNegotiationList->end();
    while (enfirst != enlast)
    {
      // check if we have an accepted presentation context for this abstract syntax
      if (0 != ASC_findAcceptedPresentationContextID(&assoc, (*enfirst).getAbstractSyntaxC()))
      {
        // create and populate SOPClassExtendedNegotiationSubItem object
        currentItem = new SOPClassExtendedNegotiationSubItem();
        currentItem->sopClassUID = (*enfirst).getAbstractSyntaxC();
        currentItem->serviceClassAppInfoLength = OFstatic_cast(unsigned short, (*enfirst).getLength());
        currentItem->serviceClassAppInfo = new unsigned char[currentItem->serviceClassAppInfoLength];
        memcpy(currentItem->serviceClassAppInfo, (*enfirst).getRaw() , currentItem->serviceClassAppInfoLength);
        enlist->push_back(currentItem);
      }
      ++enfirst;
    }

    if (enlist->size() > 0)
      ASC_setAcceptedExtNegList(assoc.params, enlist);
      else delete enlist;
  }

  return result;
}

void DcmAssociationConfiguration::setAlwaysAcceptDefaultRole(const OFBool enabled)
{
  alwaysAcceptDefaultRole_ = enabled;
}



void DcmAssociationConfiguration::dumpProfiles(
    STD_NAMESPACE ostream &out,
    const OFString& profileName)
{
  const DcmProfileEntry* profile = NULL;
  // dump single profile if desired
  if ( !profileName.empty() )
  {
    profile = profiles_.getProfile(profileName);
    if (profile == NULL)
    {
      out << "No such profile: " << profileName << OFendl;
      return;
    }
    else
    {
      dumpProfile(out, profile, profileName);
    }
  }
  else
  {
    // no profile given: dump all profiles
    OFMap<OFString, DcmProfileEntry*>::const_iterator it = profiles_.begin();
    OFMap<OFString, DcmProfileEntry*>::const_iterator end = profiles_.end();
    if ( it == end )
    {
      out << "No profiles defined" << OFendl;
      return;
    }
    while ( it != end )
    {
      dumpProfile(out, (*it).second, profileName);
      it++;
    }
  }
  return;
}


void DcmAssociationConfiguration::dumpProfile(
  STD_NAMESPACE ostream &out,
  const DcmProfileEntry* profile,
  const OFString& profileName)
{
  if ( profile == NULL )
  {
    out << "No such profile (NULL)" << OFendl;
    return;
  }
  // print header for this profile
  out << "-----------------------------------------------------------" << OFendl;
  out << "Dumping Profile: " << profileName << OFendl;

  OFString presContext = profile->getPresentationContextKey();
  if ( presContext.empty() )
  {
    out << "Presentation context list name empty, no presentation contexts configured" << OFendl;
  }
  else
  {
    // print all presentation contexts
    const DcmPresentationContextList* pclist = contexts_.getPresentationContextList(presContext);
    if ( pclist != NULL )
    {
      out << "Dumping presentation context list " << presContext << ": " << OFendl;
      size_t count = 1;
      // for each presentation context, list all of its configured transfer syntaxes
      OFListConstIterator(DcmPresentationContextItem) pc = pclist->begin();
      while ( pc != pclist->end() )
      {
        OFString ts = (*pc).getTransferSyntaxKey();
        const DcmTransferSyntaxList *tsList = NULL;
        if (!ts.empty())
        {
          tsList = xferSyntaxes_.getTransferSyntaxList(ts.c_str());
        }
        out << "Presentation Context #" << count << ", abstract syntax: " << pc->getAbstractSyntax() << OFendl;
        out << "Dumping Transfer Syntaxes list " << ts << OFendl;
        if ( ts.empty() || (tsList == NULL) || (tsList->empty()))
        {
          out << "None defined" << OFendl;
        }
        else
        {
          OFListConstIterator(DcmUIDHandler) uid = tsList->begin();
          size_t tsnum = 1;
          while ( uid != tsList->end() )
          {
            out << "  Transfer Syntax #" << tsnum << ": " << (uid)->c_str() << OFendl;
            uid++;
          }
        }
        pc++;
        count++;
      }
    }
    else
    {
       out << "No such presentation context list with that name: " << presContext << OFendl;
    }
  }
  // dump extended negotiation items
  const char * ext = profile->getExtendedNegotiationKey();
  if ( ext )
  {
    out << "Extended negotiation " << ext << " configured (not dumped)" << OFendl;
  }
  else
  {
    out << "No extended negotiation configured" << OFendl;
  }
  // dump role selection items
  const char* role = profile->getRoleSelectionKey();
  if ( role )
  {
     out << "Dumping Role selection items " << role << OFendl;
     const DcmRoleSelectionList* roleList = roleselection_.getRoleSelectionList(role);
     if ( roleList )
     {
       OFListConstIterator(DcmRoleSelectionItem) item = roleList->begin();
       size_t count = 1;
       while ( item != roleList->end() )
       {
         out << "  Item #" << count << ": Abstract syntax " << (*item).getAbstractSyntax().str() << ", role " << ASC_role2String((*item).getRole()) << OFendl;
         item++; count++;
       }
     }
     else
     {
       out << "Error: Role list is configured but empty" << OFendl;
     }
  }
  else
  {
    out << "No role selection items configured" << OFendl;
  }
  // print footer for this profile
  out << "-----------------------------------------------------------" << OFendl;
}

OFString DcmAssociationConfiguration::findTSKey(
  const OFList<OFString>& tslist)
{
  if ( tslist.empty() )
    return "";

  // loop over all transfer syntax lists configured
  OFMap<OFString, DcmTransferSyntaxList*>::const_iterator configIT = xferSyntaxes_.begin();
  while (configIT !=  xferSyntaxes_.end())
  {
    // loop over transfer syntaxes in one of the configured lists
    // lists must have at least the same size, otherwise this is not a match
    if ( (*configIT).second->size() == tslist.size())
    {
      OFListIterator(DcmUIDHandler) configTS = (*configIT).second->begin(); // single configured ts
      OFListConstIterator(OFString) inputTS = tslist.begin(); // single input ts
      OFBool isMatch = OFTrue;
      // now compare each configured ts in list with each input ts pairwise.
      // this works since size is the same and order is significant
      while (configTS !=  (*configIT).second->end())
      {
        // as long as we ts match, continue
        if ( (*configTS).str() == *inputTS )
        {
          configTS++;
          inputTS++;
        }
        else
        {
          isMatch = OFFalse;
          break;
        }
      }
      // if we found a match, return the ts key of the matching list
      if ( isMatch )
      {
        return (*configIT).first;
      }
    } // else ts list sizes differ -> no match
    // try next configured ts list
    configIT++;
  }

  // no matching ts list found, return empty key
  return "";
}
