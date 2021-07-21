/*
 *
 *  Copyright (C) 2001-2021, OFFIS e.V.
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
 *  Author:  Michael Onken
 *
 *  Purpose: Base class for converter from image file to DICOM
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/libi2d/i2doutpl.h"
#include "dcmtk/dcmdata/dcdatset.h"
#include "dcmtk/dcmdata/dcelem.h"
#include "dcmtk/dcmdata/dcdeftag.h"

I2DOutputPlug::I2DOutputPlug()
: m_doAttribChecking(OFTrue)
, m_inventMissingType2Attribs(OFTrue)
, m_inventMissingType1Attribs(OFTrue)
{
}


I2DOutputPlug::~I2DOutputPlug()
{
}


void I2DOutputPlug::setValidityChecking(
  OFBool doChecks,
  OFBool insertMissingType2,
  OFBool inventMissingType1)
{
  m_doAttribChecking = doChecks;
  m_inventMissingType2Attribs = insertMissingType2;
  m_inventMissingType1Attribs = inventMissingType1;
}

OFString I2DOutputPlug::checkType1Attrib(
  const DcmTagKey& key,
  DcmDataset* targetDset) const
{
  OFString err;
  if (targetDset->tagExists(key))
  {
    DcmElement *elem = NULL;
    if (targetDset->findAndGetElement(key, elem).bad() || !elem || (elem->getLength() == 0))
    {
      err = "I2DOutputPlug: Empty value for type 1 attribute: ";
      err += DcmTag(key).getTagName();
      err += "\n";
    }
  }
  else
  {
    err = "I2DOutputPlug: Missing type 1 attribute: ";
    err += DcmTag(key).getTagName();
    err += "\n";
  }
  return err;
}

OFString I2DOutputPlug::checkAndInventType1Attrib(
  const DcmTagKey& key,
  DcmDataset* targetDset,
  const OFString& defaultValue) const
{
  OFBool exists = targetDset->tagExists(key);
  if (!exists && !m_inventMissingType1Attribs)
  {
    OFString err = "I2DOutputPlug: Missing type 1 attribute: "; err += DcmTag(key).getTagName(); err += "\n";
    return err;
  }
  DcmElement *elem;
  OFCondition cond = targetDset->findAndGetElement(key, elem);
  if (cond.bad() || !elem || (elem->getLength() == 0))
  {
    if (!m_inventMissingType1Attribs)
    {
      OFString err;
      err += "I2DOutputPlug: Empty value for type 1 attribute: ";
      err += DcmTag(key).getTagName();
      err += "\n";
      return err;
    }
    //holds element to insert in item
    elem = NULL;
    DcmTag tag(key); OFBool wasError = OFFalse;
    //if dicom element could be created, insert in to item and modify to value
    if ( DcmItem::newDicomElement(elem, tag).good())
    {
        if (targetDset->insert(elem, OFTrue).good())
        {
          if (elem->putString(defaultValue.c_str()).good())
          {
            DCMDATA_LIBI2D_DEBUG("I2DOutputPlug: Inserting missing type 1 attribute: " << tag.getTagName() << " with value '" << defaultValue << "'");
          } else wasError = OFTrue;
        } else wasError = OFTrue;
    } else wasError = OFTrue;
    if (wasError)
    {
      OFString err = "Unable to insert type 1 attribute ";
      err += tag.getTagName(); err += " with value '"; err += defaultValue; err += "'\n";
      return err;
    }
  }
  return "";
};


OFString I2DOutputPlug::checkAndInventType1CodeSQ(
    const DcmTagKey& key,
    DcmDataset* targetDset,
    const OFString& codeValue,
    const OFString& codeMeaning,
    const OFString& codingSchemeDesignator) const
{
  OFString err;
  if (! targetDset->tagExists(key) && ! m_inventMissingType1Attribs)
  {
    err = "I2DOutputPlug: Missing type 1 code sequence attribute: "; err += DcmTag(key).getTagName(); err += "\n";
    return err;
  }

  if (m_inventMissingType1Attribs)
  {
    DcmTag tag(key);
    DcmItem *ditem = NULL;
    OFCondition cond = targetDset->findOrCreateSequenceItem(key, ditem, 0);
    if (cond.good() || ditem )
    {
      // add these attributes to the given sequence item only if they are not present already
      if (cond.good() && (! ditem->tagExists(DCM_CodeValue)))
      {
        DCMDATA_LIBI2D_DEBUG("I2DOutputPlug: Inserting missing type 1 attribute: " << DcmTag(DCM_CodeValue).getTagName() << " with value '" << codeValue << "' into " << tag.getTagName());
        cond = ditem->putAndInsertOFStringArray(DCM_CodeValue, codeValue, OFFalse);
      }

      if (cond.good() && (! ditem->tagExists(DCM_CodeMeaning)))
      {
        DCMDATA_LIBI2D_DEBUG("I2DOutputPlug: Inserting missing type 1 attribute: " << DcmTag(DCM_CodeMeaning).getTagName() << " with value '" << codeMeaning << "' into " << tag.getTagName());
        cond = ditem->putAndInsertOFStringArray(DCM_CodeMeaning, codeMeaning, OFFalse);
      }

      if (cond.good() && (! ditem->tagExists(DCM_CodingSchemeDesignator)))
      {
        DCMDATA_LIBI2D_DEBUG("I2DOutputPlug: Inserting missing type 1 attribute: " << DcmTag(DCM_CodingSchemeDesignator).getTagName() << " with value '" << codingSchemeDesignator << "' into " << tag.getTagName());
        cond = ditem->putAndInsertOFStringArray(DCM_CodingSchemeDesignator, codingSchemeDesignator, OFFalse);
      }

      if (cond.bad())
      {
        err = "Unable to insert type 1 code sequence attribute ";
        err += tag.getTagName(); err += " with value ("; err += codeValue; err += ", "; err += codeMeaning; err += ", "; err += codingSchemeDesignator; err += ")\n";
      }
    }
  }

  return err;
}


OFString I2DOutputPlug::checkAndInventType2Attrib(
  const DcmTagKey& key,
  DcmDataset* targetDset,
  const OFString& defaultValue) const
{
  OFString err;
  OFBool exists = targetDset->tagExists(key);
  if (!exists)
  {
    if (m_inventMissingType2Attribs)
    {
      //holds element to insert in item
      DcmElement *elem = NULL;
      DcmTag tag(key); OFBool wasError = OFFalse;
      //if dicom element could be created, insert in to item and modify to value
      if ( DcmItem::newDicomElement(elem, tag).good())
      {
        if (targetDset->insert(elem, OFTrue).good())
        {
          OFCondition result;
          if (!defaultValue.empty()) // only insert value if not empty(e. g. empty type 2 sequences)
          {
            result = elem->putString(defaultValue.c_str());
          }
          if (result.good())
          {
            DCMDATA_LIBI2D_DEBUG("I2DOutputPlug: Inserting missing type 2 attribute: " << tag.getTagName() << " with value '" << (defaultValue.empty() ? "<empty>" : defaultValue) << "'");
          } else wasError = OFTrue;
        } else wasError = OFTrue;
      } else wasError = OFTrue;
      if (wasError)
      {
        err += "Unable to insert type 2 attribute "; err += tag.getTagName(); err += " with value '"; err += defaultValue; err += "'\n";
      }
    }
    else
    {
      err = "Image2Dcm: Missing type 2 attribute: "; err += DcmTag(key).getTagName(); err += "\n";
      return err;
    }
  }
  return err;
};

