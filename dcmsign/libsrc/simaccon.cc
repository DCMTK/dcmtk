/*
 *
 *  Copyright (C) 1998-2001, OFFIS
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
 *    classes: SiMACConstructor
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2001-09-26 14:30:25 $
 *  CVS/RCS Revision: $Revision: 1.4 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"

#ifdef WITH_OPENSSL

#include "simaccon.h"
#include "simac.h"
#include "dcitem.h"
#include "dcvrat.h"

// block size used for the memory buffer
#define SiMACConstructor_BlockSize 16384


SiMACConstructor::SiMACConstructor()
: stream(SiMACConstructor_BlockSize, OFFalse)
, dumpFile(NULL)
{
}

 
SiMACConstructor::~SiMACConstructor()
{
}

void SiMACConstructor::setDumpFile(FILE *f)
{
  dumpFile = f;
}


OFCondition SiMACConstructor::flushBuffer(SiMAC& mac)
{
  OFCondition result = EC_Normal;
  void *bufptr = NULL;
  Uint32 bufLen = 0;
  stream.GetBuffer(bufptr, bufLen);
  if (bufLen > 0) 
  {
    if (dumpFile) fwrite(bufptr, 1, (size_t)bufLen, dumpFile);
    result = mac.digest((unsigned char *)bufptr, bufLen);
  }
  return result;
}


OFCondition SiMACConstructor::encodeElement(DcmElement *element, SiMAC& mac, E_TransferSyntax oxfer)
{
  if (element == NULL) return EC_IllegalCall;
  OFCondition result = EC_Normal;
  OFBool last = OFFalse;
  while (!last) 
  {
    result = element->writeSignatureFormat(stream, oxfer, EET_ExplicitLength);
    if (result == EC_StreamNotifyClient) result = flushBuffer(mac); 
    else 
    {
      last=OFTrue;
    }
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
    if ((EC_Normal == tagList->getTagVal(key, i)) && (key == elementTag)) return OFTrue;
  }
  return OFFalse;
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
    if ((EC_Normal == result) && (inTagList(element, tagListIn))) 
    {
      // if the element is signable, we should encode it
      if (element->getTag().isSignable())
      {
        result = encodeElement(element, mac, oxfer);
        if (EC_Normal == result)
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

const int simaccon_cc_dummy_to_keep_linker_from_moaning = 0;

#endif

/*
 *  $Log: simaccon.cc,v $
 *  Revision 1.4  2001-09-26 14:30:25  meichel
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

