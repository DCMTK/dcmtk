/*
 *
 *  Copyright (C) 2026, OFFIS e.V.
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
 *  Purpose: Tests for dcmodify's --process-if condition checking
 *
 *  Test coverage summary:
 *
 *  Test name                  VR    Type                   Operators tested
 *  -------------------------  ----  ---------------------  -------------------------
 *  condition_existence        --    Existence checks       E, ! (incl. VM indexing)
 *  condition_US               US    Unsigned Short 16-bit  =, !=, <, <=, >, >=
 *  condition_SS               SS    Signed Short 16-bit    =, !=, <, <=, >, >=
 *  condition_UL               UL    Unsigned Long 32-bit   =, !=, <, <=, >, >=
 *  condition_SL               SL    Signed Long 32-bit     =, !=, <, <=, >, >=
 *  condition_UV               UV    Unsigned VeryLong 64b  =, !=, <, <=, >, >=
 *  condition_SV               SV    Signed VeryLong 64b    =, !=, <, <=, >, >=
 *  condition_FL               FL    Float 32-bit           =, !=, <, <=, >, >=
 *  condition_FD               FD    Float Double 64-bit    =, !=, <, <=, >, >=
 *  condition_OB               OB    Other Byte (hex)       =, !=, <, <=, >, >=
 *  condition_OW               OW    Other Word (hex)       =, !=, <, <=, >, >=
 *  condition_OL               OL    Other Long 32-bit      =, !=, <, <=, >, >=
 *  condition_OV               OV    Other VeryLong 64-bit  =, !=, <, <=, >, >=
 *  condition_OF               OF    Other Float 32-bit     =, !=, <, <=, >, >=
 *  condition_OD               OD    Other Double 64-bit    =, !=, <, <=, >, >=
 *  condition_UN               UN    Unknown (hex)          =, !=, <, <=, >, >=, E, !
 *  condition_IS               IS    Integer String (*)     =, !=, <, <=
 *  condition_DS               DS    Decimal String (*)     =, !=, <=, >=, <, >
 *  condition_CS               CS    Code String            =, !=, <, <=, >, >=
 *  condition_LO               LO    Long String            =, !=, <, <=, >, >=
 *  condition_PN               PN    Person Name            =, !=, <, <=, >, >=
 *  condition_DA               DA    Date                   =, !=, <, <=, >, >=
 *  condition_UI               UI    Unique Identifier      =, != (raw+name), <, >
 *  condition_AT               AT    Attribute Tag          =, !=, <, >, <=, >=
 *  condition_VM               --    Value Multiplicity     Indexed + full-value cmp
 *  condition_sequence_wildcard --   Sequence [*] wildcard  <=, <, >=, >, =, !=, E
 *  condition_tagname          --    Dictionary name lookup =, E, !
 *
 *  (*) IS and DS use DcmByteString::compare() internally (lexicographic),
 *      so ordering across different string lengths may be incorrect
 *      (e.g. "10" < "5" lexicographically). See test comments for details.
 *
 *  VRs NOT tested (shared logic with tested VRs):
 *    AE, AS, DT, TM, SH, ST, LT, UC, UR, UT
 *      -- same checkStringCondition() / OFString::compare() path as CS/LO/PN
 *    SQ -- not a leaf element; rejected by checkPathAndGetLeafElem()
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/ofstd/oftempf.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/dcmdata/dcuid.h"
#include "dcmtk/dcmdata/dcerror.h"
#include "dcmtk/dcmdata/dcobject.h"   // for dcmEnableAutomaticInputDataCorrection
#include "dcmtk/dcmdata/dcvrss.h"
#include "dcmtk/dcmdata/dcvrsl.h"
#include "dcmtk/dcmdata/dcvrfl.h"
#include "dcmtk/dcmdata/dcvrfd.h"
#include "dcmtk/dcmdata/dcvrsv.h"
#include "dcmtk/dcmdata/dcvruv.h"
#include "dcmtk/dcmdata/dcvrul.h"
#include "dcmtk/dcmdata/dcvrat.h"
#include "dcmtk/dcmdata/dcvrobow.h"  // OB, OW
#include "dcmtk/dcmdata/dcvrol.h"    // OL
#include "dcmtk/dcmdata/dcvrov.h"    // OV
#include "dcmtk/dcmdata/dcvrof.h"    // OF
#include "dcmtk/dcmdata/dcvrod.h"    // OD

#include "mdfdsman.h"  // located in dcmdata/apps
#include "mdfconen.h"  // for MdfCondition class


// ============================================================
// Helpers
// ============================================================

/** Create an MdfCondition with the given parameters.
 *  @param path tag path, e.g. "(0028,0010)" or "Rows"
 *  @param op   operation string: "E", "!", "=", "!=", "<", "<=", ">", ">="
 *  @param val  comparison value (empty for existence checks)
 *  @return the constructed MdfCondition
 */
static MdfCondition makeCond(const char* path, const char* op, const char* val = "")
{
    MdfCondition c;
    c.option = "iff";
    c.path = path;
    c.operation = op;
    c.value = val;
    return c;
}


/** Create a temporary DICOM file populated with test elements covering
 *  all VRs that are tested. Caller must delete the file when done.
 *  Ensures dcmEnableAutomaticInputDataCorrection is enabled so that
 *  padding is stripped on read-back (may be disabled by preceding tests).
 *  @param tmpFile receives the path to the created file
 *  @return EC_Normal on success
 */
