/*
 *
 *  Copyright (C) 1996-2001, OFFIS
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
 *  Module:  dcmimage
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose: class DcmQuantFloydSteinberg
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2002-01-25 13:32:10 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/libsrc/diqtfs.cc,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"
#include "diqtfs.h"
#include "offname.h"    /* for OFFilenameCreator::myrand_r */

BEGIN_EXTERN_C
#include <sys/types.h> // needed for time()
#include <time.h>      // needed for time()
END_EXTERN_C


DcmQuantFloydSteinberg::DcmQuantFloydSteinberg()
: thisrerr(NULL)
, nextrerr(NULL)
, thisgerr(NULL)
, nextgerr(NULL)
, thisberr(NULL)
, nextberr(NULL)
, temperr (NULL)
, fs_direction(0)
, columns(0)
{
}

DcmQuantFloydSteinberg::~DcmQuantFloydSteinberg()
{
  cleanup();
}

void DcmQuantFloydSteinberg::cleanup()
{
  delete[] thisrerr;
  delete[] nextrerr;
  delete[] thisgerr;
  delete[] nextgerr;
  delete[] thisberr;
  delete[] nextberr;
}

OFCondition DcmQuantFloydSteinberg::initialize(unsigned long cols)
{
  columns = cols;
  cleanup();
  unsigned int now = (unsigned int) time(NULL);

  /* Initialize Floyd-Steinberg error vectors. */
  thisrerr = new long[columns+2];
  if (! thisrerr) return EC_MemoryExhausted;

  nextrerr = new long[columns+2];
  if (! nextrerr) return EC_MemoryExhausted;

  thisgerr = new long[columns+2];
  if (! thisgerr) return EC_MemoryExhausted;

  nextgerr = new long[columns+2];
  if (! nextgerr) return EC_MemoryExhausted;

  thisberr = new long[columns+2];
  if (! thisberr) return EC_MemoryExhausted;

  nextberr = new long[columns+2];
  if (! nextberr) return EC_MemoryExhausted;

  for (unsigned long col = 0; col < columns + 2; ++col)
  {
      thisrerr[col] = OFFilenameCreator::myrand_r(&now) % ( DcmQuantFloydSteinbergScale * 2 ) - DcmQuantFloydSteinbergScale;
      thisgerr[col] = OFFilenameCreator::myrand_r(&now) % ( DcmQuantFloydSteinbergScale * 2 ) - DcmQuantFloydSteinbergScale;
      thisberr[col] = OFFilenameCreator::myrand_r(&now) % ( DcmQuantFloydSteinbergScale * 2 ) - DcmQuantFloydSteinbergScale;
      /* (random errors in [-1 .. 1]) */
  }
  fs_direction = 1;

  return EC_Normal;
}


/*
 *
 * CVS/RCS Log:
 * $Log: diqtfs.cc,v $
 * Revision 1.1  2002-01-25 13:32:10  meichel
 * Initial release of new color quantization classes and
 *   the dcmquant tool in module dcmimage.
 *
 *
 */
