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
 *    classes: SiMACConstructor
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2000-11-07 16:49:05 $
 *  CVS/RCS Revision: $Revision: 1.1 $
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


SI_E_Condition SiMACConstructor::flushBuffer(SiMAC& mac)
{
  SI_E_Condition result = SI_EC_Normal;
  void *bufptr = NULL;
  Uint32 bufLen = 0;
  stream.GetBuffer(bufptr, bufLen);
  if (bufLen > 0) 
  {
    if (dumpFile) fwrite(bufptr, 1, bufLen, dumpFile);
    result = mac.digest((unsigned char *)bufptr, bufLen);
  }
  return result;
}


SI_E_Condition SiMACConstructor::encodeElement(DcmElement *element, SiMAC& mac, E_TransferSyntax oxfer)
{
  if (element == NULL) return SI_EC_IllegalCall;
  SI_E_Condition result = SI_EC_Normal;
  E_Condition cond = EC_Normal;
  OFBool last = OFFalse;
  while (!last) 
  {
    cond = element->writeSignatureFormat(stream, oxfer, EET_ExplicitLength);
    if (cond == EC_StreamNotifyClient) result = flushBuffer(mac); 
    else 
    {
      last=OFTrue;
      if (EC_Normal != cond) result = SI_EC_DcmDataFailure;
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


SI_E_Condition SiMACConstructor::encodeDataset(
  DcmItem& item, 
  SiMAC& mac, 
  E_TransferSyntax oxfer,
  DcmAttributeTag &tagListOut,
  DcmAttributeTag *tagListIn)
{
  tagListOut.clear();
  if (! item.canWriteXfer(oxfer, EXS_Unknown)) return SI_EC_WrongTransferSyntax;  
  SI_E_Condition result = SI_EC_Normal;
  item.transferInit();
  unsigned long numElements = item.card();
  DcmElement *element;
  for (unsigned long i=0; i < numElements; i++)
  {
    element = item.getElement(i);    
    if ((SI_EC_Normal == result) && (inTagList(element, tagListIn))) 
    {
      // if the element is signable, we should encode it
      if (element->getTag().isSignable())
      {
        result = encodeElement(element, mac, oxfer);
        if (SI_EC_Normal == result)
        {
          if (EC_Normal != tagListOut.putTagVal(element->getTag(), tagListOut.getVM())) result = SI_EC_DcmDataFailure;
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
 *  Revision 1.1  2000-11-07 16:49:05  meichel
 *  Initial release of dcmsign module for DICOM Digital Signatures
 *
 *
 */

