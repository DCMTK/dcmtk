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
 *  Module: dcmsign
 *
 *  Author: Marco Eichelberg
 *
 *  Purpose:
 *    classes: DcmSignature
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2000-11-07 16:49:02 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"

#ifdef WITH_OPENSSL

#include "dcmsign.h"
#include "dcstack.h"
#include "dcitem.h"
#include "dcvrat.h"
#include "dcvrcs.h"
#include "dcvrdt.h"
#include "dcvrobow.h"
#include "dcvrus.h"
#include "dcuid.h"
#include "dcsequen.h"
#include "dcdeftag.h"

#include "sialgo.h"
#include "sicert.h"
#include "simac.h"
#include "simaccon.h"
#include "simd5.h"
#include "siprivat.h"
#include "siripemd.h"
#include "sisha1.h"
#include "sisprof.h"
#include "sitstamp.h"

BEGIN_EXTERN_C
#include <openssl/evp.h>
#include <openssl/err.h>
END_EXTERN_C


/* static helper methods */

void DcmSignature::initializeLibrary()
{
  OpenSSL_add_all_algorithms();
  ERR_load_crypto_strings();
}


Uint16 DcmSignature::getMACIDnumber(DcmItem &item)
{
  Uint16 macIDnumber = 0;
  DcmStack stack;
  if ((EC_Normal == item.search(DCM_MACIDnumber, stack, ESM_fromHere, OFFalse))&& (stack.top()->isLeaf()))
  {
    ((DcmElement *)(stack.top()))->getUint16(macIDnumber);
  }
  return macIDnumber;
}
  
  
void DcmSignature::currentDateTime(OFString &str)
{
  char buf[40];
  time_t tt = time(NULL);
  struct tm *ts = gmtime(&tt);
  int gm_hour = ts->tm_hour;
  int gm_min = ts->tm_min;

  ts = localtime(&tt);  
  char zoneSign = '+';
  int zoneMin = (ts->tm_hour - gm_hour)*60 + (ts->tm_min - gm_min);
  if (zoneMin < 0)
  {
    zoneMin = -zoneMin;
    zoneSign = '-';
  }
  int zoneHour = zoneMin / 60;
  zoneMin = zoneMin % 60;
  sprintf(buf, "%04d%02d%02d%02d%02d%02d.000000%c%02d%02d", (1900 + ts->tm_year), 
    ts->tm_mon + 1, ts->tm_mday, ts->tm_hour, ts->tm_min, ts->tm_sec, zoneSign, zoneHour, zoneMin);
  str = buf;
  return;
}


/* DcmSignature non-static methods */


DcmSignature::DcmSignature()
: currentItem(NULL)
, macParametersSq(NULL)
, signatureSq(NULL)
, dumpFile(NULL)
, selectedSignatureItem(NULL)
, selectedMacParametersItem(NULL)
, selectedCertificate(NULL)
{
}


DcmSignature::~DcmSignature()
{
  deselect();
}


void DcmSignature::deselect()
{
  selectedSignatureItem = NULL;
  selectedMacParametersItem = NULL;
  delete selectedCertificate;
  selectedCertificate = NULL;
}


void DcmSignature::setDumpFile(FILE *f)
{
  dumpFile = f;
}


void DcmSignature::attach(DcmItem *dataset)  
{
  deselect();
  currentItem = dataset;
  DcmStack stack;
  if (currentItem)
  {
    if ((EC_Normal == currentItem->search(DCM_MACParametersSequence, stack, ESM_fromHere, OFFalse)) && (stack.top()->ident() == EVR_SQ))
    {
      macParametersSq = (DcmSequenceOfItems *)stack.top();
    } else macParametersSq = NULL;
    stack.clear();
    if ((EC_Normal == currentItem->search(DCM_DigitalSignaturesSequence, stack, ESM_fromHere, OFFalse)) && (stack.top()->ident() == EVR_SQ))
    {
      signatureSq = (DcmSequenceOfItems *)stack.top();
    } else signatureSq = NULL;
  } else {
    macParametersSq = NULL;
    signatureSq = NULL;
  }
  return; 
}


void DcmSignature::detach()
{
  deselect();
  currentItem = NULL;
  macParametersSq = NULL;
  signatureSq = NULL;
}