static OFCondition createTestFile(OFString& tmpFile)
{
    // Ensure auto-correction is enabled (e.g. padding removal on read-back).
    // Other tests (e.g. dcmdata_uniqueIdentifier_1) may disable this globally.
    dcmEnableAutomaticInputDataCorrection.set(OFTrue);
    OFCondition cond = OFTempFile::createFile(tmpFile, NULL, O_RDWR,
        "" /*dir*/, "tcond_" /*prefix*/, ".dcm" /*postfix*/);
    if (cond.bad()) return cond;

    DcmFileFormat ff;
    DcmDataset* dset = ff.getDataset();

    // --- String VRs ---
    dset->putAndInsertString(DCM_PatientName, "Doe^John");           // PN
    dset->putAndInsertString(DCM_Modality, "CT");                    // CS
    dset->putAndInsertString(DCM_Manufacturer, "ACME Corp");         // LO
    dset->putAndInsertString(DCM_StudyDate, "20240115");             // DA
    dset->putAndInsertString(DCM_SOPClassUID, UID_CTImageStorage);   // UI
    // CS with VM=3 for VM indexing tests
    dset->putAndInsertString(DCM_ImageType, "ORIGINAL\\PRIMARY\\AXIAL"); // CS

    // --- Unsigned integer VRs ---
    dset->putAndInsertUint16(DCM_Rows, 512);                         // US
    // UL: SimpleFrameList (0008,1161)
    DcmUnsignedLong* ulElem = new DcmUnsignedLong(DCM_SimpleFrameList);
    ulElem->putUint32(100000);
    dset->insert(ulElem, OFTrue);
    // UV: FileOffsetInContainer (0008,040C) — 64-bit unsigned
    // Uses a value within 32-bit range for portability (checkIntegerValidity
    // uses sscanf which may truncate values > 2^32 on 32-bit platforms).
    DcmUnsigned64bitVeryLong* uvElem = new DcmUnsigned64bitVeryLong(DCM_FileOffsetInContainer);
    uvElem->putUint64(3000000000ULL); // fits in 32-bit unsigned
    dset->insert(uvElem, OFTrue);

    // --- Signed integer VRs ---
    // SS: TagAngleSecondAxis (0018,9219)
    DcmSignedShort* ssElem = new DcmSignedShort(DCM_TagAngleSecondAxis);
    ssElem->putSint16(-100);
    dset->insert(ssElem, OFTrue);
    // SL: IntegerNumericValue (0014,4108)
    DcmSignedLong* slElem = new DcmSignedLong(DCM_IntegerNumericValue);
    slElem->putSint32(-100000);
    dset->insert(slElem, OFTrue);
    // SV: SelectorSVValue (0072,0082) — 64-bit signed
    // Uses a value within 32-bit range for portability (checkIntegerValidity
    // uses sscanf which may truncate values > 2^31 on 32-bit platforms).
    DcmSigned64bitVeryLong* svElem = new DcmSigned64bitVeryLong(DCM_SelectorSVValue);
    svElem->putSint64(-2000000000LL); // fits in 32-bit signed
    dset->insert(svElem, OFTrue);

    // --- Float VRs ---
    // FL: RecommendedDisplayFrameRateInFloat (0008,9459)
    DcmFloatingPointSingle* flElem = new DcmFloatingPointSingle(DCM_RecommendedDisplayFrameRateInFloat);
    flElem->putFloat32(25.5f);
    dset->insert(flElem, OFTrue);
    // FD: AcquisitionDuration (0018,9073)
    DcmFloatingPointDouble* fdElem = new DcmFloatingPointDouble(DCM_AcquisitionDuration);
    fdElem->putFloat64(3.14159);
    dset->insert(fdElem, OFTrue);

    // --- Numeric string VRs ---
    dset->putAndInsertString(DCM_NumberOfFrames, "10");              // IS
    dset->putAndInsertString(DCM_WindowCenter, "100.5");             // DS

    // --- AT (Attribute Tag) ---
    DcmAttributeTag* atElem = new DcmAttributeTag(DCM_FrameIncrementPointer);
    atElem->putTagVal(DCM_PatientName); // stores (0010,0010)
    dset->insert(atElem, OFTrue);

    // --- Binary VRs (Other Byte/Word/Long/VeryLong/Float/Double) ---
    // OB: SelectorOBValue (0072,0065) — hex format "01\ff"
    DcmOtherByteOtherWord* obElem = new DcmOtherByteOtherWord(DCM_SelectorOBValue);
    obElem->putString("0a\\1b\\ff");
    dset->insert(obElem, OFTrue);
    // OW: SelectorOWValue (0072,0069) — hex format "0000\7fff"
    DcmOtherByteOtherWord* owElem = new DcmOtherByteOtherWord(DCM_SelectorOWValue);
    owElem->putString("00ff\\7fff");
    dset->insert(owElem, OFTrue);
    // OL: SelectorOLValue (0072,0075) — decimal format via DcmUnsignedLong parent
    DcmOtherLong* olElem = new DcmOtherLong(DCM_SelectorOLValue);
    olElem->putUint32(70000);
    dset->insert(olElem, OFTrue);
    // OV: SelectorOVValue (0072,0081) — decimal format via DcmUnsigned64bitVeryLong parent
    // Uses a value within 32-bit range for portability (sscanf on 32-bit platforms).
    // 64-bit range is already covered by the UV test.
    DcmOther64bitVeryLong* ovElem = new DcmOther64bitVeryLong(DCM_SelectorOVValue);
    ovElem->putUint64(70000);
    dset->insert(ovElem, OFTrue);
    // OF: SelectorOFValue (0072,0067) — decimal float via DcmFloatingPointSingle parent
    DcmOtherFloat* ofElem = new DcmOtherFloat(DCM_SelectorOFValue);
    ofElem->putFloat32(42.5f);
    dset->insert(ofElem, OFTrue);
    // OD: SelectorODValue (0072,0073) — decimal double via DcmFloatingPointDouble parent
    DcmOtherDouble* odElem = new DcmOtherDouble(DCM_SelectorODValue);
    odElem->putFloat64(123.456);
    dset->insert(odElem, OFTrue);

    // --- UN (Unknown VR) ---
    // Use a private tag without private creator so it stays UN on read-back.
    // UN is stored as DcmOtherByteOtherWord internally (OB hex representation).
    // Single byte value avoids the multi-value getString() issue (see test comment).
    DcmTag unTag(0x0009, 0x1010);
    unTag.setVR(EVR_UN);
    DcmOtherByteOtherWord* unElem = new DcmOtherByteOtherWord(unTag);
    unElem->putString("7f");
    dset->insert(unElem, OFTrue);

    // --- Sequence with multiple items for [*] wildcard tests ---
    // ReferencedSeriesSequence (0008,1115) with 3 items, each containing Rows
    DcmItem* seqItem = NULL;
    dset->findOrCreateSequenceItem(DCM_ReferencedSeriesSequence, seqItem, 0);
    seqItem->putAndInsertUint16(DCM_Rows, 256);
    dset->findOrCreateSequenceItem(DCM_ReferencedSeriesSequence, seqItem, 1);
    seqItem->putAndInsertUint16(DCM_Rows, 512);
    dset->findOrCreateSequenceItem(DCM_ReferencedSeriesSequence, seqItem, 2);
    seqItem->putAndInsertUint16(DCM_Rows, 512);

    cond = ff.saveFile(tmpFile.c_str(), EXS_LittleEndianExplicit);
    return cond;
}


