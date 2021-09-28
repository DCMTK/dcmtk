/*
 *
 *  Copyright (C) 2000-2021, OFFIS e.V.
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

#include "dcmtk/config/osconfig.h"

#ifdef WITH_OPENSSL

#include "dcmtk/dcmsign/dcsighlp.h"
#include "dcmtk/dcmdata/dcdeftag.h" /* for attribute tag constants */
#include "dcmtk/dcmdata/dcdicent.h" /* for class DcmDictEntry */
#include "dcmtk/dcmdata/dcdict.h"   /* for class DcmDataDictionary */
#include "dcmtk/dcmdata/dcitem.h"   /* for class DcmItem */
#include "dcmtk/dcmdata/dcsequen.h" /* for class DcmSequenceOfItems */
#include "dcmtk/dcmdata/dcvrat.h"   /* for class DcmAttributeTag */
#include "dcmtk/dcmsign/dcsignat.h" /* for class DcmSignature */
#include "dcmtk/dcmsign/sicert.h"   /* for class SiCertificate */
#include "dcmtk/dcmsign/sicertvf.h" /* for class SiCertificateVerifier */
#include "dcmtk/dcmsign/sisprof.h"  /* for class SiSecurityProfile */
#include "dcmtk/dcmsign/sitsfs.h"   /* for class SiTimeStampFS */
#include "dcmtk/dcmsign/sitypes.h"  /* for logger macros */
#include "dcmtk/dcmsign/sicreapr.h" /* for class SiCreatorProfile */
#include "dcmtk/dcmsign/siautopr.h" /* for class SiAuthorizationProfile */
#include "dcmtk/dcmsign/sisrvpr.h"  /* for class SiStructuredReportingVerificationProfile */
#include "dcmtk/ofstd/ofexit.h"     /* for common exit codes */
#include "dcmtk/dcmsign/siexit.h"   /* for dcmsign specific exit codes */


DcmSignatureHelper::DcmSignatureHelper()
{
}

DcmSignatureHelper::~DcmSignatureHelper()
{
}

int DcmSignatureHelper::readNextToken(const char *c, int& pos, DcmTagKey& key, Uint32& idx)
{
  OFString aString;
  int lpos = pos;
  int spos = 0;
  while(isspace(OFstatic_cast(unsigned char, c[lpos]))) ++lpos; // ignore leading space

  if (c[lpos]=='\0') return -1; // EOF
  if (c[lpos]=='.')
  {
    ++pos;
    return 3; // period
  }
  if (c[lpos]=='[')
  {
    spos = ++lpos;
    while ((c[lpos] >= '0')&&(c[lpos] <= '9')) ++lpos;
    if (c[lpos] != ']') return 0; // parse error
    unsigned long newindex = 0;
    if (1 != sscanf(c+spos,"%lu", &newindex)) return 0; // parse error
    idx = OFstatic_cast(Uint32, newindex);
    pos = ++lpos;
    return 2; // index
  }
  if (c[lpos]=='(')
  {
    spos = ++lpos;
    while ((c[lpos] != ')')&&(c[lpos] != '\0')) ++lpos;
    if (c[lpos] != ')') return 0; // parse error
    unsigned int group=0;
    unsigned int elem=0;
    if (2 != sscanf(c+spos,"%x,%x", &group, &elem)) return 0; // parse error
    key = DcmTagKey(OFstatic_cast(Uint16, group),OFstatic_cast(Uint16, elem));
    pos = ++lpos;
    return 1; // tag key
  }
  spos = lpos;
  while ( ((c[lpos] >= 'a')&&(c[lpos] <= 'z')) || ((c[lpos] >= 'A')&&(c[lpos] <= 'Z')) || ((c[lpos] >= '0')&&(c[lpos] <= '9'))) ++lpos;
  aString.append(c + spos, (lpos-spos));
  const DcmDataDictionary& globalDataDict = dcmDataDict.rdlock();
  const DcmDictEntry *dicent = globalDataDict.findEntry(aString.c_str());
  if (dicent)
  {
    key = dicent->getKey();
    dcmDataDict.rdunlock();
    pos = lpos;
    return 1; // tag key;
  }
  dcmDataDict.rdunlock();
  DCMSIGN_ERROR("attribute name '" << aString.c_str() << "' unknown.");
  return 0; // parse error
}


