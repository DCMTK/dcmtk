/*
 *
 *  Copyright (C) 2025, OFFIS e.V.
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
 *  Purpose: test program for DcmCharString and derived classes
 *
 */


#include <dcmtk/dcmdata/dcvrpn.h>
#include <dcmtk/dcmdata/dcvrsh.h>
#include <dcmtk/dcmdata/dcvrst.h>
#include <dcmtk/dcmdata/dcvruc.h>
#include <dcmtk/dcmdata/dcvrut.h>

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/dcmdata/dcdatset.h"
#include "dcmtk/dcmdata/dcchrstr.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcvrlo.h"
#include "dcmtk/dcmdata/dcvrlt.h"


OFTEST(dcmdata_charString_derived_getVM)
{
    // backslashes are not delimiters in LT, ST and UT,
    // but are in SH, LO, UC and PN
    DcmDataset dataset;
    DcmLongString* longString = new DcmLongString(DCM_StudyDescription, 0);
    dataset.insert(longString);
    OFCHECK_EQUAL(longString->getVM(), 0);
    longString->putString("One\\Two\\Three");
    OFCHECK_EQUAL(longString->getVM(), 3);

    DcmShortString* shortString = new DcmShortString(DCM_AccessionNumber, 0);
    dataset.insert(shortString);
    OFCHECK_EQUAL(shortString->getVM(), 0);
    shortString->putString("One\\Two\\Three");
    OFCHECK_EQUAL(shortString->getVM(), 3);

    DcmUnlimitedCharacters* unlimitedChars = new DcmUnlimitedCharacters(DCM_GeneticModificationsDescription, 0);
    dataset.insert(unlimitedChars);
    OFCHECK_EQUAL(unlimitedChars->getVM(), 0);
    unlimitedChars->putString("One\\Two\\Three");
    OFCHECK_EQUAL(unlimitedChars->getVM(), 3);

    DcmPersonName* personName = new DcmPersonName(DCM_PatientName, 0);
    dataset.insert(personName);
    OFCHECK_EQUAL(personName->getVM(), 0);
    personName->putString("One\\Two\\Three");
    OFCHECK_EQUAL(personName->getVM(), 3);

    DcmLongText* longText = new DcmLongText(DCM_InventoryPurpose, 0);
    dataset.insert(longText);
    OFCHECK_EQUAL(0, longText->getVM());
    longText->putString("One\\Two\\Three");
    OFCHECK_EQUAL(1, longText->getVM());

    DcmShortText* shortText = new DcmShortText(DCM_InventoryPurpose, 0);
    dataset.insert(shortText);
    OFCHECK_EQUAL(shortText->getVM(), 0);
    shortText->putString("One\\Two\\Three");
    OFCHECK_EQUAL(shortText->getVM(), 1);

    DcmUnlimitedText* unlimitedText = new DcmUnlimitedText(DCM_StrainAdditionalInformation, 0);
    dataset.insert(unlimitedText);
    OFCHECK_EQUAL(unlimitedText->getVM(), 0);
    unlimitedText->putString("One\\Two\\Three");
    OFCHECK_EQUAL(unlimitedText->getVM(), 1);
}

OFTEST(dcmdata_charString_getVM_multibyte) {
    DcmDataset dataset;
    DcmLongString* studyDescr = new DcmLongString(DCM_StudyDescription, 0);
    dataset.insert(studyDescr);

    // single-byte/single-value encoding (Latin1)
    dataset.putAndInsertString(DCM_SpecificCharacterSet, "ISO_IR 100");
    // \x5c is the backslash character
    studyDescr->putString("Smith\\\x83\x5c");
    OFCHECK_EQUAL(studyDescr->getVM(), 3);

    // multi-byte/single-value encoding
    dataset.putAndInsertString(DCM_SpecificCharacterSet, "GB18030");
    // \x5c is now part of a 2-byte kanji character, not a backslash
    OFCHECK_EQUAL(studyDescr->getVM(), 2);

    // single-byte/multi-value encoding (Latin1)
    dataset.putAndInsertString(DCM_SpecificCharacterSet, "ISO_IR 100\\ISO_IR 126");
    studyDescr->putString("Dionysios=\x1b\x2d\x46\xc4\xe9\xef\xed\xf5\xf3\xe9\xef\xf2");
    OFCHECK_EQUAL(studyDescr->getVM(), 1);
    // backslash inside a single-byte code extension
    studyDescr->putString("Dionysios=\x1b\x2d\x46\xc4\xe9\xef\\\xed\xf5\xf3\xe9\xef\xf2");
    OFCHECK_EQUAL(studyDescr->getVM(), 2);

    // code extension with multi-byte encoding
    dataset.putAndInsertString(DCM_SpecificCharacterSet, "ISO 2022 IR 13\\ISO 2022 IR 87");
    studyDescr->putString("One\\Two\\Three");
    OFCHECK_EQUAL(3, studyDescr->getVM());
    // delimiter-like byte in a multi-byte string
    studyDescr->putString("Smith=\x1b$BK\\x1b(J");
    OFCHECK_EQUAL(studyDescr->getVM(), 1);
}

OFTEST(dcmdata_charString_getOFString) {
    DcmDataset dataset;
    DcmLongString* studyDescr = new DcmLongString(DCM_StudyDescription, 0);
    dataset.insert(studyDescr);

    // single-byte/single-value encoding (Latin1)
    dataset.putAndInsertString(DCM_SpecificCharacterSet, "ISO_IR 100");
    // \x5c is the backslash character
    OFString stringValue;
    studyDescr->putString("John\\\x83\x5cSmith");
    OFCHECK(studyDescr->getOFString(stringValue, 0).good());
    OFCHECK_EQUAL(stringValue, "John");
    OFCHECK(studyDescr->getOFString(stringValue, 1).good());
    OFCHECK_EQUAL(stringValue, "\x83");
    OFCHECK(studyDescr->getOFString(stringValue, 2).good());
    OFCHECK_EQUAL(stringValue, "Smith");


    // multi-byte/single-value encoding
    dataset.putAndInsertString(DCM_SpecificCharacterSet, "GB18030");
    // \x5c is now part of a 2-byte kanji character, not a backslash
    OFCHECK(studyDescr->getOFString(stringValue, 0).good());
    OFCHECK_EQUAL(stringValue, "John");
    OFCHECK(studyDescr->getOFString(stringValue, 1).good());
    OFCHECK_EQUAL(stringValue, "\x83\x5cSmith");

    // code extension with multi-byte encoding
    dataset.putAndInsertString(DCM_SpecificCharacterSet, "ISO 2022 IR 13\\ISO 2022 IR 87");
    // delimiter-like byte in a multi-byte string
    studyDescr->putString("Smith=\x1b$BK\\x1b(J");
    OFCHECK(studyDescr->getOFString(stringValue, 0).good());
    OFCHECK_EQUAL(stringValue, "Smith=\x1b$BK\\x1b(J");
}

// also missing tests and probably specific implementation
// for checkStringValue, verify, getOFStringArray, putOFStringAtPos, writeJson
