/*
 *
 *  Copyright (C) 2017, Open Connections GmbH
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
 *  Module:  dcmdata
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Tests for DcmItem's newDicomElement() helper methods
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/dcmdata/dcitem.h"
#include "dcmtk/dcmdata/dcelem.h"
#include "dcmtk/dcmdata/dcitem.h"
#include "dcmtk/dcmdata/dcdict.h"
#include "dcmtk/dcmdata/dcdicent.h"

// Definitions of private tag info we use in this test

#define PRIVATE_CREATOR_NAME "John Doe Testing Corporation"
#define PRIVATE_ELEMENT_STRINGELEM 0x0029, 0x1000
#define PRIVATE_ELEMENT_NOTINDICT 0x0029, 0x1111
#define PRIVATE_STRING_VALUE "Test Value"

/** Register private tag(s) in runtime dictionary
 */
void registerPrivateTag()

{
  DcmDataDictionary &dict = dcmDataDict.wrlock();
  dict.addEntry(new DcmDictEntry(PRIVATE_ELEMENT_STRINGELEM, EVR_LO, "StringAttributeForTesting", 1, 1, "private", OFTrue, PRIVATE_CREATOR_NAME));
  dcmDataDict.unlock();

}


OFTEST(dcmdata_newDicomElementPrivate)
{
  // Pre-register the private tags we want to use for this test in the dictionary
  registerPrivateTag();

  // Test cases when constructing a known, private element:

  // API 1: Construct DICOM element and get it as a return value.
  DcmElement *elem = DcmItem::newDicomElement(DcmTagKey(PRIVATE_ELEMENT_STRINGELEM), PRIVATE_CREATOR_NAME);
  // Check whether creation as well as tag and value assignment are correct
  OFCHECK(elem);
  OFCHECK(elem->putOFStringArray(PRIVATE_STRING_VALUE).good());
  OFCHECK(elem->getTag().getXTag() == DcmTagKey(PRIVATE_ELEMENT_STRINGELEM));
  OFCHECK(elem->getVR() == EVR_LO);
  OFCHECK(elem->putOFStringArray(PRIVATE_STRING_VALUE).good());
  char *charVal = NULL;
  OFCHECK(elem->getString(charVal).good());
  OFCHECK(OFString(charVal) == PRIVATE_STRING_VALUE);
  charVal = NULL;

  // API 2: Construct DICOM element and get it as parameter output value
  DcmElement* elem2 = NULL;
  // Check whether creation as well as tag and value assignment are correct
  OFCHECK(DcmItem::newDicomElement(elem2, DcmTagKey(PRIVATE_ELEMENT_STRINGELEM), PRIVATE_CREATOR_NAME).good());
  OFCHECK(elem2->getTag().getXTag() == DcmTagKey(PRIVATE_ELEMENT_STRINGELEM));
  OFCHECK(elem->getVR() == EVR_LO);
  OFCHECK(elem2->putOFStringArray(PRIVATE_STRING_VALUE).good());
  OFCHECK(elem2->getString(charVal).good());
  OFCHECK(OFString(charVal) == PRIVATE_STRING_VALUE);

  delete elem;
  delete elem2;

  // Test cases when constructing an unknown, private element resulting in creation
  // with VR "EVR_UNKNOWN":

  // API 1
  DcmElement *elem_un = NULL;
  elem_un = DcmItem::newDicomElement(DcmTagKey(PRIVATE_ELEMENT_NOTINDICT), PRIVATE_CREATOR_NAME);
  OFCHECK(elem_un);
  OFCHECK(elem_un->getTag().getXTag() == DcmTagKey(PRIVATE_ELEMENT_NOTINDICT));
  OFCHECK(elem_un->getVR() == EVR_UNKNOWN);
  delete elem_un;

  // API 2
  elem_un = NULL;
  OFCHECK(DcmItem::newDicomElement(elem_un, DcmTagKey(PRIVATE_ELEMENT_NOTINDICT), PRIVATE_CREATOR_NAME).good());
  OFCHECK(elem_un);
  OFCHECK(elem_un->getVR() == EVR_UNKNOWN);
  delete elem_un;
}