unsigned long DcmSignature::numberOfSignatures()
{
  if (signatureSq) return signatureSq->card(); else return 0;
}


SI_E_Condition DcmSignature::removeSignature(unsigned long i)
{
  if (signatureSq == NULL) return SI_EC_IllegalCall;
  unsigned long seqCard = signatureSq->card();
  if (i >= seqCard) return SI_EC_IllegalCall;
  DcmItem *removalItem = signatureSq->getItem(i);
  if (removalItem == NULL) return SI_EC_IllegalCall; // should never happen

  // check mac ID number and whether it is unique  
  Uint16 macIDnumber = getMACIDnumber(*removalItem);
  OFBool macIDunique = OFTrue;
  DcmItem *tmpItem=NULL;
  unsigned long j=0;
  for (j=0; j < seqCard; ++j)
  {
    tmpItem = signatureSq->getItem(j);
    if ((i != j) && tmpItem && (macIDnumber == getMACIDnumber(*tmpItem))) macIDunique = OFFalse;
  }

  // delete signature item
  delete signatureSq->remove(i);

  // delete MAC item if appropriate
  if (macParametersSq && macIDunique)
  {
    j = 0;
    while (j < macParametersSq->card())
    {
      tmpItem = macParametersSq->getItem(j);
      if (tmpItem && (macIDnumber == getMACIDnumber(*tmpItem))) delete macParametersSq->remove(j); else ++j;
    }
  }
  
  if (signatureSq->card() == 0) 
  {
    delete currentItem->remove(signatureSq);
    signatureSq = NULL;
  }

  if (macParametersSq->card() == 0) 
  {
    delete currentItem->remove(macParametersSq);
    macParametersSq = NULL;
  }

  return SI_EC_Normal;
}


