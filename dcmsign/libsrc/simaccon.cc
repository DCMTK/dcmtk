/*
 *
 *  Copyright (C) 1998-2010, OFFIS e.V.
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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:38 $
 *  CVS/RCS Revision: $Revision: 1.12 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
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
    if (dumpFile) fwrite(bufptr, 1, (size_t)bufLen, dumpFile);
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

/*
 *  $Log: simaccon.cc,v $
 *  Revision 1.12  2010-10-14 13:14:38  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.11  2010-10-04 13:23:07  joergr
 *  Added explicit type cast in order to keep VisualStudio 2008 quiet.
 *
 *  Revision 1.10  2007-11-29 14:42:21  meichel
 *  Write methods now handle large raw data elements (such as pixel data)
 *    without loading everything into memory. This allows very large images to
 *    be sent over a network connection, or to be copied without ever being
 *    fully in memory.
 *
 *  Revision 1.9  2007/02/19 16:53:45  meichel
 *  Class DcmOutputStream and related classes are now safe for use with
 *    large files (2 GBytes or more) if supported by compiler and operating system.
 *
 *  Revision 1.8  2005/12/08 15:47:24  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.7  2002/12/16 12:57:51  meichel
 *  Minor modification to shut up linker on MacOS X when compiling
 *    without OpenSSL support
 *
 *  Revision 1.6  2002/08/27 17:21:01  meichel
 *  Initial release of new DICOM I/O stream classes that add support for stream
 *    compression (deflated little endian explicit VR transfer syntax)
 *
 *  Revision 1.5  2001/11/16 15:50:54  meichel
 *  Adapted digital signature code to final text of supplement 41.
 *
 *  Revision 1.4  2001/09/26 14:30:25  meichel
 *  Adapted dcmsign to class OFCondition
 *
 *  Revision 1.3  2001/06/01 15:50:54  meichel
 *  Updated copyright header
 *
 *  Revision 1.2  2000/11/07 18:07:08  joergr
 *  Minor code purifications to keep Sun CC 2.0.1 quiet.
 *
 *  Revision 1.1  2000/11/07 16:49:05  meichel
 *  Initial release of dcmsign module for DICOM Digital Signatures
 *
 *
 */
