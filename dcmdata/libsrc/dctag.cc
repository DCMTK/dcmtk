/*
 *
 *  Copyright (C) 1994-2010, OFFIS e.V.
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
 *  Module:  dcmdata
 *
 *  Author:  Gerd Ehlers, Andrew Hewett
 *
 *  Purpose: class DcmTag
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:09 $
 *  CVS/RCS Revision: $Revision: 1.24 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dctag.h"
#include "dcmtk/dcmdata/dcerror.h"    /* for dcmdata error constants */
#include "dcmtk/dcmdata/dcdict.h"
#include "dcmtk/dcmdata/dcdicent.h"

#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#include "dcmtk/ofstd/ofstdinc.h"


DcmTag::DcmTag()
  : vr(EVR_UNKNOWN),
    tagName(NULL),
    privateCreator(NULL),
    errorFlag(EC_InvalidTag)
{
}

DcmTag::DcmTag(const DcmTagKey& akey)
  : DcmTagKey(akey),
    vr(EVR_UNKNOWN),
    tagName(NULL),
    privateCreator(NULL),
    errorFlag(EC_InvalidTag)
{
    lookupVRinDictionary();
}

DcmTag::DcmTag(Uint16 g, Uint16 e)
  : DcmTagKey(g, e),
    vr(EVR_UNKNOWN),
    tagName(NULL),
    privateCreator(NULL),
    errorFlag(EC_InvalidTag)
{
    lookupVRinDictionary();
}

DcmTag::DcmTag(Uint16 g, Uint16 e, const DcmVR& avr)
  : DcmTagKey(g, e),
    vr(avr),
    tagName(NULL),
    privateCreator(NULL),
    errorFlag(EC_Normal)
{
}

DcmTag::DcmTag(const DcmTagKey& akey, const DcmVR& avr)
  : DcmTagKey(akey),
    vr(avr),
    tagName(NULL),
    privateCreator(NULL),
    errorFlag(EC_Normal)
{
}

DcmTag::DcmTag(const DcmTag& tag)
  : DcmTagKey(tag),
    vr(tag.vr),
    tagName(NULL),
    privateCreator(NULL),
    errorFlag(tag.errorFlag)
{
  updateTagName(tag.tagName);
  updatePrivateCreator(tag.privateCreator);
}


// ********************************


DcmTag::~DcmTag()
{
  delete[] tagName;
  delete[] privateCreator;
}


// ********************************


DcmTag& DcmTag::operator= ( const DcmTag& tag )
{
    if (this != &tag)
    {
      updateTagName(tag.tagName);
      updatePrivateCreator(tag.privateCreator);
      DcmTagKey::set(tag);
      vr = tag.vr;
      errorFlag = tag.errorFlag;
    }
    return *this;
}

// ********************************

void DcmTag::lookupVRinDictionary()
{
    const DcmDataDictionary& globalDataDict = dcmDataDict.rdlock();
    const DcmDictEntry *dictRef = globalDataDict.findEntry(*this, privateCreator);
    if (dictRef)
    {
        vr = dictRef->getVR();
        errorFlag = EC_Normal;
    }
    dcmDataDict.unlock();
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
  const DcmDictEntry *dictRef = globalDataDict.findEntry(*this, privateCreator);
  if (dictRef) newTagName=dictRef->getTagName();
  if (newTagName == NULL) newTagName = DcmTag_ERROR_TagName;
  updateTagName(newTagName);
  dcmDataDict.unlock();

  if (tagName) return tagName;
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


OFCondition DcmTag::findTagFromName(const char *name,
                                    DcmTag &value)
{
    OFCondition result = EC_IllegalParameter;
    /* check parameters first */
    if ((name != NULL) && (strlen(name) > 0))
    {
        result = EC_Normal;
        unsigned int grp = 0xffff;
        unsigned int elm = 0xffff;
        /* check whether tag name has format 'gggg,eeee' */
        if (sscanf(name, "%x,%x", &grp, &elm) == 2)
        {
            /* store resulting tag value */
            value.set(OFstatic_cast(Uint16, grp), OFstatic_cast(Uint16, elm));
            value.lookupVRinDictionary();
        } else {
            /* it is a name: look up in the dictionary */
            const DcmDataDictionary &globalDataDict = dcmDataDict.rdlock();
            const DcmDictEntry *dicent = globalDataDict.findEntry(name);
            /* store resulting tag value */
            if (dicent != NULL)
            {
              value.set(dicent->getKey());
              value.setVR(dicent->getVR());
            }
            else
                result = EC_TagNotFound;
            dcmDataDict.unlock();
        }
    }
    return result;
}


const char* DcmTag::getPrivateCreator() const
{
    return privateCreator;
}

void DcmTag::setPrivateCreator(const char *privCreator)
{
    // a new private creator code probably changes the name
    // of the tag. Enforce new dictionary lookup the next time
    // getTagName() is called.
    updateTagName(NULL);
    updatePrivateCreator(privCreator);
}

void DcmTag::updateTagName(const char *c)
{
    delete[] tagName;
    if (c)
    {
      tagName = new char[strlen(c) + 1];
      if (tagName) strcpy(tagName,c);
    } else tagName = NULL;
}

void DcmTag::updatePrivateCreator(const char *c)
{
    delete[] privateCreator;
    if (c)
    {
      privateCreator = new char[strlen(c) + 1];
      if (privateCreator) strcpy(privateCreator,c);
    } else privateCreator = NULL;
}


/*
** CVS/RCS Log:
** $Log: dctag.cc,v $
** Revision 1.24  2010-10-14 13:14:09  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.23  2009-06-04 17:10:19  joergr
** Fixed minor source code formatting issues.
**
** Revision 1.22  2008-02-04 11:51:56  onken
** Added missing VR lookup for function findTagFromName().
**
** Revision 1.21  2005-12-08 15:41:39  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.20  2004/02/04 16:45:38  joergr
** Adapted type casts to new-style typecast operators defined in ofcast.h.
** Removed acknowledgements with e-mail addresses from CVS log.
**
** Revision 1.19  2003/03/21 13:08:04  meichel
** Minor code purifications for warnings reported by MSVC in Level 4
**
** Revision 1.18  2002/11/27 12:06:52  meichel
** Adapted module dcmdata to use of new header file ofstdinc.h
**
** Revision 1.17  2002/07/23 14:21:34  meichel
** Added support for private tag data dictionaries to dcmdata
**
** Revision 1.16  2002/05/24 09:49:44  joergr
** Renamed some parameters/variables to avoid ambiguities.
**
** Revision 1.15  2002/04/30 13:12:58  joergr
** Added static helper function to convert strings (tag names or group/element
** numbers) to DICOM tag objects.
**
** Revision 1.14  2002/04/16 13:43:22  joergr
** Added configurable support for C++ ANSI standard includes (e.g. streams).
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
