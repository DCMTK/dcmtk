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
 *  Purpose: Interface of class DcmFloatingPointDouble
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2002-04-25 09:51:42 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcvrfd.h,v $
 *  CVS/RCS Revision: $Revision: 1.16 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCVRFD_H
#define DCVRFD_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "ofconsol.h"
#include "dcerror.h"
#include "dctypes.h"
#include "dcelem.h"



class DcmFloatingPointDouble : public DcmElement 
{
public:
    DcmFloatingPointDouble(const DcmTag &tag, const Uint32 len = 0);
    DcmFloatingPointDouble( const DcmFloatingPointDouble &newFD );
    virtual ~DcmFloatingPointDouble();

    DcmFloatingPointDouble &operator=(const DcmFloatingPointDouble &obj) { DcmElement::operator=(obj); return *this; }

    virtual DcmEVR ident() const { return EVR_FD; }
    virtual void print(ostream & out, const OFBool showFullData = OFTrue,
		       const int level = 0, const char *pixelFileName = NULL,
		       size_t *pixelCounter = NULL);
    virtual unsigned long getVM(void);

    virtual OFCondition putFloat64Array(const Float64 * doubleVal,
					const unsigned long numDoubles);

    virtual OFCondition putFloat64(const Float64 doubleval,    // one double
				   const unsigned long position = 0); 
                                                               // at any position

    virtual OFCondition putString(const char * value);  // double as Strings

    virtual OFCondition getFloat64Array(Float64 * & doubleVals);
    virtual OFCondition getFloat64(Float64 & doubleVal, 
				   const unsigned long pos = 0);

    /** get specified value as a character string
     *  @param value variable in which the result value is stored
     *  @param pos index of the value in case of multi-valued elements (0..vm-1)
     *  @param normalize not used
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getOFString(OFString &value,
                                    const unsigned long pos,
                                    OFBool normalize = OFTrue);

    virtual OFCondition verify(const OFBool autocorrect = OFFalse);
};


#endif // DCVRFD_H

/*
** CVS/RCS Log:
** $Log: dcvrfd.h,v $
** Revision 1.16  2002-04-25 09:51:42  joergr
** Added getOFString() implementation.
**
** Revision 1.15  2001/09/25 17:19:31  meichel
** Adapted dcmdata to class OFCondition
**
** Revision 1.14  2001/06/01 15:48:49  meichel
** Updated copyright header
**
** Revision 1.13  2000/04/14 15:31:34  meichel
** Removed default value from output stream passed to print() method.
**   Required for use in multi-thread environments.
**
** Revision 1.12  2000/03/08 16:26:23  meichel
** Updated copyright header.
**
** Revision 1.11  2000/03/03 14:05:26  meichel
** Implemented library support for redirecting error messages into memory
**   instead of printing them to stdout/stderr for GUI applications.
**
** Revision 1.10  2000/02/10 10:50:54  joergr
** Added new feature to dcmdump (enhanced print method of dcmdata): write
** pixel data/item value fields to raw files.
**
** Revision 1.9  1999/03/31 09:25:00  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.8  1998/11/12 16:47:48  meichel
** Implemented operator= for all classes derived from DcmObject.
**
** Revision 1.7  1997/07/21 08:25:14  andreas
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.6  1997/04/18 08:13:29  andreas
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
** Revision 1.5  1996/08/05 08:45:31  andreas
** new print routine with additional parameters:
**         - print into files
**         - fix output length for elements
** corrected error in search routine with parameter ESM_fromStackTop
**
** Revision 1.4  1996/01/29 13:38:16  andreas
** - new put method for every VR to put value as a string
** - better and unique print methods
**
** Revision 1.3  1996/01/05 13:23:05  andreas
** - changed to support new streaming facilities
** - more cleanups
** - merged read / write methods for block and file transfer
**
*/
