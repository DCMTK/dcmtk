/*
 *
 *  Copyright (C) 2002-2010, OFFIS e.V.
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
 *  Module:  ofstd
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose: test program for the non-trivial fseek and ftell implementations
 *           in class OFFile
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-12-20 12:05:20 $
 *  CVS/RCS Revision: $Revision: 1.8 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/offile.h"
#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/ofstd/ofconsol.h"
#include "dcmtk/ofstd/ofstd.h"

#define INCLUDE_CTIME
#define INCLUDE_CSTDLIB
#define INCLUDE_IOSTREAM
#include "dcmtk/ofstd/ofstdinc.h"

// size of block (Uint32 values, not bytes): 1 MByte
#define BLOCKSIZE 0x40000
// size of file (in blocks): 6 GByte
#define FILESIZE 0x1800

#define FILENAME "testfile.$$$"

OFBool fillFile(OFFile& file)
{
  Uint32 *buf = new Uint32[BLOCKSIZE];
  if (buf == NULL)
  {
    COUT << "unable to allocate buffer" << OFendl;
    return OFFalse;
  }

  // value counter. We can create 2^32 = 16 GByte of different values
  Uint32 v = 0;
  Uint32 *current;
  Uint32 *end = buf + BLOCKSIZE; // pointer past end of buffer

  COUT << "[0%------------25%-------------50%--------------75%----------100%]\n[" << STD_NAMESPACE flush;

  // we will write FILESIZE blocks
  for (Uint32 i = 0; i < FILESIZE; ++i)
  {
    current = buf;
    while (current != end) *current++ = v++;
    if (1 != file.fwrite(buf, BLOCKSIZE * sizeof(Uint32), 1))
    {
      COUT << "write error after " << i << " blocks" << OFendl;
      delete[] buf;
      return OFFalse;
    }
    if (i % (FILESIZE / 64) == 0) COUT << "*" << STD_NAMESPACE flush;
  }

  COUT << "]" << OFendl << OFendl;
  delete[] buf;
  return OFTrue;
}

Uint32 myRand(Uint32 max)
{
   return OFstatic_cast(Uint32, (OFstatic_cast(double, max) * rand() / RAND_MAX)); // 0.. max * RAND_MAX
}

OFBool seekFile(OFFile &file)
{
  offile_off_t pos, lastpos;
  offile_off_t expected;
  int result;
  Uint32 i;
  Uint32 v;
  Uint32 block;
  Uint32 offset;

  COUT << "Seeking to start of blocks using SEEK_SET\n"
       << "[0%------------25%-------------50%--------------75%----------100%]\n[" << STD_NAMESPACE flush;

  // fseek to each 4th block using SEEK_SET
  for (i = 0; i <= FILESIZE; i += 4)
  {
    pos = i * BLOCKSIZE * sizeof(Uint32);
    result = file.fseek(pos, SEEK_SET);
    if (result)
    {
      COUT << "Error: fseek(SEEK_SET) error when seeking to block " << i << OFendl;
      return OFFalse;
    }
    if (1 == file.fread(&v, sizeof(Uint32), 1))
    {
      // successfully read value. Now check if the value is correct.
      if (v != pos / sizeof(Uint32))
      {
        COUT << "\nError: unexpected data read after fseek(SEEK_SET) to block " << i << ": expected "
             << OFstatic_cast(unsigned long, pos / sizeof(Uint32)) << ", found " << v << OFendl;
        return OFFalse;
      }
    }
    else
    {
      // read error. This is only OK if we are at the end of file and EOF is flagged.
      if (i != FILESIZE)
      {
        COUT << "\nError: unexpected read error after fseek(SEEK_SET) to block " << i << OFendl;
        return OFFalse;
      }
      if (!file.eof())
      {
        COUT << "\nError: missing EOF after fseek(SEEK_SET) to end of file" << OFendl;
        return OFFalse;
      }
    }
    if ((i % (FILESIZE / 64) == 0) && (i != FILESIZE)) COUT << "*" << STD_NAMESPACE flush;
  }
  COUT << "]" << OFendl << OFendl;

  COUT << "Seeking to random positions using SEEK_SET\n"
       << "[0%------------25%-------------50%--------------75%----------100%]\n[" << STD_NAMESPACE flush;

  // fseek to random blocks using SEEK_SET
  for (i = 0; i < 1024; ++i)
  {
    block = myRand(FILESIZE - 1);
    offset = myRand(BLOCKSIZE - 1);
    pos = block * BLOCKSIZE * sizeof(Uint32) + offset * sizeof(Uint32);
    result = file.fseek(pos, SEEK_SET);
    if (result)
    {
      COUT << "Error: fseek(SEEK_SET) error when seeking to block " << block << " offset " << offset << OFendl;
      return OFFalse;
    }
    if (1 == file.fread(&v, sizeof(Uint32), 1))
    {
      // successfully read value. Now check if the value is correct.
      if (v != pos / sizeof(Uint32))
      {
        COUT << "\nError: unexpected data read after fseek(SEEK_SET) to block " << block
             << " offset " << offset << ": expected "
             << OFstatic_cast(unsigned long, pos / sizeof(Uint32)) << ", found " << v << OFendl;
        return OFFalse;
      }
    }
    else
    {
        COUT << "\nError: unexpected read error after fseek(SEEK_SET) to block " << i << OFendl;
        return OFFalse;
    }
    if (i % (1024 / 64) == 0) COUT << "*" << STD_NAMESPACE flush;
  }
  COUT << "]" << OFendl << OFendl;

  COUT << "Seeking to start of blocks using SEEK_END\n"
       << "[0%------------25%-------------50%--------------75%----------100%]\n[" << STD_NAMESPACE flush;

  // fseek to each 4th block using SEEK_END
  for (i = 0; i < FILESIZE; i += 4)
  {
    pos = OFstatic_cast(offile_off_t, -1) * (FILESIZE - i) * BLOCKSIZE * sizeof(Uint32);

    result = file.fseek(pos, SEEK_END);
    if (result)
    {
      COUT << "Error: fseek(SEEK_END) error when seeking to block " << i << OFendl;
      return OFFalse;
    }
    if (1 == file.fread(&v, sizeof(Uint32), 1))
    {
      // successfully read value. Now check if the value is correct.
      expected = OFstatic_cast(offile_off_t, BLOCKSIZE) * i;
      if (v != OFstatic_cast(Uint32, expected))
      {
        COUT << "\nError: unexpected data read after fseek(SEEK_END) to block " << FILESIZE-i << ": expected "
             << OFstatic_cast(unsigned long, expected) << ", found " << v << OFendl;
        return OFFalse;
      }
    }
    else
    {
      // read error. This is only OK if we are at the end of file and EOF is flagged.
      if (i != 0)
      {
        COUT << "\nError: unexpected read error after fseek(SEEK_END) to block " << FILESIZE-i << OFendl;
        return OFFalse;
      }
      if (!file.eof())
      {
        COUT << "\nError: missing EOF after fseek(SEEK_END) to end of file" << OFendl;
        return OFFalse;
      }
    }
    if ((i % (FILESIZE / 64) == 0) && (i != FILESIZE)) COUT << "*" << STD_NAMESPACE flush;
  }
  COUT << "]" << OFendl << OFendl;

  COUT << "Seeking to random positions using SEEK_END\n"
       << "[0%------------25%-------------50%--------------75%----------100%]\n[" << STD_NAMESPACE flush;

  // fseek to random blocks using SEEK_END
  for (i = 0; i < 1024; ++i)
  {
    block = myRand(FILESIZE - 2); // this avoids that pos can ever be 0, which would cause us to read after the end of file.
    offset = myRand(BLOCKSIZE - 1);
    pos = OFstatic_cast(offile_off_t, -1) * (FILESIZE - block - 1) * BLOCKSIZE * sizeof(Uint32) + offset * sizeof(Uint32);
    result = file.fseek(pos, SEEK_END);
    if (result)
    {
      COUT << "Error: fseek(SEEK_END) error when seeking to block " << block << " offset " << offset << OFendl;
      return OFFalse;
    }
    if (1 == file.fread(&v, sizeof(Uint32), 1))
    {
      // successfully read value. Now check if the value is correct.
      expected = (OFstatic_cast(offile_off_t, FILESIZE) * BLOCKSIZE * sizeof(Uint32) + pos) / sizeof(Uint32);
      if (v != OFstatic_cast(Uint32, expected))
      {
        COUT << "\nError: unexpected data read after fseek(SEEK_END) to block " << block
             << " offset " << offset << ": expected "
             << OFstatic_cast(unsigned long, expected) << ", found " << v << OFendl;
        return OFFalse;
      }
    }
    else
    {
        COUT << "\nError: unexpected read error after fseek(SEEK_END) to block " << i << OFendl;
        return OFFalse;
    }
    if (i % (1024 / 64) == 0) COUT << "*" << STD_NAMESPACE flush;
  }
  COUT << "]" << OFendl << OFendl;

  COUT << "Seeking to start of blocks using SEEK_CUR\n"
       << "[0%------------25%-------------50%--------------75%----------100%]\n[" << STD_NAMESPACE flush;

  // fseek to each 4th block using SEEK_CUR
  pos = BLOCKSIZE * sizeof(Uint32) * 4;
  file.rewind();
  for (i = 0; i <= FILESIZE; i += 4)
  {
    if (i > 0) result = file.fseek(pos-sizeof(Uint32), SEEK_CUR); else result = 0;
    if (result)
    {
      COUT << "Error: fseek(SEEK_CUR) error when seeking to block " << i << OFendl;
      return OFFalse;
    }
    if (1 == file.fread(&v, sizeof(Uint32), 1))
    {
      // successfully read value. Now check if the value is correct.
      expected = OFstatic_cast(offile_off_t, BLOCKSIZE) * i;
      if (v != OFstatic_cast(Uint32, expected))
      {
        COUT << "\nError: unexpected data read after fseek(SEEK_CUR) to block " << i << ": expected "
             << OFstatic_cast(unsigned long, expected) << ", found " << v << OFendl;
        return OFFalse;
      }
    }
    else
    {
      // read error. This is only OK if we are at the end of file and EOF is flagged.
      if (i != FILESIZE)
      {
        COUT << "\nError: unexpected read error after fseek(SEEK_CUR) to block " << i << OFendl;
        return OFFalse;
      }
      if (!file.eof())
      {
        COUT << "\nError: missing EOF after fseek(SEEK_CUR) to end of file" << OFendl;
        return OFFalse;
      }
    }
    if ((i % (FILESIZE / 64) == 0) && (i != FILESIZE)) COUT << "*" << STD_NAMESPACE flush;
  }
  COUT << "]" << OFendl << OFendl;

  COUT << "Seeking to random positions using SEEK_CUR\n"
       << "[0%------------25%-------------50%--------------75%----------100%]\n[" << STD_NAMESPACE flush;

  // fseek to random blocks using SEEK_CUR
  file.rewind();
  lastpos = 0;
  for (i = 0; i < 1024; ++i)
  {
    block = myRand(FILESIZE - 1);
    offset = myRand(BLOCKSIZE - 1);
    pos = block * BLOCKSIZE * sizeof(Uint32) + offset * sizeof(Uint32);
    result = file.fseek((pos-lastpos), SEEK_CUR);
    if (result)
    {
      COUT << "Error: fseek(SEEK_CUR) error when seeking to block " << block << " offset " << offset << OFendl;
      return OFFalse;
    }
    if (1 == file.fread(&v, sizeof(Uint32), 1))
    {
      // successfully read value. Now check if the value is correct.
      if (v != pos / sizeof(Uint32))
      {
        COUT << "\nError: unexpected data read after fseek(SEEK_CUR) to block " << block
             << " offset " << offset << ": expected "
             << OFstatic_cast(unsigned long, pos / sizeof(Uint32)) << ", found " << v << OFendl;
        return OFFalse;
      }
      lastpos = pos+sizeof(Uint32); // we have read a few bytes from the stream, so our position has changed
    }
    else
    {
        COUT << "\nError: unexpected read error after fseek(SEEK_CUR) to block " << i << OFendl;
        return OFFalse;
    }
    if (i % (1024 / 64) == 0) COUT << "*" << STD_NAMESPACE flush;
  }
  COUT << "]" << OFendl << OFendl;

  return OFTrue;
}

int main()
{
  COUT << "Test program for LFS support in DCMTK class OFFile\n" << OFendl;

  // initialize random generator
  srand(OFstatic_cast(unsigned int, time(NULL)));

  // check if typedefs are large enough
  COUT << "Checking typedefs.\n"
       << "- size of offile_off_t: " << sizeof(offile_off_t);
  if (sizeof(offile_off_t) > 4) COUT << " - OK\n"; else COUT << " - too small, no LFS support\n";
  COUT << "- size of offile_fpos_t: " << sizeof(offile_fpos_t);
  if (sizeof(offile_fpos_t) > 4) COUT << " - OK\n"; else COUT << " - too small, no LFS support\n";
  if ((sizeof(offile_off_t) <= 4 || sizeof(offile_fpos_t) <= 4))
  {
    COUT << "No LFS support available. LFS test failed." << OFendl;
    return 10;
  }

  OFFile file;

  if (OFStandard::fileExists(FILENAME))
  {
    COUT << "\nData file " << FILENAME << " already exists, let's just use it." << OFendl << STD_NAMESPACE flush;
  }
  else
  {
    COUT << "\nCreating a 6 GByte data file." << OFendl << STD_NAMESPACE flush;
    if (!file.fopen(FILENAME, "w+b"))
    {
      COUT << "Error: Unable to create file " << FILENAME << ". LFS test failed." << OFendl;
      return 10;
    }

    if (! fillFile(file))
    {
      COUT << "Error: Unable to write 6 GByte of data. LFS test failed." << OFendl;
      return 10;
    }

    if (! seekFile(file))
    {
      COUT << "Error: fseek() tests unsuccessful. LFS test failed." << OFendl;
      return 10;
    }

    COUT << "Closing file." << OFendl << STD_NAMESPACE flush;

    file.fclose();
  }

  COUT << "Re-opening file in read-only mode." << OFendl << STD_NAMESPACE flush;

  if (!file.fopen(FILENAME, "rb"))
  {
    COUT << "Error: Unable to open file " << FILENAME << ". LFS test failed." << OFendl;
    return 10;
  }

  if (! seekFile(file))
  {
    COUT << "Error: fseek() tests unsuccessful. LFS test failed." << OFendl;
    return 10;
  }


  return 0;
}

/*
 * CVS/RCS Log:
 * $Log: toffile.cc,v $
 * Revision 1.8  2010-12-20 12:05:20  joergr
 * Added explicit type casts in order to keep gcc 2.95.3 quiet.
 *
 * Revision 1.7  2010-12-13 13:08:37  uli
 * Fix toffile by moving some casts to the correct position.
 *
 * Revision 1.6  2010-12-06 13:10:27  joergr
 * Restructured calculation of seek position in order to avoid warning messages.
 *
 * Revision 1.5  2010-11-22 13:08:08  uli
 * Use a macro instead of repeating the filename of the test file.
 *
 * Revision 1.4  2010-10-14 13:15:15  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.3  2010-08-05 08:38:11  uli
 * Fixed some warnings from -Wold-style-cast.
 *
 * Revision 1.2  2009-08-07 16:18:48  meichel
 * Fixed some seek offset computations in SEEK_END tests
 *
 * Revision 1.1  2006/08/21 12:41:10  meichel
 * Added test application that checks whether class OFFile can correctly
 *   process large files (> 4 GBytes), including emulations of fseek and ftell,
 *   which are non-trivial on certain platforms such as Win32.
 *
 *
 */
