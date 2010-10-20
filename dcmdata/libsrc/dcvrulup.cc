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
 *  Author:  Gerd Ehlers, Andreas Barth
 *
 *  Purpose: Implementation of class DcmUnsignedLongOffset
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-20 16:44:18 $
 *  CVS/RCS Revision: $Revision: 1.30 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dcvrulup.h"

#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#include "dcmtk/ofstd/ofstdinc.h"


// ********************************


DcmUnsignedLongOffset::DcmUnsignedLongOffset(const DcmTag &tag,
                                             const Uint32 len)
  : DcmUnsignedLong(tag, len),
    nextRecord(NULL)
{
}


DcmUnsignedLongOffset::DcmUnsignedLongOffset(const DcmUnsignedLongOffset &old)
  : DcmUnsignedLong(old),
    nextRecord(old.nextRecord)
{
}


DcmUnsignedLongOffset& DcmUnsignedLongOffset::operator=(const DcmUnsignedLongOffset& obj)
{
  if (this != &obj)
  {
    // copy parent's member variables
    DcmUnsignedLong::operator=(obj);
    // copy member variables
    nextRecord = obj.nextRecord;
  }
  return *this;
}


DcmUnsignedLongOffset::~DcmUnsignedLongOffset()
{
}


OFCondition DcmUnsignedLongOffset::copyFrom(const DcmObject& rhs)
{
  if (this != &rhs)
  {
    if (rhs.ident() != ident()) return EC_IllegalCall;
    *this = OFstatic_cast(const DcmUnsignedLongOffset &, rhs);
  }
  return EC_Normal;
}


// ********************************


DcmEVR DcmUnsignedLongOffset::ident() const
{
    /* internal type identifier */
    return EVR_up;
}


OFCondition DcmUnsignedLongOffset::clear()
{
    /* call inherited method */
    errorFlag = DcmUnsignedLong::clear();
    /* remove reference to object */
    nextRecord = NULL;
    return errorFlag;
}


// ********************************


DcmObject* DcmUnsignedLongOffset::getNextRecord()
{
    errorFlag = EC_Normal;
    /* return pointer to currently stored object reference */
    return nextRecord;
}


DcmObject *DcmUnsignedLongOffset::setNextRecord(DcmObject *record)
{
    errorFlag = EC_Normal;
    /* store new object reference */
    nextRecord = record;
    return record;
}


// ********************************


OFCondition DcmUnsignedLongOffset::verify(const OFBool autocorrect)
{
    /* call inherited method */
    errorFlag = DcmUnsignedLong::verify(autocorrect);
    /* perform additional checks on the stored value */
    Uint32 *uintVals;
    errorFlag = getUint32Array(uintVals);
    if (errorFlag.good() && (getLengthField() > 0) && (uintVals != NULL) && (*uintVals != 0) && (nextRecord == NULL))
        errorFlag = EC_CorruptedData;
    return errorFlag;
}


