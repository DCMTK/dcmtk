/*
 *
 *  Copyright (C) 1994-2001, OFFIS
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
 *  Module:  dcmdata
 *
 *  Author:  Gerd Ehlers, Andrew Hewett
 *
 *  Purpose: class DcmTag
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2002-04-16 13:43:22 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dctag.cc,v $
 *  CVS/RCS Revision: $Revision: 1.14 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <string.h>

#include "ofstream.h"
#include "ofconsol.h"
#include "dcdict.h"
#include "dctag.h"
#include "dcdeftag.h"


// ********************************

DcmTag::DcmTag()
  : vr(EVR_UNKNOWN),
    tagName(NULL),
    errorFlag(EC_InvalidTag)
{
}

DcmTag::DcmTag(const DcmTagKey& akey)
  : DcmTagKey(akey),
    vr(EVR_UNKNOWN),
    tagName(NULL),
    errorFlag(EC_InvalidTag)
{
    const DcmDataDictionary& globalDataDict = dcmDataDict.rdlock();
    const DcmDictEntry *dictRef = globalDataDict.findEntry(akey);
    if (dictRef)
    {
        vr = dictRef->getVR();
        errorFlag = EC_Normal;
    }
    dcmDataDict.unlock();
}
    
DcmTag::DcmTag(Uint16 g, Uint16 e)
  : DcmTagKey(g, e), 
    vr(EVR_UNKNOWN),
    tagName(NULL),
    errorFlag(EC_InvalidTag)
{
    const DcmDataDictionary& globalDataDict = dcmDataDict.rdlock();
    const DcmDictEntry *dictRef = globalDataDict.findEntry(DcmTagKey(g, e));
    if (dictRef)
    {
        vr = dictRef->getVR();
        errorFlag = EC_Normal;
    }
    dcmDataDict.unlock();
}

DcmTag::DcmTag(Uint16 g, Uint16 e, const DcmVR& avr)
  : DcmTagKey(g, e),
    vr(avr),
    tagName(NULL),
    errorFlag(EC_Normal)
{
}

DcmTag::DcmTag(const DcmTagKey& akey, const DcmVR& avr)
  : DcmTagKey(akey),
    vr(avr),
    tagName(NULL),
    errorFlag(EC_Normal)
{
}

DcmTag::DcmTag(const DcmTag& tag)
  : DcmTagKey(tag),
    vr(tag.vr),
    tagName(NULL),
    errorFlag(tag.errorFlag)
{
  if (tag.tagName)
  {
    tagName = new char[strlen(tag.tagName)+1];
    if (tagName) strcpy(tagName,tag.tagName);
  }
}



// ********************************


DcmTag::~DcmTag()
{
  delete[] tagName;
}


// ********************************


DcmTag& DcmTag::operator= ( const DcmTag& tag )
{
    if (this != &tag)
    {
      delete[] tagName;
      if (tag.tagName)
      {
        tagName = new char[strlen(tag.tagName)+1];
        if (tagName) strcpy(tagName,tag.tagName);
      } else tagName = NULL;
      
      DcmTagKey::set(tag);
      vr = tag.vr;
      errorFlag = tag.errorFlag;
    }
    return *this;
}

// ********************************

DcmTag& DcmTag::operator= ( const DcmTagKey& key )
{
    DcmTagKey::set(key);
    delete[] tagName;
    tagName = NULL;

    const DcmDataDictionary& globalDataDict = dcmDataDict.rdlock();
    const DcmDictEntry *dictRef = globalDataDict.findEntry(key);
    if (dictRef)
    {
        vr = dictRef->getVR();
        errorFlag = EC_Normal;
    } else {
        vr.setVR(EVR_UNKNOWN);
        errorFlag = EC_InvalidTag;
    }
    dcmDataDict.unlock();
    return *this;
}

// ********************************


DcmVR DcmTag::setVR( const DcmVR& avr )    // resolve ambiguous VR
{
    vr = avr;

    if ( vr.getEVR() == EVR_UNKNOWN ) {
        errorFlag = EC_InvalidVR;
    } else {
        errorFlag = EC_Normal;
    }
    return vr;
}


const char *DcmTag::getTagName()
{
  if (tagName) return tagName;
  
  const char *newTagName = NULL;
  const DcmDataDictionary& globalDataDict = dcmDataDict.rdlock();
  const DcmDictEntry *dictRef = globalDataDict.findEntry(*this);
  if (dictRef) newTagName=dictRef->getTagName();
  if (newTagName==NULL) newTagName = DcmTag_ERROR_TagName;
  tagName = new char[strlen(newTagName)+1];
  if (tagName) 
  {  
    strcpy(tagName,newTagName);
    dcmDataDict.unlock();
    return tagName;
  }
  dcmDataDict.unlock();
  return DcmTag_ERROR_TagName;
}

OFBool DcmTag::isSignable() const
{
  OFBool result = isSignableTag();
  if (result) result = (! isUnknownVR());
  return result;
}

OFBool DcmTag::isUnknownVR() const
{
  OFBool result = OFFalse;
  switch (vr.getValidEVR()) // this is the VR we're going to write in explicit VR
  {
    case EVR_UNKNOWN:
    case EVR_UNKNOWN2B:
    case EVR_UN:
      result = OFTrue;
      break;
    default:
      /* nothing */
      break;
  }
  return result;
}


/*
** CVS/RCS Log:
** $Log: dctag.cc,v $
** Revision 1.14  2002-04-16 13:43:22  joergr
** Added configurable support for C++ ANSI standard includes (e.g. streams).
** Thanks to Andreas Barth <Andreas.Barth@bruker-biospin.de> for his
** contribution.
**
** Revision 1.13  2001/11/19 15:23:29  meichel
** Cleaned up signature code to avoid some gcc warnings.
**
** Revision 1.12  2001/11/16 15:55:05  meichel
** Adapted digital signature code to final text of supplement 41.
**
** Revision 1.11  2001/06/01 15:49:11  meichel
** Updated copyright header
**
** Revision 1.10  2000/05/03 14:19:10  meichel
** Added new class GlobalDcmDataDictionary which implements read/write lock
**   semantics for safe access to the DICOM dictionary from multiple threads
**   in parallel. The global dcmDataDict now uses this class.
**
** Revision 1.9  2000/04/14 16:01:00  meichel
** Restructured class DcmTag. Instances don't keep a permanent pointer
**   to a data dictionary entry anymore. Required for MT applications.
**
** Revision 1.8  2000/03/08 16:26:42  meichel
** Updated copyright header.
**
** Revision 1.7  2000/03/03 14:05:37  meichel
** Implemented library support for redirecting error messages into memory
**   instead of printing them to stdout/stderr for GUI applications.
**
** Revision 1.6  1999/03/31 09:25:41  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.5  1998/07/15 15:52:09  joergr
** Removed several compiler warnings reported by gcc 2.8.1 with
** additional options, e.g. missing copy constructors and assignment
** operators, initialization of member variables in the body of a
** constructor instead of the member initialization list, hiding of
** methods by use of identical names, uninitialized member variables,
** missing const declaration of char pointers. Replaced tabs by spaces.
**
** Revision 1.4  1996/03/12 15:24:21  hewett
** Added constructor to allow direct setting of the VR.
**
** Revision 1.3  1996/01/05 13:27:44  andreas
** - changed to support new streaming facilities
** - unique read/write methods for file and block transfer
** - more cleanups
**
** Revision 1.2  1995/11/23 17:02:54  hewett
** Updated for loadable data dictionary.  Some cleanup (more to do).
**
*/