SI_E_Condition DcmSignature::allocateMACID(Uint16& newID)
{
  newID = 0xffff;
  if (currentItem == NULL) return SI_EC_IllegalCall;
  if ((signatureSq==NULL)&&(macParametersSq == NULL)) 
  {
    newID = 0; // no signature yet, can use 0.
    return SI_EC_Normal; 
  }

  unsigned long i;
  DcmItem *tmpItem = NULL;
  char *isAllocated = new char[65536];
  if (isAllocated==NULL) return SI_EC_MemoryExhausted;

  SI_E_Condition result = SI_EC_MacIDsExhausted;

  for (i=0; i < 65536; ++i) isAllocated[i]=0;
  if (signatureSq)
  {
    unsigned long sqCard = signatureSq->card();
    for (i=0; i < sqCard; ++i)
    {
      tmpItem = signatureSq->getItem(i);
      if (tmpItem) isAllocated[getMACIDnumber(*tmpItem)] = 1;      
    }
  }
  if (macParametersSq)
  {
    unsigned long macCard = macParametersSq->card();
    for (i=0; i < macCard; ++i)
    {
      tmpItem = macParametersSq->getItem(i);
      if (tmpItem) isAllocated[getMACIDnumber(*tmpItem)] = 1;      
    }
  }
  i = 0;
  while (i<65536)
  {
    if (!isAllocated[i])
    {
      newID = i;
      i = 65536;
      result = SI_EC_Normal;
    } else ++i;
  }
  delete[] isAllocated;
  return result;
}

  
SI_E_Condition DcmSignature::createSignature(
    SiPrivateKey& key, 
    SiCertificate& cert, 
    SiMAC& mac,
    SiSecurityProfile& profile, 
    E_TransferSyntax xfer,
    OFBool alwaysIncludeTagList,
    const DcmAttributeTag *tagList,
    SiTimeStamp *timeStamp)
{
  // do some checks first
  if (currentItem == NULL) return SI_EC_IllegalCall;
  if (! key.matchesCertificate(cert)) return SI_EC_CertificateDoesNotMatchPrivateKey;
  if (! profile.isAllowableMAC(mac)) return SI_EC_MacDoesNotMatchProfile;
  if (! profile.isAllowableAlgorithmType(key.getKeyType())) return SI_EC_AlgorithmDoesNotMatchProfile;
  if (! profile.isAllowableTransferSyntax(xfer)) return SI_EC_TransferSyntaxDoesNotMatchProfile;

  SI_E_Condition result = SI_EC_Normal;

  // update tag list if present
  DcmAttributeTag *updatedTagList = NULL;
  if (tagList) 
  {
    updatedTagList = new DcmAttributeTag(*tagList);
    if (updatedTagList == NULL) result = SI_EC_MemoryExhausted;
    else result = profile.updateAttributeList(*currentItem, *updatedTagList);
  }

  // first create MAC of DICOM object
  SiAlgorithm *algorithm = NULL;
  SiMACConstructor constructor;
  if (dumpFile) constructor.setDumpFile(dumpFile);
  DcmAttributeTag *tagListOut = new DcmAttributeTag(DCM_DataElementsSigned);
  if (tagListOut == NULL) result = SI_EC_MemoryExhausted;
  unsigned long sigLength = 0;
  unsigned char *signature = NULL;

  if (SI_EC_Normal == result)
  {
    algorithm = key.createAlgorithmForPrivateKey();
    if (algorithm)
    {
      mac.initialize();
      sigLength = algorithm->getSize();
      signature = new unsigned char[sigLength];
      if (signature == NULL) result = SI_EC_MemoryExhausted; 
      else
      {
        result = constructor.encodeDataset(*currentItem, mac, xfer, *tagListOut, updatedTagList);
      }
    } else result = SI_EC_MemoryExhausted;
  }

  // then sign MAC
  if (SI_EC_Normal == result)
  {      
    unsigned long digestLength = mac.getSize();
    unsigned char *digest = new unsigned char[digestLength];
    if (digest == NULL) result = SI_EC_MemoryExhausted;
    else
    {
      result = mac.finalize(digest);
      if (SI_EC_Normal == result) result = algorithm->sign(digest, digestLength, mac.macType(), signature, sigLength);
      delete[] digest;
    } 
  }      

  // make sure we have a MAC parameter sequence
  if ((SI_EC_Normal == result) && (macParametersSq == NULL))
  {      
    macParametersSq = new DcmSequenceOfItems(DCM_MACParametersSequence);
    if (macParametersSq == NULL) result = SI_EC_MemoryExhausted; 
    else
    {
      if (EC_Normal != currentItem->insert(macParametersSq)) result = SI_EC_DcmDataFailure;
    }
  }

  // make sure we have a digital signature sequence
  if ((SI_EC_Normal == result) && (signatureSq == NULL))
  {      
    signatureSq = new DcmSequenceOfItems(DCM_DigitalSignaturesSequence);
    if (signatureSq == NULL) result = SI_EC_MemoryExhausted; 
    else
    {
      if (EC_Normal != currentItem->insert(signatureSq)) result = SI_EC_DcmDataFailure;
    }
  }

  // allocate MAC ID
  Uint16 macID = 0;
  if (SI_EC_Normal == result) result = allocateMACID(macID);

  // write digital signature sequence item
  if (SI_EC_Normal == result)
  {
    DcmItem *seqItem = new DcmItem();
    if (seqItem)
    {
      // MAC ID Number
      if (SI_EC_Normal == result)
      {
        DcmUnsignedShort *elemMacID = new DcmUnsignedShort(DCM_MACIDnumber);
        if (elemMacID)
        {
          if ((EC_Normal != elemMacID->putUint16(macID)) || (EC_Normal != seqItem->insert(elemMacID)))
          {
            delete elemMacID;
            result = SI_EC_DcmDataFailure;
          }
        } else result = SI_EC_MemoryExhausted;
      }

      // Digital Signature UID
      if (SI_EC_Normal == result)
      {
        char newUID[66];
        dcmGenerateUniqueIdentifer(newUID);
        DcmUniqueIdentifier *elemSigUID = new DcmUniqueIdentifier(DCM_DigitalSignatureUID);
        if (elemSigUID)
        {
          if ((EC_Normal != elemSigUID->putString(newUID)) || (EC_Normal != seqItem->insert(elemSigUID)))
          {
            delete elemSigUID;
            result = SI_EC_DcmDataFailure;
          }
        } else result = SI_EC_MemoryExhausted;
      }

      // Digital Signature Date/Time
      if (SI_EC_Normal == result)
      {
      	OFString aString;
      	currentDateTime(aString);
        DcmDateTime *elemDT = new DcmDateTime(DCM_DigitalSignatureDateTime);
        if (elemDT)
        {
          if ((EC_Normal != elemDT->putOFStringArray(aString)) || (EC_Normal != seqItem->insert(elemDT)))
          {
            delete elemDT;
            result = SI_EC_DcmDataFailure;
          }
        } else result = SI_EC_MemoryExhausted;
      }
      
      // Certificate of Signer and Certificate Type
      if (SI_EC_Normal == result)
      {
        if (EC_Normal != cert.write(*seqItem)) result = SI_EC_DcmDataFailure;
      }
            
      // Signature
      if (SI_EC_Normal == result)
      {
      	OFString aString;
      	currentDateTime(aString);
        DcmOtherByteOtherWord *elemSig = new DcmOtherByteOtherWord(DCM_Signature);
        if (elemSig)
        {
          elemSig->setVR(EVR_OB);
          if ((EC_Normal != elemSig->putUint8Array((Uint8 *) signature, sigLength)) || (EC_Normal != seqItem->insert(elemSig)))
          {
            delete elemSig;
            result = SI_EC_DcmDataFailure;
          }
        } else result = SI_EC_MemoryExhausted;
      }
            
      // Timestamp and Timestamp Type
      if (SI_EC_Normal == result)
      {
        if (timeStamp)
        {
          // this would be the right time to request the time stamp
          result = timeStamp->stamp(signature, sigLength);
          if (SI_EC_Normal == result)
          {
            if (EC_Normal != timeStamp->write(*seqItem)) result = SI_EC_DcmDataFailure;
          }
        }
      }

      if (SI_EC_Normal == result)
      {
        if (EC_Normal != signatureSq->append(seqItem))
        {
          delete seqItem;
          result = SI_EC_DcmDataFailure;
        }
      }
    } else result = SI_EC_MemoryExhausted;
  }

  // write MAC parameters sequence item
  if (SI_EC_Normal == result)
  {
    DcmItem *macItem = new DcmItem();
    if (macItem)
    {
      // MAC ID Number
      if (SI_EC_Normal == result)
      {
        DcmUnsignedShort *elemMacID = new DcmUnsignedShort(DCM_MACIDnumber);
        if (elemMacID)
        {
          if ((EC_Normal != elemMacID->putUint16(macID)) || (EC_Normal != macItem->insert(elemMacID)))
          {
            delete elemMacID;
            result = SI_EC_DcmDataFailure;
          }
        } else result = SI_EC_MemoryExhausted;
      }

      // MAC Calculation Transfer Syntax UID
      if (SI_EC_Normal == result)
      {
        DcmUniqueIdentifier *elemXferUID = new DcmUniqueIdentifier(DCM_MACCalculationTransferSyntaxUID);
        if (elemXferUID)
        {
          DcmXfer xid(xfer);
          if ((EC_Normal != elemXferUID->putString(xid.getXferID())) || (EC_Normal != macItem->insert(elemXferUID)))
          {
            delete elemXferUID;
            result = SI_EC_DcmDataFailure;
          }
        } else result = SI_EC_MemoryExhausted;
      }

      // MAC Algorithm Identifier
      if (SI_EC_Normal == result)
      {
        DcmCodeString *elemMacAlgo = new DcmCodeString(DCM_MACAlgorithm);
        if (elemMacAlgo)
        {
          if ((EC_Normal != elemMacAlgo->putString(mac.getDefinedTerm())) || (EC_Normal != macItem->insert(elemMacAlgo)))
          {
            delete elemMacAlgo;
            result = SI_EC_DcmDataFailure;
          }
        } else result = SI_EC_MemoryExhausted;
      }

      // Data Elements Signed
      if (SI_EC_Normal == result)
      {
      	if (alwaysIncludeTagList || (! profile.isUniversalAttributeList(*currentItem, *tagListOut)))
      	{
          if (EC_Normal == macItem->insert(tagListOut))
          {
            tagListOut = NULL; // make sure we don't delete tagListOut later
          } else {
            // tagListOut is deleted elsewhere
            result = SI_EC_DcmDataFailure;
          }
      	}
      }

      if (SI_EC_Normal == result)
      {
        if (EC_Normal != macParametersSq->append(macItem))
        {
          delete macItem;
          result = SI_EC_DcmDataFailure;
        }
      }
    } else result = SI_EC_MemoryExhausted;
  }
    
  delete[] signature;
  delete tagListOut;
  delete updatedTagList;
  delete algorithm;
  return result;
}