char *DcmSignatureHelper::readTextFile(const char *filename)
{
  char *result = NULL;
  FILE *file = fopen(filename, "rb");
  if (file)
  {
    fseek(file, 0, SEEK_END);
    long numBytes = ftell(file);
    fseek(file, 0, SEEK_SET);
    result = new char[numBytes+1];
    if (result)
    {
      result[numBytes] = '\0'; // ensure zero termination of text field
      if (OFstatic_cast(size_t, numBytes) != fread(result, 1, OFstatic_cast(size_t, numBytes), file))
      {
        DCMSIGN_WARN("read error in file " << filename);
        delete[] result;
        result = NULL;
      }
    }
    fclose(file);
  } else {
    DCMSIGN_ERROR("file not found: " << filename);
  }
  return result;
}


int DcmSignatureHelper::parseTextFile(const char *filename, DcmAttributeTag& tagList)
{
  char *c=readTextFile(filename);
  if (c==NULL) return EXITCODE_CANNOT_READ_TAG_FILE; // bail out
  int position = 0;
  int token = 0;
  Uint32 idx = 0;
  DcmTagKey key;
  int result = EXITCODE_NO_ERROR;
  do
  {
    token = readNextToken(c, position, key, idx);
    if (token == 1) // we have read a tag key
    {
      if (EC_Normal != tagList.putTagVal(key, tagList.getVM()))
      {
        result = EXITCODE_SYNTAX_ERROR_IN_TAG_FILE;
        token = -1;
      }
    }
    else if (token >= 0)
    {
      DCMSIGN_ERROR("parse error in text file '" << filename << "'");
      result = EXITCODE_SYNTAX_ERROR_IN_TAG_FILE;
      token = -1;
    }
  } while (token >= 0);
  delete[] c;
  return result;
}


OFBool DcmSignatureHelper::addTag(const char *c, DcmAttributeTag& tagList)
{
  OFBool result = OFFalse;
  unsigned int group = 0xffff;
  unsigned int elem = 0xffff;
  if (sscanf(c, "%x,%x", &group, &elem ) != 2 )
  {
    /* it is a name */
    const DcmDataDictionary& globalDataDict = dcmDataDict.rdlock();
    const DcmDictEntry *dicent = globalDataDict.findEntry(c);
    if (dicent)
    {
      if (EC_Normal == tagList.putTagVal(dicent->getKey(), tagList.getVM())) result = OFTrue;
    }
    dcmDataDict.rdunlock();
  } else {
    if (EC_Normal == tagList.putTagVal(DcmTagKey(OFstatic_cast(Uint16, group),OFstatic_cast(Uint16, elem)), tagList.getVM())) result = OFTrue;
  }
  return result;
}


DcmItem *DcmSignatureHelper::locateItemforSignatureCreation(DcmItem& dataset, const char *location)
{
  DcmTagKey key;
  Uint32 idx;
  int pos = 0;
  int token = 0;
  int expected = 1;
  OFBool finished = OFFalse;
  DcmItem *result = &dataset;
  DcmSequenceOfItems *sq = NULL;
  DcmStack stack;
  do
  {
    token = readNextToken(location, pos, key, idx);
    if ((token != expected)&&(token != -1))
    {
      DCMSIGN_ERROR("parse error in item location string '" << location << "'");
      return NULL;
    }
    if (token == -1)
    {
      if (! finished)
      {
        DCMSIGN_ERROR("item location string '" << location << "' incomplete.");
        return NULL;
      }
      return result;
    }
    if (token == 1)
    {
      // we have read a tag key
      stack.clear();
      if (EC_Normal != result->search(key, stack, ESM_fromHere, OFFalse))
      {
        DCMSIGN_ERROR("attribute " << key << " not found in dataset (item location string is '" << location << "')");
        return NULL;
      }
      if (stack.top()->ident() == EVR_SQ)
      {
        sq = OFstatic_cast(DcmSequenceOfItems *, (stack.top()));
      } else {
        DCMSIGN_ERROR("attribute " << key << " is not a sequence (item location string is '" << location << "')");
        return NULL;
      }
      expected = 2;
      finished = OFFalse;
    }
    else if (token == 2)
    {
      // we have read an index
      if (sq == NULL)
      {
        DCMSIGN_ERROR("sequence not found in item location string '" << location << "'");
        return NULL;
      }
      if (idx >= sq->card())
      {
        DCMSIGN_ERROR("sequence " << sq->getTag() << " only has " << sq->card()
          << " items, cannot locate item " << idx << " (item location string is '" << location << "')");
        return NULL;
      }
      result = sq->getItem(idx);
      if (result == NULL)
      {
        DCMSIGN_ERROR("item not found in item location string '" << location << "'");
        return NULL;
      }
      expected = 3;
      finished = OFTrue;
    }
    else if (token == 3)
    {
      // we have read a period
      expected = 1;
      finished = OFFalse;
    }
  } while (token > 0);
  return NULL;
}


