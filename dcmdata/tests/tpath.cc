/*
 *
 *  Copyright (C) 2008-2017, OFFIS e.V.
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
 *  Purpose: Test program for testing path features of DcmItem
 *           and DcmSequenceOfItem
 *
 */

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmdata/dcpath.h"


static OFLogger tpathLogger = OFLog::getLogger("dcmtk.test.tpath");

#define CHECK_GOOD(check) OFCHECK((check).good())

#define RETURN_ERROR(msg) do { \
    OFOStringStream oss; \
    OFCondition cond; \
    oss << msg << OFStringStream_ends; \
    OFSTRINGSTREAM_GETSTR(oss, str) \
    cond = makeOFCondition(OFM_dcmdata, 1, OF_error, str); \
    OFSTRINGSTREAM_FREESTR(str) \
    return cond; \
} while (0)


static OFCondition testPathInsertionsWithoutWildcard(const OFString& path,
                                                     DcmDataset *dset,
                                                     const Uint16& expectedNumObjects,
                                                     const OFBool& expectFailed = OFFalse,
                                                     const OFBool& createIfNecessary = OFTrue)
{
  DcmPathProcessor proc;
  OFCondition result = proc.findOrCreatePath(dset, path, createIfNecessary);
  if (result.bad())
  {
    if (!expectFailed)
    {
      OFLOG_DEBUG(tpathLogger, "Path: " << path << " ... FAILED!");
      RETURN_ERROR("Path " << (createIfNecessary ? "insertion" : "lookup") << " failed: " << result.text());
    }
    return EC_Normal;
  }

  // get results
  OFList< DcmPath* > results;
  Uint32 numResults = proc.getResults(results);
  if (numResults != 1)
  {
    // non-wildcard insertion should ALWAYS only return one result
    OFLOG_DEBUG(tpathLogger, "Path: " << path << " ... FAILED!");
    RETURN_ERROR("Returned path list does contain more than one result but no wildcard was specified");
  }
  DcmPath* oneResult = * (results.begin());

  if (oneResult->size() != expectedNumObjects)
  {
    if (!expectFailed)
    {
      OFLOG_DEBUG(tpathLogger, "Path: " << path << " ... FAILED!");
      RETURN_ERROR("Returned object list does not contain " << expectedNumObjects << " but only " << oneResult->size() << "elements");
    }
  }
  else
  {
    OFListIterator(DcmPathNode*) it = oneResult->begin();
    for (Uint16 i=0; i < expectedNumObjects; i++)
    {
      if (*it == NULL)
      {
        OFLOG_DEBUG(tpathLogger, "Path: " << path << " ... FAILED!");
        RETURN_ERROR("Path insertion failed: One of the created objects is NULL");
      }
      it++;
    }
  }

  OFLOG_DEBUG(tpathLogger, "Path: " << path << " ... OK");
  return EC_Normal;
}

static OFCondition testPathInsertionsWithWildcard(const OFString& path,
                                                  DcmDataset *dset,
                                                  const Uint16& expectedNumResults,
                                                  const OFBool& expectFailed = OFFalse,
                                                  const OFBool& createIfNecessary = OFTrue)
{
  DcmPathProcessor proc;
  OFCondition result = proc.findOrCreatePath(dset, path, createIfNecessary);
  if (result.bad())
  {
    if (!expectFailed)
    {
      OFLOG_DEBUG(tpathLogger, "Path: " << path << " ... FAILED!");
      RETURN_ERROR("Path " << (createIfNecessary ? "insertion" : "lookup") << " failed: " << result.text());
    }
    return EC_Normal;
  }

  // get results
  OFList< DcmPath* > results;
  Uint32 numResults = proc.getResults(results);
  if ( (numResults != expectedNumResults) && !expectFailed )
  {
    OFLOG_DEBUG(tpathLogger, "Path: " << path << " ... FAILED!");
    RETURN_ERROR("Expected " << expectedNumResults << " but " << numResults << " results were returned");
  }
  // check results
  OFListIterator(DcmPath*) oneResult = results.begin();
  while (oneResult != results.end())
  {
    if (*oneResult)
    {
      OFListIterator(DcmPathNode*) it = (*oneResult)->begin();
      while (it != (*oneResult)->end())
      {
        if (*it == NULL)
        {
          OFLOG_DEBUG(tpathLogger, "Path: " << path << " ... FAILED!");
          RETURN_ERROR("Path insertion failed: One of the result paths contains NULL");
        }
        it++;
      }
    }
    else
    {
      OFLOG_DEBUG(tpathLogger, "Path: " << path << " ... FAILED!");
      RETURN_ERROR("Path insertion failed: One of the returned paths is NULL");
    }
    oneResult++;
  }
  OFLOG_DEBUG(tpathLogger, "Path: " << path << " ... OK");
  return EC_Normal;
}

