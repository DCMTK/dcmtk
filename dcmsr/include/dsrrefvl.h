/*
 *
 *  Copyright (C) 2000, OFFIS
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
 *  Module: dcmsr
 *
 *  Author: Joerg Riesmeier
 *
 *  Purpose:
 *    classes: DSRReferenceValue
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2000-10-13 07:49:30 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DSRREFVL_H
#define DSRREFVL_H

#include "osconfig.h"   /* make sure OS specific configuration is included first */

#include "dsrtypes.h"

#include "ofstring.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class for reference values
 */
class DSRReferenceValue
{

  public:

    DSRReferenceValue();

    DSRReferenceValue(const OFString &sopClassUID,
                      const OFString &sopInstanceUID);

    DSRReferenceValue(const DSRReferenceValue &referenceValue);

    virtual ~DSRReferenceValue();

    DSRReferenceValue &operator=(const DSRReferenceValue &referenceValue);

    virtual void clear();

    virtual OFBool isValid() const;

    virtual E_Condition print(ostream &stream,
                              const size_t flags) const;

    virtual E_Condition readSequence(DcmItem &dataset,
                                     OFConsole *logStream);

    virtual E_Condition writeSequence(DcmItem &dataset,
                                      OFConsole *logStream) const;

    virtual E_Condition renderHTML(ostream &docStream,
                                   ostream &annexStream,
                                   size_t &annexNumber,
                                   const size_t flags,
                                   OFConsole *logStream) const;

    const OFString &getSOPClassUID() const
    {
        return SOPClassUID;
    }

    const OFString &getSOPInstanceUID() const
    {
        return SOPInstanceUID;
    }

    const DSRReferenceValue &getValue() const
    {
        return *this;
    }

    E_Condition getValue(DSRReferenceValue &referenceValue) const;

    E_Condition setSOPClassUID(const OFString &sopClassUID);

    E_Condition setSOPInstanceUID(const OFString &sopInstanceUID);

    E_Condition setValue(const OFString &sopClassUID,
                         const OFString &sopInstanceUID);

    E_Condition setValue(const DSRReferenceValue &referenceValue);


  protected:

    virtual E_Condition readItem(DcmItem &dataset,
                                 OFConsole *logStream);

    virtual E_Condition writeItem(DcmItem &dataset,
                                  OFConsole *logStream) const;

    virtual OFBool checkSOPClassUID(const OFString &sopClassUID) const;

    virtual OFBool checkSOPInstanceUID(const OFString &sopInstanceUID) const;

    OFString SOPClassUID;
    OFString SOPInstanceUID;
};


#endif


/*
 *  CVS/RCS Log:
 *  $Log: dsrrefvl.h,v $
 *  Revision 1.1  2000-10-13 07:49:30  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