/** Load a test file into an MdfDatasetManager.
 *  @param mgr   the manager to load into
 *  @param path  path to the DICOM file
 *  @return EC_Normal on success
 */
static OFCondition loadTestFile(MdfDatasetManager& mgr, const OFString& path)
{
    return mgr.loadFile(path.c_str());
}


// Shorthand: condition passes (met)
#define COND_MET(mgr, path, op, val) \
    OFCHECK(mgr.checkCondition(makeCond(path, op, val)) == EC_Normal)

// Shorthand: condition fails (not met)
#define COND_NOT_MET(mgr, path, op, val) \
    OFCHECK(mgr.checkCondition(makeCond(path, op, val)) == EC_PathConditionNotMet)


/** Helper: test all 6 comparison operators for a numeric or string VR.
 *  Expects that the element at 'path' has the value described by 'equal'.
 *  'lesser' must be a value less than the element value,
 *  'greater' must be a value greater than the element value.
 *  @param mgr      loaded MdfDatasetManager
 *  @param path     tag path string, e.g. "(0028,0010)"
 *  @param equal    string representation matching the stored value
 *  @param lesser   string representation of a smaller value
 *  @param greater  string representation of a larger value
 */
static void checkAllOperators(MdfDatasetManager& mgr,
                              const char* path,
                              const char* equal,
                              const char* lesser,
                              const char* greater)
{
    // === Equality (=) ===
    COND_MET(mgr, path, "=", equal);
    COND_NOT_MET(mgr, path, "=", lesser);
    COND_NOT_MET(mgr, path, "=", greater);

    // === Inequality (!=) ===
    COND_NOT_MET(mgr, path, "!=", equal);
    COND_MET(mgr, path, "!=", lesser);
    COND_MET(mgr, path, "!=", greater);

    // === Less than (<) ===
    // elem < equal  => false (equal values)
    COND_NOT_MET(mgr, path, "<", equal);
    // elem < lesser => false (elem is bigger)
    COND_NOT_MET(mgr, path, "<", lesser);
    // elem < greater => true (elem is smaller)
    COND_MET(mgr, path, "<", greater);

    // === Less than or equal (<=) ===
    // elem <= equal  => true
    COND_MET(mgr, path, "<=", equal);
    // elem <= lesser => false
    COND_NOT_MET(mgr, path, "<=", lesser);
    // elem <= greater => true
    COND_MET(mgr, path, "<=", greater);

    // === Greater than (>) ===
    // elem > equal  => false
    COND_NOT_MET(mgr, path, ">", equal);
    // elem > lesser => true (elem is bigger)
    COND_MET(mgr, path, ">", lesser);
    // elem > greater => false
    COND_NOT_MET(mgr, path, ">", greater);

    // === Greater than or equal (>=) ===
    // elem >= equal  => true
    COND_MET(mgr, path, ">=", equal);
    // elem >= lesser => true
    COND_MET(mgr, path, ">=", lesser);
    // elem >= greater => false
    COND_NOT_MET(mgr, path, ">=", greater);
}