void DcmSignatureHelper::printSignatureItemPosition(DcmStack& stack, OFString& str)
{
  str.clear();
  DcmObject *elem = NULL;
  DcmSequenceOfItems *sq = NULL;
  unsigned long sqCard=0;
  const char *tagname = NULL;
  unsigned long m=0;
  char buf[30];
  if (stack.card() > 2)
  {
    // signature is located within a sequence
    for (unsigned long l=stack.card()-2; l>0; --l) // loop over all elements except the stack top and bottom
    {
      elem = stack.elem(l);
      if (elem)
      {
        if ((elem->ident() == EVR_item) && sq)
        {
          sqCard = sq->card();
          for (m=0; m<sqCard; m++)
          {
            if (sq->getItem(m) == elem)
            {
              sprintf(buf, "[%lu]", m);
              str.append(buf);
            }
          }
        }
        else
        {
          if (str.size() > 0) str.append(".");
          sq = OFstatic_cast(DcmSequenceOfItems *, elem);
          DcmTag currentTag(elem->getTag());
          tagname = currentTag.getTagName();
          if (tagname) str.append(tagname); else
          {
            sprintf(buf, "(%04x,%04x)", elem->getTag().getGroup(), elem->getTag().getElement());
            str.append(buf);
          }
          if (elem->ident() == EVR_SQ) sq = OFstatic_cast(DcmSequenceOfItems *, elem); else sq = NULL;
        }
      }
    }
  } else {
    // signature is located in the main dataset
    str = "Main Dataset";
  }
}


int DcmSignatureHelper::do_sign(
  DcmItem *dataset,
  SiPrivateKey& key,
  SiCertificate& cert,
  SiMAC *opt_mac,
  SiSecurityProfile *opt_profile,
  DcmAttributeTag *opt_tagList,
  E_TransferSyntax opt_signatureXfer,
  FILE *dumpFile,
  SiSignaturePurpose::E_SignaturePurposeType opt_sigPurpose,
  SiTimeStamp *timeStamp)
{
  OFCondition sicond = opt_profile->inspectSignatureDataset(*dataset);
  if (sicond.good())
  {
    DcmSignature signer;
    signer.attach(dataset);
    signer.setDumpFile(dumpFile);
    sicond = signer.createSignature(key, cert, *opt_mac, *opt_profile, opt_signatureXfer, opt_tagList, timeStamp, opt_sigPurpose);
    signer.detach();
  }

  if (sicond.bad())
  {
    DCMSIGN_ERROR(sicond.text() << " while creating signature in main dataset");
    return EXITCODE_SIGNATURE_CREATION_FAILED;
  }
  return EXITCODE_NO_ERROR;
}


int DcmSignatureHelper::do_sign_item(
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
  SiTimeStamp *timeStamp)
{
  OFCondition sicond = EC_Normal;
  DcmItem *sigItem = locateItemforSignatureCreation(*dataset, opt_location);
  if (sigItem == NULL) sicond = SI_EC_ItemLocationNotFound;
  else
  {
    sicond = opt_profile->inspectSignatureDataset(*sigItem);
    if (sicond.good())
    {
      DcmSignature signer;
      signer.attach(sigItem);
      signer.setDumpFile(dumpFile);
      sicond = signer.createSignature(key, cert, *opt_mac, *opt_profile, opt_signatureXfer, opt_tagList, timeStamp, opt_sigPurpose);
      signer.detach();
    }
  }

  if (sicond.bad())
  {
    DCMSIGN_ERROR(sicond.text() << " while creating signature in item '" << opt_location << "'");
    return EXITCODE_SIGNATURE_CREATION_FAILED;
  }
  return EXITCODE_NO_ERROR;
}


