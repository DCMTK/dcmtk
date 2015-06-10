/*
 *
 *  Copyright (C) 2015, OFFIS e.V.
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
 *  Module: dcmsr
 *
 *  Author: Joerg Riesmeier
 *
 *  Purpose:
 *    test program for classes DSRBasicCodedEntry and DSRCodedEntryValue
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/dcmsr/dsrcodvl.h"


OFTEST(dcmsr_setCodeValueType)
{
    /* first, try the standard case (short code value) */
    DSRCodedEntryValue codedEntry("121206", "DCM", "Distance", DSRTypes::CVT_Short);
    OFCHECK(codedEntry.isValid());
    OFCHECK_EQUAL(codedEntry.getCodeValueType(), DSRTypes::CVT_Short);
    /* then, set a long code value */
    OFCHECK(codedEntry.setCode("621566751000087104", "SCT", "Invasive diagnostic procedure", DSRTypes::CVT_Long).good());
    OFCHECK_EQUAL(codedEntry.getCodeValueType(), DSRTypes::CVT_Long);
    /* and finally, set a URN code value */
    OFCHECK(codedEntry.setCode("urn:lex:us:federal:codified.regulation:2013-04-25;45CFR164", "99TEST", "HIPAA Privacy Rule", DSRTypes::CVT_URN).good());
    OFCHECK_EQUAL(codedEntry.getCodeValueType(), DSRTypes::CVT_URN);
}


OFTEST(dcmsr_determineCodeValueType)
{
    /* first, try the standard case (short code value) */
    DSRCodedEntryValue codedEntry("121206", "DCM", "Distance");
    OFCHECK(codedEntry.isValid());
    OFCHECK_EQUAL(codedEntry.getCodeValueType(), DSRTypes::CVT_Short);
    /* then, set a long code value */
    OFCHECK(codedEntry.setCode("621566751000087104", "SCT", "Invasive diagnostic procedure").good());
    OFCHECK_EQUAL(codedEntry.getCodeValueType(), DSRTypes::CVT_Long);
    /* and finally, set a URN code value */
    OFCHECK(codedEntry.setCode("urn:lex:us:federal:codified.regulation:2013-04-25;45CFR164", "99TEST", "HIPAA Privacy Rule").good());
    OFCHECK_EQUAL(codedEntry.getCodeValueType(), DSRTypes::CVT_URN);
    /* also check an empty code */
    codedEntry.clear();
    OFCHECK(!codedEntry.isValid());
    OFCHECK_EQUAL(codedEntry.getCodeValueType(), DSRTypes::CVT_auto);
    /* and a very short code value */
    OFCHECK(codedEntry.setCode("0", "99TEST", "-").good());
    OFCHECK_EQUAL(codedEntry.getCodeValueType(), DSRTypes::CVT_Short);
}


OFTEST(dcmsr_writeCodeSequence)
{
    DcmDataset dataset;
    /* first, try the standard case (short code value) */
    DSRCodedEntryValue codedEntry("121206", "DCM", "Distance", DSRTypes::CVT_Short);
    OFCHECK(codedEntry.writeSequence(dataset, DCM_ConceptNameCodeSequence).good());
    OFCHECK(dataset.tagExistsWithValue(DCM_CodeValue, OFTrue /*searchIntoSub*/));
    OFCHECK(!dataset.tagExists(DCM_LongCodeValue, OFTrue /*searchIntoSub*/));
    OFCHECK(!dataset.tagExists(DCM_URNCodeValue, OFTrue /*searchIntoSub*/));
    /* then, set a long code value */
    dataset.clear();
    OFCHECK(codedEntry.setCode("621566751000087104", "SCT", "Invasive diagnostic procedure", DSRTypes::CVT_Long).good());
    OFCHECK(codedEntry.writeSequence(dataset, DCM_ConceptNameCodeSequence).good());
    OFCHECK(!dataset.tagExists(DCM_CodeValue, OFTrue /*searchIntoSub*/));
    OFCHECK(dataset.tagExistsWithValue(DCM_LongCodeValue, OFTrue /*searchIntoSub*/));
    OFCHECK(!dataset.tagExists(DCM_URNCodeValue, OFTrue /*searchIntoSub*/));
    /* and finally, set a URN code value */
    dataset.clear();
    OFCHECK(codedEntry.setCode("urn:lex:us:federal:codified.regulation:2013-04-25;45CFR164", "99TEST", "HIPAA Privacy Rule", DSRTypes::CVT_URN).good());
    OFCHECK(codedEntry.writeSequence(dataset, DCM_ConceptNameCodeSequence).good());
    OFCHECK(!dataset.tagExists(DCM_CodeValue, OFTrue /*searchIntoSub*/));
    OFCHECK(!dataset.tagExists(DCM_LongCodeValue, OFTrue /*searchIntoSub*/));
    OFCHECK(dataset.tagExistsWithValue(DCM_URNCodeValue, OFTrue /*searchIntoSub*/));
}


