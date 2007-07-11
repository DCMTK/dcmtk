/*
 *
 *  Copyright (C) 1994-2007, OFFIS
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
 *  Author:  Marco Eichelberg
 *
 *  Purpose: Test application for partial element access API
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2007-07-11 10:42:28 $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#include "dcmtk/ofstd/ofstdinc.h"

#ifdef HAVE_GUSI_H
#include <GUSI.h>
#endif

#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmdata/dcdebug.h"
#include "dcmtk/dcmdata/cmdlnarg.h"
#include "dcmtk/ofstd/ofconapp.h"
#include "dcmtk/dcmdata/dcuid.h"       /* for dcmtk version name */
#include "dcmtk/dcmdata/dcostrmz.h"    /* for dcmZlibCompressionLevel */
#include "dcmtk/dcmdata/dcistrmz.h"    /* for dcmZlibExpectRFC1950Encoding */
#include "dcmtk/dcmdata/dcfcache.h"

#ifdef WITH_ZLIB
#include <zlib.h>        /* for zlibVersion() */
#endif

#define OFFIS_CONSOLE_APPLICATION "tstpread"

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

// ********************************************

#define BUFSIZE 32768

void createTestDataset(DcmDataset *dset, unsigned char *buffer)
{
  // byte array, VR=OB
  dset->putAndInsertUint8Array(DCM_EncapsulatedDocument, buffer, BUFSIZE);

  // short array, VR=US
  dset->putAndInsertUint16Array(DCM_RWavePointer, (Uint16 *)buffer, BUFSIZE/sizeof(Uint16));

  // long array, VR=UL
  DcmElement *elem = new DcmUnsignedLong(DCM_TableOfPixelValues);
  elem->putUint32Array((Uint32 *)buffer, BUFSIZE/sizeof(Uint32));
  dset->insert(elem);

  // double array, VR=FD
  elem = new DcmFloatingPointDouble(DCM_TableOfYBreakPoints);
  elem->putFloat64Array((Float64 *)buffer, BUFSIZE/sizeof(Float64));
  dset->insert(elem);
}

OFCondition sequentialNonOverlappingRead(DcmElement *delem, DcmFileCache *dcache, unsigned char *buffer)
{
    unsigned char *target = new unsigned char[BUFSIZE];
    Uint32 offset = 0;
    Uint32 bytes_to_read = 0;
    OFCondition cond;

    while (offset < BUFSIZE)
    {
      bytes_to_read = (rand() % 20)+1;  // read 1 to 20 bytes

      // make sure we don't attempt to read beyond the end of the attribute value
      if (offset + bytes_to_read > BUFSIZE) bytes_to_read = BUFSIZE - offset;

      cond = delem->getPartialValue(target, offset, bytes_to_read, dcache);
      if (cond.bad())
      {
        delete[] target;
        return cond;
      }

      if (0 != memcmp(target, buffer+offset, bytes_to_read))
      {
        CERR << "Error: unexpected sequence of " << bytes_to_read << " bytes found at offset " << offset << OFendl;
        CERR << "Expected: ";
        for (Uint32 i=0; i<bytes_to_read; ++i)
        {
          CERR << std::hex << (int) (buffer[offset+i]) << " ";
        }
        CERR << OFendl << "Found   : ";
        for (Uint32 i=0; i<bytes_to_read; ++i)
        {
          CERR << std::hex << (int) (target[i]) << " ";
        }
        CERR << OFendl;
        delete[] target;
        return EC_IllegalCall;
      }

      offset += bytes_to_read;
    }
    delete[] target;
    return EC_Normal;
}