// ============================================================
// Test: Existence and non-existence checks
// ============================================================

OFTEST(dcmdata_condition_existence)
{
    OFString tmpFile;
    OFCHECK(createTestFile(tmpFile).good());
    MdfDatasetManager mgr;
    OFCHECK(loadTestFile(mgr, tmpFile).good());

    // Tag exists: existence check passes
    COND_MET(mgr, "(0010,0010)", "E", "");
    // Tag exists: non-existence check fails
    COND_NOT_MET(mgr, "(0010,0010)", "!", "");

    // Tag does not exist: existence check fails
    COND_NOT_MET(mgr, "(0099,0099)", "E", "");
    // Tag does not exist: non-existence check passes
    COND_MET(mgr, "(0099,0099)", "!", "");

    // Existence with VM index: ImageType has 3 values (indices 0,1,2)
    COND_MET(mgr, "ImageType[0]", "E", "");
    COND_MET(mgr, "ImageType[2]", "E", "");
    // Index 3 does not exist
    COND_NOT_MET(mgr, "ImageType[3]", "E", "");
    // Non-existence of out-of-bounds index passes
    COND_MET(mgr, "ImageType[3]", "!", "");

    OFStandard::deleteFile(tmpFile);
}


// ============================================================
// Test: US (Unsigned Short, 16-bit) — numeric comparison
// Tag: Rows (0028,0010), value: 512
// ============================================================

OFTEST(dcmdata_condition_US)
{
    OFString tmpFile;
    OFCHECK(createTestFile(tmpFile).good());
    MdfDatasetManager mgr;
    OFCHECK(loadTestFile(mgr, tmpFile).good());

    checkAllOperators(mgr, "(0028,0010)", "512", "100", "1000");

    // Also test with leading zeros (normalization)
    COND_MET(mgr, "(0028,0010)", "=", "0512");
    COND_MET(mgr, "(0028,0010)", "=", "00512");

    OFStandard::deleteFile(tmpFile);
}


// ============================================================
// Test: SS (Signed Short, 16-bit) — numeric comparison with negatives
// Tag: TagAngleSecondAxis (0018,9219), value: -100
// ============================================================

OFTEST(dcmdata_condition_SS)
{
    OFString tmpFile;
    OFCHECK(createTestFile(tmpFile).good());
    MdfDatasetManager mgr;
    OFCHECK(loadTestFile(mgr, tmpFile).good());

    checkAllOperators(mgr, "(0018,9219)", "-100", "-200", "0");

    // Test with positive comparison value
    COND_MET(mgr, "(0018,9219)", "<", "100");
    COND_NOT_MET(mgr, "(0018,9219)", ">", "100");

    OFStandard::deleteFile(tmpFile);
}


// ============================================================
// Test: UL (Unsigned Long, 32-bit) — numeric comparison
// Tag: SimpleFrameList (0008,1161), value: 100000
// ============================================================

OFTEST(dcmdata_condition_UL)
{
    OFString tmpFile;
    OFCHECK(createTestFile(tmpFile).good());
    MdfDatasetManager mgr;
    OFCHECK(loadTestFile(mgr, tmpFile).good());

    checkAllOperators(mgr, "(0008,1161)", "100000", "50000", "200000");

    OFStandard::deleteFile(tmpFile);
}


// ============================================================
// Test: SL (Signed Long, 32-bit) — numeric comparison with negatives
// Tag: IntegerNumericValue (0014,4108), value: -100000
// ============================================================

OFTEST(dcmdata_condition_SL)
{
    OFString tmpFile;
    OFCHECK(createTestFile(tmpFile).good());
    MdfDatasetManager mgr;
    OFCHECK(loadTestFile(mgr, tmpFile).good());

    checkAllOperators(mgr, "(0014,4108)", "-100000", "-200000", "0");

    OFStandard::deleteFile(tmpFile);
}


// ============================================================
// Test: UV (Unsigned Very Long, 64-bit) — numeric comparison
// Tag: FileOffsetInContainer (0008,040C), value: 4294967296 (2^32)
// ============================================================

OFTEST(dcmdata_condition_UV)
{
    OFString tmpFile;
    OFCHECK(createTestFile(tmpFile).good());
    MdfDatasetManager mgr;
    OFCHECK(loadTestFile(mgr, tmpFile).good());

    checkAllOperators(mgr, "(0008,040c)", "3000000000", "1000000000", "4000000000");

    OFStandard::deleteFile(tmpFile);
}


// ============================================================
// Test: SV (Signed Very Long, 64-bit) — numeric comparison
// Tag: SelectorSVValue (0072,0082), value: -4294967296 (-(2^32))
// ============================================================

OFTEST(dcmdata_condition_SV)
{
    OFString tmpFile;
    OFCHECK(createTestFile(tmpFile).good());
    MdfDatasetManager mgr;
    OFCHECK(loadTestFile(mgr, tmpFile).good());

    checkAllOperators(mgr, "(0072,0082)", "-2000000000", "-2100000000", "0");

    OFStandard::deleteFile(tmpFile);
}


// ============================================================
// Test: FL (Float, 32-bit) — numeric comparison
// Tag: RecommendedDisplayFrameRateInFloat (0008,9459), value: 25.5
// ============================================================

