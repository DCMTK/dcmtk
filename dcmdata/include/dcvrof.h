/*
 *
 *  Copyright (C) 2002, OFFIS
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
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: Interface of class DcmOtherFloat
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2002-12-06 12:06:39 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcvrof.h,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DCVROF_H
#define DCVROF_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcvrfl.h"


/** a class representing the DICOM value representation 'Other Float String' (OF)
 */
class DcmOtherFloat
  : public DcmFloatingPointSingle
{

 public:

    /** constructor.
     *  Create new element from given tag and length.
     *  @param tag DICOM tag for the new element
     *  @param len value length for the new element
     */
    DcmOtherFloat(const DcmTag &tag,
                  const Uint32 len = 0);

    /** copy constructor
     *  @param old element to be copied
     */
    DcmOtherFloat(const DcmOtherFloat &old);

    /** destructor
     */
    virtual ~DcmOtherFloat();

    /** assignment operator
     *  @param obj element to be assigned/copied
     *  @return reference to this object
     */
    DcmOtherFloat &operator=(const DcmOtherFloat &obj);

    /** get element type identifier
     *  @return type identifier of this class (EVR_OF)
     */
    virtual DcmEVR ident() const;

    /** get value multiplicity
     *  @return always returns 1 (according to the DICOM standard)
     */
    virtual unsigned long getVM();
};


#endif // DCVROF_H


/*
 * CVS/RCS Log:
 * $Log: dcvrof.h,v $
 * Revision 1.1  2002-12-06 12:06:39  joergr
 * Added support for new value representation Other Float String (OF).
 *
 *
 */
