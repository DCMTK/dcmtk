/*
 *
 *  Copyright (C) 2000-2023, OFFIS e.V.
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
#include "dcmtk/dcmdata/dcvrdt.h"
#include "dcmtk/dcmdata/dcuid.h"
#include "dcmtk/dcmdata/dcsequen.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/dcmsign/sialgo.h"
#include "dcmtk/dcmsign/sicert.h"
#include "dcmtk/dcmsign/simac.h"
#include "dcmtk/dcmsign/simaccon.h"
#include "dcmtk/dcmsign/siprivat.h"
#include "dcmtk/dcmsign/simdmac.h"
#include "dcmtk/dcmsign/sisprof.h"
#include "dcmtk/dcmsign/sitstamp.h"
#include "dcmtk/dcmsign/sitsfs.h"

BEGIN_EXTERN_C
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#ifdef HAVE_OPENSSL_PROVIDER_H
#include <openssl/provider.h>
#endif
END_EXTERN_C


/* static helper methods */

#ifdef HAVE_OPENSSL_PROTOTYPE_OSSL_PROVIDER_LOAD
static OSSL_PROVIDER *legacyProvider = NULL;
static OSSL_PROVIDER *defaultProvider = NULL;
#endif

void DcmSignature::initializeLibrary()
{
  OPENSSL_no_config();
  SSL_library_init();
  SSL_load_error_strings();
  OpenSSL_add_all_algorithms();
#ifdef HAVE_OPENSSL_PROTOTYPE_OSSL_PROVIDER_LOAD
  // load the legacy provider, which is needed for RIPEMD160
  if (legacyProvider == NULL) legacyProvider = OSSL_PROVIDER_load(NULL, "legacy");
  if (legacyProvider == NULL) DCMSIGN_ERROR("Unable to load the OpenSSL Legacy Provider, RIPEMD160 and MD5 not available");

  // explicitly also load the default provider
  if (defaultProvider == NULL) defaultProvider = OSSL_PROVIDER_load(NULL, "default");
  if (defaultProvider == NULL) DCMSIGN_ERROR("Unable to load the OpenSSL Default Provider");
#endif
}

void DcmSignature::cleanupLibrary()
{
#ifdef HAVE_OPENSSL_PROTOTYPE_OSSL_PROVIDER_LOAD
  if (legacyProvider)
  {
    OSSL_PROVIDER_unload(legacyProvider);
    legacyProvider = NULL;
  }
  if (defaultProvider)
  {
    OSSL_PROVIDER_unload(defaultProvider);
    defaultProvider = NULL;
  }
#endif
}

OFCondition DcmSignature::getMACIDnumber(DcmItem &item, Uint16& macid)
{
  macid = 0xFFFF;
  DcmStack stack;
  OFCondition result = item.search(DCM_MACIDNumber, stack, ESM_fromHere, OFFalse);
  if (result.good() && (stack.top()->isLeaf()))
  {
    result = ((DcmElement *)(stack.top()))->getUint16(macid);
  }
  return result;
}