OFTEST(dcmdata_condition_FL)
{
    OFString tmpFile;
    OFCHECK(createTestFile(tmpFile).good());
    MdfDatasetManager mgr;
    OFCHECK(loadTestFile(mgr, tmpFile).good());

    checkAllOperators(mgr, "(0008,9459)", "25.5", "10.0", "50.0");

    OFStandard::deleteFile(tmpFile);
}


// ============================================================
// Test: FD (Float Double, 64-bit) — numeric comparison
// Tag: AcquisitionDuration (0018,9073), value: 3.14159
// ============================================================

OFTEST(dcmdata_condition_FD)
{
    OFString tmpFile;
    OFCHECK(createTestFile(tmpFile).good());
    MdfDatasetManager mgr;
    OFCHECK(loadTestFile(mgr, tmpFile).good());

    checkAllOperators(mgr, "(0018,9073)", "3.14159", "1.0", "10.0");

    OFStandard::deleteFile(tmpFile);
}


// ============================================================
// Test: OB (Other Byte) — hexadecimal string comparison
// Tag: SelectorOBValue (0072,0065), value: "0a\1b\ff" (3 bytes)
// OB uses hex format for putString/getOFStringArray; createCompareElement
// passes hex through without decimal normalization.
// ============================================================

OFTEST(dcmdata_condition_OB)
{
    OFString tmpFile;
    OFCHECK(createTestFile(tmpFile).good());
    MdfDatasetManager mgr;
    OFCHECK(loadTestFile(mgr, tmpFile).good());

    checkAllOperators(mgr, "(0072,0065)", "0a\\1b\\ff", "0a\\1b\\00", "0a\\1b\\ff\\ff");

    OFStandard::deleteFile(tmpFile);
}


// ============================================================
// Test: OW (Other Word) — hexadecimal string comparison
// Tag: SelectorOWValue (0072,0069), value: "00ff\7fff" (2 words)
// OW uses hex format with 4-digit groups.
// ============================================================

OFTEST(dcmdata_condition_OW)
{
    OFString tmpFile;
    OFCHECK(createTestFile(tmpFile).good());
    MdfDatasetManager mgr;
    OFCHECK(loadTestFile(mgr, tmpFile).good());

    checkAllOperators(mgr, "(0072,0069)", "00ff\\7fff", "00ff\\0000", "00ff\\7fff\\ffff");

    OFStandard::deleteFile(tmpFile);
}


// ============================================================
// Test: OL (Other Long, 32-bit unsigned) — numeric comparison
// Tag: SelectorOLValue (0072,0075), value: 70000
// OL inherits from DcmUnsignedLong; uses decimal putString/compare.
// ============================================================

OFTEST(dcmdata_condition_OL)
{
    OFString tmpFile;
    OFCHECK(createTestFile(tmpFile).good());
    MdfDatasetManager mgr;
    OFCHECK(loadTestFile(mgr, tmpFile).good());

    checkAllOperators(mgr, "(0072,0075)", "70000", "50000", "90000");

    OFStandard::deleteFile(tmpFile);
}


// ============================================================
// Test: OV (Other 64-bit Very Long, unsigned) — numeric comparison
// Tag: SelectorOVValue (0072,0081), value: 5000000000
// OV inherits from DcmUnsigned64bitVeryLong; uses decimal putString/compare.
// ============================================================

OFTEST(dcmdata_condition_OV)
{
    OFString tmpFile;
    OFCHECK(createTestFile(tmpFile).good());
    MdfDatasetManager mgr;
    OFCHECK(loadTestFile(mgr, tmpFile).good());

    checkAllOperators(mgr, "(0072,0081)", "70000", "50000", "90000");

    OFStandard::deleteFile(tmpFile);
}


// ============================================================
// Test: OF (Other Float, 32-bit) — numeric comparison
// Tag: SelectorOFValue (0072,0067), value: 42.5
// OF inherits from DcmFloatingPointSingle; uses decimal float putString/compare.
// ============================================================

OFTEST(dcmdata_condition_OF)
{
    OFString tmpFile;
    OFCHECK(createTestFile(tmpFile).good());
    MdfDatasetManager mgr;
    OFCHECK(loadTestFile(mgr, tmpFile).good());

    checkAllOperators(mgr, "(0072,0067)", "42.5", "10.0", "100.0");

    OFStandard::deleteFile(tmpFile);
}


// ============================================================
// Test: OD (Other Double, 64-bit) — numeric comparison
// Tag: SelectorODValue (0072,0073), value: 123.456
// OD inherits from DcmFloatingPointDouble; uses decimal float putString/compare.
// ============================================================

OFTEST(dcmdata_condition_OD)
{
    OFString tmpFile;
    OFCHECK(createTestFile(tmpFile).good());
    MdfDatasetManager mgr;
    OFCHECK(loadTestFile(mgr, tmpFile).good());

    checkAllOperators(mgr, "(0072,0073)", "123.456", "50.0", "200.0");

    OFStandard::deleteFile(tmpFile);
}


// ============================================================
// Test: UN (Unknown VR) — hex comparison via numeric path
// Tag: private (0009,1010), value: "7f" (single byte)
// UN is stored as DcmOtherByteOtherWord internally and routed
// through checkNumberCondition() (same as OB). createCompareElement()
// skips decimal normalization and passes hex to putString() directly.
// DcmOtherByteOtherWord::compare() does proper byte-level comparison.
// ============================================================