OFCondition sequentialOverlappingRead(DcmElement *delem, DcmFileCache *dcache, unsigned char *buffer)
{
    unsigned char *target = new unsigned char[BUFSIZE];
    Uint32 offset = 0;
    Uint32 bytes_to_read = 0;
    OFCondition cond;

    while (offset < BUFSIZE)
    {
      bytes_to_read = (rand() % 20)+1;  // read 1 to 20 bytes

      // make sure we don't attempt to read beyond the end of the attribute value
      if (offset + bytes_to_read > BUFSIZE) bytes_to_read = BUFSIZE - offset;

      cond = delem->getPartialValue(target, offset, bytes_to_read, dcache);
      if (cond.bad())
      {
        delete[] target;
        return cond;
      }

      if (0 != memcmp(target, buffer+offset, bytes_to_read))
      {
        CERR << "Error: unexpected sequence of " << bytes_to_read << " bytes found at offset " << offset << OFendl;
        CERR << "Expected: ";
        for (Uint32 i=0; i<bytes_to_read; ++i)
        {
          CERR << std::hex << (int) (buffer[offset+i]) << " ";
        }
        CERR << OFendl << "Found   : ";
        for (Uint32 i=0; i<bytes_to_read; ++i)
        {
          CERR << std::hex << (int) (target[i]) << " ";
        }
        CERR << OFendl;
        delete[] target;
        return EC_IllegalCall;
      }

      offset += bytes_to_read;
      if (offset > 4) offset -= (rand() % 4); // let the read operations overlap by 0-3 bytes
    }
    delete[] target;
    return EC_Normal;
}

OFCondition randomRead(DcmElement *delem, DcmFileCache *dcache, unsigned char *buffer)
{
    unsigned char *target = new unsigned char[BUFSIZE];
    Uint32 offset = 0;
    Uint32 bytes_to_read = 0;
    OFCondition cond;

    for (int i=1000; i; --i)
    {
      bytes_to_read = (rand() % 20)+1;  // read 1 to 20 bytes
      offset = rand() % BUFSIZE;

      // make sure we don't attempt to read beyond the end of the attribute value
      if (offset + bytes_to_read > BUFSIZE) bytes_to_read = BUFSIZE - offset;

      cond = delem->getPartialValue(target, offset, bytes_to_read, dcache);
      if (cond.bad())
      {
        delete[] target;
        return cond;
      }

      if (0 != memcmp(target, buffer+offset, bytes_to_read))
      {
        CERR << "Error: unexpected sequence of " << bytes_to_read << " bytes found at offset " << offset << OFendl;
        CERR << "Expected: ";
        for (Uint32 i=0; i<bytes_to_read; ++i)
        {
          CERR << std::hex << (int) (buffer[offset+i]) << " ";
        }
        CERR << OFendl << "Found   : ";
        for (Uint32 i=0; i<bytes_to_read; ++i)
        {
          CERR << std::hex << (int) (target[i]) << " ";
        }
        CERR << OFendl;
        delete[] target;
        return EC_IllegalCall;
      }

      offset += bytes_to_read;
    }
    delete[] target;
    return EC_Normal;
}

OFCondition sequentialNonOverlappingRead(DcmDataset *dset, unsigned char *buffer)
{
  DcmFileCache cache;
  DcmElement *delem = NULL;
  OFCondition cond;

  cond = dset->findAndGetElement(DCM_EncapsulatedDocument, delem);
  if (cond.bad()) return cond;

  cond = sequentialNonOverlappingRead(delem, &cache, buffer);
  if (cond.bad()) return cond;

  cond = dset->findAndGetElement(DCM_RWavePointer, delem);
  if (cond.bad()) return cond;

  cond = sequentialNonOverlappingRead(delem, &cache, buffer);
  if (cond.bad()) return cond;

  cond = dset->findAndGetElement(DCM_TableOfPixelValues, delem);
  if (cond.bad()) return cond;

  cond = sequentialNonOverlappingRead(delem, &cache, buffer);
  if (cond.bad()) return cond;

  cond = dset->findAndGetElement(DCM_TableOfYBreakPoints, delem);
  if (cond.bad()) return cond;

  cond = sequentialNonOverlappingRead(delem, &cache, buffer);

  return cond;
}

