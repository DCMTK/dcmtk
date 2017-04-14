/*
 *
 *  Copyright (C) 2015-2017, Open Connections GmbH
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation are maintained by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module:  dcmiod
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Static helper functionality for dcmiod module
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dctypes.h"    // logger
#include "dcmtk/dcmiod/iodutil.h"
#include "dcmtk/dcmiod/iodrules.h"

#include "dcmtk/dcmdata/dcmetinf.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcdict.h"
#include "dcmtk/dcmdata/dcdicent.h"
#include "dcmtk/dcmdata/dcsequen.h"
#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/dcmdata/dcvrda.h"
#include "dcmtk/dcmdata/dcvrtm.h"
#include "dcmtk/dcmdata/dcuid.h"
#include "dcmtk/dcmdata/dcitem.h"


// --- static helpers ---

OFCondition DcmIODUtil::getAndCheckElementFromDataset(DcmItem &dataset,
                                                      DcmElement &delem,
                                                      const OFString &vm,
                                                      const OFString &type,
                                                      const char *moduleName)
{
  DcmStack stack;
  const DcmTagKey tagKey = delem.getTag();
  OFCondition result = dataset.search(tagKey, stack, ESM_fromHere, OFFalse /*searchIntoSub*/);
  if (result.good())
  {
    /* copy object from search stack */
    result = delem.copyFrom(*stack.top());
    /* we need a reference to the original element in order to determine the SpecificCharacterSet */
    checkElementValue(OFstatic_cast(DcmElement *, stack.top()), tagKey, vm, type, result, moduleName);
  }
  /* the element could not be found in the dataset */
  else
  {
    checkElementValue(delem, vm, type, result, moduleName);
  }
  return result;
}


OFCondition DcmIODUtil::getAndCheckElementFromDataset(DcmItem &dataset,
                                                      const DcmTagKey& tagKey,
                                                      DcmElement*& delem,
                                                      const OFString &vm,
                                                      const OFString &type,
                                                      const char *moduleName)
{
  if (delem)
    return EC_IllegalParameter;

  DcmStack stack;
  OFCondition result = dataset.search(tagKey, stack, ESM_fromHere, OFFalse /*searchIntoSub*/);
  if (result.good())
  {
    /* copy object from search stack */
    delem = OFstatic_cast ( DcmElement*, stack.top()->clone() );
    /* we need a reference to the original element in order to determine the SpecificCharacterSet */
    checkElementValue(OFstatic_cast(DcmElement *, stack.top()), tagKey, vm, type, result, moduleName);
  }
  /* the element could not be found in the dataset */
  else
    checkElementValue(delem, tagKey, vm, type, result, moduleName);

  return result;
}



OFCondition DcmIODUtil::getAndCheckElementFromDataset(DcmItem &dataset,
                                                      DcmElement &delem,
                                                      const IODRule* rule)
{
  if (rule == NULL)
    return EC_CannotCheck;

  return getAndCheckElementFromDataset(dataset, delem, rule->getVM(), rule->getType(), rule->getModule().c_str());
}


OFCondition DcmIODUtil::getAndCheckElementFromDataset(DcmItem &dataset,
                                                      DcmElement*& delem,
                                                      const IODRule* rule)
{
  if (rule == NULL)
    return EC_CannotCheck;

  return getAndCheckElementFromDataset(dataset, rule->getTagKey(), delem, rule->getVM(), rule->getType(), rule->getModule().c_str());
}



OFCondition DcmIODUtil::copyElementToDataset(OFCondition &result,
                                             DcmItem &dataset,
                                             const DcmElement &delem,
                                             const IODRule* rule)
{
  if (rule == NULL)
    return EC_CannotCheck;

  if (result.bad())
    return result;

  // addElementToDataset() consumes element, so create a copy
  DcmElement * copy = OFstatic_cast(DcmElement*, delem.clone());
  if (!copy)
    return EC_MemoryExhausted;

  return addElementToDataset(result, dataset, copy, rule);
}


OFCondition DcmIODUtil::copyElementToDataset(OFCondition &result,
                                             DcmItem &dataset,
                                             const DcmElement &delem,
                                             const OFString &vm,
                                             const OFString &type,
                                             const char *moduleName)
{
  // Create temporary rule (in order to avoid code-copying)
  IODRule rule(delem.getTag(), vm, type, moduleName, DcmIODTypes::IE_UNDEFINED);
  return copyElementToDataset(result, dataset, delem, &rule);
}