SI_E_Condition DcmSignature::insertRevision()
{
  if (currentItem == NULL) return SI_EC_IllegalCall;
  OFString aString;
  char uid[66];
  DcmElement *elem = new DcmDateTime(DCM_RevisionDateTime);
  if (elem)
  {
    currentDateTime(aString);
    if (EC_Normal != elem->putOFStringArray(aString)) return SI_EC_DcmDataFailure;
    if (EC_Normal != currentItem->insert(elem, OFTrue)) return SI_EC_DcmDataFailure; // replace if alread present
  } else return SI_EC_MemoryExhausted;
  elem = new DcmUniqueIdentifier(DCM_RevisionUID);
  if (elem) 
  {
    dcmGenerateUniqueIdentifer(uid);
    if (EC_Normal != elem->putString(uid)) return SI_EC_DcmDataFailure;
    if (EC_Normal != currentItem->insert(elem, OFTrue)) return SI_EC_DcmDataFailure; // replace if alread present
  } else return SI_EC_MemoryExhausted;
  return SI_EC_Normal;
}


SI_E_Condition DcmSignature::selectSignature(unsigned long i)
{
  deselect();
  if ((signatureSq == NULL) || (i >= signatureSq->card())) return SI_EC_IllegalCall;
 
  selectedSignatureItem = signatureSq->getItem(i);
  if (selectedSignatureItem == NULL) return SI_EC_IllegalCall;
  Uint16 macID = getMACIDnumber(*selectedSignatureItem);
  if (macParametersSq)
  {
    DcmItem *tmpItem=NULL;
    unsigned long cardMac = macParametersSq->card();
    for (unsigned long j=0; j<cardMac; j++)
    {
      tmpItem = macParametersSq->getItem(j);
      if (macID == getMACIDnumber(*tmpItem))
      {
        selectedMacParametersItem = tmpItem;
        break;
      }
    }
  }
  selectedCertificate = new SiCertificate();
  if (selectedCertificate == NULL) return SI_EC_MemoryExhausted;  
  selectedCertificate->read(*selectedSignatureItem);
  return SI_EC_Normal;
}

