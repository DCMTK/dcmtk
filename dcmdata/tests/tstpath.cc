/*
 *
 *  Copyright (C) 2008-2010, OFFIS e.V.
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
 *  Update Date:      $Date: 2010-10-14 13:15:05 $
 *  CVS/RCS Revision: $Revision: 1.13 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmdata/cmdlnarg.h"
#include "dcmtk/ofstd/ofconapp.h"
#include "dcmtk/dcmdata/dcpath.h"

#define OFFIS_CONSOLE_APPLICATION "tstpath"

static OFLogger tstpathLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION);

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

static void testPathInsertionsWithoutWildcard(const OFString& path,
                                              DcmDataset *dset,
                                              const Uint16& expectedNumObjects,
                                              const OFBool& expectFailed = OFFalse,
                                              const OFBool& createIfNecessary = OFTrue)
{
  OFLOG_INFO(tstpathLogger, "Path: " << path);
  DcmPathProcessor proc;
  OFCondition result = proc.findOrCreatePath(dset, path, createIfNecessary);
  if (result.bad())
  {
    if (!expectFailed)
        OFLOG_ERROR(tstpathLogger, " ...FAILED! Path " << (createIfNecessary ? "insertion" : "lookup") << " failed: " << result.text());
    return;
  }

  // get results
  OFList< DcmPath* > results;
  Uint32 numResults = proc.getResults(results);
  if (numResults != 1)
  {
    // non-wildcard insertion should ALWAYS only return one result
    OFLOG_ERROR(tstpathLogger, "...FAILED! Returned path list does contain more than one result but no wildcard was specified");
    return;
  }
  DcmPath* oneResult = * (results.begin());

  if (oneResult->size() != expectedNumObjects)
  {
    if (!expectFailed)
    {
        OFLOG_ERROR(tstpathLogger, "...FAILED! Returned object list does not contain " << expectedNumObjects << " but only " << oneResult->size() << "elements");
        return;
    }
  }
  else
  {
    OFListIterator(DcmPathNode*) it = oneResult->begin();
    for (Uint16 i=0; i < expectedNumObjects; i++)
    {
      if (*it == NULL)
      {
        OFLOG_ERROR(tstpathLogger, " ...FAILED! Path insertion failed: One of the created objects is NULL");
        return;
      }
      it++;
    }
  }

  OFLOG_INFO(tstpathLogger, " ...OK");
}


static void testPathInsertionsWithWildcard(const OFString& path,
                                           DcmDataset *dset,
                                           const Uint16& expectedNumResults,
                                           const OFBool& expectFailed = OFFalse,
                                           const OFBool& createIfNecessary = OFTrue)
{
  OFLOG_INFO(tstpathLogger, "Path: " << path);
  DcmPathProcessor proc;
  OFCondition result = proc.findOrCreatePath(dset, path, createIfNecessary);
  if (result.bad())
  {
    if (!expectFailed)
        OFLOG_ERROR(tstpathLogger, " ...FAILED! Path " << (createIfNecessary ? "insertion" : "lookup") << " failed: " << result.text());
    return;
  }

  // get results
  OFList< DcmPath* > results;
  Uint32 numResults = proc.getResults(results);
  if ( (numResults != expectedNumResults) && !expectFailed )
  {
    OFLOG_ERROR(tstpathLogger, " ...FAILED!: Expected " << expectedNumResults << " but " << numResults << " results were returned");
    return;
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
          OFLOG_INFO(tstpathLogger, " ...FAILED! Path insertion failed: One of the result paths contains NULL");
          return;
        }
        it++;
      }
    }
    else
    {
      OFLOG_INFO(tstpathLogger, " ...FAILED! Path insertion failed: One of the returned path is NULL");
      return;
    }
    oneResult++;
  }
  OFLOG_INFO(tstpathLogger, " ...OK");
}

int main(int argc, char *argv[])
{
#ifdef HAVE_GUSI_H
  GUSISetup(GUSIwithSIOUXSockets);
  GUSISetup(GUSIwithInternetSockets);
#endif

  OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION , "Tests DcmItem/DcmSequenceOfItem's path access features", rcsid);
  OFCommandLine cmd;

  cmd.addGroup("general options:");
   cmd.addOption("--help",    "-h", "print this help text and exit", OFCommandLine::AF_Exclusive);
   cmd.addOption("--version",       "print version information and exit", OFCommandLine::AF_Exclusive);
   OFLog::addOptions(cmd);

  /* evaluate command line */
  prepareCmdLineArgs(argc, argv, OFFIS_CONSOLE_APPLICATION);
  if (app.parseCommandLine(cmd, argc, argv, OFCommandLine::PF_ExpandWildcards))
  {
    /* check exclusive options first */
    if (cmd.hasExclusiveOption())
    {
      if (cmd.findOption("--version"))
      {
          app.printHeader(OFTrue /*print host identifier*/);
          COUT << OFendl << "External libraries used:";
          COUT << " none" << OFendl;
          return 0;
       }
    }
  }

  /* command line parameters */
  OFLog::configureFromCommandLine(cmd, app);

  /* make sure data dictionary is loaded */
  if (!dcmDataDict.isDictionaryLoaded())
  {
      OFLOG_WARN(tstpathLogger, "no data dictionary loaded, "
           << "check environment variable: " << DCM_DICT_ENVIRONMENT_VARIABLE);
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

  OFLOG_INFO(tstpathLogger, "These insertions should work:\n"
                         << "=============================");
  path = "PatientID";
  testPathInsertionsWithoutWildcard(path, dset, 1);
  path = "ContentSequence";
  testPathInsertionsWithoutWildcard(path, dset, 1);
  path = "(0040,A730)";
  testPathInsertionsWithoutWildcard(path, dset, 1);
  path = "(0040,A730)[5]";
  testPathInsertionsWithoutWildcard(path, dset, 2);
  path = "(0040,A730)[5].ContentSequence[3].ConceptNameCodeSequence[0].CodeValue";
  testPathInsertionsWithoutWildcard(path, dset, 7);
  path = "ContentSequence[5].ContentSequence[3].ConceptNameCodeSequence[0].(0008,0104)";
  testPathInsertionsWithoutWildcard(path, dset, 7);

  OFLOG_INFO(tstpathLogger, "\nThese insertions should NOT work (wrong syntax):\n"
                           << "================================================");

  path = "ContentSequences";
  testPathInsertionsWithoutWildcard(path, dset, 1, OFTrue);
  path = "(1234,ContentSequences";
  testPathInsertionsWithoutWildcard(path, dset, 1, OFTrue);
  path = "(00X4,A730)";
  testPathInsertionsWithoutWildcard(path, dset, 1, OFTrue);
  path = "(0040,A730)[-5]";
  testPathInsertionsWithoutWildcard(path, dset, 2, OFTrue);
  path = "(0040,A730)[a].ContentSequence[3].ConceptNameCodeSequence[0].CodeValue";
  testPathInsertionsWithoutWildcard(path, dset, 7, OFTrue);

  OFLOG_INFO(tstpathLogger, "\nThese find routines should work:\n"
                           << "================================");

  path = "PatientID";
  testPathInsertionsWithoutWildcard(path, dset, 1, OFFalse, OFFalse /* do not create */);
  path = "ContentSequence";
  testPathInsertionsWithoutWildcard(path, dset, 1, OFFalse, OFFalse /* do not create */);
  path = "(0040,A730)";
  testPathInsertionsWithoutWildcard(path, dset, 1, OFFalse, OFFalse /* do not create */);
  path = "(0040,A730)[5]";
  testPathInsertionsWithoutWildcard(path, dset, 2, OFFalse, OFFalse /* do not create */);
  path = "(0040,A730)[5].ContentSequence[3].ConceptNameCodeSequence[0].CodeValue";
  testPathInsertionsWithoutWildcard(path, dset, 7, OFFalse, OFFalse /* do not create */);
  path = "ContentSequence[5].ContentSequence[3].ConceptNameCodeSequence[0].(0008,0104)";
  testPathInsertionsWithoutWildcard(path, dset, 7, OFFalse, OFFalse /* do not create */);

  OFLOG_INFO(tstpathLogger, "\nThese find routines should NOT work:\n"
                            << "====================================");

  path = "PatientName"; // was never inserted
  testPathInsertionsWithoutWildcard(path, dset, 1, OFTrue, OFFalse /* do not create */);
  path = "(0040,A730)[6]"; // we only have 6 items in there (not 7)
  testPathInsertionsWithoutWildcard(path, dset, 2, OFTrue, OFFalse /* do not create */);
  path = "ConceptNameCodeSequence"; // should not exist on main level
  testPathInsertionsWithoutWildcard(path, dset, 1, OFTrue, OFFalse /* do not create */);

  OFLOG_INFO(tstpathLogger, "\nChecking dataset length:\n"
                           << "========================");
  Uint32 length = dset->calcElementLength(EXS_LittleEndianExplicit,EET_ExplicitLength);
  OFLOG_INFO(tstpathLogger, "Checking whether length of encoded dataset matches pre-calculated length");
  if (length == precalculatedLength)
  {
    OFLOG_INFO(tstpathLogger, " ...OK");
  }
  else
  {
    OFLOG_ERROR(tstpathLogger, " ...FAILED: Length is " << length << ". Should be " << precalculatedLength << "\n"
            << "Please check dump and adapt test in case of false alarm:\n");
    CERR << "Dump of assembled test object:" << OFendl;
    dset->print(CERR);
    CERR << OFendl;
    CERR << "Dump of pre-defined template:" << OFendl;
    CERR << precalculatedDump << OFendl;
    exit(1);
  }

  /* ********************************************************************* */
  /* Test insertions using wildcards                                       */
  /* ********************************************************************* */

  OFLOG_INFO(tstpathLogger, "\nThese wildcard insertions should work:\n"
                           << "======================================");
  path = "ContentSequence[*].ContentSequence[3].PatientName";
  testPathInsertionsWithWildcard(path, dset, 6, OFFalse /* should work*/, OFTrue /*do create*/);
  path = "ContentSequence[*].ContentSequence[0].PatientID";
  testPathInsertionsWithWildcard(path, dset, 6, OFFalse /* should work*/, OFTrue /*do create*/);

  OFLOG_INFO(tstpathLogger, "\nTesting wildcard insertions should NOT work:\n"
                           << "============================================");
  path = "SourceImageSequence[*]";
  testPathInsertionsWithWildcard(path, dset, 0, OFTrue /* should fail*/, OFTrue /*do create*/);
  path = "ContentSequence[*].SourceImageSequence[*]";
  testPathInsertionsWithWildcard(path, dset, 0, OFTrue /* should fail*/, OFTrue /*do create*/);

  OFLOG_INFO(tstpathLogger, "\nChecking dataset length:\n"
                           << "========================");
  length = dset->calcElementLength(EXS_LittleEndianExplicit,EET_ExplicitLength);
  OFLOG_INFO(tstpathLogger, "Checking whether length of encoded dataset matches pre-calculated length");
  if (length == precalculatedLength2)
  {
    OFLOG_INFO(tstpathLogger, " ...OK");
  }
  else
  {
    OFLOG_ERROR(tstpathLogger, " ...FAILED: Length is " << length << ". Should be " << precalculatedLength2 << "\n"
            << "Please check dump and adapt test in case of false alarm:");
    CERR << "Dump of assembled test object:" << OFendl;
    dset->print(CERR);
    CERR << OFendl;
    CERR << "Dump of pre-defined template:" << OFendl;
    CERR << precalculatedDump2 << OFendl;
    exit(1);
  }

  exit(0);
}

/*
 * CVS/RCS Log:
 * $Log: tstpath.cc,v $
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
