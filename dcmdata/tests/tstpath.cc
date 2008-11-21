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
 *  Update Date:      $Date: 2008-11-21 16:18:32 $
 *  CVS/RCS Revision: $Revision: 1.3 $
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

static void testPathInsertion(const OFString& path,
                              DcmDataset *dset,
                              const Uint16& expectedNumObjects,
                              const OFBool& expectFailed = OFFalse,
                              const OFBool& createIfNecessary = OFTrue)
{
  OFBool failed = OFFalse;
  OFList<DcmObject*> objList;
  if (opt_verbose) COUT << path; COUT.flush();
  OFCondition result = dset->findOrCreatePath(path, objList, createIfNecessary);
  if (result.bad())
  {
    if (!expectFailed || opt_verbose) CERR << " ...FAILED! Path " << (createIfNecessary ? "insertion" : "lookup") << " failed: " << result.text() << OFendl;
    failed = OFTrue;
  }
  else
  {
    if (objList.size() != expectedNumObjects)
    {
      if (!expectFailed || opt_verbose) CERR << "...FAILED! Returned object list does not contain " << expectedNumObjects << " but only " << objList.size() << "elements" << OFendl;
      failed = OFTrue;
    }
    else
    {
      OFListIterator(DcmObject*) it = objList.begin();
      for (Uint16 i=0; i < expectedNumObjects; i++)
      {
        if (*it == NULL)
        {
          if (!expectFailed || opt_verbose) CERR << " ...FAILED! Path insertion failed: One of the created objects is NULL" << OFendl;
          failed = OFTrue;
        }
        it++;
      }
      if (failed == expectFailed)
      {
        if (opt_verbose)
          COUT << "...OK" << OFendl;
      }
    }
  }
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
  const Uint32 precalculatedLength = 148;

  if (opt_verbose) COUT << "These insertions should work:" << OFendl;
  if (opt_verbose) COUT << "=============================" << OFendl;
  path = "PatientID";
  testPathInsertion(path, dset, 1);
  path = "ContentSequence";
  testPathInsertion(path, dset, 1);
  path = "(0040,A730)";
  testPathInsertion(path, dset, 1);
  path = "(0040,A730)[5]";
  testPathInsertion(path, dset, 2);
  path = "(0040,A730)[5].ContentSequence[3].ConceptNameCodeSequence[0].CodeValue";
  testPathInsertion(path, dset, 7);
  path = "ContentSequence[5].ContentSequence[3].ConceptNameCodeSequence[0].(0008,0104)";
  testPathInsertion(path, dset, 7);

  if (opt_verbose) COUT << OFendl << "These insertions should NOT work (wrong syntax):" << OFendl;
  if (opt_verbose) COUT           << "================================================" << OFendl;

  path = "ContentSequences";
  testPathInsertion(path, dset, 1, OFTrue);
  path = "(1234,ContentSequences";
  testPathInsertion(path, dset, 1, OFTrue);
  path = "(00X4,A730)";
  testPathInsertion(path, dset, 1, OFTrue);
  path = "(0040,A730)[-5]";
  testPathInsertion(path, dset, 2, OFTrue);
  path = "(0040,A730)[a].ContentSequence[3].ConceptNameCodeSequence[0].CodeValue";
  testPathInsertion(path, dset, 7, OFTrue);

  if (opt_verbose) COUT << OFendl << "These find routines should work:" << OFendl;
  if (opt_verbose) COUT           << "================================" << OFendl;

  path = "PatientID";
  testPathInsertion(path, dset, 1, OFFalse, OFFalse /* do not create */);
  path = "ContentSequence";
  testPathInsertion(path, dset, 1, OFFalse, OFFalse /* do not create */);
  path = "(0040,A730)";
  testPathInsertion(path, dset, 1, OFFalse, OFFalse /* do not create */);
  path = "(0040,A730)[5]";
  testPathInsertion(path, dset, 2, OFFalse, OFFalse /* do not create */);
  path = "(0040,A730)[5].ContentSequence[3].ConceptNameCodeSequence[0].CodeValue";
  testPathInsertion(path, dset, 7, OFFalse, OFFalse /* do not create */);
  path = "ContentSequence[5].ContentSequence[3].ConceptNameCodeSequence[0].(0008,0104)";
  testPathInsertion(path, dset, 7, OFFalse, OFFalse /* do not create */);

  if (opt_verbose) COUT << OFendl << "These find routines should NOT work:" << OFendl;
  if (opt_verbose) COUT           << "====================================" << OFendl;

  path = "PatientsName"; // was never inserted
  testPathInsertion(path, dset, 1, OFTrue, OFFalse /* do not create */);
  path = "(0040,A730)[6]"; // we only have 6 items in there (not 7)
  testPathInsertion(path, dset, 2, OFTrue, OFFalse /* do not create */);
  path = "ConceptNameCodeSequence"; // should not exist on main level
  testPathInsertion(path, dset, 1, OFTrue, OFFalse /* do not create */);

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
  }
  return 0;
}

/*
 * CVS/RCS Log:
 * $Log: tstpath.cc,v $
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