OFCondition sequentialOverlappingRead(DcmDataset *dset, unsigned char *buffer)
{
  DcmFileCache cache;
  DcmElement *delem = NULL;
  OFCondition cond;

  cond = dset->findAndGetElement(DCM_EncapsulatedDocument, delem);
  if (cond.bad()) return cond;

  cond = sequentialOverlappingRead(delem, &cache, buffer);
  if (cond.bad()) return cond;

  cond = dset->findAndGetElement(DCM_RWavePointer, delem);
  if (cond.bad()) return cond;

  cond = sequentialOverlappingRead(delem, &cache, buffer);
  if (cond.bad()) return cond;

  cond = dset->findAndGetElement(DCM_TableOfPixelValues, delem);
  if (cond.bad()) return cond;

  cond = sequentialOverlappingRead(delem, &cache, buffer);
  if (cond.bad()) return cond;

  cond = dset->findAndGetElement(DCM_TableOfYBreakPoints, delem);
  if (cond.bad()) return cond;

  cond = sequentialOverlappingRead(delem, &cache, buffer);

  return cond;
}

OFCondition randomRead(DcmDataset *dset, unsigned char *buffer)
{
  DcmFileCache cache;
  DcmElement *delem = NULL;
  OFCondition cond;

  cond = dset->findAndGetElement(DCM_EncapsulatedDocument, delem);
  if (cond.bad()) return cond;

  cond = randomRead(delem, &cache, buffer);
  if (cond.bad()) return cond;

  cond = dset->findAndGetElement(DCM_RWavePointer, delem);
  if (cond.bad()) return cond;

  cond = randomRead(delem, &cache, buffer);
  if (cond.bad()) return cond;

  cond = dset->findAndGetElement(DCM_TableOfPixelValues, delem);
  if (cond.bad()) return cond;

  cond = randomRead(delem, &cache, buffer);
  if (cond.bad()) return cond;

  cond = dset->findAndGetElement(DCM_TableOfYBreakPoints, delem);
  if (cond.bad()) return cond;

  cond = randomRead(delem, &cache, buffer);

  return cond;
}

int main(int argc, char *argv[])
{

#ifdef HAVE_GUSI_H
  GUSISetup(GUSIwithSIOUXSockets);
  GUSISetup(GUSIwithInternetSockets);
#endif

  SetDebugLevel(( 0 ));
  int opt_debugMode = 0;
  OFBool opt_verbose = OFFalse;

  OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION , "Convert DICOM file encoding", rcsid);
  OFCommandLine cmd;

  cmd.addGroup("general options:");
   cmd.addOption("--help",    "-h", "print this help text and exit", OFCommandLine::AF_Exclusive);
   cmd.addOption("--version",       "print version information and exit", OFCommandLine::AF_Exclusive);
   cmd.addOption("--verbose", "-v", "verbose mode, print processing details");
   cmd.addOption("--debug",   "-d", "debug mode, print debug information");

    /* evaluate command line */
    prepareCmdLineArgs(argc, argv, OFFIS_CONSOLE_APPLICATION);
    if (app.parseCommandLine(cmd, argc, argv, OFCommandLine::PF_ExpandWildcards))
    {
      /* check exclusive options first */

      if (cmd.hasExclusiveOption())
      {
        if (cmd.findOption("--version"))
        {
            app.printHeader(OFTrue /*print host identifier*/);          // uses ofConsole.lockCerr()
            CERR << OFendl << "External libraries used:";
#ifdef WITH_ZLIB
            CERR << OFendl << "- ZLIB, Version " << zlibVersion() << OFendl;
#else
            CERR << " none" << OFendl;
#endif
            return 0;
         }
      }

      /* command line parameters */

      if (cmd.findOption("--verbose")) opt_verbose = OFTrue;
      if (cmd.findOption("--debug")) opt_debugMode = 5;
    }

    SetDebugLevel((opt_debugMode));

    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded())
    {
        CERR << "Warning: no data dictionary loaded, "
             << "check environment variable: "
             << DCM_DICT_ENVIRONMENT_VARIABLE << OFendl;
    }

    if (opt_verbose)
        COUT << "Creating test dataset" << OFendl;

    DcmFileFormat dfile;

    unsigned char *buffer = new unsigned char[BUFSIZE];
    unsigned char *bufptr = buffer;
    srand(time(NULL));
    for (int i = BUFSIZE; i; --i)
    {
      *bufptr++ = (unsigned char) rand();
    }

    createTestDataset(dfile.getDataset(), buffer);
    OFCondition cond = EC_Normal;

    if (opt_verbose)
        COUT << "Writing test files" << OFendl;

    cond = dfile.saveFile("test_be.dcm", EXS_BigEndianExplicit);
    if (cond.bad()) { CERR << "Error: " << cond.text() << OFendl; return 10; }
    cond = dfile.saveFile("test_le.dcm", EXS_LittleEndianExplicit);
    if (cond.bad()) { CERR << "Error: " << cond.text() << OFendl; return 10; }