void DcmSignature::currentDateTime(OFString &str)
{
  str = "";
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
, selectedTimestamp(NULL)
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
  delete selectedTimestamp;
  selectedTimestamp = NULL;
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

  Uint16 macIDnumber = 0;
  OFCondition result = getMACIDnumber(*removalItem, macIDnumber);
  if (result.bad()) return result;

  // check mac ID number and whether it is unique
  OFBool macIDunique = OFTrue;
  DcmItem *tmpItem=NULL;
  unsigned long j=0;
  Uint16 macIDnumber2 = 0;
  for (j=0; j < seqCard; ++j)
  {
    tmpItem = signatureSq->getItem(j);
    if ((i != j) && tmpItem && (getMACIDnumber(*tmpItem, macIDnumber2).good()) && (macIDnumber == macIDnumber2)) macIDunique = OFFalse;
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
      if (tmpItem && (getMACIDnumber(*tmpItem, macIDnumber2).good()) && (macIDnumber == macIDnumber2)) delete macParametersSq->remove(j); else ++j;
    }
  }

  if (signatureSq->card() == 0)
  {
    delete currentItem->remove(signatureSq);
    signatureSq = NULL;
  }

  if (macParametersSq && macParametersSq->card() == 0)
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
  Uint16 macID = 0;

  for (i=0; i < 65536; ++i) isAllocated[i]=0;
  if (signatureSq)
  {
    unsigned long sqCard = signatureSq->card();
    for (i=0; i < sqCard; ++i)
    {
      tmpItem = signatureSq->getItem(i);
      if (tmpItem && getMACIDnumber(*tmpItem, macID).good())
      {
        isAllocated[macID] = 1;
      }
    }
  }
  if (macParametersSq)
  {
    unsigned long macCard = macParametersSq->card();
    for (i=0; i < macCard; ++i)
    {
      tmpItem = macParametersSq->getItem(i);
      if (tmpItem && getMACIDnumber(*tmpItem, macID).good())
      {
        isAllocated[macID] = 1;
      }
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
    SiTimeStamp *timeStamp,
    SiSignaturePurpose::E_SignaturePurposeType sigPurpose)
{
  // do some checks first
  if (currentItem == NULL) return EC_IllegalCall;
  if (! key.matchesCertificate(cert)) return SI_EC_CertificateDoesNotMatchPrivateKey;
  if (! profile.isAllowableMAC(mac)) return SI_EC_MacDoesNotMatchProfile;
  if (! profile.isAllowableAlgorithmType(key.getKeyType())) return SI_EC_AlgorithmDoesNotMatchProfile;
  if (! profile.isAllowableTransferSyntax(xfer)) return SI_EC_TransferSyntaxDoesNotMatchProfile;

  OFCondition result = EC_Normal;

  // print a warning if we have a weak (i.e. too short) key in the certificate
  cert.checkForWeakKey();

  // print a warning if the certificate has already expired
  if (cert.isCertExpiredNow())
  {
    OFString expiry;
    cert.getCertValidityNotAfter(expiry);
    DCMSIGN_WARN("Certificate has expired " << expiry << ", signature will be invalid.");
  }

  // print a warning if the certificate is not yet valid
  if (cert.isCertNotYetValidNow())
  {
    OFString notBefore;
    cert.getCertValidityNotBefore(notBefore);
    DCMSIGN_WARN("Certificate is not yet valid, validity starts " << notBefore << ", signature will be invalid.");
  }

  // create updated tag list
  DcmAttributeTag *updatedTagList = NULL;
  if (tagList)
  {
    updatedTagList = new DcmAttributeTag(*tagList);
    if (updatedTagList == NULL) result = EC_MemoryExhausted;
    else result = profile.updateAttributeList(*currentItem, *updatedTagList);
  }
  else
  {
    updatedTagList = new DcmAttributeTag(DCM_DataElementsSigned);
    if (updatedTagList == NULL) result = EC_MemoryExhausted;
    else result = profile.createAttributeList(*currentItem, *updatedTagList);
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

      // Digital Signature Purpose Code Sequence
      if (result.good())
      {

        // check if the signature profile requires a certain signature purpose
        sigPurpose = SiSignaturePurpose::determineOverridePurpose(sigPurpose, profile.getOverrideSignaturePurpose());

        // create digital signature purpose code sequence (unless sigPurpose is ESP_none)
        result = SiSignaturePurpose::insertDigitalSignaturePurposeCodeSQ(*seqItem, sigPurpose);
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
  unsigned long digestLength = mac.getSize();
  unsigned char *digest = new unsigned char[digestLength];

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

  // check whether all tags from tagList are really present in the signature
  if (result.good())
  {
    result = checkListOfSignedTags(tagList, tagListOut);
  }

  // sign MAC
  if (result.good())
  {
    if (digest == NULL) result = EC_MemoryExhausted;
    else
    {
      result = mac.finalize(digest);

      if (dcmsignLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
      {
        OFString logString;
        char c[5];
        for (size_t i=0; i<digestLength; i++)
        {
           sprintf(c, "%02hu", OFstatic_cast(unsigned short, digest[i]));
           logString += c;
        }
        DCMSIGN_DEBUG("DcmSignature::createSignature(): " << mac.getDefinedTerm() << " MAC = " << logString);
      }

      if (result.good()) result = algorithm->sign(digest, digestLength, mac.macType(), signature, sigLength);
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
          // now is the right time to request the certified time stamp

          // check if the digital signature has an odd number of bytes
          if (algorithm->keyType() == EKT_DSA || algorithm->keyType() == EKT_EC)
          {
              // DSA and ECDSA signatures are encoded in DER and can have odd length. We must remove
              // the pad byte before feeding the signature to the timestamping layer.
              adjustASN1SequenceLength(signature, sigLength);
          }

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
  delete[] digest;
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

  selectedMacParametersItem = NULL;
  Uint16 macID = 0xFFFF;
  OFCondition result = getMACIDnumber(*selectedSignatureItem, macID);
  if (result.bad())
  {
    DCMSIGN_WARN("Cannot read MAC ID number in DigitalSignaturesSequence.");
  }

  if (macParametersSq)
  {
    Uint16 macID2 = 0;
    DcmItem *tmpItem=NULL;
    unsigned long cardMac = macParametersSq->card();
    for (unsigned long j=0; j<cardMac; j++)
    {
      tmpItem = macParametersSq->getItem(j);
      if (result.good() && getMACIDnumber(*tmpItem, macID2).good() &&  macID == macID2)
      {
        selectedMacParametersItem = tmpItem;
        break;
      }
    }
  }
  selectedCertificate = new SiCertificate();
  if (selectedCertificate == NULL) return EC_MemoryExhausted;
  result = selectedCertificate->read(*selectedSignatureItem);
  if (result.bad())
  {
    // a failure to read the certificate should not cause a failure
    // of the "selectSignature" operation. We actually want the
    // signature verification to fail in this case.
    DCMSIGN_DEBUG(result.text() << " while reading certificate.");
  }

  selectedTimestamp = new SiTimeStampFS();
  if (selectedTimestamp == NULL) return EC_MemoryExhausted;
  result = selectedTimestamp->read(*selectedSignatureItem);
  if (result.bad() && (result != EC_TagNotFound))
  {
    DCMSIGN_WARN(result.text() << " while reading timestamp.");
    delete selectedTimestamp;
    selectedTimestamp = NULL;
  }

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
        if (macidentifier == SI_DEFTERMS_RIPEMD160) mac = new SiMDMAC(EMT_RIPEMD160);
        else if (macidentifier == SI_DEFTERMS_SHA1) mac = new SiMDMAC(EMT_SHA1);
        else if (macidentifier == SI_DEFTERMS_MD5)  mac = new SiMDMAC(EMT_MD5);
        else if (macidentifier == SI_DEFTERMS_SHA256) mac = new SiMDMAC(EMT_SHA256);
        else if (macidentifier == SI_DEFTERMS_SHA384) mac = new SiMDMAC(EMT_SHA384);
        else if (macidentifier == SI_DEFTERMS_SHA512) mac = new SiMDMAC(EMT_SHA512);
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
      else if (tagList->getVM() == 0) result = SI_EC_VerificationFailed_NoDataElementsSigned; // DataElementsSigned is present but empty
    }
    else result = SI_EC_VerificationFailed_NoDataElementsSigned; // DataElementsSigned is absent or does not have "AT" VR
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
    SiMACConstructor constructor;
    if (dumpFile) constructor.setDumpFile(dumpFile);

    // encode main dataset
    result = constructor.encodeDatasetForVerification(*currentItem, *mac, xfer, tagList);

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
      unsigned long sigLength = signature->getLength();
      Uint8 *sigData = NULL;
      if ((signature->getUint8Array(sigData)).bad() || (sigData == NULL)) result = SI_EC_VerificationFailed_NoSignature;
      else
      {
        // check if the digital signature has an odd number of bytes
        if (selectedCertificate->getKeyType() == EKT_DSA || selectedCertificate->getKeyType() == EKT_EC)
        {
            // DSA and ECDSA signatures are encoded in DER and can have odd length. We must remove
            // the pad byte before feeding the signature to the OpenSSL layer.
            adjustASN1SequenceLength(sigData, sigLength);
        }

        unsigned long digestLength = mac->getSize();
        unsigned char *digest = new unsigned char[digestLength];
        if (digest == NULL) result =  EC_MemoryExhausted;
        else
        {
          result = mac->finalize(digest);
          if (result.good())
          {
            if (dcmsignLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
            {
              OFString logString;
              char c[5];
              for (size_t i=0; i<digestLength; i++)
              {
                 sprintf(c, "%02hu", OFstatic_cast(unsigned short, digest[i]));
                 logString += c;
              }
              DCMSIGN_DEBUG("DcmSignature::verifyCurrent(): " << mac->getDefinedTerm() << " MAC = " << logString);
            }
            result = algorithm->verify(digest, digestLength, mac->macType(), sigData, sigLength, verified);
            if ((result.good()) && (! verified)) result = SI_EC_VerificationFailed_Corrupted;
          }
          delete[] digest;
        }
      }
      delete algorithm;
    } else result = SI_EC_VerificationFailed_NoCertificate;
  }

  // check signature date against certificate expiry date
  if (result.good())
  {
    DcmDateTime *dt = getCurrentSignatureDateTime();
    if (dt)
    {
      OFDateTime odt;
      result = dt->getOFDateTime(odt);
      if (result.good())
      {
        // convert DICOM datetime string to YYYYMMDDHHMMSSZ format.
        // Note that we ignore the timezone here, which not correct
        // (we should adjust the datetime for the timezone)
        OFString asn1dt;
        char buf[40];
        OFStandard::snprintf(buf, 40, "%04u%02u%02u%02u%02u%02uZ",
          odt.getDate().getYear(), odt.getDate().getMonth(), odt.getDate().getDay(),
          odt.getTime().getHour(), odt.getTime().getMinute(), odt.getTime().getIntSecond());
        asn1dt = buf;
        if (selectedCertificate->isCertExpiredAt(asn1dt))
        {
          // certificate was already expired at signature date time
          result = SI_EC_VerificationFailed_CertExpiredAtSignature;
        }
        else if (selectedCertificate->isCertNotYetValidAt(asn1dt))
        {
          // certificate was not yet valid at signature date time,
          // so this is probably a backdated signature
          result = SI_EC_VerificationFailed_CertNotYetValidAtSig;
        }
      }
    }
    else
    {
      DCMSIGN_WARN("DigitalSignatureDateTime attribute is invalid");
      result = SI_EC_VerificationFailed_CertExpiredAtSignature;
    }
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
  do
  {
    if (item.search(DCM_DigitalSignaturesSequence, stack, ESM_afterStackTop, OFTrue).good())
    {
      // We have found a DigitalSignaturesSequence. Check if the sequence is
      // located within the OriginalAttributesSequence. This would indicate that
      // this is an "old" signature that was valid before some update was applied
      // to the DICOM object. This signature thus cannot and should not be verified.
      // We currently have no code that would "undo" the modifications by moving
      // the content of the OriginalAttributesSequence back to its original place
      // and then try to verify the signature. For now we only inform the user
      // about the situation.
      DcmObject *nextItem = NULL;
      unsigned long card = stack.card();
      OFBool inOriginalAttributesSequence = OFFalse;
      for (unsigned long i=0; i<card; ++i)
      {
        nextItem = stack.elem(i);
        if (nextItem && nextItem->getTag() == DCM_OriginalAttributesSequence)
        {
          DCMSIGN_WARN("Found Digital Signature Sequence within the Original Attributes Sequence, ignoring.");
          inOriginalAttributesSequence = OFTrue;
          break; // break out of for loop
        }
      }
      if (inOriginalAttributesSequence) continue; // skip to next iteration of do-while loop

      // we're not in the OriginalAttributesSequence. Return a pointer to the dataset
      // or item in which the DigitalSignaturesSequence is contained.
      nextItem = stack.elem(1);
      if (nextItem && ((nextItem->ident() == EVR_item) || (nextItem->ident() == EVR_dataset))) return (DcmItem *)nextItem;
    }
    return NULL; // no signature found or invalid VR (i.e. something has gone very wrong)
  } while (OFTrue);
}

OFCondition DcmSignature::getCurrentMacID(Uint16& macID)
{
  if (NULL == selectedSignatureItem) return EC_IllegalCall;
  return getMACIDnumber(*selectedSignatureItem, macID);
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
    if ((((DcmElement *)(stack.top()))->getString(uid)).good() && uid)
    {
      DcmXfer xf(uid);
      if (xf == EXS_Unknown) str=uid; else
      {
        str = "=";
        str.append(xf.getXferName());
      }
    } else result = SI_EC_VerificationFailed_NoMAC;
  } else result = SI_EC_VerificationFailed_NoMAC;
  return result;
}


OFCondition DcmSignature::getCurrentSignaturePurpose(OFString& codeValue, OFString& codeMeaning, OFString& codingSchemeDesignator)
{
  codeValue.clear();
  codeMeaning.clear();
  codingSchemeDesignator.clear();

  if ((NULL == selectedSignatureItem)||(NULL == selectedMacParametersItem)) return EC_IllegalCall;

  DcmItem *item = NULL;
  OFCondition result = selectedSignatureItem->findAndGetSequenceItem(DCM_DigitalSignaturePurposeCodeSequence, item, 0);
  if (result.good())
  {
    result = item->findAndGetOFString(DCM_CodeValue, codeValue);
    if (result.good()) result = item->findAndGetOFString(DCM_CodeValue, codeValue);
    if (result.good()) result = item->findAndGetOFString(DCM_CodeMeaning, codeMeaning);
    if (result.good()) result = item->findAndGetOFString(DCM_CodingSchemeDesignator, codingSchemeDesignator);
  }
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

DcmDateTime *DcmSignature::getCurrentSignatureDateTime()
{
  DcmStack stack;
  if (selectedSignatureItem && (selectedSignatureItem->search(DCM_DigitalSignatureDateTime, stack, ESM_fromHere, OFFalse)).good() && (stack.top()->isLeaf()))
  {
     if (stack.top()->ident() == EVR_DT)
     {
         return OFreinterpret_cast(DcmDateTime *, stack.top());
     }
  }
  return NULL;
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

SiTimeStamp *DcmSignature::getCurrentTimestamp()
{
  return selectedTimestamp;
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

DcmItem *DcmSignature::getSelectedSignatureItem()
{
  return selectedSignatureItem;
}

void DcmSignature::adjustASN1SequenceLength(const unsigned char *buf, unsigned long& buflen)
{
   // ASN.1 sequence objects encoded in DER can have odd length. Remove the pad byte, if any.
   if (((buflen > 2) && buf && (buf[0] == 0x30) && (buf[1] < 128) && (buf[1]+3U == buflen)) || // one-byte length encoding
       ((buflen > 4) && buf && (buf[0] == 0x30) && (buf[1] == 0x82) && ((256UL * buf[2]) + buf[3] + 5U == buflen))) // two-byte length encoding
   {
       // The first byte of the signature is 0x30 (DER encoding for SEQUENCE)
       // and length field is one byte shorter than our buffer size.
       // Adjust buflen to remove the pad byte.
       --buflen;
   }
}


OFCondition DcmSignature::verifySignatureProfile(SiSecurityProfile &sprof)
{
  if (NULL == currentItem) return EC_IllegalCall;
  if (selectedMacParametersItem == NULL) return SI_EC_VerificationFailed_NoMAC;
  if ((selectedCertificate == NULL)||(selectedCertificate->getKeyType() == EKT_none)) return SI_EC_VerificationFailed_NoCertificate;

  E_TransferSyntax xfer = EXS_Unknown;
  DcmStack stack;

  // check if the profile only applies to main dataset level
  if ((currentItem->ident() != EVR_dataset) && sprof.mainDatasetRequired())
    return SI_EC_DatasetDoesNotMatchProfile;

  // let the signature profile handler inspect the dataset first
  OFCondition result = sprof.inspectSignatureDataset(*currentItem);

  // check MAC Calculation Transfer Syntax UID
  if (result.good())
  {
    if ((selectedMacParametersItem->search(DCM_MACCalculationTransferSyntaxUID, stack, ESM_fromHere, OFFalse)).good() && (stack.top()->isLeaf()))
    {
      char *uid = NULL;
      if ((((DcmElement *)(stack.top()))->getString(uid)).good())
      {
        DcmXfer xf(uid);
        xfer = xf.getXfer();
        if ((xfer == EXS_Unknown) || (! sprof.isAllowableTransferSyntax(xfer))) result = SI_EC_TransferSyntaxDoesNotMatchProfile;
      } else result = SI_EC_VerificationFailed_NoMAC;
    } else result = SI_EC_VerificationFailed_NoMAC;
  }

  // check signature algorithm type
  if (result.good())
  {
    if (! sprof.isAllowableAlgorithmType(selectedCertificate->getKeyType())) result = SI_EC_AlgorithmDoesNotMatchProfile;
  }

  // check MAC Algorithm
  if (result.good())
  {
    E_MACType mac = EMT_RIPEMD160;
    stack.clear();
    if ((selectedMacParametersItem->search(DCM_MACAlgorithm, stack, ESM_fromHere, OFFalse)).good() && (stack.top()->isLeaf()))
    {
      OFString macidentifier;
      if ((((DcmElement *)(stack.top()))->getOFString(macidentifier, 0)).good())
      {
        if (macidentifier == SI_DEFTERMS_RIPEMD160) mac = EMT_RIPEMD160;
        else if (macidentifier == SI_DEFTERMS_SHA1) mac = EMT_SHA1;
        else if (macidentifier == SI_DEFTERMS_MD5)  mac = EMT_MD5;
        else if (macidentifier == SI_DEFTERMS_SHA256) mac = EMT_SHA256;
        else if (macidentifier == SI_DEFTERMS_SHA384) mac = EMT_SHA384;
        else if (macidentifier == SI_DEFTERMS_SHA512) mac = EMT_SHA512;
        else result = SI_EC_VerificationFailed_UnsupportedMACAlgorithm;
        if (result.good() && (! sprof.isAllowableMACType(mac))) result = SI_EC_MacDoesNotMatchProfile;
      } else result = SI_EC_VerificationFailed_NoMAC;
    } else result = SI_EC_VerificationFailed_NoMAC;
  }

  // check list of Data Elements Signed
  if (result.good())
  {
    stack.clear();
    if ((selectedMacParametersItem->search(DCM_DataElementsSigned, stack, ESM_fromHere, OFFalse)).good() && (stack.top()->ident() == EVR_AT))
    {
      DcmAttributeTag *tagList = new DcmAttributeTag(*((DcmAttributeTag *)(stack.top())));
      if (tagList == NULL) result = EC_MemoryExhausted;
      else
      {
        if (! sprof.checkAttributeList(*currentItem, *tagList)) result = SI_EC_DataElementsSignedDoesNotMatchProfile;
      }
      delete tagList;
    }
    else result = SI_EC_VerificationFailed_NoDataElementsSigned; // DataElementsSigned is absent or does not have "AT" VR
  }

  return result;
}

OFCondition DcmSignature::checkListOfSignedTags(const DcmAttributeTag *tagList, const DcmAttributeTag *tagListOut)
{
  if (tagListOut == NULL) return EC_IllegalCall;

  OFCondition result = EC_Normal;
  if (tagList)
  {
    DcmAttributeTag tagListNonConst(*tagList); // we need a non-const copy of tagList
    DcmTagKey key;
    unsigned long vm = tagListNonConst.getVM();
    for (unsigned long i=0; i < vm; i++)
    {
      if ((tagListNonConst.getTagVal(key, i)).good() && (! inTagList(key, *tagListOut)))
      {
        result = EC_TagNotFound;
        DCMSIGN_ERROR("attribute " << key << " should be signed but is not present in the dataset.");
      }
    }
  }
  return result;
}

OFBool DcmSignature::inTagList(const DcmTagKey &tag, const DcmAttributeTag& tagList)
{
  DcmTagKey key;
  DcmAttributeTag tagListNonConst(tagList); // we need a non-const copy of tagList
  unsigned long vm = tagListNonConst.getVM();
  for (unsigned long i=0; i < vm; i++)
  {
    if ((tagListNonConst.getTagVal(key, i)).good() && (key == tag)) return OFTrue;
  }
  return OFFalse;
}



#else /* WITH_OPENSSL */

#include "dcmtk/dcmsign/sidefine.h"

DCMTK_DCMSIGN_EXPORT int dcmsign_cc_dummy_to_keep_linker_from_moaning = 0;

#endif
