/*
 *
 *  Copyright (C) 2008, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module:  dcmdata
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Test program for testing path features of DcmItem
 *           and DcmSequenceOfItem
 *
 *  Last Update:      $Author: onken $
 *  Update Date:      $Date: 2008-12-05 13:28:55 $
 *  CVS/RCS Revision: $Revision: 1.5 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmdata/cmdlnarg.h"
#include "dcmtk/ofstd/ofconapp.h"
#include "dcmtk/dcmdata/dcdebug.h" /* For SetDebugLevel() */

#define OFFIS_CONSOLE_APPLICATION "tstpath"

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

static int opt_debugMode = 0;
static OFBool opt_verbose = OFFalse;

static void testPathInsertionsWithoutWildcard(const OFString& path,
                                              DcmDataset *dset,
                                              const Uint16& expectedNumObjects,
                                              const OFBool& expectFailed = OFFalse,
                                              const OFBool& createIfNecessary = OFTrue)
{
  OFList< DcmObject* > oneResult;
  if (opt_verbose) COUT << "Path: " << path; COUT.flush();
  OFCondition result = dset->findOrCreatePath(path, oneResult, createIfNecessary);
  if (result.bad())
  {
    if (!expectFailed || opt_verbose) CERR << " ...FAILED! Path " << (createIfNecessary ? "insertion" : "lookup") << " failed: " << result.text() << OFendl;
    return;
  }
  else
  {
    if (oneResult.size() != expectedNumObjects)
    {
      if (!expectFailed || opt_verbose) CERR << "...FAILED! Returned object list does not contain " << expectedNumObjects << " but only " << oneResult.size() << "elements" << OFendl;
      if (!expectFailed) return;
    }
    else
    {
      OFListIterator(DcmObject*) it = oneResult.begin();
      for (Uint16 i=0; i < expectedNumObjects; i++)
      {
        if (*it == NULL)
        {
          CERR << " ...FAILED! Path insertion failed: One of the created objects is NULL" << OFendl;
          return;
        }
        it++;
      }
    }
  }
  if (opt_verbose)
    COUT << " ...OK" << OFendl;
}


static void testPathInsertionsWithWildcard(const OFString& path,
                                           DcmDataset *dset,
                                           const Uint16& expectedNumResults,
                                           const OFBool& expectFailed = OFFalse,
                                           const OFBool& createIfNecessary = OFTrue)
{
  OFList< OFList<DcmObject*>* > results;
  OFList< DcmObject* > prefix;
  OFList< DcmObject* >* oneResult;
  if (opt_verbose) COUT << path; COUT.flush();
  OFCondition result = dset->findOrCreateWildcardPath(path, results, prefix, createIfNecessary);
  if (result.bad())
  {
    if (!expectFailed || opt_verbose) CERR << " ...FAILED! Path " << (createIfNecessary ? "insertion" : "lookup") << " failed: " << result.text() << OFendl;
    return;
  }
  else
  {
    if ( (results.size() != expectedNumResults) && !expectFailed )
    {

      CERR << " ...FAILED!: Expected " << expectedNumResults <<  " but " << results.size() << " results were returned" << OFendl;
      while (results.size() > 0)
      {
        oneResult = NULL;
        oneResult = results.front();
        if (oneResult)
        {
          delete oneResult; oneResult = NULL;
        }
        results.pop_front();
      }
      return;
    }
    while (results.size() > 0)
    {
      oneResult = NULL;
      oneResult = results.front();
      if (oneResult)
      {
        OFListIterator(DcmObject*) it = oneResult->begin();
        while (it != oneResult->end())
        {
          if (*it == NULL)
          {
            if (opt_verbose) CERR << " ...FAILED! Path insertion failed: One of the result paths contains NULL" << OFendl;
            return;
          }
          it++;
        }
        delete oneResult; oneResult = NULL;
      }
      else
      {
        if (opt_verbose) CERR << " ...FAILED! Path insertion failed: One of the returned path is NULL" << OFendl;
        return;
      }
      results.pop_front();
    }
  }
  if (opt_verbose)
    COUT << " ...OK" << OFendl;
}