OFTEST(dcmsr_useBasicCodedEntry)
{
    /* first, define some code constants */
    const DSRBasicCodedEntry code1("121206", "DCM", "Distance");
    const DSRBasicCodedEntry code2("621566751000087104", "SCT", "Invasive diagnostic procedure", DSRTypes::CVT_auto);
    const DSRBasicCodedEntry code3("urn:lex:us:federal:codified.regulation:2013-04-25;45CFR164", "99TEST", "HIPAA Privacy Rule", DSRTypes::CVT_auto);
    /* then, use them as a coded entry value */
    DSRCodedEntryValue codedEntry(code1);
    OFCHECK(codedEntry.isValid());
    OFCHECK(!codedEntry.usesEnhancedEncodingMode());
    OFCHECK_EQUAL(codedEntry.getCodeValueType(), DSRTypes::CVT_Short);
    OFCHECK_EQUAL(codedEntry.getCodeValue(), "121206");
    OFCHECK_EQUAL(codedEntry.getCodingSchemeDesignator(), "DCM");
    OFCHECK_EQUAL(codedEntry.getCodingSchemeVersion(), "" /*empty*/);
    OFCHECK_EQUAL(codedEntry.getCodeMeaning(), "Distance");
    /* use setCode() method */
    OFCHECK(codedEntry.setCode(code2, OFTrue /*check*/).good());
    OFCHECK(codedEntry.isValid());
    OFCHECK(!codedEntry.usesEnhancedEncodingMode());
    OFCHECK_EQUAL(codedEntry.getCodeValueType(), DSRTypes::CVT_Long);
    OFCHECK_EQUAL(codedEntry.getCodeValue(), "621566751000087104");
    OFCHECK_EQUAL(codedEntry.getCodingSchemeDesignator(), "SCT");
    OFCHECK_EQUAL(codedEntry.getCodingSchemeVersion(), "" /*empty*/);
    OFCHECK_EQUAL(codedEntry.getCodeMeaning(), "Invasive diagnostic procedure");
    /* use assignment operator (implicitly convert DSRBasicCodedEntry to DSRCodedEntryValue) */
    codedEntry = code3;
    OFCHECK(codedEntry.isValid());
    OFCHECK(!codedEntry.usesEnhancedEncodingMode());
    OFCHECK_EQUAL(codedEntry.getCodeValueType(), DSRTypes::CVT_URN);
    OFCHECK_EQUAL(codedEntry.getCodeValue(), "urn:lex:us:federal:codified.regulation:2013-04-25;45CFR164");
    OFCHECK_EQUAL(codedEntry.getCodingSchemeDesignator(), "99TEST");
    OFCHECK_EQUAL(codedEntry.getCodingSchemeVersion(), "" /*empty*/);
    OFCHECK_EQUAL(codedEntry.getCodeMeaning(), "HIPAA Privacy Rule");
}
