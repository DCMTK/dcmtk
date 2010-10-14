/*
 *
 *  Copyright (C) 2000-2010, OFFIS e.V.
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
 *    classes: DcmSignature
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:38 $
 *  CVS/RCS Revision: $Revision: 1.6 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"

#ifdef WITH_OPENSSL

#include "dcmtk/dcmsign/dcsignat.h"
#include "dcmtk/dcmdata/dcstack.h"
#include "dcmtk/dcmdata/dcitem.h"
#include "dcmtk/dcmdata/dcvrat.h"
#include "dcmtk/dcmdata/dcvrcs.h"
#include "dcmtk/dcmdata/dcvrdt.h"
#include "dcmtk/dcmdata/dcvrui.h"
#include "dcmtk/dcmdata/dcvrobow.h"
#include "dcmtk/dcmdata/dcvrus.h"
#include "dcmtk/dcmdata/dcuid.h"
#include "dcmtk/dcmdata/dcsequen.h"
#include "dcmtk/dcmdata/dcdeftag.h"

#include "dcmtk/dcmsign/sialgo.h"
#include "dcmtk/dcmsign/sicert.h"
#include "dcmtk/dcmsign/simac.h"
#include "dcmtk/dcmsign/simaccon.h"
#include "dcmtk/dcmsign/simd5.h"
#include "dcmtk/dcmsign/siprivat.h"
#include "dcmtk/dcmsign/siripemd.h"
#include "dcmtk/dcmsign/sisha1.h"
#include "dcmtk/dcmsign/sisprof.h"
#include "dcmtk/dcmsign/sitstamp.h"

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
  if (item.search(DCM_MACIDNumber, stack, ESM_fromHere, OFFalse).good() && (stack.top()->isLeaf()))
  {
    ((DcmElement *)(stack.top()))->getUint16(macIDnumber);
  }
  return macIDnumber;
}
  
  
void DcmSignature::currentDateTime(OFString &str)
{
  DcmDateTime::getCurrentDateTime(str, OFTrue /*seconds*/, OFTrue /*fraction*/, OFTrue /*timeZone*/);
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
    if ((currentItem->search(DCM_MACParametersSequence, stack, ESM_fromHere, OFFalse)).good() && (stack.top()->ident() == EVR_SQ))
    {
      macParametersSq = (DcmSequenceOfItems *)stack.top();
    } else macParametersSq = NULL;
    stack.clear();
    if ((currentItem->search(DCM_DigitalSignaturesSequence, stack, ESM_fromHere, OFFalse)).good() && (stack.top()->ident() == EVR_SQ))
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


OFCondition DcmSignature::removeSignature(unsigned long i)
{
  if (signatureSq == NULL) return EC_IllegalCall;
  unsigned long seqCard = signatureSq->card();
  if (i >= seqCard) return EC_IllegalCall;
  DcmItem *removalItem = signatureSq->getItem(i);
  if (removalItem == NULL) return EC_IllegalCall; // should never happen

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

  return EC_Normal;
}


