/*
 *
 *  Copyright (C) 1994-2002, OFFIS
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
 *  Author:  Gerd Ehlers, Andreas Barth
 *
 *  Purpose: Interface of class DcmCodeString
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2002-12-06 12:49:14 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcvrcs.h,v $
 *  CVS/RCS Revision: $Revision: 1.13 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DCVRCS_H
#define DCVRCS_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcbytstr.h"


/** a class representing the DICOM value representation 'Code String' (CS)
 */
class DcmCodeString
  : public DcmByteString
{

  public:

    /** constructor.
     *  Create new element from given tag and length.
     *  @param tag DICOM tag for the new element
     *  @param len value length for the new element
     */
    DcmCodeString(const DcmTag &tag,
                  const Uint32 len = 0);

    /** copy constructor
     *  @param old element to be copied
     */
    DcmCodeString(const DcmCodeString &old);

    /** destructor
     */
    virtual ~DcmCodeString();

    /** assignment operator
     *  @param obj element to be assigned/copied
     *  @return reference to this object
     */
    DcmCodeString &operator=(const DcmCodeString &obj);

    /** get element type identifier
     *  @return type identifier of this class (EVR_CS)
     */
    virtual DcmEVR ident() const;

    /** get a copy of a particular string component
     *  @param stringVal variable in which the result value is stored
     *  @param pos index of the value in case of multi-valued elements (0..vm-1)
     *  @param normalize delete leading and trailing spaces if OFTrue
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getOFString(OFString &stringVal,
                                    const unsigned long pos,
                                    OFBool normalize = OFTrue);
};


#endif // DCVRCS_H


/*
** CVS/RCS Log:
** $Log: dcvrcs.h,v $
** Revision 1.13  2002-12-06 12:49:14  joergr
** Enhanced "print()" function by re-working the implementation and replacing
** the boolean "showFullData" parameter by a more general integer flag.
** Added doc++ documentation.
** Made source code formatting more consistent with other modules/files.
**
** Revision 1.12  2002/04/25 09:48:10  joergr
** Removed getOFStringArray() implementation.
**
** Revision 1.11  2001/09/25 17:19:30  meichel
** Adapted dcmdata to class OFCondition
**
** Revision 1.10  2001/06/01 15:48:48  meichel
** Updated copyright header
**
** Revision 1.9  2000/03/08 16:26:22  meichel
** Updated copyright header.
**
** Revision 1.8  1999/03/31 09:24:57  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.7  1998/11/12 16:47:46  meichel
** Implemented operator= for all classes derived from DcmObject.
**
** Revision 1.6  1997/09/11 15:13:12  hewett
** Modified getOFString method arguments by removing a default value
** for the pos argument.  By requiring the pos argument to be provided
** ensures that callers realise getOFString only gets one component of
** a multi-valued string.
**
** Revision 1.5  1997/08/29 08:32:40  andreas
** - Added methods getOFString and getOFStringArray for all
**   string VRs. These methods are able to normalise the value, i. e.
**   to remove leading and trailing spaces. This will be done only if
**   it is described in the standard that these spaces are not relevant.
**   These methods do not test the strings for conformance, this means
**   especially that they do not delete spaces where they are not allowed!
**   getOFStringArray returns the string with all its parts separated by \
**   and getOFString returns only one value of the string.
**   CAUTION: Currently getString returns a string with trailing
**   spaces removed (if dcmEnableAutomaticInputDataCorrection == OFTrue) and
**   truncates the original string (since it is not copied!). If you rely on this
**   behaviour please change your application now.
**   Future changes will ensure that getString returns the original
**   string from the DICOM object (NULL terminated) inclusive padding.
**   Currently, if you call getOF... before calling getString without
**   normalisation, you can get the original string read from the DICOM object.
**
** Revision 1.4  1996/01/09 11:06:17  andreas
** New Support for Visual C++
** Correct problems with inconsistent const declarations
**
** Revision 1.3  1996/01/05 13:23:03  andreas
** - changed to support new streaming facilities
** - more cleanups
** - merged read / write methods for block and file transfer
**
*/
