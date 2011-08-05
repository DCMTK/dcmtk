/*
 *
 *  Copyright (C) 2008-2011, OFFIS e.V.
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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2011-08-05 14:08:59 $
 *  CVS/RCS Revision: $Revision: 1.5 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
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
  // TODO TODO TODO TODO TODO TODO TODO TODO
#ifdef HAVE_GUSI_H
  GUSISetup(GUSIwithSIOUXSockets);
  GUSISetup(GUSIwithInternetSockets);
#endif

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

/*
 * CVS/RCS Log:
 * $Log: tpath.cc,v $
 * Revision 1.5  2011-08-05 14:08:59  joergr
 * Fixed output of test program in debug mode.
 *
 * Revision 1.4  2011-08-05 10:20:37  joergr
 * Renamed test logger in order to reflect the current name of the program.
 *
 * Revision 1.3  2011-07-06 13:04:18  uli
 * Fixed some inconsistencies in test names.
 *
 * Revision 1.2  2011-06-07 08:29:59  uli
 * Stop using the log levels INFO, WARN, ERROR and FATAL in tests.
 *
 * Revision 1.1  2011-05-25 10:05:55  uli
 * Imported oftest and converted existing tests to oftest.
 *
 * Revision 1.13  2010-10-14 13:15:05  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.12  2010-08-23 07:22:56  meichel
 * Minor changes needed for compilation on MSVC6
 *
 * Revision 1.11  2010-08-10 07:08:22  joergr
 * Re-added newline character removed by last commit and moved another newline
 * to the right place.
 *
 * Revision 1.10  2010-08-09 17:53:02  onken
 * Fixed path test.
 *
 * Revision 1.9  2010-08-09 13:41:54  joergr
 * Updated data dictionary to 2009 edition of the DICOM standard. From now on,
 * the official "keyword" is used for the attribute name which results in a
 * number of minor changes (e.g. "PatientsName" is now called "PatientName").
 *
 * Revision 1.8  2009-11-04 09:58:11  uli
 * Switched to logging mechanism provided by the "new" oflog module
 *
 * Revision 1.7  2009-04-20 16:03:12  joergr
 * Fixed typo.
 *
 * Revision 1.6  2008-12-12 11:44:41  onken
 * Moved path access functions to separate classes
 *
 * Revision 1.5  2008-12-05 13:28:55  onken
 * Changed test application to test splitted findOrCreate() path API.
 *
 * Revision 1.4  2008-12-04 16:56:38  onken
 * Extended application to also test new findOrCreatePath() wildcard features.
 *
 * Revision 1.3  2008-11-21 16:18:32  onken
 * Changed implementation of findOrCreatePath() to make use of function
 * newDicomElement() which also knows how to handle EVRs like ox correctly.
 *
 * Revision 1.2  2008-10-15 16:07:08  onken
 * Fixed CVS information in source's header comment.
 *
 * Revision 1.1  2008-10-15 15:59:02  onken
 * Added test program for testing DcmItem's and DcmSequenceOfItem's path access
 * features.
 *
 */