OFCondition DcmIODUtil::addElementToDataset(OFCondition &result,
                                            DcmItem &dataset,
                                            DcmElement *delem,
                                            const IODRule* rule)
{
  OFBool insertionOK = OFFalse;
  if (result.good())
  {
    if (rule != NULL)
    {
      // Create empty type 2 element if required
      OFString type = rule->getType();
      if (delem == NULL)
      {
        if (type == "2")
        {
          delem = DcmItem::newDicomElement(rule->getTagKey());
          if (delem == NULL)
          {
            result = EC_MemoryExhausted;
            return result;
          };
        } else if (type != "1")
        {
          // Not type 1 or type 2 means the is type 1C, 2C or 3. For those it is
          // fine to not insert anything
          return EC_Normal;
        }
        else
        {
          // type 1 is missing
          DCMIOD_ERROR(DcmTag(rule->getTagKey()).getTagName() << " " << rule->getTagKey() << " absent in " << rule->getModule() << " (type " << rule->getType() << ")");
          result = IOD_EC_MissingAttribute;
          return result;
        }
      }
      // At this point, we certainly have an element. Check its value (empty ok for type 2)
      if ((type == "2") || !delem->isEmpty())
      {
        // Insert non-empty element or empty "type 2" element. First, perform the insertion, and then
        // check the value. This is (at least) required for checking the character set of string values which
        // relies on the element context, i.e. the surrounding item or dataset.
        result = dataset.insert(delem, OFTrue /*replaceOld*/);
        if (result.good())
        {
          result = checkElementValue(*delem, rule->getVM(), type, result, rule->getModule().c_str());
        }
        if (result.good())
        {
          insertionOK = OFTrue;
        }
        // remove element if value is invalid
        else
        {
          dataset.remove(delem);
        }
      }
      else if (type == "1")
      {
        // Empty element value not allowed for "type 1"
        result = EC_InvalidValue;
        checkElementValue(*delem, rule->getVM(), type, result, rule->getModule().c_str());
      }
    }
    else
    {
      // No rule, no checks
      result = EC_CannotCheck;
    }
  }
  if (!insertionOK)
  {
    // Delete element since dataset did not take over ownership
    delete delem;
  }

  return result;
}


const OFString& DcmIODUtil::currentDate(OFString &dateString)
{
  if (DcmDate::getCurrentDate(dateString).bad())
  {
    dateString  = "";
  }
  return dateString;
}


const OFString& DcmIODUtil::currentTime(OFString &timeString)
{
  if (DcmTime::getCurrentTime(timeString, OFTrue /*seconds*/, OFFalse /*fraction*/).bad())
  {
    timeString = "";
  }
  return timeString;
}


OFCondition DcmIODUtil::checkElementValue(const DcmElement *delem,
                                          const DcmTagKey &tagKey,
                                          const OFString &vm,
                                          const OFString &type,
                                          const OFCondition &searchCond,
                                          const char *moduleName)
{
  OFCondition result = EC_Normal;
  const OFString tagName = DcmTag(tagKey).getTagName();
  const OFString module = (moduleName == NULL) ? "IOD" : moduleName;
  /* NB: type 1C and 2C cannot be checked, assuming to be optional */
  if (((type == "1") || (type == "2")) && searchCond.bad())
  {
    DCMIOD_WARN(tagName << " " << tagKey << " absent in " << module << " (type " << type << ")");
    result = IOD_EC_MissingAttribute;
  }
  else if ((delem == NULL) || OFconst_cast(DcmElement*, delem)->isEmpty(OFTrue /*normalize*/))   // cast away constness of delem; value modification can happen (eg. to remove padding)
  {
    /* however, type 1C should never be present with empty value */
    if (((type == "1") || (type == "1C")) && searchCond.good())
    {
      DCMIOD_WARN(tagName << " " << tagKey << " empty in " << module << " (type " << type << ")");
      result = EC_MissingValue;
    }
  } else {
    result = OFconst_cast(DcmElement*, delem)->checkValue(vm, OFTrue /*oldFormat*/);   // cast away constness of delem; value modification can happen (eg. to remove padding)
    if (result == EC_InvalidCharacter)
    {
      DCMIOD_WARN(tagName << " " << tagKey << " contains invalid character(s) in " << module);
    }
    else if (result == EC_ValueRepresentationViolated)
    {
      DCMIOD_WARN(tagName << " " << tagKey << " violates VR definition in " << module);
    }
    else if (result == EC_ValueMultiplicityViolated)
    {
      const OFString vmText = (delem->getVR() == EVR_SQ) ? " #items" : " VM";
      DCMIOD_WARN(tagName << " " << tagKey << vmText << " != " << vm << " in " << module);
    }
    else if (result == EC_MaximumLengthViolated)
    {
      DCMIOD_WARN(tagName << " " << tagKey << " violates maximum VR length in " << module);
    }
    else if (result.bad())
    {
      DCMIOD_WARN("INTERNAL ERROR while checking value of " << tagName << " " << tagKey << " in " << module);
      result = EC_Normal;
    }
  }
  return result;
}


