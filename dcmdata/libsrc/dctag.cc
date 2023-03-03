/*
 *
 *  Copyright (C) 1994-2023, OFFIS e.V.
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
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dctag.h"
#include "dcmtk/dcmdata/dcerror.h"    /* for dcmdata error constants */
#include "dcmtk/dcmdata/dcdict.h"
#include "dcmtk/dcmdata/dcdicent.h"
#include "dcmtk/ofstd/ofstd.h"

DcmTag::DcmTag()
  : vr(EVR_UNKNOWN),
    tagName(NULL),
    privateCreator(NULL),
    errorFlag(EC_InvalidTag)
{
}


DcmTag::DcmTag(const DcmTagKey& akey, const char *privCreator)
  : DcmTagKey(akey),
    vr(EVR_UNKNOWN),
    tagName(NULL),
    privateCreator(NULL),
    errorFlag(EC_InvalidTag)
{
    if (privCreator)
        updatePrivateCreator(privCreator);
    lookupVRinDictionary();
}


DcmTag::DcmTag(Uint16 g, Uint16 e, const char *privCreator)
  : DcmTagKey(g, e),
    vr(EVR_UNKNOWN),
    tagName(NULL),
    privateCreator(NULL),
    errorFlag(EC_InvalidTag)
{
    if (privCreator)
        updatePrivateCreator(privCreator);
    lookupVRinDictionary();
}


DcmTag::DcmTag(const DcmTagKey& akey, const DcmVR& avr)
  : DcmTagKey(akey),
    vr(avr),
    tagName(NULL),
    privateCreator(NULL),
    errorFlag(EC_Normal)
{
}


DcmTag::DcmTag(Uint16 g, Uint16 e, const DcmVR& avr)
  : DcmTagKey(g, e),
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


DcmTag::DcmTag(const DcmTag& tag, const DcmVR& avr)
  : DcmTagKey(tag),
    vr(avr),
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


DcmTag& DcmTag::operator=(const DcmTag& tag)
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


OFBool DcmTag::operator==(const DcmTag& tag) const
{
    OFBool result = DcmTagKey::operator==(tag);
    if (result)
    {
         // check whether private creator identifiers are identical
        if ((privateCreator != NULL) && (tag.getPrivateCreator() != NULL))
            result = (strcmp(privateCreator, tag.getPrivateCreator()) == 0);
        else if (privateCreator != tag.getPrivateCreator())
            result = OFFalse;
    }
    return result;
}


OFBool DcmTag::operator!=(const DcmTag& tag) const
{
    OFBool result = DcmTagKey::operator!=(tag);
    if (!result)
    {
         // check whether private creator identifiers are different
        if ((privateCreator != NULL) && (tag.getPrivateCreator() != NULL))
            result = (strcmp(privateCreator, tag.getPrivateCreator()) != 0);
        else if (privateCreator != tag.getPrivateCreator())
            result = OFTrue;
    }
    return result;
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
    dcmDataDict.rdunlock();
}


// ********************************


DcmVR DcmTag::setVR(const DcmVR& avr)    // resolve ambiguous VR
{
    vr = avr;

    if (vr.getEVR() == EVR_UNKNOWN)
    {
        errorFlag = EC_InvalidVR;
    } else {
        errorFlag = EC_Normal;
    }
    return vr;
}


const char *DcmTag::getTagName()
{
    if (tagName)
        return tagName;

    const char *newTagName = NULL;
    const DcmDataDictionary& globalDataDict = dcmDataDict.rdlock();
    const DcmDictEntry *dictRef = globalDataDict.findEntry(*this, privateCreator);
    if (dictRef)
        newTagName=dictRef->getTagName();
    if (newTagName == NULL)
        newTagName = DcmTag_ERROR_TagName;
    updateTagName(newTagName);
    dcmDataDict.rdunlock();

    if (tagName)
        return tagName;
    return DcmTag_ERROR_TagName;
}


OFBool DcmTag::isSignable() const
{
    OFBool result = isSignableTag();
    if (result)
        result = !isUnknownVR();
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


OFCondition DcmTag::findTagFromName(const char *name, DcmTag &value)
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
            dcmDataDict.rdunlock();
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
    // a new private creator identifier probably changes the name of the tag.
    // Enforce new dictionary lookup the next time getTagName() is called.
    updateTagName(NULL);
    updatePrivateCreator(privCreator);
}


void DcmTag::updateTagName(const char *c)
{
    delete[] tagName;
    if (c)
    {
        size_t buflen = strlen(c) + 1;
        tagName = new char[buflen];
        if (tagName)
            OFStandard::strlcpy(tagName, c, buflen);
    } else
        tagName = NULL;
}


void DcmTag::updatePrivateCreator(const char *c)
{
    delete[] privateCreator;
    if (c)
    {
        size_t buflen = strlen(c) + 1;
        privateCreator = new char[buflen];
        if (privateCreator)
            OFStandard::strlcpy(privateCreator, c, buflen);
    } else
        privateCreator = NULL;
}