SI_E_Condition DcmSignature::verifyCurrent()
{
  if (NULL == selectedSignatureItem) return SI_EC_IllegalCall;
  if (selectedMacParametersItem == NULL) return SI_EC_VerificationFailed_NoMAC;
  if ((selectedCertificate == NULL)||(selectedCertificate->getKeyType() == EKT_none)) return SI_EC_VerificationFailed_NoCertificate;

  SI_E_Condition result = SI_EC_Normal;
  DcmAttributeTag *tagList = NULL;
  SiMAC *mac = NULL;
  E_TransferSyntax xfer = EXS_Unknown;
  DcmStack stack;
  DcmOtherByteOtherWord *signature = NULL;
  
  // read MAC Calculation Transfer Syntax UID
  if (SI_EC_Normal == result)
  {
    if ((EC_Normal == selectedMacParametersItem->search(DCM_MACCalculationTransferSyntaxUID, stack, ESM_fromHere, OFFalse))&& (stack.top()->isLeaf()))
    {
      char *uid = NULL;
      if (EC_Normal == ((DcmElement *)(stack.top()))->getString(uid))
      {
        DcmXfer xf(uid);
        xfer = xf.getXfer();
        if (xfer == EXS_Unknown) result = SI_EC_WrongTransferSyntax;
      } else result = SI_EC_VerificationFailed_NoMAC;
    } else result = SI_EC_VerificationFailed_NoMAC;
  }

  // read MAC Algorithm
  if (SI_EC_Normal == result)
  {
    stack.clear();
    if ((EC_Normal == selectedMacParametersItem->search(DCM_MACAlgorithm, stack, ESM_fromHere, OFFalse))&& (stack.top()->isLeaf()))
    {
      OFString macidentifier;
      if (EC_Normal == ((DcmElement *)(stack.top()))->getOFString(macidentifier, 0))
      {
        if (macidentifier == SI_DEFTERMS_RIPEMD160) mac = new SiRIPEMD160();
        else if (macidentifier == SI_DEFTERMS_SHA1) mac = new SiSHA1();
        else if (macidentifier == SI_DEFTERMS_MD5)  mac = new SiMD5();
        if (mac == NULL) result = SI_EC_VerificationFailed_UnsupportedMACAlgorithm;
      } else result = SI_EC_VerificationFailed_NoMAC;
    } else result = SI_EC_VerificationFailed_NoMAC;
  }     

  // read Data Elements Signed
  if (SI_EC_Normal == result)
  {
    stack.clear();
    if ((EC_Normal == selectedMacParametersItem->search(DCM_DataElementsSigned, stack, ESM_fromHere, OFFalse))&& (stack.top()->ident() == EVR_AT))
    {
      tagList = new DcmAttributeTag(*((DcmAttributeTag *)(stack.top())));
      if (tagList == NULL) result = SI_EC_MemoryExhausted;
    }
  }     

  // read Signature
  if (SI_EC_Normal == result)
  {
    stack.clear();
    if ((EC_Normal == selectedSignatureItem->search(DCM_Signature, stack, ESM_fromHere, OFFalse))&& (stack.top()->isLeaf()))
    {
      signature = new DcmOtherByteOtherWord(*((DcmOtherByteOtherWord *)(stack.top())));
      if (signature == NULL) result = SI_EC_MemoryExhausted;
    } else result = SI_EC_VerificationFailed_NoSignature;
  }     

  // create MAC
  if (SI_EC_Normal == result)
  {
    DcmAttributeTag tagListOut(DCM_DataElementsSigned);
    SiMACConstructor constructor;
    if (dumpFile) constructor.setDumpFile(dumpFile);
    result = constructor.encodeDataset(*currentItem, *mac, xfer, tagListOut, tagList);
  }

  // finally verify signature
  if (SI_EC_Normal == result)
  {  
    SiAlgorithm *algorithm = selectedCertificate->createAlgorithmForPublicKey();
    if (algorithm)
    {
      OFBool verified = OFTrue;
      Uint32 sigLength = signature->getLength();
      Uint8 *sigData = NULL;
      if ((EC_Normal != signature->getUint8Array(sigData)) || (sigData == NULL)) result = SI_EC_VerificationFailed_NoSignature;
      else 
      {
        unsigned long digestLength = mac->getSize();
        unsigned char *digest = new unsigned char[digestLength];
        if (digest == NULL) result =  SI_EC_MemoryExhausted;
        else
        {
          result = mac->finalize(digest);
          if (SI_EC_Normal == result)
          {
            result = algorithm->verify(digest, digestLength, mac->macType(), sigData, sigLength, verified);
            if ((SI_EC_Normal == result) && (! verified)) result = SI_EC_VerificationFailed_Corrupted;
          }
          delete[] digest;
        }
      }    
      delete algorithm;
    } else result = SI_EC_VerificationFailed_NoCertificate;
  }
  
  delete tagList;
  delete mac;
  return result;
}