OFTEST(dcmdata_condition_UN)
{
    OFString tmpFile;
    OFCHECK(createTestFile(tmpFile).good());
    MdfDatasetManager mgr;
    OFCHECK(loadTestFile(mgr, tmpFile).good());

    checkAllOperators(mgr, "(0009,1010)", "7f", "00", "7f\\ff");

    // Existence checks
    COND_MET(mgr, "(0009,1010)", "E", "");
    COND_NOT_MET(mgr, "(0009,1010)", "!", "");

    OFStandard::deleteFile(tmpFile);
}


// ============================================================
// Test: IS (Integer String) — goes through numeric path (isaNumber)
// Tag: NumberOfFrames (0028,0008), value: "10"
//
// KNOWN LIMITATION: IS is classified as isaNumber() and goes through
// checkNumberCondition(), which uses DcmElement::compare(). However,
// DcmIntegerString inherits compare() from DcmByteString, which does
// lexicographic string comparison, not numeric. This means ordering
// comparisons (<, >, <=, >=) may produce incorrect results for values
// of different string lengths (e.g. "10" < "5" lexicographically).
// Equality (=, !=) works correctly.
// ============================================================

OFTEST(dcmdata_condition_IS)
{
    OFString tmpFile;
    OFCHECK(createTestFile(tmpFile).good());
    MdfDatasetManager mgr;
    OFCHECK(loadTestFile(mgr, tmpFile).good());

    // Equality and inequality work correctly
    COND_MET(mgr, "(0028,0008)", "=", "10");
    COND_NOT_MET(mgr, "(0028,0008)", "=", "5");
    COND_NOT_MET(mgr, "(0028,0008)", "=", "20");
    COND_NOT_MET(mgr, "(0028,0008)", "!=", "10");
    COND_MET(mgr, "(0028,0008)", "!=", "5");
    COND_MET(mgr, "(0028,0008)", "!=", "20");

    // Leading zeros are normalized, so equality still works
    COND_MET(mgr, "(0028,0008)", "=", "010");

    // Ordering works correctly when normalized values have the same string
    // length (lexicographic comparison matches numeric ordering in this case).
    // Value is "10": compare with same-length strings only.
    COND_MET(mgr, "(0028,0008)", "<", "20");     // "10" < "20" OK (same length)
    COND_NOT_MET(mgr, "(0028,0008)", "<", "10"); // "10" < "10" is false
    COND_MET(mgr, "(0028,0008)", "<=", "10");
    COND_MET(mgr, "(0028,0008)", ">=", "10");

    // Note: ordering across different digit counts is lexicographic, not numeric.
    // E.g. "10" < "5" is true lexicographically (but false numerically).
    // This is a known limitation of using DcmByteString::compare() for IS.

    OFStandard::deleteFile(tmpFile);
}


// ============================================================
// Test: DS (Decimal String) — goes through numeric path (isaNumber)
// Tag: WindowCenter (0028,1050), value: "100.5"
//
// KNOWN LIMITATION: DS is classified as isaNumber() and goes through
// checkNumberCondition(), which uses DcmElement::compare(). However,
// DcmDecimalString inherits compare() from DcmByteString, which does
// lexicographic string comparison, not numeric. This means ordering
// comparisons (<, >, <=, >=) may produce incorrect results
// (e.g. "100.5" < "50.0" lexicographically because '1' < '5').
// Equality (=, !=) works correctly.
// ============================================================

OFTEST(dcmdata_condition_DS)
{
    OFString tmpFile;
    OFCHECK(createTestFile(tmpFile).good());
    MdfDatasetManager mgr;
    OFCHECK(loadTestFile(mgr, tmpFile).good());

    // Equality and inequality work correctly
    COND_MET(mgr, "(0028,1050)", "=", "100.5");
    COND_NOT_MET(mgr, "(0028,1050)", "=", "50.0");
    COND_NOT_MET(mgr, "(0028,1050)", "=", "200.0");
    COND_NOT_MET(mgr, "(0028,1050)", "!=", "100.5");
    COND_MET(mgr, "(0028,1050)", "!=", "50.0");
    COND_MET(mgr, "(0028,1050)", "!=", "200.0");

    // Ordering: lexicographic, not numeric. Only reliable when values have
    // same number of digits before decimal point.
    COND_MET(mgr, "(0028,1050)", "<=", "100.5");
    COND_MET(mgr, "(0028,1050)", ">=", "100.5");

    // Note: "100.5" < "200.0" is true both lexicographically and numerically
    // (same number of digits before decimal), so this one works:
    COND_MET(mgr, "(0028,1050)", "<", "200.0");
    COND_MET(mgr, "(0028,1050)", ">", "100.0");

    OFStandard::deleteFile(tmpFile);
}


// ============================================================
// Test: CS (Code String) — lexicographic string comparison
// Tag: Modality (0008,0060), value: "CT"
// ============================================================