OFCondition DcmIODUtil::checkElementValue(const DcmElement &delem,
                                          const OFString &vm,
                                          const OFString &type,
                                          const OFCondition &searchCond,
                                          const char *moduleName)
{
  /* call the real function */
  return checkElementValue(&delem, delem.getTag(), vm, type, searchCond, moduleName);
}


OFCondition DcmIODUtil::getStringValueFromElement(const DcmElement &delem,
                                                  OFString &stringValue,
                                                  const signed long pos)
{
  OFCondition result = EC_Normal;
  // cast away constness of delem; value modification can happen (eg. to remove padding)
  if (pos < 0)
    result = OFconst_cast(DcmElement &, delem).getOFStringArray(stringValue);
  else
    result = OFconst_cast(DcmElement &, delem).getOFString(stringValue, OFstatic_cast(unsigned long, pos));
  if (result.bad())
    stringValue.clear();
  return result;
}


OFCondition DcmIODUtil::getFloat64ValueFromElement(const DcmElement& delem,
                                                   Float64& result,
                                                   const unsigned long pos)
{
  return OFconst_cast(DcmElement &, delem).getFloat64(result, OFstatic_cast(unsigned long, pos));
}


OFCondition DcmIODUtil::getFloat64ValuesFromElement(const DcmElement& delem,
                                                    OFVector<Float64>& result)
{
  Float64* floats = NULL;
  OFCondition cond = OFconst_cast(DcmElement &, delem).getFloat64Array(floats);
  if (cond.good())
  {
    size_t vm = OFconst_cast(DcmElement &, delem).getVM();
    for (size_t n = 0; n < vm; n++)
    {
      result.push_back(floats[n]);
    }
  }
  return cond;
}


OFCondition DcmIODUtil::getStringValueFromItem(const DcmTagKey& key,
                                               DcmItem& item,
                                               OFString& result,
                                               const signed long& pos)
{
  DcmElement *elem = NULL;
  item.findAndGetElement(key, elem);
  if (elem != NULL)
    return DcmIODUtil::getStringValueFromElement(*elem, result, pos);
  else
    return EC_TagNotFound;
}


OFCondition DcmIODUtil::getFloat64ValueFromItem(const DcmTagKey& key,
                                                DcmItem& item,
                                                Float64& result,
                                                const unsigned long& pos)
{
  DcmElement *elem = NULL;
  OFCondition cond = item.findAndGetElement(key, elem);
  if (elem != NULL)
  {
    cond = DcmIODUtil::getFloat64ValueFromElement(*elem, result, pos);
  }
  return cond;
}


OFCondition DcmIODUtil::getFloat64ValuesFromItem(const DcmTagKey& key,
                                                 DcmItem& item,
                                                 OFVector<Float64>& result)
{
  DcmElement *elem = NULL;
  OFCondition cond = item.findAndGetElement(key, elem);
  if (elem != NULL)
  {
    cond = DcmIODUtil::getFloat64ValuesFromElement(*elem, result);
  }
  return cond;
}


OFCondition DcmIODUtil::setUint16ValuesOnElement(DcmElement &delem,
                                                 const OFVector<Uint16>& values,
                                                 const OFString& vm,
                                                 const OFBool check)
{
  OFCondition result;
  OFVector<Uint16>::const_iterator it = values.begin();
  for (size_t count=0; count < values.size(); count++ )
  {
    result = delem.putUint16( (*it) );
    if ( result.bad() )
    {
      DCMIOD_WARN(delem.getTag().getXTag() << ": Setting value " << " #" << count << " to \" " << *it << "\" not possible");
    }
    else if (check)
    {
      result = DcmElement::checkVM(OFstatic_cast(unsigned long, values.size()), vm);
    }
    it++;
  }
  return result;
}


OFCondition DcmIODUtil::getUint16ValuesFromElement(DcmElement &delem,
                                                   OFVector<Uint16>& values)
{
  OFCondition result;
  const size_t count = delem.getVM();
  values.reserve(count);
  for (size_t i = 0;  i < count; i++)
  {
    Uint16 val;
    result = delem.getUint16(val, OFstatic_cast(unsigned long, i));
    if (result.bad())
    {
      DCMIOD_WARN(delem.getTag().getXTag() << ": Getting value " << " #" << i << " not possible");
      break;
    }
    values.push_back(val);
  }
  return result;
}


void DcmIODUtil::checkSubSequence(OFCondition& result,
                                  DcmItem& surroundingItem,
                                  const DcmTagKey& seqKey,
                                  const OFString& cardinality,
                                  const OFString& type,
                                  const OFString& module)
{
  OFCondition exists = EC_Normal;
  /* check result */
  if ( result.good() )
  {
    DcmSequenceOfItems *seq = NULL;
    exists = surroundingItem.findAndGetSequence(seqKey, seq);
    result = DcmIODUtil::checkElementValue(seq, seqKey, cardinality, type, exists, module.c_str());
  }
}