OFCondition DcmSignature::allocateMACID(Uint16& newID)
{
  newID = 0xffff;
  if (currentItem == NULL) return EC_IllegalCall;
  if ((signatureSq==NULL)&&(macParametersSq == NULL)) 
  {
    newID = 0; // no signature yet, can use 0.
    return EC_Normal; 
  }

  unsigned long i;
  DcmItem *tmpItem = NULL;
  char *isAllocated = new char[65536];
  if (isAllocated==NULL) return EC_MemoryExhausted;

  OFCondition result = SI_EC_MacIDsExhausted;

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
      newID = (Uint16)i;
      i = 65536;
      result = EC_Normal;
    } else ++i;
  }
  delete[] isAllocated;
  return result;
}

  
OFCondition DcmSignature::createSignature(
    SiPrivateKey& key, 
    SiCertificate& cert, 
    SiMAC& mac,
    SiSecurityProfile& profile, 
    E_TransferSyntax xfer,
    const DcmAttributeTag *tagList,
    SiTimeStamp *timeStamp)
{
  // do some checks first
  if (currentItem == NULL) return EC_IllegalCall;
  if (! key.matchesCertificate(cert)) return SI_EC_CertificateDoesNotMatchPrivateKey;
  if (! profile.isAllowableMAC(mac)) return SI_EC_MacDoesNotMatchProfile;
  if (! profile.isAllowableAlgorithmType(key.getKeyType())) return SI_EC_AlgorithmDoesNotMatchProfile;
  if (! profile.isAllowableTransferSyntax(xfer)) return SI_EC_TransferSyntaxDoesNotMatchProfile;

  OFCondition result = EC_Normal;

  // update tag list if present
  DcmAttributeTag *updatedTagList = NULL;
  if (tagList) 
  {
    updatedTagList = new DcmAttributeTag(*tagList);
    if (updatedTagList == NULL) result = EC_MemoryExhausted;
    else result = profile.updateAttributeList(*currentItem, *updatedTagList);
  }

  // make sure we have a MAC parameter sequence
  if ((result.good()) && (macParametersSq == NULL))
  {      
    macParametersSq = new DcmSequenceOfItems(DCM_MACParametersSequence);
    if (macParametersSq == NULL) result = EC_MemoryExhausted; 
    else
    {
      result = currentItem->insert(macParametersSq, OFTrue);
    }
  }

  // make sure we have a digital signature sequence
  if ((result.good()) && (signatureSq == NULL))
  {      
    signatureSq = new DcmSequenceOfItems(DCM_DigitalSignaturesSequence);
    if (signatureSq == NULL) result = EC_MemoryExhausted; 
    else
    {
      result = currentItem->insert(signatureSq, OFTrue);
    }
  }

  // allocate MAC ID
  Uint16 macID = 0;
  if (result.good()) result = allocateMACID(macID);

  // create initial part of digital signature sequence item
  // which has to be included into the MAC calculation
  DcmItem *seqItem = NULL;
  if (result.good())
  {
    seqItem = new DcmItem();
    if (seqItem)
    {
      // MAC ID Number
      if (result.good())
      {
        DcmUnsignedShort *elemMacID = new DcmUnsignedShort(DCM_MACIDNumber);
        if (elemMacID)
        {
          result = elemMacID->putUint16(macID);
          if (result.good()) result = seqItem->insert(elemMacID, OFTrue);
          if (result.bad())
          {
            delete elemMacID;
          }
        } else result = EC_MemoryExhausted;
      }

      // Digital Signature UID
      if (result.good())
      {
        char newUID[66];
        dcmGenerateUniqueIdentifier(newUID);
        DcmUniqueIdentifier *elemSigUID = new DcmUniqueIdentifier(DCM_DigitalSignatureUID);
        if (elemSigUID)
        {
          result = elemSigUID->putString(newUID);
          if (result.good()) result = seqItem->insert(elemSigUID, OFTrue);
          if (result.bad())
          {
            delete elemSigUID;
          }
        } else result = EC_MemoryExhausted;
      }

      // Digital Signature Date/Time
      if (result.good())
      {
        OFString aString;
        currentDateTime(aString);
        DcmDateTime *elemDT = new DcmDateTime(DCM_DigitalSignatureDateTime);
        if (elemDT)
        {
          result = elemDT->putOFStringArray(aString);
          if (result.good()) result = seqItem->insert(elemDT, OFTrue);
          if (result.bad())
          {
            delete elemDT;
          }
        } else result = EC_MemoryExhausted;
      }
      
      // Certificate of Signer and Certificate Type
      if (result.good())
      {
        result = cert.write(*seqItem);
      }

    } else result = EC_MemoryExhausted;
  }

  // now create MAC of DICOM object
  SiAlgorithm *algorithm = NULL;
  SiMACConstructor constructor;
  if (dumpFile) constructor.setDumpFile(dumpFile);
  DcmAttributeTag *tagListOut = new DcmAttributeTag(DCM_DataElementsSigned);
  if (tagListOut == NULL) result = EC_MemoryExhausted;
  unsigned long sigLength = 0;
  unsigned char *signature = NULL;

  if (result.good())
  {
    algorithm = key.createAlgorithmForPrivateKey();
    if (algorithm)
    {
      mac.initialize();
      sigLength = algorithm->getSize();
      signature = new unsigned char[sigLength];
      if (signature == NULL) result = EC_MemoryExhausted; 
      else
      {
        // encode main dataset
        result = constructor.encodeDataset(*currentItem, mac, xfer, *tagListOut, updatedTagList);
        // encode required attributes from the digital signatures sequence
        if (result.good()) result = constructor.encodeDigitalSignatureItem(*seqItem, mac, xfer);
        // flush stream constructor
        if (result.good()) result = constructor.flush(mac);
      }
    } else result = EC_MemoryExhausted;
  }

  // sign MAC
  if (result.good())
  {      
    unsigned long digestLength = mac.getSize();
    unsigned char *digest = new unsigned char[digestLength];
    if (digest == NULL) result = EC_MemoryExhausted;
    else
    {
      result = mac.finalize(digest);
      if (result.good()) result = algorithm->sign(digest, digestLength, mac.macType(), signature, sigLength);
      delete[] digest;
    } 
  }      

  // complete and write digital signature sequence item
  if (result.good())
  {
    if (seqItem)
    {
            
      // Signature
      if (result.good())
      {
        DcmOtherByteOtherWord *elemSig = new DcmOtherByteOtherWord(DCM_Signature);
        if (elemSig)
        {
          elemSig->setVR(EVR_OB);
          result = elemSig->putUint8Array((Uint8 *) signature, sigLength);
          if (result.good()) result = seqItem->insert(elemSig, OFTrue);
          if (result.bad())
          {
            delete elemSig;
          }
        } else result = EC_MemoryExhausted;
      }
            
      // Timestamp and Timestamp Type
      if (result.good())
      {
        if (timeStamp)
        {
          // this would be the right time to request the time stamp
          result = timeStamp->stamp(signature, sigLength);
          if (result.good())
          {
            result = timeStamp->write(*seqItem);
          }
        }
      }

      if (result.good())
      {
        result = signatureSq->append(seqItem);
        if (result.bad())
        {
          delete seqItem;
        }
      }
    } else result = EC_MemoryExhausted;
  }

  // write MAC parameters sequence item
  if (result.good())
  {
    DcmItem *macItem = new DcmItem();
    if (macItem)
    {
      // MAC ID Number
      if (result.good())
      {
        DcmUnsignedShort *elemMacID = new DcmUnsignedShort(DCM_MACIDNumber);
        if (elemMacID)
        {
          result = elemMacID->putUint16(macID);
          if (result.good()) result = macItem->insert(elemMacID, OFTrue);
          if (result.bad())
          {
            delete elemMacID;
          }
        } else result = EC_MemoryExhausted;
      }

      // MAC Calculation Transfer Syntax UID
      if (result.good())
      {
        DcmUniqueIdentifier *elemXferUID = new DcmUniqueIdentifier(DCM_MACCalculationTransferSyntaxUID);
        if (elemXferUID)
        {
          DcmXfer xid(xfer);
          result = elemXferUID->putString(xid.getXferID());
          if (result.good()) result = macItem->insert(elemXferUID, OFTrue);
          if (result.bad())
          {
            delete elemXferUID;
          }
        } else result = EC_MemoryExhausted;
      }

      // MAC Algorithm Identifier
      if (result.good())
      {
        DcmCodeString *elemMacAlgo = new DcmCodeString(DCM_MACAlgorithm);
        if (elemMacAlgo)
        {
          result = elemMacAlgo->putString(mac.getDefinedTerm());
          if (result.good()) result = macItem->insert(elemMacAlgo, OFTrue);
          if (result.bad())
          {
            delete elemMacAlgo;
          }
        } else result = EC_MemoryExhausted;
      }

      // Data Elements Signed
      if (result.good())
      {
        result = macItem->insert(tagListOut, OFTrue);
        if (result.good())
        {
          tagListOut = NULL; // make sure we don't delete tagListOut later
        }
      }

      if (result.good())
      {
        result = macParametersSq->append(macItem);
        if (result.bad())
        {
          delete macItem;
        }
      }
    } else result = EC_MemoryExhausted;
  }
    
  delete[] signature;
  delete tagListOut;
  delete updatedTagList;
  delete algorithm;
  return result;
}


