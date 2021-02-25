/*
 *
 *  Copyright (C) 1994-2021, OFFIS e.V.
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
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/ofstd/ofrand.h"
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmdata/dcuid.h"       /* for dcmtk version name */
#include "dcmtk/dcmdata/dcostrmz.h"    /* for dcmZlibCompressionLevel */
#include "dcmtk/dcmdata/dcistrmz.h"    /* for dcmZlibExpectRFC1950Encoding */
#include "dcmtk/dcmdata/dcfcache.h"

#ifdef WITH_ZLIB
#include <zlib.h>        /* for zlibVersion() */
#endif

static OFLogger tstpreadLogger = OFLog::getLogger("dcmtk.test.tstpread");

// ********************************************

#define BUFSIZE 32768

static void createTestDataset(DcmDataset *dset, unsigned char *buffer)
{
  // byte array, VR=OB
  dset->putAndInsertUint8Array(DCM_EncapsulatedDocument, buffer, BUFSIZE);

  // short array, VR=US
  dset->putAndInsertUint16Array(DCM_RWavePointer, OFreinterpret_cast(Uint16 *, buffer), BUFSIZE/OFstatic_cast(Uint32, sizeof(Uint16)));

  // long array, VR=UL
  DcmElement *elem = new DcmUnsignedLong(DCM_TableOfPixelValues);
  elem->putUint32Array(OFreinterpret_cast(Uint32 *, buffer), BUFSIZE/OFstatic_cast(Uint32, sizeof(Uint32)));
  dset->insert(elem);

  // double array, VR=FD
  elem = new DcmFloatingPointDouble(DCM_TableOfYBreakPoints);
  elem->putFloat64Array(OFreinterpret_cast(Float64 *, buffer), BUFSIZE/OFstatic_cast(Uint32, sizeof(Float64)));
  dset->insert(elem);
}

static OFCondition sequentialNonOverlappingRead(OFRandom& rnd, DcmElement *delem, DcmFileCache *dcache, unsigned char *buffer)
{
    unsigned char *target = new unsigned char[BUFSIZE];
    Uint32 offset = 0;
    Uint32 bytes_to_read = 0;
    OFCondition cond;

    while (offset < BUFSIZE)
    {
      bytes_to_read = (rnd.getRND32() % 20)+1;  // read 1 to 20 bytes

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
        OFCHECK_FAIL(c_str);
        OFSTRINGSTREAM_FREESTR(c_str)
        delete[] target;
        return EC_IllegalCall;
      }

      offset += bytes_to_read;
    }
    delete[] target;
    return EC_Normal;
}

static OFCondition sequentialOverlappingRead(OFRandom& rnd, DcmElement *delem, DcmFileCache *dcache, unsigned char *buffer)
{
    unsigned char *target = new unsigned char[BUFSIZE];
    Uint32 offset = 0;
    Uint32 bytes_to_read = 0;
    OFCondition cond;

    while (offset < BUFSIZE)
    {
      bytes_to_read = (rnd.getRND32() % 20)+1;  // read 1 to 20 bytes

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
        OFCHECK_FAIL(c_str);
        OFSTRINGSTREAM_FREESTR(c_str)
        delete[] target;
        return EC_IllegalCall;
      }

      offset += bytes_to_read;
      if ((offset > 4) && (offset < BUFSIZE)) offset -= (rnd.getRND32() % 4); // let the read operations overlap by 0-3 bytes
    }
    delete[] target;
    return EC_Normal;
}

static OFCondition randomRead(OFRandom& rnd, DcmElement *delem, DcmFileCache *dcache, unsigned char *buffer)
{
    unsigned char *target = new unsigned char[BUFSIZE];
    Uint32 offset = 0;
    Uint32 bytes_to_read = 0;
    OFCondition cond;

    for (int i=1000; i; --i)
    {
      bytes_to_read = (rnd.getRND32() % 20)+1;  // read 1 to 20 bytes
      offset = rnd.getRND32() % BUFSIZE;

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
        OFCHECK_FAIL(c_str);
        OFSTRINGSTREAM_FREESTR(c_str)
        delete[] target;
        return EC_IllegalCall;
      }

      offset += bytes_to_read;
    }
    delete[] target;
    return EC_Normal;
}