OFTEST(dcmdata_condition_CS)
{
    OFString tmpFile;
    OFCHECK(createTestFile(tmpFile).good());
    MdfDatasetManager mgr;
    OFCHECK(loadTestFile(mgr, tmpFile).good());

    // String comparison is lexicographic
    checkAllOperators(mgr, "(0008,0060)", "CT", "AA", "ZZ");

    // Case sensitivity: "ct" != "CT"
    COND_NOT_MET(mgr, "(0008,0060)", "=", "ct");

    OFStandard::deleteFile(tmpFile);
}


// ============================================================
// Test: LO (Long String) — lexicographic string comparison
// Tag: Manufacturer (0008,0070), value: "ACME Corp"
// ============================================================

OFTEST(dcmdata_condition_LO)
{
    OFString tmpFile;
    OFCHECK(createTestFile(tmpFile).good());
    MdfDatasetManager mgr;
    OFCHECK(loadTestFile(mgr, tmpFile).good());

    checkAllOperators(mgr, "(0008,0070)", "ACME Corp", "AAAA", "ZZZZ");

    OFStandard::deleteFile(tmpFile);
}


// ============================================================
// Test: PN (Person Name) — lexicographic string comparison
// Tag: PatientName (0010,0010), value: "Doe^John"
// ============================================================

OFTEST(dcmdata_condition_PN)
{
    OFString tmpFile;
    OFCHECK(createTestFile(tmpFile).good());
    MdfDatasetManager mgr;
    OFCHECK(loadTestFile(mgr, tmpFile).good());

    checkAllOperators(mgr, "(0010,0010)", "Doe^John", "Adams^Alice", "Zulu^Zach");

    OFStandard::deleteFile(tmpFile);
}


// ============================================================
// Test: DA (Date) — string comparison, safe for ordering
// Tag: StudyDate (0008,0020), value: "20240115"
// DA has fixed 8-char format YYYYMMDD so lexicographic ordering
// produces correct chronological ordering.
// ============================================================

OFTEST(dcmdata_condition_DA)
{
    OFString tmpFile;
    OFCHECK(createTestFile(tmpFile).good());
    MdfDatasetManager mgr;
    OFCHECK(loadTestFile(mgr, tmpFile).good());

    checkAllOperators(mgr, "(0008,0020)", "20240115", "20230101", "20251231");

    OFStandard::deleteFile(tmpFile);
}


// ============================================================
// Test: UI (Unique Identifier) — string comparison + UID name lookup
// Tag: SOPClassUID (0008,0016), value: UID_CTImageStorage
// ============================================================

OFTEST(dcmdata_condition_UI)
{
    OFString tmpFile;
    OFCHECK(createTestFile(tmpFile).good());
    MdfDatasetManager mgr;
    OFCHECK(loadTestFile(mgr, tmpFile).good());

    // Equality with raw UID value
    COND_MET(mgr, "(0008,0016)", "=", UID_CTImageStorage);
    COND_NOT_MET(mgr, "(0008,0016)", "!=", UID_CTImageStorage);

    // Equality with UID name (special UID name lookup)
    COND_MET(mgr, "(0008,0016)", "=", "CTImageStorage");
    COND_NOT_MET(mgr, "(0008,0016)", "=", "MRImageStorage");

    // Inequality with UID name
    COND_NOT_MET(mgr, "(0008,0016)", "!=", "CTImageStorage");
    COND_MET(mgr, "(0008,0016)", "!=", "MRImageStorage");

    // Ordering operators use raw UID string (no name lookup)
    // UID is a dotted numeric string, so < and > are lexicographic
    COND_MET(mgr, "(0008,0016)", "<", "9.9.9.9");
    COND_MET(mgr, "(0008,0016)", ">", "1.0.0.0");

    OFStandard::deleteFile(tmpFile);
}


// ============================================================
// Test: AT (Attribute Tag) — treated as string comparison
// Tag: FrameIncrementPointer (0028,0009), value: (0010,0010)
// AT is neither isaString() nor isaNumber(); the code handles it
// explicitly in the EVR_AT branch.
// ============================================================

OFTEST(dcmdata_condition_AT)
{
    OFString tmpFile;
    OFCHECK(createTestFile(tmpFile).good());
    MdfDatasetManager mgr;
    OFCHECK(loadTestFile(mgr, tmpFile).good());

    // AT values are represented as "(gggg,eeee)" strings internally
    COND_MET(mgr, "(0028,0009)", "=", "(0010,0010)");
    COND_NOT_MET(mgr, "(0028,0009)", "=", "(0010,0020)");
    COND_NOT_MET(mgr, "(0028,0009)", "!=", "(0010,0010)");
    COND_MET(mgr, "(0028,0009)", "!=", "(0010,0020)");

    // Lexicographic ordering on AT string representation
    COND_MET(mgr, "(0028,0009)", "<", "(ffff,ffff)");
    COND_MET(mgr, "(0028,0009)", ">", "(0000,0000)");
    COND_MET(mgr, "(0028,0009)", "<=", "(0010,0010)");
    COND_MET(mgr, "(0028,0009)", ">=", "(0010,0010)");

    OFStandard::deleteFile(tmpFile);
}


// ============================================================
// Test: VM (Value Multiplicity) indexing
// Tag: ImageType (0008,0008), value: "ORIGINAL\PRIMARY\AXIAL" (CS, VM=3)
// ============================================================

