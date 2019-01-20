/*
 *
 *  Copyright (C) 1998-2019, OFFIS e.V.
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
 *    classes: SiMACConstructor
 *
 */

#include "dcmtk/config/osconfig.h"

#ifdef WITH_OPENSSL

#include "dcmtk/dcmsign/simaccon.h"
#include "dcmtk/dcmsign/simac.h"
#include "dcmtk/dcmdata/dcitem.h"
#include "dcmtk/dcmdata/dcvrat.h"
#include "dcmtk/dcmdata/dcwcache.h"

// block size used for the memory buffer
#define SiMACConstructor_BlockSize 16384


SiMACConstructor::SiMACConstructor()
: buf(new unsigned char[SiMACConstructor_BlockSize])
, stream(buf, SiMACConstructor_BlockSize)
, dumpFile(NULL)
{
}


SiMACConstructor::~SiMACConstructor()
{
  delete[] buf;
}

void SiMACConstructor::setDumpFile(FILE *f)
{
  dumpFile = f;
}


OFCondition SiMACConstructor::flushBuffer(SiMAC& mac)
{
  OFCondition result = EC_Normal;
  void *bufptr = NULL;
  offile_off_t bufLen = 0;
  stream.flushBuffer(bufptr, bufLen);
  if (bufLen > 0)
  {
    if (dumpFile)
    {
      if (fwrite(bufptr, 1, OFstatic_cast(size_t, bufLen), dumpFile) != OFstatic_cast(size_t, bufLen))
      {
        // We are apparently unable to write the byte stream to a dump file.
        // This does not prevent us, however, from creating a valid digital signature.
        // Therefore, issue a warning but continue.
        DCMSIGN_WARN("Write error while dumping byte stream to file");
      }
    }
    result = mac.digest((unsigned char *)bufptr, (unsigned long)bufLen);
  }
  return result;
}


OFCondition SiMACConstructor::encodeElement(DcmElement *element, SiMAC& mac, E_TransferSyntax oxfer)
{
  if (element == NULL) return EC_IllegalCall;
  DcmWriteCache wcache;
  OFCondition result = EC_Normal;
  OFBool last = OFFalse;
  element->transferInit();
  while (!last)
  {
    result = element->writeSignatureFormat(stream, oxfer, EET_ExplicitLength, &wcache);
    if (result == EC_StreamNotifyClient) result = flushBuffer(mac);
    else
    {
      last=OFTrue;
    }
  }
  element->transferEnd();
  return result;
}


OFCondition SiMACConstructor::flush(SiMAC& mac)
{
  OFCondition result = EC_Normal;
  while (! stream.isFlushed() && result.good())
  {
    stream.flush();
    result = flushBuffer(mac);
  }
  return result;
}


OFBool SiMACConstructor::inTagList(const DcmElement *element, DcmAttributeTag *tagList)
{
  if (element == NULL) return OFFalse;
  if (tagList == NULL) return OFTrue; // tag list absent, use all tags

  DcmTagKey key;
  const DcmTag &elementTag = element->getTag();
  unsigned long vm = tagList->getVM();
  for (unsigned long i=0; i < vm; i++)
  {
    if ((tagList->getTagVal(key, i)).good() && (key == elementTag)) return OFTrue;
  }
  return OFFalse;
}

OFCondition SiMACConstructor::encodeDigitalSignatureItem(
  DcmItem& signatureItem,
  SiMAC& mac,
  E_TransferSyntax oxfer)
{
  if (! signatureItem.canWriteXfer(oxfer, EXS_Unknown)) return SI_EC_WrongTransferSyntax;
  OFCondition result = EC_Normal;
  signatureItem.transferInit();
  unsigned long numElements = signatureItem.card();
  DcmElement *element;
  DcmTagKey tagkey;
  for (unsigned long i=0; i < numElements; i++)
  {
    element = signatureItem.getElement(i);
    if (result.good())
    {
      if (element->isSignable())
      {
        tagkey = element->getTag().getXTag();
        if ((tagkey != DCM_CertificateOfSigner) &&
            (tagkey != DCM_Signature) &&
            (tagkey != DCM_CertifiedTimestampType) &&
            (tagkey != DCM_CertifiedTimestamp))
        {
          result = encodeElement(element, mac, oxfer);
        }
      }
    }
  }

  /* done, flush stream buffer */
  result = flushBuffer(mac);
  signatureItem.transferEnd();
  return result;
}


OFCondition SiMACConstructor::encodeDataset(
  DcmItem& item,
  SiMAC& mac,
  E_TransferSyntax oxfer,
  DcmAttributeTag &tagListOut,
  DcmAttributeTag *tagListIn)
{
  tagListOut.clear();
  if (! item.canWriteXfer(oxfer, EXS_Unknown)) return SI_EC_WrongTransferSyntax;
  OFCondition result = EC_Normal;
  item.transferInit();
  unsigned long numElements = item.card();
  DcmElement *element;
  for (unsigned long i=0; i < numElements; i++)
  {
    element = item.getElement(i);
    if (result.good() && (inTagList(element, tagListIn)))
    {
      // if the element is signable, we should encode it
      if (element->isSignable())
      {
        result = encodeElement(element, mac, oxfer);
        if (result.good())
        {
          result = tagListOut.putTagVal(element->getTag(), tagListOut.getVM());
        }
      }
    }
  }

  /* done, flush stream buffer */
  result = flushBuffer(mac);
  item.transferEnd();
  return result;
}

#else /* WITH_OPENSSL */

int simaccon_cc_dummy_to_keep_linker_from_moaning = 0;

#endif
