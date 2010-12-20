/*
 *
 *  Copyright (C) 1994-2010, OFFIS e.V.
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
 *  Author:  Marco Eichelberg
 *
 *  Purpose: Test application for partial element access API
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-12-20 11:19:40 $
 *  CVS/RCS Revision: $Revision: 1.10 $
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

static OFLogger tstpreadLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION);

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

// ********************************************

#define BUFSIZE 32768

void createTestDataset(DcmDataset *dset, unsigned char *buffer)
{
  // byte array, VR=OB
  dset->putAndInsertUint8Array(DCM_EncapsulatedDocument, buffer, BUFSIZE);

  // short array, VR=US
  dset->putAndInsertUint16Array(DCM_RWavePointer, OFreinterpret_cast(Uint16 *, buffer), BUFSIZE/sizeof(Uint16));

  // long array, VR=UL
  DcmElement *elem = new DcmUnsignedLong(DCM_TableOfPixelValues);
  elem->putUint32Array(OFreinterpret_cast(Uint32 *, buffer), BUFSIZE/sizeof(Uint32));
  dset->insert(elem);

  // double array, VR=FD
  elem = new DcmFloatingPointDouble(DCM_TableOfYBreakPoints);
  elem->putFloat64Array(OFreinterpret_cast(Float64 *, buffer), BUFSIZE/sizeof(Float64));
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
        OFOStringStream str;
        str << "Error: unexpected sequence of " << bytes_to_read << " bytes found at offset " << offset << OFendl;
        str << "Expected: ";
        for (Uint32 i=0; i<bytes_to_read; ++i)
        {
          str << STD_NAMESPACE hex << OFstatic_cast(int, buffer[offset+i]) << " ";
        }
        str << OFendl << "Found   : ";
        for (Uint32 j=0; j<bytes_to_read; ++j)
        {
          str << STD_NAMESPACE hex << OFstatic_cast(int, target[j]) << " ";
        }
        str << OFStringStream_ends;

        OFSTRINGSTREAM_GETSTR(str, c_str)
        OFLOG_ERROR(tstpreadLogger, c_str);
        OFSTRINGSTREAM_FREESTR(c_str)
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
        OFOStringStream str;
        str << "Error: unexpected sequence of " << bytes_to_read << " bytes found at offset " << offset << OFendl;
        str << "Expected: ";
        for (Uint32 i=0; i<bytes_to_read; ++i)
        {
          str << STD_NAMESPACE hex << OFstatic_cast(int, buffer[offset+i]) << " ";
        }
        str << OFendl << "Found   : ";
        for (Uint32 j=0; j<bytes_to_read; ++j)
        {
          str << STD_NAMESPACE hex << OFstatic_cast(int, target[j]) << " ";
        }
        str << OFStringStream_ends;
        OFSTRINGSTREAM_GETSTR(str, c_str)
        OFLOG_ERROR(tstpreadLogger, c_str);
        OFSTRINGSTREAM_FREESTR(c_str)
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
        OFOStringStream str;
        str << "Error: unexpected sequence of " << bytes_to_read << " bytes found at offset " << offset << OFendl;
        str << "Expected: ";
        for (Uint32 k=0; k<bytes_to_read; ++k)
        {
          str << STD_NAMESPACE hex << OFstatic_cast(int, buffer[offset+k]) << " ";
        }
        str << OFendl << "Found   : ";
        for (Uint32 j=0; j<bytes_to_read; ++j)
        {
          str << STD_NAMESPACE hex << OFstatic_cast(int, target[j]) << " ";
        }
        str << OFStringStream_ends;
        OFSTRINGSTREAM_GETSTR(str, c_str)
        OFLOG_ERROR(tstpreadLogger, c_str);
        OFSTRINGSTREAM_FREESTR(c_str)
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

  OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION , "Test partial element access API", rcsid);
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
#ifdef WITH_ZLIB
          COUT << OFendl << "- ZLIB, Version " << zlibVersion() << OFendl;
#else
          COUT << " none" << OFendl;
#endif
          return 0;
        }
      }

      /* command line parameters */
      OFLog::configureFromCommandLine(cmd, app);
    }

    /* print resource identifier */
    OFLOG_DEBUG(tstpreadLogger, rcsid << OFendl);

    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded())
    {
      OFLOG_WARN(tstpreadLogger, "no data dictionary loaded, check environment variable: "
        << DCM_DICT_ENVIRONMENT_VARIABLE);
    }

    OFLOG_INFO(tstpreadLogger, "Creating test dataset");

    DcmFileFormat dfile;

    unsigned char *buffer = new unsigned char[BUFSIZE];
    unsigned char *bufptr = buffer;
    srand(OFstatic_cast(unsigned int, time(NULL)));
    for (int i = BUFSIZE; i; --i)
    {
      *bufptr++ = OFstatic_cast(unsigned char, rand());
    }

    createTestDataset(dfile.getDataset(), buffer);
    OFCondition cond = EC_Normal;

    OFLOG_INFO(tstpreadLogger, "Writing test files");

    cond = dfile.saveFile("test_be.dcm", EXS_BigEndianExplicit);
    if (cond.bad()) { OFLOG_FATAL(tstpreadLogger, cond.text()); return 10; }
    cond = dfile.saveFile("test_le.dcm", EXS_LittleEndianExplicit);
    if (cond.bad()) { OFLOG_FATAL(tstpreadLogger, cond.text()); return 10; }