int main(int argc, char *argv[])
{
#ifdef HAVE_GUSI_H
  GUSISetup(GUSIwithSIOUXSockets);
  GUSISetup(GUSIwithInternetSockets);
#endif

  SetDebugLevel(( 0 ));
  OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION , "Tests DcmItem/DcmSequenceOfItem's path access features", rcsid);
  OFCommandLine cmd;

  cmd.addGroup("general options:");
   cmd.addOption("--help",      "-h", "print this help text and exit", OFCommandLine::AF_Exclusive);
   cmd.addOption("--version",         "print version information and exit", OFCommandLine::AF_Exclusive);
   cmd.addOption("--arguments",       "print expanded command line arguments");
   cmd.addOption("--verbose",   "-v", "verbose mode, print processing details");
   cmd.addOption("--debug",     "-d", "debug mode, print debug information");

  /* evaluate command line */
  prepareCmdLineArgs(argc, argv, OFFIS_CONSOLE_APPLICATION);
  if (app.parseCommandLine(cmd, argc, argv, OFCommandLine::PF_ExpandWildcards))
  {
    /* check whether to print the command line arguments */
    if (cmd.findOption("--arguments"))
      app.printArguments();

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

    /* command line parameters */

    if (cmd.findOption("--verbose")) opt_verbose = OFTrue;
    if (cmd.findOption("--debug")) opt_debugMode = 5;
  }

  if (opt_debugMode)
      app.printIdentifier();
  SetDebugLevel((opt_debugMode));

  /* make sure data dictionary is loaded */
  if (!dcmDataDict.isDictionaryLoaded())
  {
      CERR << "Warning: no data dictionary loaded, "
           << "check environment variable: "
           << DCM_DICT_ENVIRONMENT_VARIABLE << OFendl;
  }

  DcmFileFormat dcmff;
  DcmDataset *dset = dcmff.getDataset();
  OFString path;
  const OFString precalculatedDump = "# Dicom-Data-Set\n# Used TransferSyntax: Unknown Transfer Syntax\n(0040,a730) SQ (Sequence with explicit length #=6)      #   0, 1 ContentSequence\n  (fffe,e000) na (Item with explicit length #=0)          #   0, 1 Item\n  (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n  (fffe,e000) na (Item with explicit length #=0)          #   0, 1 Item\n  (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n  (fffe,e000) na (Item with explicit length #=0)          #   0, 1 Item\n  (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n  (fffe,e000) na (Item with explicit length #=0)          #   0, 1 Item\n  (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n  (fffe,e000) na (Item with explicit length #=0)          #   0, 1 Item\n  (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n  (fffe,e000) na (Item with explicit length #=1)          #   0, 1 Item\n    (0040,a730) SQ (Sequence with explicit length #=4)      #   0, 1 ContentSequence\n      (fffe,e000) na (Item with explicit length #=0)          #   0, 1 Item\n      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n      (fffe,e000) na (Item with explicit length #=0)          #   0, 1 Item\n      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n      (fffe,e000) na (Item with explicit length #=0)          #   0, 1 Item\n      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n      (fffe,e000) na (Item with explicit length #=1)          #   0, 1 Item\n        (0040,a043) SQ (Sequence with explicit length #=1)      #   0, 1 ConceptNameCodeSequence\n          (fffe,e000) na (Item with explicit length #=2)          #   0, 1 Item\n            (0008,0100) SH (no value available)                     #   0, 0 CodeValue\n            (0008,0104) LO (no value available)                     #   0, 0 CodeMeaning\n          (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n        (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n    (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n  (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n(fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
  const OFString precalculatedDump2 = "# Dicom-Data-Set\n# Used TransferSyntax: Unknown Transfer Syntax\n(0010,0020) LO (no value available)                     #   0, 0 PatientID\n(0040,a730) SQ (Sequence with explicit length #=6)      #   0, 1 ContentSequence\n  (fffe,e000) na (Item with explicit length #=1)          #   0, 1 Item\n    (0040,a730) SQ (Sequence with explicit length #=1)      #   0, 1 ContentSequence\n      (fffe,e000) na (Item with explicit length #=1)          #   0, 1 Item\n        (0010,0020) LO (no value available)                     #   0, 0 PatientID\n      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n    (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n  (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n  (fffe,e000) na (Item with explicit length #=1)          #   0, 1 Item\n    (0040,a730) SQ (Sequence with explicit length #=1)      #   0, 1 ContentSequence\n      (fffe,e000) na (Item with explicit length #=1)          #   0, 1 Item\n        (0010,0020) LO (no value available)                     #   0, 0 PatientID\n      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n    (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n  (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n  (fffe,e000) na (Item with explicit length #=1)          #   0, 1 Item\n    (0040,a730) SQ (Sequence with explicit length #=1)      #   0, 1 ContentSequence\n      (fffe,e000) na (Item with explicit length #=1)          #   0, 1 Item\n        (0010,0020) LO (no value available)                     #   0, 0 PatientID\n      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n    (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n  (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n  (fffe,e000) na (Item with explicit length #=1)          #   0, 1 Item\n    (0040,a730) SQ (Sequence with explicit length #=1)      #   0, 1 ContentSequence\n      (fffe,e000) na (Item with explicit length #=1)          #   0, 1 Item\n        (0010,0020) LO (no value available)                     #   0, 0 PatientID\n      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n    (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n  (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n  (fffe,e000) na (Item with explicit length #=1)          #   0, 1 Item\n    (0040,a730) SQ (Sequence with explicit length #=1)      #   0, 1 ContentSequence\n      (fffe,e000) na (Item with explicit length #=1)          #   0, 1 Item\n        (0010,0020) LO (no value available)                     #   0, 0 PatientID\n      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n    (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n  (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n  (fffe,e000) na (Item with explicit length #=1)          #   0, 1 Item\n    (0040,a730) SQ (Sequence with explicit length #=4)      #   0, 1 ContentSequence\n      (fffe,e000) na (Item with explicit length #=2)          #   0, 1 Item\n        (0010,0010) PN (no value available)                     #   0, 0 PatientsName\n        (0010,0020) LO (no value available)                     #   0, 0 PatientID\n      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n      (fffe,e000) na (Item with explicit length #=1)          #   0, 1 Item\n        (0010,0010) PN (no value available)                     #   0, 0 PatientsName\n      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n      (fffe,e000) na (Item with explicit length #=1)          #   0, 1 Item\n        (0010,0010) PN (no value available)                     #   0, 0 PatientsName\n      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n      (fffe,e000) na (Item with explicit length #=2)          #   0, 1 Item\n        (0010,0010) PN (no value available)                     #   0, 0 PatientsName\n        (0040,a043) SQ (Sequence with explicit length #=1)      #   0, 1 ConceptNameCodeSequence\n          (fffe,e000) na (Item with explicit length #=2)          #   0, 1 Item\n            (0008,0100) SH (no value available)                     #   0, 0 CodeValue\n            (0008,0104) LO (no value available)                     #   0, 0 CodeMeaning\n          (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n        (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n    (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n  (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n(fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
  const Uint32 precalculatedLength = 148;
  const Uint32 precalculatedLength2 = 328;

  /* ********************************************************************* */
  /* Test insertions using no wildcards                                    */
  /* ********************************************************************* */

  if (opt_verbose) COUT << "These insertions should work:" << OFendl;
  if (opt_verbose) COUT << "=============================" << OFendl;
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

  if (opt_verbose) COUT << OFendl << "These insertions should NOT work (wrong syntax):" << OFendl;
  if (opt_verbose) COUT           << "================================================" << OFendl;

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

  if (opt_verbose) COUT << OFendl << "These find routines should work:" << OFendl;
  if (opt_verbose) COUT           << "================================" << OFendl;

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

  if (opt_verbose) COUT << OFendl << "These find routines should NOT work:" << OFendl;
  if (opt_verbose) COUT           << "====================================" << OFendl;

  path = "PatientsName"; // was never inserted
  testPathInsertionsWithoutWildcard(path, dset, 1, OFTrue, OFFalse /* do not create */);
  path = "(0040,A730)[6]"; // we only have 6 items in there (not 7)
  testPathInsertionsWithoutWildcard(path, dset, 2, OFTrue, OFFalse /* do not create */);
  path = "ConceptNameCodeSequence"; // should not exist on main level
  testPathInsertionsWithoutWildcard(path, dset, 1, OFTrue, OFFalse /* do not create */);

  if (opt_verbose) COUT << OFendl << OFendl << "Checking dataset length:" << OFendl;
  if (opt_verbose) COUT           << "====================================" << OFendl;
  Uint32 length = dset->calcElementLength(EXS_LittleEndianExplicit,EET_ExplicitLength);
  if (opt_verbose) COUT << "Checking whether length of encoded dataset matches pre-calculated length";
  if (length == precalculatedLength)
  {
    if (opt_verbose)
      COUT << " ...OK" << OFendl;
  }
  else
  {
    CERR << " ...FAILED: Length is " << length << ". Should be " << precalculatedLength << OFendl;
    CERR << "Please check dump and adapt test in case of false alarm:" << OFendl;
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

  if (opt_verbose) COUT << OFendl << OFendl << "These wildcard insertions should work:" << OFendl;
  if (opt_verbose) COUT           << "====================================" << OFendl;
  path = "ContentSequence[*].ContentSequence[*].PatientsName";
  testPathInsertionsWithWildcard(path, dset, 4, OFFalse /* should work*/, OFTrue /*do create*/);
  path = "ContentSequence[*].ContentSequence[0].PatientID";
  testPathInsertionsWithWildcard(path, dset, 6, OFFalse /* should work*/, OFTrue /*do create*/);

  if (opt_verbose) COUT << OFendl << OFendl << "Testing wildcard insertions should NOT work:" << OFendl;
  if (opt_verbose) COUT           << "====================================" << OFendl;
  path = "SourceImageSequence[*]";
  testPathInsertionsWithWildcard(path, dset, 0, OFTrue /* should fail*/, OFTrue /*do create*/);
  path = "ContentSequence[*].SourceImageSequence[*]";
  testPathInsertionsWithWildcard(path, dset, 0, OFTrue /* should fail*/, OFTrue /*do create*/);

  if (opt_verbose) COUT << OFendl << OFendl << "Checking dataset length:" << OFendl;
  if (opt_verbose) COUT           << "====================================" << OFendl;
  length = dset->calcElementLength(EXS_LittleEndianExplicit,EET_ExplicitLength);
  if (opt_verbose) COUT << "Checking whether length of encoded dataset matches pre-calculated length";
  if (length == precalculatedLength2)
  {
    if (opt_verbose)
      COUT << " ...OK" << OFendl;
  }
  else
  {
    CERR << " ...FAILED: Length is " << length << ". Should be " << precalculatedLength << OFendl;
    CERR << "Please check dump and adapt test in case of false alarm:" << OFendl;
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