DcmItem *DcmSignature::findFirstSignatureItem(DcmItem& item, DcmStack& stack)
{
  stack.clear();
  stack.push(&item);
  return findNextSignatureItem(item, stack);
}

DcmItem *DcmSignature::findNextSignatureItem(DcmItem& item, DcmStack& stack)
{
  if (EC_Normal == item.search(DCM_DigitalSignaturesSequence, stack, ESM_afterStackTop, OFTrue))
  {
    DcmObject *nextItem = stack.elem(1);
    if (nextItem && ((nextItem->ident() == EVR_item) || (nextItem->ident() == EVR_dataset))) return (DcmItem *)nextItem;
  }
  return NULL;
}

SI_E_Condition DcmSignature::getCurrentMacID(Uint16& macID)
{
  if (NULL == selectedSignatureItem) return SI_EC_IllegalCall;
  macID = getMACIDnumber(*selectedSignatureItem);
  return SI_EC_Normal;
}

SI_E_Condition DcmSignature::getCurrentMacXferSyntaxName(OFString& str)
{
  str.clear();
  if ((NULL == selectedSignatureItem)||(NULL == selectedMacParametersItem)) return SI_EC_IllegalCall;
  SI_E_Condition result = SI_EC_Normal;
  DcmStack stack;
  
  // read MAC Calculation Transfer Syntax UID
  if ((EC_Normal == selectedMacParametersItem->search(DCM_MACCalculationTransferSyntaxUID, stack, ESM_fromHere, OFFalse))&& (stack.top()->isLeaf()))
  {
    char *uid = NULL;
    if (EC_Normal == ((DcmElement *)(stack.top()))->getString(uid))
    {
      DcmXfer xf(uid);
      if (xf.getXfer() == EXS_Unknown) str=uid; else 
      {
      	str="=";
      	str.append(xf.getXferName());
      }
    } else result = SI_EC_VerificationFailed_NoMAC;
  } else result = SI_EC_VerificationFailed_NoMAC;
  return result;
}

