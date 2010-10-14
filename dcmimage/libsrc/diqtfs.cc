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
 *  Module:  dcmimage
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose: class DcmQuantFloydSteinberg
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:14 $
 *  CVS/RCS Revision: $Revision: 1.5 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmimage/diqtfs.h"
#include "dcmtk/ofstd/offname.h"    /* for OFFilenameCreator::myrand_r */

#define INCLUDE_CTIME
#include "dcmtk/ofstd/ofstdinc.h"

BEGIN_EXTERN_C
#include <sys/types.h> // needed for time()
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
  unsigned int now = OFstatic_cast(unsigned int, time(NULL));

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
 * Revision 1.5  2010-10-14 13:14:14  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.4  2005/12/08 15:42:29  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.3  2003/12/17 16:34:57  joergr
 * Adapted type casts to new-style typecast operators defined in ofcast.h.
 *
 * Revision 1.2  2002/11/27 14:16:58  meichel
 * Adapted module dcmimage to use of new header file ofstdinc.h
 *
 * Revision 1.1  2002/01/25 13:32:10  meichel
 * Initial release of new color quantization classes and
 *   the dcmquant tool in module dcmimage.
 *
 *
 */