OFCondition DcmIODUtil::getAndCheckSingleItem(DcmSequenceOfItems& seq,
                                              DcmItem*& item,
                                              const DcmTagKey& checkKey)
{
  item = NULL;
  const OFString tagName = OFconst_cast(DcmTag*, &seq.getTag())->getTagName(); // getTagName is not const...
  if (checkKey != DCM_UndefinedTagKey)
  {
    if (seq.getTag().getXTag() != checkKey)
    {
      DCMIOD_ERROR("Expected sequence " << checkKey << " but got " << &seq.getTag() << "(" << tagName << ")");
      return EC_ItemNotFound;
    }
  }

  // get actual tag name and cardinality
  const Uint32 card = seq.card();
  if (card != 1)
  {
    if (card > 1)
    {
      DCMIOD_WARN("Only single item allowed in " << tagName << ", ignoring " << card -1 << " other items");
    }
    else // card = 0
    {
      DCMIOD_WARN("Cannot read from sequence " << tagName << ": No item found");
      return EC_ItemNotFound;
    }
  }
  // get item and return it
  item = seq.getItem(0);
  if (item == NULL)
  {
    DCMIOD_ERROR("Cannot read item from " << tagName << ": No item found (internal error)");
    return EC_CorruptedData;
  }
  return EC_Normal;
}


OFCondition DcmIODUtil::checkSOPClass(DcmItem* item,
                                      const OFString& desiredSOPClass,
                                      OFString& valueFound)
{
  valueFound.clear();
  if ( (item == NULL) || (item->card() == 0) )
  {
    DCMIOD_TRACE("Cannot check SOP Class UID: Dataset not present or empty");
    return EC_IllegalParameter;
  }
  OFCondition result = item->findAndGetOFString(DCM_SOPClassUID, valueFound);
  if ( result.bad() )
  {
    DCMIOD_TRACE("No SOP Class UID in file, giving up");
    return EC_TagNotFound;
  }
  if (valueFound != desiredSOPClass)
  {
    DCMIOD_TRACE("File is not of SOP Class " << desiredSOPClass << ", but instead SOP Class is " << dcmFindNameOfUID(valueFound.c_str(), valueFound.c_str()) );
    return EC_InvalidValue;
  }
  return EC_Normal;
}


OFBool DcmIODUtil::isSequenceTag(const DcmTagKey& key,
                                 const OFString& privateCreator)
{
  const DcmDataDictionary& globalDataDict = dcmDataDict.rdlock();
  const DcmDictEntry *dictRef = NULL;
  if (privateCreator.empty())
    dictRef = globalDataDict.findEntry(key, NULL);
  else
    dictRef = globalDataDict.findEntry(key, privateCreator.c_str());
  DcmVR vr;
  if (dictRef)
  {
    vr = dictRef->getVR();
  }
  dcmDataDict.unlock();
  if (vr.getEVR() == EVR_SQ)
    return OFTrue;
  return OFFalse;
}


const DcmTagKey DcmIODUtil::parseTagKey(const OFString& keyString)
{
  unsigned int g, e;
  DcmTagKey resultKey = DCM_UndefinedTagKey;
  if (sscanf(keyString.c_str(), "(%x,%x)", &g, &e) == 2)
  {
    resultKey.set(OFstatic_cast(Uint16, g), OFstatic_cast(Uint16, e));
  }
  return resultKey;
}


OFCondition DcmIODUtil::decompress(DcmDataset& dset)
{
  DcmXfer xfer = dset.getOriginalXfer();
  if (xfer.isEncapsulated())
  {
    if (EC_Normal != dset.chooseRepresentation(EXS_LittleEndianExplicit, NULL))
    {
      DCMIOD_ERROR("No conversion to uncompressed transfer syntax possible!");
      return IOD_EC_CannotDecompress;
    }
  }

  return EC_Normal;
}


OFString DcmIODUtil::createUID(const Uint8 level)
{
  char uid[100];
  switch (level)
  {
    case 0: dcmGenerateUniqueIdentifier(uid, SITE_INSTANCE_UID_ROOT); break;
    case 1: dcmGenerateUniqueIdentifier(uid, SITE_SERIES_UID_ROOT); break;
    case 2: dcmGenerateUniqueIdentifier(uid, SITE_STUDY_UID_ROOT); break;
    default: dcmGenerateUniqueIdentifier(uid, SITE_STUDY_UID_ROOT);
  }
  return uid;
}


Uint16 DcmIODUtil::limitMaxFrames(const size_t numFramesPresent,
                                  const OFString& warning)
{
  if (numFramesPresent > 65535)
  {
    DCMIOD_WARN(warning);
    return 65535;
  }
  return OFstatic_cast(Uint16, numFramesPresent);
}