int DcmSignatureHelper::do_verify(
    DcmItem *dataset,
    SiCertificateVerifier& certVerifier,
    E_SignatureVerificationPolicy verificationPolicy,
    E_TimestampVerificationPolicy timestampPolicy)
{
  OFCondition sicond = EC_Normal;
  DcmStack stack;
  DcmSignature signer;
  OFString aString;

  // this counter counts the number of signatures found in the dataset
  int counter = 0;

  // this counter contains the number of signatures in the dataset
  // that have failed validation
  int corrupt_counter = 0;

  unsigned long numSignatures = 0;
  unsigned long l=0;
  DcmItem *sigItem = DcmSignature::findFirstSignatureItem(*dataset, stack);

  // this flag is set to true if we find any signature in the dataset
  // that complies with the verification policy.
  OFBool verificationPolicyFulfilled = OFFalse;
  const char *verificationPolicyName = "(undefined)";
  OFString s;

  // for these verification policies, there is nothing to check,
  // ESVP_requireSignature is checked elsewhere.
  if ((verificationPolicy == ESVP_verifyIfPresent) ||
      (verificationPolicy == ESVP_requireSignature))
      verificationPolicyFulfilled = OFTrue;

  while (sigItem)
  {
    signer.attach(sigItem);
    numSignatures = signer.numberOfSignatures();
    for (l=0; l<numSignatures; l++)
    {
      OFBool cert_expiry = OFFalse;
      OFBool checkTimestamp = OFFalse;
      if (EC_Normal == signer.selectSignature(l))
      {
        ++counter;
        printSignatureDetails(signer, stack, counter);

        if (dcmsignLogger.isEnabledFor(OFLogger::INFO_LOG_LEVEL))
          aString = "  Signature Verification      : ";
          else aString = "  Signature Verification : ";

        // verify signature profile
        switch (verificationPolicy)
        {
          case ESVP_requireCreatorRSASignature:
            {
              verificationPolicyName = "Creator RSA Signature";
              SiCreatorProfile sprofCr;
              if (! verificationPolicyFulfilled) verificationPolicyFulfilled = signer.verifySignatureProfile(sprofCr).good();
            }
            break;
          case ESVP_requireAuthorizationRSASignature:
            {
              verificationPolicyName = "Authorization RSA Signature";
              SiAuthorizationProfile sprofAu;
              if (! verificationPolicyFulfilled) verificationPolicyFulfilled = signer.verifySignatureProfile(sprofAu).good();
            }
            break;
          case ESVP_requireSRRSASignature:
            {
              // there are two types of possible matches here:
              // either the dataset is an UNVERIFIED SR and SiStructuredReportingProfile matches,
              // or the dataset is a VERIFIED SR and SiStructuredReportingVerificationProfile matches.
              // We select the profile based on the value of VerificationFlag on main dataset level.
              verificationPolicyName = "SR RSA Signature";
              if (! verificationPolicyFulfilled)
              {
                if (dataset->findAndGetOFString(DCM_VerificationFlag, s).good() && (s == "VERIFIED"))
                {
                  SiStructuredReportingVerificationProfile sprofSRV;
                  verificationPolicyFulfilled = signer.verifySignatureProfile(sprofSRV).good();
                }
                else
                {
                  SiStructuredReportingProfile sprofSR;
                  verificationPolicyFulfilled = signer.verifySignatureProfile(sprofSR).good();
                }
              }
            }
            break;
          case ESVP_verifyIfPresent:
          case ESVP_requireSignature:
            break;
          // There is deliberately no default here because if we extend
          // the enum then we will have to extend this code.
        }

        // first verify if the signature matches the dataset
        sicond = signer.verifyCurrent();
        if (sicond.good())
        {
          // now check if we can successfully verify the signer's certificate
          SiCertificate *cert = signer.getCurrentCertificate();
          if (cert)
          {
            // print a warning if we have a weak (i.e. too short) key in the certificate
            cert->checkForWeakKey();

            if (certVerifier.verifyCertificate(*cert).good())
            {
              DCMSIGN_WARN(aString << "OK");
              checkTimestamp = OFTrue;
            }
            else
            {
              cert_expiry = certVerifier.lastErrorIsCertExpiry();
              DCMSIGN_WARN(aString << "signature is OK but certificate verification failed: " << certVerifier.lastError());
              corrupt_counter++;
            }
          }
          else
          {
            DCMSIGN_WARN(aString << "failed, certificate empty or invalid");
            corrupt_counter++;
          }
        } else {
          DCMSIGN_WARN(aString << sicond.text());
          corrupt_counter++;
        }

        // check certified timestamp (if any).
        // We only do this if the signature verification has passed
        // or if the signature is OK but the signer certificate has expired,
        // which may be "healed" by a timestamp that is still valid
        if (checkTimestamp || cert_expiry)
        {
          printTimestampDetails(signer, timestampPolicy);

          SiTimeStamp *tstamp = signer.getCurrentTimestamp();
          OFBool haveTS = (tstamp != NULL) && tstamp->have_tsinfo();
          if (haveTS)
          {
            if (timestampPolicy != ETVP_ignoreTS)
            {
              // timestamp is present and according to our policy we should verify it
              if (dcmsignLogger.isEnabledFor(OFLogger::INFO_LOG_LEVEL))
                aString = "  Timestamp Verification      : ";
                else aString = "  Timestamp Verification : ";

              sicond = tstamp->verifyTSSignature(certVerifier);
              if (sicond.good())
              {
                sicond = tstamp->verifyTSToken(certVerifier, *signer.getSelectedSignatureItem(), *signer.getCurrentCertificate());
                if (sicond.good())
                {
                  if (cert_expiry)
                  {
                    // we have an expired signature but a timestamp that is still valid,
                    // which means that the timestamp "heals" the failed signature verification
                    corrupt_counter--;
                    DCMSIGN_WARN(aString << "OK, extends validity period of expired signature.");
                  }
                  else
                  {
                    DCMSIGN_WARN(aString << "OK");
                  }
                }
                else
                {
                  OFString errString;
                  tstamp->lastError(errString); // check if we have an OpenSSL error message
                  if (errString.length() == 0) errString = sicond.text(); // use DCMTK error message otherwise
                  DCMSIGN_WARN(aString << "timestamp signature verification failed: " << errString);
                  if (!cert_expiry) corrupt_counter++; // if cert_expiry is true, the counter has already been increased
                }
              }
              else
              {
                OFString errString;
                tstamp->lastError(errString);
                DCMSIGN_WARN(aString << "timestamp signature verification failed: " << errString);
                if (!cert_expiry) corrupt_counter++; // if cert_expiry is true, the counter has already been increased
              }
            }
          }
          else
          {
            // no timestamp present. Check if this is a problem.
            if (timestampPolicy == ETVP_requireTS)
            {
              DCMSIGN_WARN("  Certified timestamp : absent, but required by timestamp policy");
              if (!cert_expiry) corrupt_counter++; // if cert_expiry is true, the counter has already been increased
            }
          }
        } // if (checkTimestamp)
      } // if (EC_Normal == signer.selectSignature(l))
    } // for (l=0; l<numSignatures; l++)
    signer.detach();
    sigItem = DcmSignature::findNextSignatureItem(*dataset, stack);
  } // while (sigItem)

  if (counter == 0)
  {
    if (verificationPolicy == ESVP_verifyIfPresent)
    {
      DCMSIGN_WARN("no signatures found in dataset.");
    }
    else
    {
      // no signature present but according to the signature policy, we would have expected one
      DCMSIGN_ERROR("no signatures found although required by the signature verification policy.");
      return EXITCODE_NO_SIGNATURES_PRESENT;
    }
  }
  else
  {
    DCMSIGN_INFO(counter << " signatures verified in dataset, " << corrupt_counter << " corrupted.");
  }

  if (! verificationPolicyFulfilled)
  {
    DCMSIGN_ERROR("No signature in this dataset fulfills the required " << verificationPolicyName << " signature policy");
    return EXITCODE_SIGNATURE_VERIFICATION_POLICY;
  }

  // return non-zero if any verification has failed
  return (corrupt_counter == 0 ? EXITCODE_NO_ERROR : EXITCODE_SIGNATURE_VERIFICATION_FAILED);
}