OFCondition DcmSignature::selectSignature(unsigned long i)
{
  deselect();
  if ((signatureSq == NULL) || (i >= signatureSq->card())) return EC_IllegalCall;
 
  selectedSignatureItem = signatureSq->getItem(i);
  if (selectedSignatureItem == NULL) return EC_IllegalCall;
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
  if (selectedCertificate == NULL) return EC_MemoryExhausted;  
  selectedCertificate->read(*selectedSignatureItem);
  return EC_Normal;
}

OFCondition DcmSignature::verifyCurrent()
{
  if (NULL == selectedSignatureItem) return EC_IllegalCall;
  if (selectedMacParametersItem == NULL) return SI_EC_VerificationFailed_NoMAC;
  if ((selectedCertificate == NULL)||(selectedCertificate->getKeyType() == EKT_none)) return SI_EC_VerificationFailed_NoCertificate;

  OFCondition result = EC_Normal;
  DcmAttributeTag *tagList = NULL;
  SiMAC *mac = NULL;
  E_TransferSyntax xfer = EXS_Unknown;
  DcmStack stack;
  DcmOtherByteOtherWord *signature = NULL;
  
  // read MAC Calculation Transfer Syntax UID
  if (result.good())
  {
    if ((selectedMacParametersItem->search(DCM_MACCalculationTransferSyntaxUID, stack, ESM_fromHere, OFFalse)).good() && (stack.top()->isLeaf()))
    {
      char *uid = NULL;
      if ((((DcmElement *)(stack.top()))->getString(uid)).good())
      {
        DcmXfer xf(uid);
        xfer = xf.getXfer();
        if (xfer == EXS_Unknown) result = SI_EC_WrongTransferSyntax;
      } else result = SI_EC_VerificationFailed_NoMAC;
    } else result = SI_EC_VerificationFailed_NoMAC;
  }

  // read MAC Algorithm
  if (result.good())
  {
    stack.clear();
    if ((selectedMacParametersItem->search(DCM_MACAlgorithm, stack, ESM_fromHere, OFFalse)).good() && (stack.top()->isLeaf()))
    {
      OFString macidentifier;
      if ((((DcmElement *)(stack.top()))->getOFString(macidentifier, 0)).good())
      {
        if (macidentifier == SI_DEFTERMS_RIPEMD160) mac = new SiRIPEMD160();
        else if (macidentifier == SI_DEFTERMS_SHA1) mac = new SiSHA1();
        else if (macidentifier == SI_DEFTERMS_MD5)  mac = new SiMD5();
        if (mac == NULL) result = SI_EC_VerificationFailed_UnsupportedMACAlgorithm;
      } else result = SI_EC_VerificationFailed_NoMAC;
    } else result = SI_EC_VerificationFailed_NoMAC;
  }     

  // read Data Elements Signed
  if (result.good())
  {
    stack.clear();
    if ((selectedMacParametersItem->search(DCM_DataElementsSigned, stack, ESM_fromHere, OFFalse)).good() && (stack.top()->ident() == EVR_AT))
    {
      tagList = new DcmAttributeTag(*((DcmAttributeTag *)(stack.top())));
      if (tagList == NULL) result = EC_MemoryExhausted;
    }
  }     

  // read Signature
  if (result.good())
  {
    stack.clear();
    if ((selectedSignatureItem->search(DCM_Signature, stack, ESM_fromHere, OFFalse)).good() && (stack.top()->isLeaf()))
    {
      signature = new DcmOtherByteOtherWord(*((DcmOtherByteOtherWord *)(stack.top())));
      if (signature == NULL) result = EC_MemoryExhausted;
    } else result = SI_EC_VerificationFailed_NoSignature;
  }     

  // create MAC
  if (result.good())
  {
    DcmAttributeTag tagListOut(DCM_DataElementsSigned);
    SiMACConstructor constructor;
    if (dumpFile) constructor.setDumpFile(dumpFile);

    // encode main dataset
    result = constructor.encodeDataset(*currentItem, *mac, xfer, tagListOut, tagList);

    // encode required attributes from the digital signatures sequence
    if (result.good()) result = constructor.encodeDigitalSignatureItem(*selectedSignatureItem, *mac, xfer);

    // flush stream constructor
    if (result.good()) result = constructor.flush(*mac);
  }

  // finally verify signature
  if (result.good())
  {  
    SiAlgorithm *algorithm = selectedCertificate->createAlgorithmForPublicKey();
    if (algorithm)
    {
      OFBool verified = OFTrue;
      Uint32 sigLength = signature->getLength();
      Uint8 *sigData = NULL;
      if ((signature->getUint8Array(sigData)).bad() || (sigData == NULL)) result = SI_EC_VerificationFailed_NoSignature;
      else 
      {
        unsigned long digestLength = mac->getSize();
        unsigned char *digest = new unsigned char[digestLength];
        if (digest == NULL) result =  EC_MemoryExhausted;
        else
        {
          result = mac->finalize(digest);
          if (result.good())
          {
            result = algorithm->verify(digest, digestLength, mac->macType(), sigData, sigLength, verified);
            if ((result.good()) && (! verified)) result = SI_EC_VerificationFailed_Corrupted;
          }
          delete[] digest;
        }
      }    
      delete algorithm;
    } else result = SI_EC_VerificationFailed_NoCertificate;
  }
  
  delete signature;
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
  if (item.search(DCM_DigitalSignaturesSequence, stack, ESM_afterStackTop, OFTrue).good())
  {
    DcmObject *nextItem = stack.elem(1);
    if (nextItem && ((nextItem->ident() == EVR_item) || (nextItem->ident() == EVR_dataset))) return (DcmItem *)nextItem;
  }
  return NULL;
}

