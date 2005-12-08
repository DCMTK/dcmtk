/*
 *
 *  Copyright (C) 1994-2005, OFFIS
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
 *  Author:  Andrew Hewett
 *
 *  Purpose: Interface of class DcmUnlimitedText
 *           Value Representation UT is defined in Correction Proposal 101
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2005-12-08 16:29:16 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/dcmtk/dcmdata/dcvrut.h,v $
 *  CVS/RCS Revision: $Revision: 1.9 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DCVRUT_H
#define DCVRUT_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dctypes.h"
#include "dcmtk/dcmdata/dcchrstr.h"


/** a class representing the DICOM value representation 'Unlimited Text' (UT)
 */
class DcmUnlimitedText
  : public DcmCharString
{

  public:

    DcmUnlimitedText(const DcmTag &tag,
                     const Uint32 len = 0);

    DcmUnlimitedText(const DcmUnlimitedText &old);

    virtual ~DcmUnlimitedText();

    DcmUnlimitedText &operator=(const DcmUnlimitedText &obj);

    /** clone method
     *  @return deep copy of this object
     */
    virtual DcmObject *clone() const
    {
      return new DcmUnlimitedText(*this);
    }

    virtual DcmEVR ident() const;

    /** get the value multiplicity.
     *  Since the backslash "\" is not regarded as a separator the value
     *  multiplicity is always 1.
     *  @return value multiplicity of the currently stored value
     */
    virtual unsigned long getVM();

    /** get a particular components of the string value
     *  @param stringVal string variable in which the result value is stored
     *  @param pos not used since value multiplicity is always 1
     *  @param normalize remove trailing spaces if OFTrue
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getOFString(OFString &stringVal,
                                    const unsigned long pos,
                                    OFBool normalize = OFTrue);

    /** get the string value (all compenents)
     *  @param stringVal string variable in which the result value is stored
     *  @param normalize remove trailing spaces if OFTrue
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getOFStringArray(OFString &stringVal,
                                         OFBool normalize = OFTrue);
};


#endif // DCVRUT_H


/*
** CVS/RCS Log:
** $Log: dcvrut.h,v $
** Revision 1.9  2005-12-08 16:29:16  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.8  2004/07/01 12:28:25  meichel
** Introduced virtual clone method for DcmObject and derived classes.
**
** Revision 1.7  2002/12/06 12:49:21  joergr
** Enhanced "print()" function by re-working the implementation and replacing
** the boolean "showFullData" parameter by a more general integer flag.
** Added doc++ documentation.
** Made source code formatting more consistent with other modules/files.
**
** Revision 1.6  2001/09/25 17:19:36  meichel
** Adapted dcmdata to class OFCondition
**
** Revision 1.5  2001/06/01 15:48:55  meichel
** Updated copyright header
**
** Revision 1.4  2000/03/08 16:26:28  meichel
** Updated copyright header.
**
** Revision 1.3  1999/03/31 09:25:12  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.2  1998/11/12 16:47:58  meichel
** Implemented operator= for all classes derived from DcmObject.
**
** Revision 1.1  1998/01/19 13:19:40  hewett
** Initial version.
**
*/