int DcmSignatureHelper::do_remove_all(DcmItem *dataset)
{
  OFCondition sicond = EC_Normal;
  DcmSignature signer;
  int counter = 0;
  OFString aString;
  DcmStack stack;
  DcmItem *sigItem = DcmSignature::findFirstSignatureItem(*dataset, stack);
  while (sigItem)
  {
    signer.attach(sigItem);
    while (signer.numberOfSignatures() > 0)
    {
      ++counter;
      if (EC_Normal == signer.selectSignature(0))
      {
        if (EC_Normal == signer.getCurrentSignatureUID(aString))
          DCMSIGN_WARN("Signature #" << counter << " UID=" << aString);
        else
          DCMSIGN_WARN("Signature #" << counter << " UID=(unknown)");
        printSignatureItemPosition(stack, aString);
        DCMSIGN_WARN("  Location                    : " << aString);
      }
      sicond = signer.removeSignature(0);
      if (sicond != EC_Normal)
      {
        DCMSIGN_ERROR(sicond.text() << ": while removing signature");
        return EXITCODE_SIGNATURE_REMOVAL_FAILED;
      }
    }
    signer.detach();
    stack.pop(); // remove pointer to the Digital Signatures Sequence that we've just deleted.
    sigItem = DcmSignature::findNextSignatureItem(*dataset, stack);
  }
  DCMSIGN_INFO(counter << " signatures found and removed from dataset.");
  return EXITCODE_NO_ERROR;
}