#ifdef WITH_ZLIB
    cond = dfile.saveFile("test_df.dcm", EXS_DeflatedLittleEndianExplicit);
    if (cond.bad()) { OFLOG_FATAL(tstpreadLogger, cond.text()); return 10; }
#endif

    OFLOG_INFO(tstpreadLogger, "Opening test files");

    DcmFileFormat dfile_be;
    DcmFileFormat dfile_le;
    DcmFileFormat dfile_df;

    cond = dfile_be.loadFile("test_be.dcm");
    if (cond.bad()) { OFLOG_FATAL(tstpreadLogger, cond.text()); return 10; }

    cond = dfile_le.loadFile("test_le.dcm");
    if (cond.bad()) { OFLOG_FATAL(tstpreadLogger, cond.text()); return 10; }

#ifdef WITH_ZLIB
    cond = dfile_df.loadFile("test_df.dcm");
    if (cond.bad()) { OFLOG_FATAL(tstpreadLogger, cond.text()); return 10; }
#endif

    // testing sequential, non overlapping reads of partial element values
    OFLOG_INFO(tstpreadLogger, "Testing sequential, non overlapping reads of partial element values");

    cond = sequentialNonOverlappingRead(dfile_be.getDataset(), buffer);
    if (cond.bad()) { OFLOG_FATAL(tstpreadLogger, cond.text()); return 10; }

    cond = sequentialNonOverlappingRead(dfile_le.getDataset(), buffer);
    if (cond.bad()) { OFLOG_FATAL(tstpreadLogger, cond.text()); return 10; }

#ifdef WITH_ZLIB
    cond = sequentialNonOverlappingRead(dfile_df.getDataset(), buffer);
    if (cond.bad()) { OFLOG_FATAL(tstpreadLogger, cond.text()); return 10; }
#endif

    // testing random reads of partial element values
    OFLOG_INFO(tstpreadLogger, "Testing random reads of partial element values");

    cond = randomRead(dfile_be.getDataset(), buffer);
    if (cond.bad()) { OFLOG_FATAL(tstpreadLogger, cond.text()); return 10; }

    cond = randomRead(dfile_le.getDataset(), buffer);
    if (cond.bad()) { OFLOG_FATAL(tstpreadLogger, cond.text()); return 10; }

#ifdef WITH_ZLIB
    cond = randomRead(dfile_df.getDataset(), buffer);
    if (cond.bad()) { OFLOG_FATAL(tstpreadLogger, cond.text()); return 10; }
#endif

    // testing overlapping reads of partial element values
    OFLOG_INFO(tstpreadLogger, "Testing overlapping reads of partial element values");

    cond = sequentialOverlappingRead(dfile_be.getDataset(), buffer);
    if (cond.bad()) { OFLOG_FATAL(tstpreadLogger, cond.text()); return 10; }

    cond = sequentialOverlappingRead(dfile_le.getDataset(), buffer);
    if (cond.bad()) { OFLOG_FATAL(tstpreadLogger, cond.text()); return 10; }

#ifdef WITH_ZLIB
    cond = sequentialOverlappingRead(dfile_df.getDataset(), buffer);
    if (cond.bad()) { OFLOG_FATAL(tstpreadLogger, cond.text()); return 10; }
#endif

    delete[] buffer;
    return 0;
}


/*
 * CVS/RCS Log:
 * $Log: tstpread.cc,v $
 * Revision 1.10  2010-12-20 11:19:40  joergr
 * Fixed wrong console application description (apparently copied from another
 * tool) and output the resource identifier in debug mode to the logger.
 *
 * Revision 1.9  2010-11-01 10:42:44  uli
 * Fixed some compiler warnings reported by gcc with additional flags.
 *
 * Revision 1.8  2010-10-14 13:15:07  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.7  2010-10-04 12:51:34  joergr
 * Added explicit type cast in order to keep VisualStudio 2008 quiet.
 *
 * Revision 1.6  2010-08-23 07:22:56  meichel
 * Minor changes needed for compilation on MSVC6
 *
 * Revision 1.5  2010-08-05 08:38:10  uli
 * Fixed some warnings from -Wold-style-cast.
 *
 * Revision 1.4  2009-11-04 09:58:11  uli
 * Switched to logging mechanism provided by the "new" oflog module
 *
 * Revision 1.3  2008-09-25 15:43:22  joergr
 * Added support for printing the expanded command line arguments.
 * Always output the resource identifier of the command line tool in debug mode.
 *
 * Revision 1.2  2007/07/11 10:42:28  joergr
 * Fixed layout and other minor issues of the usage output (--help).
 *
 * Revision 1.1  2007/07/11 08:52:22  meichel
 * Added regression test for new method DcmElement::getPartialValue.
 *
 *
 */