OFCondition DcmSignature::getCurrentMacID(Uint16& macID)
{
  if (NULL == selectedSignatureItem) return EC_IllegalCall;
  macID = getMACIDnumber(*selectedSignatureItem);
  return EC_Normal;
}

OFCondition DcmSignature::getCurrentMacXferSyntaxName(OFString& str)
{
  str.clear();
  if ((NULL == selectedSignatureItem)||(NULL == selectedMacParametersItem)) return EC_IllegalCall;
  OFCondition result = EC_Normal;
  DcmStack stack;
  
  // read MAC Calculation Transfer Syntax UID
  if ((selectedMacParametersItem->search(DCM_MACCalculationTransferSyntaxUID, stack, ESM_fromHere, OFFalse)).good() && (stack.top()->isLeaf()))
  {
    char *uid = NULL;
    if ((((DcmElement *)(stack.top()))->getString(uid)).good())
    {
      DcmXfer xf(uid);
      if (xf.getXfer() == EXS_Unknown) str=uid; else 
      {
        str = "=";
        str.append(xf.getXferName());
      }
    } else result = SI_EC_VerificationFailed_NoMAC;
  } else result = SI_EC_VerificationFailed_NoMAC;
  return result;
}

OFCondition DcmSignature::getCurrentMacName(OFString& str)
{
  str.clear();
  if ((NULL == selectedSignatureItem)||(NULL == selectedMacParametersItem)) return EC_IllegalCall;
  OFCondition result = SI_EC_VerificationFailed_NoMAC;
  DcmStack stack;

  // read MAC Algorithm
  if ((selectedMacParametersItem->search(DCM_MACAlgorithm, stack, ESM_fromHere, OFFalse)).good() && (stack.top()->isLeaf()))
  {
    if ((((DcmElement *)(stack.top()))->getOFString(str, 0)).good()) result = EC_Normal;
  }  
  return result;
}

