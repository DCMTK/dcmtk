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
 *  Purpose: class DcmQuantPixelBoxArray
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:15 $
 *  CVS/RCS Revision: $Revision: 1.7 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofcast.h"
#include "dcmtk/dcmimage/diqtpbox.h"   /* for DcmQuantPixelBoxArray */

#ifdef USE_STD_CXX_INCLUDES
// Solaris defines qsort() in namespace std, other compilers don't...
namespace std { }
using namespace std;
#endif


BEGIN_EXTERN_C
static int sumcompare(const void *x1, const void *x2)
{
  return (*(DcmQuantPixelBoxPointer *) x2)->sum
       - (*(DcmQuantPixelBoxPointer *) x1)->sum;
}
END_EXTERN_C


void DcmQuantPixelBoxArray::sort(unsigned long boxes)
{
#ifdef DEBUG
  assert(boxes <= length);
#endif
  qsort(OFreinterpret_cast(char *, array), OFstatic_cast(unsigned int, boxes),  sizeof(DcmQuantPixelBoxPointer), sumcompare);
}


DcmQuantPixelBoxArray::DcmQuantPixelBoxArray(unsigned long entries)
: array(NULL)
, length(entries)
{
  array = new DcmQuantPixelBoxPointer[entries];
  for (unsigned long i=0; i<entries; i++) array[i] = new DcmQuantPixelBox();
}


DcmQuantPixelBoxArray::~DcmQuantPixelBoxArray()
{
  for (unsigned long i=0; i<length; i++) delete array[i];
  delete[] array;
}


/*
 *
 * CVS/RCS Log:
 * $Log: diqtpbox.cc,v $
 * Revision 1.7  2010-10-14 13:14:15  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.6  2009-08-19 14:48:54  meichel
 * Unlike some other compilers, Sun Studio 11 on Solaris declares longjmp()
 *   and qsort() in namespace std. Added code to handle this case.
 *
 * Revision 1.5  2005-12-08 15:42:32  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.4  2003/12/17 16:34:57  joergr
 * Adapted type casts to new-style typecast operators defined in ofcast.h.
 *
 * Revision 1.3  2002/12/11 18:10:22  joergr
 * Added extern "C" declaration to qsort functions to avoid warnings reported
 * by Sun CC 5.2.
 *
 * Revision 1.2  2002/07/18 12:19:50  joergr
 * Added explicit type casts to keep Sun CC 2.0.1 quiet.
 *
 * Revision 1.1  2002/01/25 13:32:11  meichel
 * Initial release of new color quantization classes and
 *   the dcmquant tool in module dcmimage.
 *
 *
 */
