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
 *  Purpose: Interface of class DcmAgeString
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2002-12-06 12:49:13 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcvras.h,v $
 *  CVS/RCS Revision: $Revision: 1.8 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DCVRAS_H
#define DCVRAS_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcbytstr.h"


/** a class representing the DICOM value representation 'Age String' (AS)
 */
class DcmAgeString
  : public DcmByteString
{

  public:

    /** constructor.
     *  Create new element from given tag and length.
     *  @param tag DICOM tag for the new element
     *  @param len value length for the new element
     */
    DcmAgeString(const DcmTag &tag,
                 const Uint32 len = 0);

    /** copy constructor
     *  @param old element to be copied
     */
    DcmAgeString(const DcmAgeString &old);

    /** destructor
     */
    virtual ~DcmAgeString(void);

    /** assignment operator
     *  @param obj element to be assigned/copied
     *  @return reference to this object
     */
    DcmAgeString &operator=(const DcmAgeString &obj);

    /** get element type identifier
     *  @return type identifier of this class (EVR_AS)
     */
    virtual DcmEVR ident(void) const;
};


#endif // DCVRAS_H


/*
** CVS/RCS Log:
** $Log: dcvras.h,v $
** Revision 1.8  2002-12-06 12:49:13  joergr
** Enhanced "print()" function by re-working the implementation and replacing
** the boolean "showFullData" parameter by a more general integer flag.
** Added doc++ documentation.
** Made source code formatting more consistent with other modules/files.
**
** Revision 1.7  2001/06/01 15:48:48  meichel
** Updated copyright header
**
** Revision 1.6  2000/03/08 16:26:21  meichel
** Updated copyright header.
**
** Revision 1.5  1999/03/31 09:24:55  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.4  1998/11/12 16:47:45  meichel
** Implemented operator= for all classes derived from DcmObject.
**
** Revision 1.3  1996/01/05 13:23:02  andreas
** - changed to support new streaming facilities
** - more cleanups
** - merged read / write methods for block and file transfer
**
*/
