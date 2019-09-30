/*
 *
 *  Copyright (C) 2000-2019, OFFIS e.V.
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
#include "dcmtk/dcmsign/sitypes.h"  /* for logger macros */
#include "dcmtk/dcmsign/sicert.h"   /* for class SiCertificate */
#include "dcmtk/dcmdata/dcitem.h"   /* for class DcmItem */
#include "dcmtk/dcmdata/dcdict.h"   /* for class DcmDataDictionary */
#include "dcmtk/dcmdata/dcdicent.h" /* for class DcmDictEntry */
#include "dcmtk/dcmdata/dcsequen.h" /* for class DcmSequenceOfItems */
#include "dcmtk/dcmdata/dcdeftag.h" /* for attribute tag constants */
#include "dcmtk/dcmdata/dcvrat.h"   /* for class DcmAttributeTag */
#include "dcmtk/dcmsign/sisprof.h"  /* for class SiSecurityProfile */
#include "dcmtk/dcmsign/dcsignat.h" /* for class DcmSignature */



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
    key = DcmTagKey(group,elem);
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
    if (numBytes > 65536)
    {
      DCMSIGN_WARN("text file too large, ignoring everything beyond 64K.");
      numBytes = 65536;
    }
    result = new char[numBytes];
    if (result)
    {
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
  if (c==NULL) return 10; // bail out
  int position = 0;
  int token = 0;
  Uint32 idx = 0;
  DcmTagKey key;
  int result = 0;
  do
  {
    token = readNextToken(c, position, key, idx);
    if (token == 1) // we have read a tag key
    {
      if (EC_Normal != tagList.putTagVal(key, tagList.getVM()))
      {
        result = 10;
        token = -1;
      }
    }
    else if (token >= 0)
    {
      DCMSIGN_ERROR("parse error in text file '" << filename << "'");
      result = 10;
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
    if (EC_Normal == tagList.putTagVal(DcmTagKey(group,elem), tagList.getVM())) result = OFTrue;
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
  char buf[20];
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
  SiSignaturePurpose::E_SignaturePurposeType opt_sigPurpose)
{
  OFCondition sicond = EC_Normal;
  if (opt_profile->inspectSignatureDataset(*dataset).bad()) return 1;
  DcmSignature signer;
  signer.attach(dataset);
  signer.setDumpFile(dumpFile);
  sicond = signer.createSignature(key, cert, *opt_mac, *opt_profile, opt_signatureXfer, opt_tagList, NULL, opt_sigPurpose);
  if (sicond != EC_Normal)
  {
    DCMSIGN_ERROR(sicond.text() << ": while creating signature in main dataset");
    return 1;
  }
  return 0;
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
  SiSignaturePurpose::E_SignaturePurposeType opt_sigPurpose)
{
  OFCondition sicond = EC_Normal;
  DcmSignature signer;
  DcmItem *sigItem = locateItemforSignatureCreation(*dataset, opt_location);
  if (sigItem == NULL) return 1;
  if (opt_profile->inspectSignatureDataset(*sigItem).bad()) return 1;

  signer.detach();
  signer.attach(sigItem);
  signer.setDumpFile(dumpFile);
  sicond = signer.createSignature(key, cert, *opt_mac, *opt_profile, opt_signatureXfer, opt_tagList, NULL, opt_sigPurpose);
  if (sicond != EC_Normal)
  {
    DCMSIGN_ERROR(sicond.text() << ": while creating signature in item '" << opt_location << "'");
    return 1;
  }
  signer.detach();
  return 0;
}


int DcmSignatureHelper::do_verify(DcmItem *dataset)
{
  OFCondition sicond = EC_Normal;
  DcmStack stack;
  DcmSignature signer;
  OFString aString;
  int counter = 0;
  int corrupt_counter = 0;
  unsigned long numSignatures = 0;
  unsigned long l=0;
  Uint16 macID = 0;
  DcmAttributeTag at(DCM_DataElementsSigned);
  DcmItem *sigItem = DcmSignature::findFirstSignatureItem(*dataset, stack);
  DcmTagKey tagkey;
  DcmTag tag;
  const char *tagName = NULL;

  while (sigItem)
  {
    signer.attach(sigItem);
    numSignatures = signer.numberOfSignatures();
    for (l=0; l<numSignatures; l++)
    {
      if (EC_Normal == signer.selectSignature(l))
      {
        ++counter;
        if (EC_Normal == signer.getCurrentSignatureUID(aString))
          DCMSIGN_WARN("Signature #" << counter << " UID=" << aString);
        else
          DCMSIGN_WARN("Signature #" << counter << " UID=" << "(unknown)");
        printSignatureItemPosition(stack, aString);
        if (dcmsignLogger.isEnabledFor(OFLogger::INFO_LOG_LEVEL))
        {
          DCMSIGN_INFO("  Location                    : " << aString);
          if (EC_Normal == signer.getCurrentMacID(macID))
            DCMSIGN_INFO("  MAC ID                      : " << macID);
          else
            DCMSIGN_INFO("  MAC ID                      : (unknown)");
          if (EC_Normal == signer.getCurrentMacName(aString))
            DCMSIGN_INFO("  MAC algorithm               : " << aString);
          else
            DCMSIGN_INFO("  MAC algorithm               : (unknown)");
          if (EC_Normal == signer.getCurrentMacXferSyntaxName(aString))
            DCMSIGN_INFO("  MAC calculation xfer syntax : " << aString);
          else
            DCMSIGN_INFO("  MAC calculation xfer syntax : (unknown)");
          // data elements signed
          if (EC_Normal != signer.getCurrentDataElementsSigned(at))
            DCMSIGN_INFO("  Data elements signed        : all elements");
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

          if (EC_Normal == signer.getCurrentSignatureDateTime(aString))
            DCMSIGN_INFO("  Signature date/time         : " << aString);
          else
            DCMSIGN_INFO("  Signature date/time         : (unknown)");
          DCMSIGN_INFO("  Certificate of signer       : ");
          SiCertificate *cert = signer.getCurrentCertificate();
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
            const char *ecname = NULL;
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
                if (ecname)
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
          aString = "  Verification                : ";
        } else {
          DCMSIGN_INFO("  Location     : " << aString);
          aString = "  Verification : ";
        }
        sicond = signer.verifyCurrent();
        if (sicond.good())
        {
          DCMSIGN_WARN(aString << "OK\n");
        } else {
          corrupt_counter++;
          DCMSIGN_WARN(aString << sicond.text() << "\n");
        }
      }
    }
    signer.detach();
    sigItem = DcmSignature::findNextSignatureItem(*dataset, stack);
  }
  if (counter == 0)
    DCMSIGN_WARN("no signatures found in dataset.");
  else
  {
    DCMSIGN_INFO(counter << " signatures verified in dataset, " << corrupt_counter << " corrupted.");
  }

  // return non-zero if any verification has failed
  return (corrupt_counter == 0 ? 0 :1);
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
        return 1;
      }
    }
    signer.detach();
    stack.pop(); // remove pointer to the Digital Signatures Sequence that we've just deleted.
    sigItem = DcmSignature::findNextSignatureItem(*dataset, stack);
  }
  DCMSIGN_INFO(counter << " signatures found and removed from dataset.");
  return 0;
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
              return 1;
            } else {
              return 0;
            }
          }
        }
      }
    }
    signer.detach();
    sigItem = DcmSignature::findNextSignatureItem(*dataset, stack);
  }
  DCMSIGN_ERROR("signature with UID '" << opt_location << "' not found.");
  return 1;
}


#else /* WITH_OPENSSL */

int sisighlp_cc_dummy_to_keep_linker_from_moaning = 0;

#endif