/*
** CVS/RCS Log:
** $Log: dcvrulup.cc,v $
** Revision 1.30  2010-10-20 16:44:18  joergr
** Use type cast macros (e.g. OFstatic_cast) where appropriate.
**
** Revision 1.29  2010-10-14 13:14:11  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.28  2008-07-17 10:31:32  onken
** Implemented copyFrom() method for complete DcmObject class hierarchy, which
** permits setting an instance's value from an existing object. Implemented
** assignment operator where necessary.
**
** Revision 1.27  2007-06-29 14:17:49  meichel
** Code clean-up: Most member variables in module dcmdata are now private,
**   not protected anymore.
**
** Revision 1.26  2005/12/08 15:42:08  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.25  2004/01/16 13:45:34  joergr
** Removed acknowledgements with e-mail addresses from CVS log.
**
** Revision 1.24  2002/12/06 13:19:27  joergr
** Enhanced "print()" function by re-working the implementation and replacing
** the boolean "showFullData" parameter by a more general integer flag.
** Made source code formatting more consistent with other modules/files.
**
** Revision 1.23  2002/11/27 12:07:00  meichel
** Adapted module dcmdata to use of new header file ofstdinc.h
**
** Revision 1.22  2002/04/16 13:43:27  joergr
** Added configurable support for C++ ANSI standard includes (e.g. streams).
**
** Revision 1.21  2001/09/25 17:20:02  meichel
** Adapted dcmdata to class OFCondition
**
** Revision 1.20  2001/06/01 15:49:22  meichel
** Updated copyright header
**
** Revision 1.19  2000/04/14 15:55:10  meichel
** Dcmdata library code now consistently uses ofConsole for error output.
**
** Revision 1.18  2000/03/08 16:26:52  meichel
** Updated copyright header.
**
** Revision 1.17  2000/03/03 14:05:41  meichel
** Implemented library support for redirecting error messages into memory
**   instead of printing them to stdout/stderr for GUI applications.
**
** Revision 1.16  2000/02/10 10:52:26  joergr
** Added new feature to dcmdump (enhanced print method of dcmdata): write
** pixel data/item value fields to raw files.
**
** Revision 1.15  2000/02/02 14:33:02  joergr
** Replaced 'delete' statements by 'delete[]' for objects created with 'new[]'.
**
** Revision 1.14  1999/04/13 16:29:36  meichel
** Fixed bug in DcmUnsignedLongOffset::print that caused an application crash
**   when very large DICOMDIRs were printed (i.e. with dcmdump).
**
** Revision 1.13  1999/03/31 09:26:02  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.12  1998/07/15 15:52:12  joergr
** Removed several compiler warnings reported by gcc 2.8.1 with
** additional options, e.g. missing copy constructors and assignment
** operators, initialization of member variables in the body of a
** constructor instead of the member initialization list, hiding of
** methods by use of identical names, uninitialized member variables,
** missing const declaration of char pointers. Replaced tabs by spaces.
**
** Revision 1.11  1997/07/21 08:25:36  andreas
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.10  1997/07/03 15:10:21  andreas
** - removed debugging functions Bdebug() and Edebug() since
**   they write a static array and are not very useful at all.
**   Cdebug and Vdebug are merged since they have the same semantics.
**   The debugging functions in dcmdata changed their interfaces
**   (see dcmdata/include/dcdebug.h)
**
** Revision 1.9  1997/04/18 08:17:22  andreas
** - The put/get-methods for all VRs did not conform to the C++-Standard
**   draft. Some Compilers (e.g. SUN-C++ Compiler, Metroworks
**   CodeWarrier, etc.) create many warnings concerning the hiding of
**   overloaded get methods in all derived classes of DcmElement.
**   So the interface of all value representation classes in the
**   library are changed rapidly, e.g.
**   OFCondition get(Uint16 & value, const unsigned long pos);
**   becomes
**   OFCondition getUint16(Uint16 & value, const unsigned long pos);
**   All (retired) "returntype get(...)" methods are deleted.
**   For more information see dcmdata/include/dcelem.h
**
** Revision 1.8  1996/08/05 08:46:25  andreas
** new print routine with additional parameters:
**         - print into files
**         - fix output length for elements
** corrected error in search routine with parameter ESM_fromStackTop
**
** Revision 1.7  1996/05/20 13:27:54  andreas
** correct minor bug in print routine
**
** Revision 1.6  1996/04/12 13:17:23  andreas
** Minor changes to support DEC ALPHA and DEC MIPS
**
** Revision 1.5  1996/03/11 13:09:44  hewett
** Corrected ambiguous use of this->get(0).
**
** Revision 1.4  1996/01/09 11:06:50  andreas
** New Support for Visual C++
** Correct problems with inconsistent const declarations
** Correct error in reading Item Delimitation Elements
**
** Revision 1.3  1996/01/05 13:27:56  andreas
** - changed to support new streaming facilities
** - unique read/write methods for file and block transfer
** - more cleanups
**
*/