#ifdef WITH_ZLIB
    cond = dfile.saveFile("test_df.dcm", EXS_DeflatedLittleEndianExplicit);
    if (cond.bad()) { CERR << "Error: " << cond.text() << OFendl; return 10; }
#endif

    if (opt_verbose)
        COUT << "Opening test files" << OFendl;

    DcmFileFormat dfile_be;
    DcmFileFormat dfile_le;
    DcmFileFormat dfile_df;

    cond = dfile_be.loadFile("test_be.dcm");
    if (cond.bad()) { CERR << "Error: " << cond.text() << OFendl; return 10; }

    cond = dfile_le.loadFile("test_le.dcm");
    if (cond.bad()) { CERR << "Error: " << cond.text() << OFendl; return 10; }

#ifdef WITH_ZLIB
    cond = dfile_df.loadFile("test_df.dcm");
    if (cond.bad()) { CERR << "Error: " << cond.text() << OFendl; return 10; }
#endif

    // testing sequential, non overlapping reads of partial element values
    if (opt_verbose)
        COUT << "Testing sequential, non overlapping reads of partial element values" << OFendl;

    cond = sequentialNonOverlappingRead(dfile_be.getDataset(), buffer);
    if (cond.bad()) { CERR << "Error: " << cond.text() << OFendl; return 10; }

    cond = sequentialNonOverlappingRead(dfile_le.getDataset(), buffer);
    if (cond.bad()) { CERR << "Error: " << cond.text() << OFendl; return 10; }

#ifdef WITH_ZLIB
    cond = sequentialNonOverlappingRead(dfile_df.getDataset(), buffer);
    if (cond.bad()) { CERR << "Error: " << cond.text() << OFendl; return 10; }
#endif

    // testing random reads of partial element values
    if (opt_verbose)
        COUT << "Testing random reads of partial element values" << OFendl;

    cond = randomRead(dfile_be.getDataset(), buffer);
    if (cond.bad()) { CERR << "Error: " << cond.text() << OFendl; return 10; }

    cond = randomRead(dfile_le.getDataset(), buffer);
    if (cond.bad()) { CERR << "Error: " << cond.text() << OFendl; return 10; }

#ifdef WITH_ZLIB
    cond = randomRead(dfile_df.getDataset(), buffer);
    if (cond.bad()) { CERR << "Error: " << cond.text() << OFendl; return 10; }
#endif

    // testing overlapping reads of partial element values
    if (opt_verbose)
        COUT << "Testing overlapping reads of partial element values" << OFendl;

    cond = sequentialOverlappingRead(dfile_be.getDataset(), buffer);
    if (cond.bad()) { CERR << "Error: " << cond.text() << OFendl; return 10; }

    cond = sequentialOverlappingRead(dfile_le.getDataset(), buffer);
    if (cond.bad()) { CERR << "Error: " << cond.text() << OFendl; return 10; }

#ifdef WITH_ZLIB
    cond = sequentialOverlappingRead(dfile_df.getDataset(), buffer);
    if (cond.bad()) { CERR << "Error: " << cond.text() << OFendl; return 10; }
#endif

    delete[] buffer;
    return 0;
}


/*
** CVS/RCS Log:
** $Log: tstpread.cc,v $
** Revision 1.2  2007-07-11 10:42:28  joergr
** Fixed layout and other minor issues of the usage output (--help).
**
** Revision 1.1  2007/07/11 08:52:22  meichel
** Added regression test for new method DcmElement::getPartialValue.
**
**
*/