int DcmSignatureHelper::do_remove(
  DcmItem *dataset,
  const char *opt_location)
{
  OFCondition sicond = EC_Normal;
  DcmSignature signer;
  OFString aString;
  DcmStack stack;
  unsigned long cardSQ;
  unsigned long i;
  DcmItem *sigItem = DcmSignature::findFirstSignatureItem(*dataset, stack);
  while (sigItem)
  {
    signer.attach(sigItem);
    cardSQ = signer.numberOfSignatures();
    for (i=0; i<cardSQ; i++)
    {
      if (EC_Normal == signer.selectSignature(i))
      {
        if (EC_Normal == signer.getCurrentSignatureUID(aString))
        {
          if (aString == opt_location)
          {
            DCMSIGN_WARN("Signature UID=" << aString);
            printSignatureItemPosition(stack, aString);
            DCMSIGN_WARN("  Location                    : " << aString);
            sicond = signer.removeSignature(i);
            if (sicond != EC_Normal)
            {
              DCMSIGN_ERROR(sicond.text() << ": while removing signature");
              return EXITCODE_SIGNATURE_REMOVAL_FAILED;
            } else {
              return EXITCODE_NO_ERROR;
            }
          }
        }
      }
    }
    signer.detach();
    sigItem = DcmSignature::findNextSignatureItem(*dataset, stack);
  }
  DCMSIGN_ERROR("signature with UID '" << opt_location << "' not found.");
  return EXITCODE_SIGNATURE_UID_NOT_FOUND;
}

int DcmSignatureHelper::do_insert_ts(DcmItem *dataset, SiTimeStampFS *timeStamp)
{
  // check parameters
  if (dataset == NULL || timeStamp == NULL) return EXITCODE_CANNOT_ACCESS_TS;

  // load timestamp query
  if (timeStamp->load_ts_query_from_file().bad())
  {
    return EXITCODE_CANNOT_READ_TSQ_FILE;
  }

  // load timestamp response
  if (timeStamp->load_ts_response_from_file().bad())
  {
    return EXITCODE_CANNOT_READ_TSR_FILE;
  }

  // load digital signature UID
  OFString uid;
  OFCondition result = timeStamp->getUIDFromFile(uid);
  if (result.bad()) return EXITCODE_CANNOT_READ_UID_FILE;

  // locate digital signature
  DcmSignature signer;
  OFString currentUID;
  DcmStack stack;
  unsigned long cardSQ;
  unsigned long i;
  OFBool found = OFFalse;

  DcmItem *sigItem = DcmSignature::findFirstSignatureItem(*dataset, stack);
  while (sigItem)
  {
    // attach the item in which a digital signatures sequence was found
    signer.attach(sigItem);
    // determine the number of signatures in this item
    cardSQ = signer.numberOfSignatures();
    for (i=0; i<cardSQ; i++)
    {
      if (signer.selectSignature(i).good() && signer.getCurrentSignatureUID(currentUID).good() && (currentUID == uid))
      {
        // we have found the right digital signature
        found = OFTrue;
        DcmItem *currentSignatureItem = signer.getSelectedSignatureItem();
        if (currentSignatureItem == NULL)
        {
          return EXITCODE_CANNOT_ACCESS_SIGNATURE; // should never happen
        }
        else
        {
          // check if the timestamp response matches the query and the dataset
          result = timeStamp->check_ts_response(*currentSignatureItem);
          if (result.bad()) return EXITCODE_TS_CONSISTENCY_CHECK_FAILED;

          // write timestamp to dataset
          result = timeStamp->write_ts_token(*currentSignatureItem);
          if (result.bad()) return EXITCODE_CANNOT_INSERT_TS;
        }
      }
    }
    signer.detach();
    if (found) sigItem = NULL; else sigItem = DcmSignature::findNextSignatureItem(*dataset, stack);
  }

  if (!found)
  {
    DCMSIGN_ERROR("signature with UID '" << uid << "' not found.");
    return EXITCODE_SIGNATURE_UID_NOT_FOUND;
  }
  return EXITCODE_NO_ERROR;
}