static OFCondition sequentialNonOverlappingRead(OFRandom& rnd, DcmDataset *dset, unsigned char *buffer)
{
  DcmFileCache cache;
  DcmElement *delem = NULL;
  OFCondition cond;

  cond = dset->findAndGetElement(DCM_EncapsulatedDocument, delem);
  if (cond.bad()) return cond;

  cond = sequentialNonOverlappingRead(rnd, delem, &cache, buffer);
  if (cond.bad()) return cond;

  cond = dset->findAndGetElement(DCM_RWavePointer, delem);
  if (cond.bad()) return cond;

  cond = sequentialNonOverlappingRead(rnd, delem, &cache, buffer);
  if (cond.bad()) return cond;

  cond = dset->findAndGetElement(DCM_TableOfPixelValues, delem);
  if (cond.bad()) return cond;

  cond = sequentialNonOverlappingRead(rnd, delem, &cache, buffer);
  if (cond.bad()) return cond;

  cond = dset->findAndGetElement(DCM_TableOfYBreakPoints, delem);
  if (cond.bad()) return cond;

  cond = sequentialNonOverlappingRead(rnd, delem, &cache, buffer);

  return cond;
}

static OFCondition sequentialOverlappingRead(OFRandom& rnd, DcmDataset *dset, unsigned char *buffer)
{
  DcmFileCache cache;
  DcmElement *delem = NULL;
  OFCondition cond;

  cond = dset->findAndGetElement(DCM_EncapsulatedDocument, delem);
  if (cond.bad()) return cond;

  cond = sequentialOverlappingRead(rnd, delem, &cache, buffer);
  if (cond.bad()) return cond;

  cond = dset->findAndGetElement(DCM_RWavePointer, delem);
  if (cond.bad()) return cond;

  cond = sequentialOverlappingRead(rnd, delem, &cache, buffer);
  if (cond.bad()) return cond;

  cond = dset->findAndGetElement(DCM_TableOfPixelValues, delem);
  if (cond.bad()) return cond;

  cond = sequentialOverlappingRead(rnd, delem, &cache, buffer);
  if (cond.bad()) return cond;

  cond = dset->findAndGetElement(DCM_TableOfYBreakPoints, delem);
  if (cond.bad()) return cond;

  cond = sequentialOverlappingRead(rnd, delem, &cache, buffer);

  return cond;
}

static OFCondition randomRead(OFRandom& rnd, DcmDataset *dset, unsigned char *buffer)
{
  DcmFileCache cache;
  DcmElement *delem = NULL;
  OFCondition cond;

  cond = dset->findAndGetElement(DCM_EncapsulatedDocument, delem);
  if (cond.bad()) return cond;

  cond = randomRead(rnd, delem, &cache, buffer);
  if (cond.bad()) return cond;

  cond = dset->findAndGetElement(DCM_RWavePointer, delem);
  if (cond.bad()) return cond;

  cond = randomRead(rnd, delem, &cache, buffer);
  if (cond.bad()) return cond;

  cond = dset->findAndGetElement(DCM_TableOfPixelValues, delem);
  if (cond.bad()) return cond;

  cond = randomRead(rnd, delem, &cache, buffer);
  if (cond.bad()) return cond;

  cond = dset->findAndGetElement(DCM_TableOfYBreakPoints, delem);
  if (cond.bad()) return cond;

  cond = randomRead(rnd, delem, &cache, buffer);

  return cond;
}