OFTEST(dcmdata_pathAccess)
{

  /* make sure data dictionary is loaded */
  if (!dcmDataDict.isDictionaryLoaded())
  {
    OFCHECK_FAIL("no data dictionary loaded, check environment variable: " DCM_DICT_ENVIRONMENT_VARIABLE);
    return;
  }

  DcmFileFormat dcmff;
  DcmDataset *dset = dcmff.getDataset();
  OFString path;
  const OFString precalculatedDump =  /* break the dump into smaller parts because MSVC6 only accepts 2048 character string constants maximum */
    "# Dicom-Data-Set\n# Used TransferSyntax: Unknown Transfer Syntax\n(0040,a730) SQ (Sequence with explicit length #=6)      #   0, 1 ContentSequence\n"
    "  (fffe,e000) na (Item with explicit length #=0)          #   0, 1 Item\n  (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n"
    "  (fffe,e000) na (Item with explicit length #=0)          #   0, 1 Item\n  (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n"
    "  (fffe,e000) na (Item with explicit length #=0)          #   0, 1 Item\n  (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n"
    "  (fffe,e000) na (Item with explicit length #=0)          #   0, 1 Item\n  (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n"
    "  (fffe,e000) na (Item with explicit length #=0)          #   0, 1 Item\n  (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n"
    "  (fffe,e000) na (Item with explicit length #=1)          #   0, 1 Item\n    (0040,a730) SQ (Sequence with explicit length #=4)      #   0, 1 ContentSequence\n"
    "      (fffe,e000) na (Item with explicit length #=0)          #   0, 1 Item\n      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n"
    "      (fffe,e000) na (Item with explicit length #=0)          #   0, 1 Item\n      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n"
    "      (fffe,e000) na (Item with explicit length #=0)          #   0, 1 Item\n      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n"
    "      (fffe,e000) na (Item with explicit length #=1)          #   0, 1 Item\n        (0040,a043) SQ (Sequence with explicit length #=1)      #   0, 1 ConceptNameCodeSequence\n"
    "          (fffe,e000) na (Item with explicit length #=2)          #   0, 1 Item\n            (0008,0100) SH (no value available)                     #   0, 0 CodeValue\n"
    "            (0008,0104) LO (no value available)                     #   0, 0 CodeMeaning\n          (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n"
    "        (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n"
    "    (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n  (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n"
    "(fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";

  const OFString precalculatedDump2 =
    "# Dicom-Data-Set\n# Used TransferSyntax: Unknown Transfer Syntax\n(0010,0020) LO (no value available)                     #   0, 0 PatientID\n"
    "(0040,a730) SQ (Sequence with explicit length #=6)      #   0, 1 ContentSequence\n  (fffe,e000) na (Item with explicit length #=1)          #   0, 1 Item\n"
    "    (0040,a730) SQ (Sequence with explicit length #=4)      #   0, 1 ContentSequence\n      (fffe,e000) na (Item with explicit length #=1)          #   0, 1 Item\n"
    "        (0010,0020) LO (no value available)                     #   0, 0 PatientID\n      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n"
    "      (fffe,e000) na (Item with explicit length #=0)          #   0, 1 Item\n      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n"
    "      (fffe,e000) na (Item with explicit length #=0)          #   0, 1 Item\n      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n"
    "      (fffe,e000) na (Item with explicit length #=1)          #   0, 1 Item\n        (0010,0010) PN (no value available)                     #   0, 0 PatientName\n"
    "      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n    (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n"
    "  (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n  (fffe,e000) na (Item with explicit length #=1)          #   0, 1 Item\n"
    "    (0040,a730) SQ (Sequence with explicit length #=4)      #   0, 1 ContentSequence\n      (fffe,e000) na (Item with explicit length #=1)          #   0, 1 Item\n"
    "        (0010,0020) LO (no value available)                     #   0, 0 PatientID\n      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n"
    "      (fffe,e000) na (Item with explicit length #=0)          #   0, 1 Item\n      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n"
    "      (fffe,e000) na (Item with explicit length #=0)          #   0, 1 Item\n      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n"
    "      (fffe,e000) na (Item with explicit length #=1)          #   0, 1 Item\n        (0010,0010) PN (no value available)                     #   0, 0 PatientName\n"
    "      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n    (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n"
    "  (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n  (fffe,e000) na (Item with explicit length #=1)          #   0, 1 Item\n"
    "    (0040,a730) SQ (Sequence with explicit length #=4)      #   0, 1 ContentSequence\n      (fffe,e000) na (Item with explicit length #=1)          #   0, 1 Item\n"
    "        (0010,0020) LO (no value available)                     #   0, 0 PatientID\n      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n"
    "      (fffe,e000) na (Item with explicit length #=0)          #   0, 1 Item\n      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n"
    "      (fffe,e000) na (Item with explicit length #=0)          #   0, 1 Item\n      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n"
    "      (fffe,e000) na (Item with explicit length #=1)          #   0, 1 Item\n        (0010,0010) PN (no value available)                     #   0, 0 PatientName\n"
    "      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n    (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n"
    "  (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n  (fffe,e000) na (Item with explicit length #=1)          #   0, 1 Item\n"
    "    (0040,a730) SQ (Sequence with explicit length #=4)      #   0, 1 ContentSequence\n      (fffe,e000) na (Item with explicit length #=1)          #   0, 1 Item\n"
    "        (0010,0020) LO (no value available)                     #   0, 0 PatientID\n      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n"
    "      (fffe,e000) na (Item with explicit length #=0)          #   0, 1 Item\n      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n"
    "      (fffe,e000) na (Item with explicit length #=0)          #   0, 1 Item\n      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n"
    "      (fffe,e000) na (Item with explicit length #=1)          #   0, 1 Item\n        (0010,0010) PN (no value available)                     #   0, 0 PatientName\n"
    "      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n    (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n"
    "  (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n  (fffe,e000) na (Item with explicit length #=1)          #   0, 1 Item\n"
    "    (0040,a730) SQ (Sequence with explicit length #=4)      #   0, 1 ContentSequence\n      (fffe,e000) na (Item with explicit length #=1)          #   0, 1 Item\n"
    "        (0010,0020) LO (no value available)                     #   0, 0 PatientID\n      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n"
    "      (fffe,e000) na (Item with explicit length #=0)          #   0, 1 Item\n      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n"
    "      (fffe,e000) na (Item with explicit length #=0)          #   0, 1 Item\n      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n"
    "      (fffe,e000) na (Item with explicit length #=1)          #   0, 1 Item\n        (0010,0010) PN (no value available)                     #   0, 0 PatientName\n"
    "      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n    (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n"
    "  (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n  (fffe,e000) na (Item with explicit length #=1)          #   0, 1 Item\n"
    "    (0040,a730) SQ (Sequence with explicit length #=4)      #   0, 1 ContentSequence\n      (fffe,e000) na (Item with explicit length #=1)          #   0, 1 Item\n"
    "        (0010,0020) LO (no value available)                     #   0, 0 PatientID\n      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n"
    "      (fffe,e000) na (Item with explicit length #=0)          #   0, 1 Item\n      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n"
    "      (fffe,e000) na (Item with explicit length #=0)          #   0, 1 Item\n      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n"
    "      (fffe,e000) na (Item with explicit length #=2)          #   0, 1 Item\n        (0010,0010) PN (no value available)                     #   0, 0 PatientName\n"
    "        (0040,a043) SQ (Sequence with explicit length #=1)      #   0, 1 ConceptNameCodeSequence\n          (fffe,e000) na (Item with explicit length #=2)          #   0, 1 Item\n"
    "            (0008,0100) SH (no value available)                     #   0, 0 CodeValue\n            (0008,0104) LO (no value available)                     #   0, 0 CodeMeaning\n"
    "          (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n        (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n"
    "      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n    (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n"
    "  (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n(fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";

  const Uint32 precalculatedLength = 148;
  const Uint32 precalculatedLength2 = 464;

  /* ********************************************************************* */
  /* Test insertions using no wildcards                                    */
  /* ********************************************************************* */

  OFLOG_DEBUG(tpathLogger, "These insertions should work:\n"
                        << "=============================");
  path = "PatientID";
  CHECK_GOOD(testPathInsertionsWithoutWildcard(path, dset, 1));
  path = "ContentSequence";
  CHECK_GOOD(testPathInsertionsWithoutWildcard(path, dset, 1));
  path = "(0040,A730)";
  CHECK_GOOD(testPathInsertionsWithoutWildcard(path, dset, 1));
  path = "(0040,A730)[5]";
  CHECK_GOOD(testPathInsertionsWithoutWildcard(path, dset, 2));
  path = "(0040,A730)[5].ContentSequence[3].ConceptNameCodeSequence[0].CodeValue";
  CHECK_GOOD(testPathInsertionsWithoutWildcard(path, dset, 7));
  path = "ContentSequence[5].ContentSequence[3].ConceptNameCodeSequence[0].(0008,0104)";
  CHECK_GOOD(testPathInsertionsWithoutWildcard(path, dset, 7));

  OFLOG_DEBUG(tpathLogger, "\nThese insertions should NOT work (wrong syntax):\n"
                          << "================================================");

  path = "ContentSequences";
  CHECK_GOOD(testPathInsertionsWithoutWildcard(path, dset, 1, OFTrue));
  path = "(1234,ContentSequences";
  CHECK_GOOD(testPathInsertionsWithoutWildcard(path, dset, 1, OFTrue));
  path = "(00X4,A730)";
  CHECK_GOOD(testPathInsertionsWithoutWildcard(path, dset, 1, OFTrue));
  path = "(0040,A730)[-5]";
  CHECK_GOOD(testPathInsertionsWithoutWildcard(path, dset, 2, OFTrue));
  path = "(0040,A730)[a].ContentSequence[3].ConceptNameCodeSequence[0].CodeValue";
  CHECK_GOOD(testPathInsertionsWithoutWildcard(path, dset, 7, OFTrue));

  OFLOG_DEBUG(tpathLogger, "\nThese find routines should work:\n"
                          << "================================");

  path = "PatientID";
  CHECK_GOOD(testPathInsertionsWithoutWildcard(path, dset, 1, OFFalse, OFFalse /* do not create */));
  path = "ContentSequence";
  CHECK_GOOD(testPathInsertionsWithoutWildcard(path, dset, 1, OFFalse, OFFalse /* do not create */));
  path = "(0040,A730)";
  CHECK_GOOD(testPathInsertionsWithoutWildcard(path, dset, 1, OFFalse, OFFalse /* do not create */));
  path = "(0040,A730)[5]";
  CHECK_GOOD(testPathInsertionsWithoutWildcard(path, dset, 2, OFFalse, OFFalse /* do not create */));
  path = "(0040,A730)[5].ContentSequence[3].ConceptNameCodeSequence[0].CodeValue";
  CHECK_GOOD(testPathInsertionsWithoutWildcard(path, dset, 7, OFFalse, OFFalse /* do not create */));
  path = "ContentSequence[5].ContentSequence[3].ConceptNameCodeSequence[0].(0008,0104)";
  CHECK_GOOD(testPathInsertionsWithoutWildcard(path, dset, 7, OFFalse, OFFalse /* do not create */));

  OFLOG_DEBUG(tpathLogger, "\nThese find routines should NOT work:\n"
                          << "====================================");

  path = "PatientName"; // was never inserted
  CHECK_GOOD(testPathInsertionsWithoutWildcard(path, dset, 1, OFTrue, OFFalse /* do not create */));
  path = "(0040,A730)[6]"; // we only have 6 items in there (not 7)
  CHECK_GOOD(testPathInsertionsWithoutWildcard(path, dset, 2, OFTrue, OFFalse /* do not create */));
  path = "ConceptNameCodeSequence"; // should not exist on main level
  CHECK_GOOD(testPathInsertionsWithoutWildcard(path, dset, 1, OFTrue, OFFalse /* do not create */));

  OFLOG_DEBUG(tpathLogger, "\nChecking dataset length:\n"
                          << "========================");
  Uint32 length = dset->calcElementLength(EXS_LittleEndianExplicit,EET_ExplicitLength);
  if (length == precalculatedLength)
  {
    OFLOG_DEBUG(tpathLogger, "Checking whether length of encoded dataset matches pre-calculated length ... OK");
  }
  else
  {
    OFLOG_DEBUG(tpathLogger, "Checking whether length of encoded dataset matches pre-calculated length ... FAILED!");
    OFLOG_DEBUG(tpathLogger, "Length is " << length << ", but should be " << precalculatedLength);
    OFLOG_DEBUG(tpathLogger, "Please check dump and adapt test in case of false alarm:" << OFendl);
    OFLOG_DEBUG(tpathLogger, "Dump of assembled test object:");
    OFLOG_DEBUG(tpathLogger, DcmObject::PrintHelper(*dset));
    OFLOG_DEBUG(tpathLogger, "Dump of pre-defined template:" << OFendl);
    OFLOG_DEBUG(tpathLogger, precalculatedDump);
    OFCHECK_FAIL("Wrong length");
  }

  /* ********************************************************************* */
  /* Test insertions using wildcards                                       */
  /* ********************************************************************* */

  OFLOG_DEBUG(tpathLogger, "\nThese wildcard insertions should work:\n"
                          << "======================================");
  path = "ContentSequence[*].ContentSequence[3].PatientName";
  CHECK_GOOD(testPathInsertionsWithWildcard(path, dset, 6, OFFalse /* should work*/, OFTrue /*do create*/));
  path = "ContentSequence[*].ContentSequence[0].PatientID";
  CHECK_GOOD(testPathInsertionsWithWildcard(path, dset, 6, OFFalse /* should work*/, OFTrue /*do create*/));

  OFLOG_DEBUG(tpathLogger, "\nTesting wildcard insertions should NOT work:\n"
                          << "============================================");
  path = "SourceImageSequence[*]";
  CHECK_GOOD(testPathInsertionsWithWildcard(path, dset, 0, OFTrue /* should fail*/, OFTrue /*do create*/));
  path = "ContentSequence[*].SourceImageSequence[*]";
  CHECK_GOOD(testPathInsertionsWithWildcard(path, dset, 0, OFTrue /* should fail*/, OFTrue /*do create*/));

  OFLOG_DEBUG(tpathLogger, "\nChecking dataset length:\n"
                          << "========================");
  length = dset->calcElementLength(EXS_LittleEndianExplicit,EET_ExplicitLength);
  if (length == precalculatedLength2)
  {
    OFLOG_DEBUG(tpathLogger, "Checking whether length of encoded dataset matches pre-calculated length ... OK");
  }
  else
  {
    OFLOG_DEBUG(tpathLogger, "Checking whether length of encoded dataset matches pre-calculated length ... FAILED!");
    OFLOG_DEBUG(tpathLogger, "Length is " << length << ", but should be " << precalculatedLength2);
    OFLOG_DEBUG(tpathLogger, "Please check dump and adapt test in case of false alarm:" << OFendl);
    OFLOG_DEBUG(tpathLogger, "Dump of assembled test object:");
    OFLOG_DEBUG(tpathLogger, DcmObject::PrintHelper(*dset));
    OFLOG_DEBUG(tpathLogger, "Dump of pre-defined template:" << OFendl);
    OFLOG_DEBUG(tpathLogger, precalculatedDump2);
    OFCHECK_FAIL("Wrong length");
  }
}