void DcmSignatureHelper::printSignatureDetails(DcmSignature& sig, DcmStack& stack, int count)
{
  OFString aString;
  OFString codeValue;
  OFString codeMeaning;
  OFString codingSchemeDesignator;
  Uint16 macID = 0;
  DcmAttributeTag at(DCM_DataElementsSigned);
  DcmTagKey tagkey;
  DcmTag tag;
  const char *tagName = NULL;

  if (EC_Normal == sig.getCurrentSignatureUID(aString))
    DCMSIGN_WARN("Signature #" << count << " UID=" << aString);
  else
    DCMSIGN_WARN("Signature #" << count << " UID=" << "(unknown)");
  printSignatureItemPosition(stack, aString);
  if (dcmsignLogger.isEnabledFor(OFLogger::INFO_LOG_LEVEL))
  {
    DCMSIGN_INFO("  Location                    : " << aString);
    if (EC_Normal == sig.getCurrentMacID(macID))
      DCMSIGN_INFO("  MAC ID                      : " << macID);
    else
      DCMSIGN_INFO("  MAC ID                      : (unknown)");
    if (EC_Normal == sig.getCurrentMacName(aString))
      DCMSIGN_INFO("  MAC algorithm               : " << aString);
    else
      DCMSIGN_INFO("  MAC algorithm               : (unknown)");
    if (EC_Normal == sig.getCurrentMacXferSyntaxName(aString))
      DCMSIGN_INFO("  MAC calculation xfer syntax : " << aString);
    else
      DCMSIGN_INFO("  MAC calculation xfer syntax : (unknown)");
    // data elements signed
    if (EC_Normal != sig.getCurrentDataElementsSigned(at))
      DCMSIGN_INFO("  Data elements signed        : (unknown)");
    else
    {
      DCMSIGN_INFO("  Data elements signed        :");
      unsigned long atVM = at.getVM();
      for (unsigned long n=0; n<atVM; n++)
      {
        if (EC_Normal == at.getTagVal(tagkey, n))
        {
          tag = tagkey;
          tagName = tag.getTagName();
          DCMSIGN_INFO("      " << tagkey << " " << (tagName != NULL ? tagName : ""));
        }
      }
    }
    if (EC_Normal == sig.getCurrentSignatureDateTime(aString))
      DCMSIGN_INFO("  Signature date/time         : " << aString);
    else
      DCMSIGN_INFO("  Signature date/time         : (unknown)");

    if (sig.getCurrentSignaturePurpose(codeValue, codeMeaning, codingSchemeDesignator).good())
      DCMSIGN_INFO("  Signature purpose           : " << codeMeaning << " (" << codeValue << ", " << codingSchemeDesignator << ")" );
    else
      DCMSIGN_INFO("  Signature purpose           : (not specified)");

    DCMSIGN_INFO("  Certificate of signer       : ");
    SiCertificate *cert = sig.getCurrentCertificate();
    if ((cert == NULL)||(cert->getKeyType()==EKT_none))
      DCMSIGN_INFO("      none");
    else
    {
      DCMSIGN_INFO("      X.509v" << cert->getX509Version());
      cert->getCertSubjectName(aString);
      DCMSIGN_INFO("      Subject                 : " << aString);
      cert->getCertIssuerName(aString);
      DCMSIGN_INFO("      Issued by               : " << aString);
      DCMSIGN_INFO("      Serial no.              : " << cert->getCertSerialNo());
      cert->getCertValidityNotBefore(aString);
      DCMSIGN_INFO("      Validity                : not before " << aString);
      cert->getCertValidityNotAfter(aString);
      DCMSIGN_INFO("      Validity                : not after " << aString);
      OFString ecname;
      switch (cert->getKeyType())
      {
        case EKT_RSA:
          DCMSIGN_INFO("      Public key              : RSA, " << cert->getCertKeyBits() << " bits");
          break;
        case EKT_DSA:
          DCMSIGN_INFO("      Public key              : DSA, " << cert->getCertKeyBits() << " bits");
          break;
        case EKT_EC:
          ecname = cert->getCertCurveName();
          if (ecname.length() > 0)
          {
            DCMSIGN_INFO("      Public key              : EC, curve " << ecname << ", " << cert->getCertKeyBits() << " bits");
          }
          else
          {
            DCMSIGN_INFO("      Public key              : EC, " << cert->getCertKeyBits() << " bits");
          }
          break;
        case EKT_DH:
          DCMSIGN_INFO("      Public key              : DH, " << cert->getCertKeyBits() << " bits");
          break;
        default:
        case EKT_none: // should never happen
          DCMSIGN_INFO("      Public key              : unknown type");
          break;
      }
    }
  } else {
    DCMSIGN_INFO("  Location     : " << aString);
  }
}


