/*
 *
 *  Copyright (C) 2019, OFFIS e.V.
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
 *    classes: SiSignaturePurpose
 *
 */
#include "dcmtk/config/osconfig.h"

#ifdef WITH_OPENSSL

#include "dcmtk/dcmsign/sipurpos.h"
#include "dcmtk/dcmsign/sitypes.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcitem.h"
#include "dcmtk/ofstd/ofstream.h"

// code value constants
static const char *purposeCodeValue[] =
{
    NULL,
    "1",   "2",   "3",   "4",   "5",  "6",
    "7",   "8",   "9",  "10",  "11", "12",
    "13",  "14", "15",  "16", "17",  "18"
};

static const size_t purposeCodeValueEntries = sizeof(purposeCodeValue) / sizeof(const char *);

// code meaning constants
static const char *purposeCodeMeaning[] =
{
    NULL,
    "Author's Signature",
    "Coauthor's Signature",
    "Co-participant's Signature",
    "Transcriptionist/Recorder Signature",
    "Verification Signature",
    "Validation Signature",
    "Consent Signature",
    "Signature Witness Signature",
    "Event Witness Signature",
    "Identity Witness Signature",
    "Consent Witness Signature",
    "Interpreter Signature",
    "Review Signature",
    "Source Signature",
    "Addendum Signature",
    "Modification Signature",
    "Administrative (Error/Edit) Signature",
    "Timestamp Signature"
};

static const size_t purposeCodeMeaningEntries = sizeof(purposeCodeMeaning) / sizeof(const char *);

const char *SiSignaturePurpose::getCodeValue(E_SignaturePurposeType purpose)
{
    size_t idx = OFstatic_cast(size_t, purpose); // cast the enum to size_t
    if ((idx > 0) && (idx < purposeCodeValueEntries)) return purposeCodeValue[idx];
    return NULL;
}

const char *SiSignaturePurpose::getCodeMeaning(E_SignaturePurposeType purpose)
{
    size_t idx = OFstatic_cast(size_t, purpose); // cast the enum to size_t
    if ((idx > 0) && (idx < purposeCodeMeaningEntries)) return purposeCodeMeaning[idx];
    return NULL;
}

const char *SiSignaturePurpose::getCodingSchemeDesignator(E_SignaturePurposeType /* purpose */)
{
  return "ASTM-sigpurpose";
}

OFCondition SiSignaturePurpose::insertDigitalSignaturePurposeCodeSQ(DcmItem& seqItem, E_SignaturePurposeType sigPurpose)
{
    if (sigPurpose == ESP_none) return EC_Normal; // no signature purpose defined

    OFCondition result = EC_Normal;
    const char *codeValue = getCodeValue(sigPurpose);
    const char *codeMeaning = getCodeMeaning(sigPurpose);
    const char *codingSchemeDesignator = getCodingSchemeDesignator(sigPurpose);

    if (codeValue && codeMeaning && codingSchemeDesignator)
    {
        DcmItem *ditem = NULL;
        result = seqItem.findOrCreateSequenceItem(DCM_DigitalSignaturePurposeCodeSequence, ditem);
        if (ditem)
        {
          if (result.good()) result = ditem->putAndInsertString(DCM_CodingSchemeDesignator, codingSchemeDesignator);
          if (result.good()) result = ditem->putAndInsertString(DCM_CodeValue, codeValue);
          if (result.good()) result = ditem->putAndInsertString(DCM_CodeMeaning, codeMeaning);
        }
    }
    else result = EC_IllegalCall; // we were called with an undefined signature purpose
    return result;
}

SiSignaturePurpose::E_SignaturePurposeType SiSignaturePurpose::determineOverridePurpose(E_SignaturePurposeType currentPurpose, E_SignaturePurposeType overridePurpose)
{
    if (overridePurpose != ESP_none)
    {
        if ((currentPurpose != ESP_none) && (currentPurpose != overridePurpose))
        {
            // we have a conflict between user selection and profile requirements
            DCMSIGN_WARN("Signature profile requires purpose code " << OFstatic_cast(int, overridePurpose) << ", ignoring user selection " << OFstatic_cast(int, currentPurpose) << ".");
        }
        // the signature profile overrides the user's selection
        currentPurpose = overridePurpose;
    }
    return currentPurpose;
}

SiSignaturePurpose::E_SignaturePurposeType SiSignaturePurpose::lookup(size_t num)
{
    if (num < purposeCodeValueEntries) return OFstatic_cast(E_SignaturePurposeType, num); else return ESP_none;
}

void SiSignaturePurpose::printSignatureCodes(STD_NAMESPACE ostream& out)
{
    out << "\nDigital Signature Purpose Codes:\n"
           "   1: Author's Signature\n"
           "   2: Coauthor's Signature\n"
           "   3: Co-participant's Signature\n"
           "   4: Transcriptionist/Recorder Signature\n"
           "   5: Verification Signature\n"
           "   6: Validation Signature\n"
           "   7: Consent Signature\n"
           "   8: Signature Witness Signature\n"
           "   9: Event Witness Signature\n"
           "  10: Identity Witness Signature\n"
           "  11: Consent Witness Signature\n"
           "  12: Interpreter Signature\n"
           "  13: Review Signature\n"
           "  14: Source Signature\n"
           "  15: Addendum Signature\n"
           "  16: Modification Signature\n"
           "  17: Administrative (Error/Edit) Signature\n"
           "  18: Timestamp Signature\n"
        << OFendl;
    return;
}

#else /* WITH_OPENSSL */
int sipurpos_cc_dummy_to_keep_linker_from_moaning = 0;
#endif