OFCondition DcmSignature::getCurrentSignatureUID(OFString& str)
{
  str.clear();
  if (NULL == selectedSignatureItem) return EC_IllegalCall;
  OFCondition result = SI_EC_VerificationFailed_NoSignature;
  DcmStack stack;

  // read signature UID
  if ((selectedSignatureItem->search(DCM_DigitalSignatureUID, stack, ESM_fromHere, OFFalse)).good() && (stack.top()->isLeaf()))
  {
    if ((((DcmElement *)(stack.top()))->getOFString(str, 0)).good()) result = EC_Normal;
  }  
  return result;
}

OFCondition DcmSignature::getCurrentSignatureDateTime(OFString& str)
{
  str.clear();
  if (NULL == selectedSignatureItem) return EC_IllegalCall;
  OFCondition result = SI_EC_VerificationFailed_NoSignature;
  DcmStack stack;

  // read signature date/time
  if ((selectedSignatureItem->search(DCM_DigitalSignatureDateTime, stack, ESM_fromHere, OFFalse)).good() && (stack.top()->isLeaf()))
  {
    if ((((DcmElement *)(stack.top()))->getOFString(str, 0)).good()) result = EC_Normal;
  }  
  return result;
}

SiCertificate *DcmSignature::getCurrentCertificate()
{
  return selectedCertificate;  
}