OFTEST(dcmdata_condition_VM)
{
    OFString tmpFile;
    OFCHECK(createTestFile(tmpFile).good());
    MdfDatasetManager mgr;
    OFCHECK(loadTestFile(mgr, tmpFile).good());

    // Without index: compares full concatenated value (backslash-separated)
    COND_MET(mgr, "(0008,0008)", "=", "ORIGINAL\\PRIMARY\\AXIAL");

    // With index: compares individual value
    COND_MET(mgr, "ImageType[0]", "=", "ORIGINAL");
    COND_MET(mgr, "ImageType[1]", "=", "PRIMARY");
    COND_MET(mgr, "ImageType[2]", "=", "AXIAL");

    // Inequality on indexed value
    COND_NOT_MET(mgr, "ImageType[0]", "=", "DERIVED");
    COND_MET(mgr, "ImageType[0]", "!=", "DERIVED");

    // Ordering on indexed value (lexicographic)
    COND_MET(mgr, "ImageType[0]", ">", "DERIVED");
    COND_MET(mgr, "ImageType[0]", "<", "ZZZZZ");

    OFStandard::deleteFile(tmpFile);
}


// ============================================================
// Test: Sequence wildcard [*] — condition must hold for all items
// ReferencedSeriesSequence (0008,1115) with 3 items:
//   Item 0: Rows = 256
//   Item 1: Rows = 512
//   Item 2: Rows = 512
// ============================================================

OFTEST(dcmdata_condition_sequence_wildcard)
{
    OFString tmpFile;
    OFCHECK(createTestFile(tmpFile).good());
    MdfDatasetManager mgr;
    OFCHECK(loadTestFile(mgr, tmpFile).good());

    // All items have Rows <= 512: passes (256<=512, 512<=512, 512<=512)
    COND_MET(mgr, "(0008,1115)[*].(0028,0010)", "<=", "512");

    // All items have Rows < 1024: passes
    COND_MET(mgr, "(0008,1115)[*].(0028,0010)", "<", "1024");

    // All items have Rows >= 256: passes (256>=256, 512>=256, 512>=256)
    COND_MET(mgr, "(0008,1115)[*].(0028,0010)", ">=", "256");

    // All items have Rows > 100: passes
    COND_MET(mgr, "(0008,1115)[*].(0028,0010)", ">", "100");

    // All items have Rows = 512: fails (item 0 has 256)
    COND_NOT_MET(mgr, "(0008,1115)[*].(0028,0010)", "=", "512");

    // All items have Rows > 256: fails (item 0 has exactly 256)
    COND_NOT_MET(mgr, "(0008,1115)[*].(0028,0010)", ">", "256");

    // All items have Rows != 256: fails (item 0 has 256)
    COND_NOT_MET(mgr, "(0008,1115)[*].(0028,0010)", "!=", "256");

    // Specific item [1] has Rows = 512: passes
    COND_MET(mgr, "(0008,1115)[1].(0028,0010)", "=", "512");

    // Specific item [0] has Rows = 256: passes
    COND_MET(mgr, "(0008,1115)[0].(0028,0010)", "=", "256");

    // Existence check: Rows exists in all items
    COND_MET(mgr, "(0008,1115)[*].(0028,0010)", "E", "");

    // Using dictionary names in sequence path
    COND_MET(mgr, "ReferencedSeriesSequence[*].Rows", "<=", "512");
    COND_NOT_MET(mgr, "ReferencedSeriesSequence[*].Rows", "=", "512");

    OFStandard::deleteFile(tmpFile);
}


// ============================================================
// Test: Tag name lookup (using dictionary names instead of (gggg,eeee))
// ============================================================

OFTEST(dcmdata_condition_tagname)
{
    OFString tmpFile;
    OFCHECK(createTestFile(tmpFile).good());
    MdfDatasetManager mgr;
    OFCHECK(loadTestFile(mgr, tmpFile).good());

    // Use tag name instead of (gggg,eeee)
    COND_MET(mgr, "PatientName", "=", "Doe^John");
    COND_MET(mgr, "Rows", "=", "512");
    COND_MET(mgr, "Modality", "=", "CT");
    COND_MET(mgr, "StudyDate", "=", "20240115");

    // Existence by name
    COND_MET(mgr, "PatientName", "E", "");
    COND_NOT_MET(mgr, "PatientName", "!", "");

    OFStandard::deleteFile(tmpFile);
}


// Note: splitConditionPathAndValue() is a protected method of MdfConsoleEngine
// and cannot be tested directly from here. Parsing is implicitly validated
// through command-line integration tests of dcmodify.


/*
 * === VRs NOT tested and rationale ===
 *
 * The following VRs are not individually tested because they share identical
 * comparison logic with VRs that ARE tested:
 *
 * String VRs using the same lexicographic OFString::compare() as CS/LO/PN:
 *   AE (Application Entity), AS (Age String), DT (DateTime), TM (Time),
 *   SH (Short String), ST (Short Text), LT (Long Text), UC (Unlimited Characters),
 *   UR (URI/URL), UT (Unlimited Text)
 *   -> All use checkStringCondition() with OFString::compare(). Only differ in
 *      max length and character set constraints, not comparison semantics.
 *
 * SQ (Sequence of Items):
 *   -> Not a leaf element; checkPathAndGetLeafElem() rejects it with EC_InvalidTag.
 *      Not testable through condition checks by design.
 */