SI_E_Condition DcmSignature::getCurrentMacName(OFString& str)
{
  str.clear();
  if ((NULL == selectedSignatureItem)||(NULL == selectedMacParametersItem)) return SI_EC_IllegalCall;
  SI_E_Condition result = SI_EC_VerificationFailed_NoMAC;
  DcmStack stack;

  // read MAC Algorithm
  if ((EC_Normal == selectedMacParametersItem->search(DCM_MACAlgorithm, stack, ESM_fromHere, OFFalse))&& (stack.top()->isLeaf()))
  {
    if (EC_Normal == ((DcmElement *)(stack.top()))->getOFString(str, 0)) result = SI_EC_Normal;
  }  
  return result;
}

SI_E_Condition DcmSignature::getCurrentSignatureUID(OFString& str)
{
  str.clear();
  if (NULL == selectedSignatureItem) return SI_EC_IllegalCall;
  SI_E_Condition result = SI_EC_VerificationFailed_NoSignature;
  DcmStack stack;

  // read signature UID
  if ((EC_Normal == selectedSignatureItem->search(DCM_DigitalSignatureUID, stack, ESM_fromHere, OFFalse))&& (stack.top()->isLeaf()))
  {
    if (EC_Normal == ((DcmElement *)(stack.top()))->getOFString(str, 0)) result = SI_EC_Normal;
  }  
  return result;
}

SI_E_Condition DcmSignature::getCurrentSignatureDateTime(OFString& str)
{
  str.clear();
  if (NULL == selectedSignatureItem) return SI_EC_IllegalCall;
  SI_E_Condition result = SI_EC_VerificationFailed_NoSignature;
  DcmStack stack;

  // read signature date/time
  if ((EC_Normal == selectedSignatureItem->search(DCM_DigitalSignatureDateTime, stack, ESM_fromHere, OFFalse))&& (stack.top()->isLeaf()))
  {
    if (EC_Normal == ((DcmElement *)(stack.top()))->getOFString(str, 0)) result = SI_EC_Normal;
  }  
  return result;
}

SiCertificate *DcmSignature::getCurrentCertificate()
{
  return selectedCertificate;  
}

SI_E_Condition DcmSignature::getCurrentDataElementsSigned(DcmAttributeTag& desig)
{
  desig.clear();
  if ((NULL == selectedSignatureItem)||(NULL == selectedMacParametersItem)) return SI_EC_IllegalCall;
  SI_E_Condition result = SI_EC_VerificationFailed_NoMAC;
  DcmStack stack;

  // read Data Elements Signed
  if ((EC_Normal == selectedMacParametersItem->search(DCM_DataElementsSigned, stack, ESM_fromHere, OFFalse))&& (stack.top()->ident() == EVR_AT))
  {
    desig = *((DcmAttributeTag *)(stack.top()));
    result = SI_EC_Normal;
  }  
  return result;
}

#else /* WITH_OPENSSL */

const int dcmsign_cc_dummy_to_keep_linker_from_moaning = 0;

#endif

/*
 *  $Log: dcmsign.cc,v $
 *  Revision 1.1  2000-11-07 16:49:02  meichel
 *  Initial release of dcmsign module for DICOM Digital Signatures
 *
 *
 */