OFTEST(dcmdata_partialElementAccess)
{
    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded())
    {
      OFCHECK_FAIL("no data dictionary loaded, check environment variable: " DCM_DICT_ENVIRONMENT_VARIABLE);
      return;
    }

    OFLOG_DEBUG(tstpreadLogger, "Creating test dataset");

    OFRandom rnd;
    DcmFileFormat dfile;

    unsigned char *buffer = new unsigned char[BUFSIZE];
    unsigned char *bufptr = buffer;
    for (int i = BUFSIZE; i; --i)
    {
      *bufptr++ = OFstatic_cast(unsigned char, rnd.getRND32());
    }

    createTestDataset(dfile.getDataset(), buffer);
    OFCondition cond = EC_Normal;

    OFLOG_DEBUG(tstpreadLogger, "Writing test files");

    cond = dfile.saveFile("test_be.dcm", EXS_BigEndianExplicit);
    if (cond.bad()) { OFCHECK_FAIL(cond.text()); }
    cond = dfile.saveFile("test_le.dcm", EXS_LittleEndianExplicit);
    if (cond.bad()) { OFCHECK_FAIL(cond.text()); }
#ifdef WITH_ZLIB
    cond = dfile.saveFile("test_df.dcm", EXS_DeflatedLittleEndianExplicit);
    if (cond.bad()) { OFCHECK_FAIL(cond.text()); }
#endif

    OFLOG_DEBUG(tstpreadLogger, "Opening test files");

    DcmFileFormat dfile_be;
    DcmFileFormat dfile_le;
    DcmFileFormat dfile_df;

    cond = dfile_be.loadFile("test_be.dcm");
    if (cond.bad()) { OFCHECK_FAIL(cond.text()); }

    cond = dfile_le.loadFile("test_le.dcm");
    if (cond.bad()) { OFCHECK_FAIL(cond.text()); }

#ifdef WITH_ZLIB
    cond = dfile_df.loadFile("test_df.dcm");
    if (cond.bad()) { OFCHECK_FAIL(cond.text()); }
#endif

    // testing sequential, non overlapping reads of partial element values
    OFLOG_DEBUG(tstpreadLogger, "Testing sequential, non overlapping reads of partial element values");

    cond = sequentialNonOverlappingRead(rnd, dfile_be.getDataset(), buffer);
    if (cond.bad()) { OFCHECK_FAIL(cond.text()); }

    cond = sequentialNonOverlappingRead(rnd, dfile_le.getDataset(), buffer);
    if (cond.bad()) { OFCHECK_FAIL(cond.text()); }

#ifdef WITH_ZLIB
    cond = sequentialNonOverlappingRead(rnd, dfile_df.getDataset(), buffer);
    if (cond.bad()) { OFCHECK_FAIL(cond.text()); }
#endif

    // testing random reads of partial element values
    OFLOG_DEBUG(tstpreadLogger, "Testing random reads of partial element values");

    cond = randomRead(rnd, dfile_be.getDataset(), buffer);
    if (cond.bad()) { OFCHECK_FAIL(cond.text()); }

    cond = randomRead(rnd, dfile_le.getDataset(), buffer);
    if (cond.bad()) { OFCHECK_FAIL(cond.text()); }

#ifdef WITH_ZLIB
    cond = randomRead(rnd, dfile_df.getDataset(), buffer);
    if (cond.bad()) { OFCHECK_FAIL(cond.text()); }
#endif

    // testing overlapping reads of partial element values
    OFLOG_DEBUG(tstpreadLogger, "Testing overlapping reads of partial element values");

    cond = sequentialOverlappingRead(rnd, dfile_be.getDataset(), buffer);
    if (cond.bad()) { OFCHECK_FAIL(cond.text()); }

    cond = sequentialOverlappingRead(rnd, dfile_le.getDataset(), buffer);
    if (cond.bad()) { OFCHECK_FAIL(cond.text()); }

#ifdef WITH_ZLIB
    cond = sequentialOverlappingRead(rnd, dfile_df.getDataset(), buffer);
    if (cond.bad()) { OFCHECK_FAIL(cond.text()); }
#endif

    unlink("test_be.dcm");
    unlink("test_le.dcm");
#ifdef WITH_ZLIB
    unlink("test_df.dcm");
#endif
    delete[] buffer;
}