OFCondition DcmSignature::getCurrentDataElementsSigned(DcmAttributeTag& desig)
{
  desig.clear();
  if ((NULL == selectedSignatureItem)||(NULL == selectedMacParametersItem)) return EC_IllegalCall;
  OFCondition result = SI_EC_VerificationFailed_NoMAC;
  DcmStack stack;

  // read Data Elements Signed
  if ((selectedMacParametersItem->search(DCM_DataElementsSigned, stack, ESM_fromHere, OFFalse)).good() && (stack.top()->ident() == EVR_AT))
  {
    desig = *((DcmAttributeTag *)(stack.top()));
    result = EC_Normal;
  }  
  return result;
}

#else /* WITH_OPENSSL */

int dcmsign_cc_dummy_to_keep_linker_from_moaning = 0;

#endif

/*
 *  $Log: dcsignat.cc,v $
 *  Revision 1.6  2010-10-14 13:14:38  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.5  2010-03-01 09:08:50  uli
 *  Removed some unnecessary include directives in the headers.
 *
 *  Revision 1.4  2009-04-29 12:21:42  joergr
 *  Fixed memory leak in method verifyCurrent().
 *
 *  Revision 1.3  2005/12/08 15:47:17  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.2  2003/07/09 13:59:50  meichel
 *  Removed unused variable
 *
 *  Revision 1.1  2002/12/20 14:53:35  wilkens
 *  Modified name clash resulting in a compiler error on Solaris 2.5.1 using
 *  compiler SC 2.0.1.
 *
 *  Revision 1.12  2002/12/16 12:57:48  meichel
 *  Minor modification to shut up linker on MacOS X when compiling
 *    without OpenSSL support
 *
 *  Revision 1.11  2002/08/27 17:21:01  meichel
 *  Initial release of new DICOM I/O stream classes that add support for stream
 *    compression (deflated little endian explicit VR transfer syntax)
 *
 *  Revision 1.10  2002/08/20 12:22:26  meichel
 *  Adapted code to new loadFile and saveFile methods, thus removing direct
 *    use of the DICOM stream classes.
 *
 *  Revision 1.9  2002/04/11 12:58:09  joergr
 *  Replaced direct call of system routines by new standard date and time
 *  functions.
 *
 *  Revision 1.8  2002/01/08 10:28:16  joergr
 *  Corrected spelling of function dcmGenerateUniqueIdentifier().
 *
 *  Revision 1.7  2001/12/04 18:41:08  meichel
 *  Updated DcmItem::insert() calls to always remove old element if present
 *    (which should never be the case anyway).
 *
 *  Revision 1.6  2001/11/16 15:50:52  meichel
 *  Adapted digital signature code to final text of supplement 41.
 *
 *  Revision 1.5  2001/09/26 14:30:23  meichel
 *  Adapted dcmsign to class OFCondition
 *
 *  Revision 1.4  2001/06/01 15:50:52  meichel
 *  Updated copyright header
 *
 *  Revision 1.3  2000/12/12 16:46:32  meichel
 *  Minor changes to keep gcc 2.7.x on SunOS 4.1.3 happy
 *
 *  Revision 1.2  2000/11/07 18:07:07  joergr
 *  Minor code purifications to keep Sun CC 2.0.1 quiet.
 *
 *  Revision 1.1  2000/11/07 16:49:02  meichel
 *  Initial release of dcmsign module for DICOM Digital Signatures
 *
 *
 */