void DcmSignatureHelper::printTimestampDetails(DcmSignature& sig, E_TimestampVerificationPolicy tsPolicy)
{
  SiTimeStamp *tstamp = sig.getCurrentTimestamp();
  OFBool haveTS = (tstamp != NULL) && tstamp->have_tsinfo();

  if (dcmsignLogger.isEnabledFor(OFLogger::INFO_LOG_LEVEL))
  {
    switch (tsPolicy)
    {
      case ETVP_verifyTSIfPresent:
        if (haveTS)
        {
          DCMSIGN_INFO("  Certified timestamp         : CMS_TSP version " << tstamp->get_tsinfo_version());
        }
        else
        {
          DCMSIGN_INFO("  Certified timestamp         : absent");
          return;
        }
        break;
      case ETVP_ignoreTS:
        if (haveTS)
        {
          DCMSIGN_INFO("  Certified timestamp         : present, but ignored according to timestamp policy");
          return;
        }
        else
        {
          DCMSIGN_INFO("  Certified timestamp         : absent");
          return;
        }
        break;

      case ETVP_requireTS:
        if (haveTS)
        {
          DCMSIGN_INFO("  Certified timestamp         : CMS_TSP version " << tstamp->get_tsinfo_version());
        }
        else
        {
          DCMSIGN_INFO("  Certified timestamp         : absent, but required by timestamp policy");
          return;
        }
        break;
    }
  }

  if (haveTS)
  {
    // Now print timestamp details
    OFString s;
    tstamp->get_tsinfo_policy_oid(s);
    DCMSIGN_INFO("      Policy OID              : " << s);
    tstamp->get_tsinfo_imprint_algorithm_name(s);
    DCMSIGN_INFO("      Imprint MAC algorithm   : " << s);
    tstamp->get_tsinfo_serial_number(s);
    DCMSIGN_INFO("      Serial number           : " << s);
    tstamp->get_tsinfo_nonce(s);
    if (s.length() == 0) s = "none";
    DCMSIGN_INFO("      Nonce                   : " << s);
    tstamp->get_tsinfo_tsa_name(s);
    if (s.length() == 0) s = "none";
    DCMSIGN_INFO("      TSA Name                : " << s);
    DCMSIGN_INFO("      Ordering                : " << (tstamp->get_tsinfo_ordering() ? "true" : "false" ));
    tstamp->get_tsinfo_accuracy(s);
    if (s.length() == 0) s = "not specified";
    DCMSIGN_INFO("      Accuracy                : " << s);
    tstamp->get_tsinfo_timestamp(s);
    DCMSIGN_INFO("      Timestamp               : " << s);
    tstamp->get_tsinfo_policy_oid(s);

    // finally, print extensions (if any)
    int numext = tstamp->get_tsinfo_numextensions();
    if (numext == 0)
    {
      DCMSIGN_INFO("      Extensions              : none");
    }
    else
    {
      DCMSIGN_INFO("      Extensions              :");
      for (int i=0; i<numext; ++i)
      {
        tstamp->get_tsinfo_extension(s, i);
        DCMSIGN_INFO("        - " << s);
      }
    }
  }
}



#else /* WITH_OPENSSL */

int sisighlp_cc_dummy_to_keep_linker_from_moaning = 0;

#endif
