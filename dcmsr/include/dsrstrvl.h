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
 *    classes: DSRStringValue
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2000-10-18 17:08:11 $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DSRSTRVL_H
#define DSRSTRVL_H

#include "osconfig.h"   /* make sure OS specific configuration is included first */

#include "dsrtypes.h"

#include "ofstring.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class for string values
 */
class DSRStringValue
{

  public:

    DSRStringValue();

    DSRStringValue(const OFString &stringValue);

    DSRStringValue(const DSRStringValue &stringValue);

    virtual ~DSRStringValue();

    DSRStringValue &operator=(const DSRStringValue &stringValue);

    virtual void clear();

    virtual OFBool isValid() const;

    void print(ostream &stream,
               const size_t maxLength = 0) const;

    E_Condition read(DcmItem &dataset,
                     const DcmTagKey &tagKey,
                     OFConsole *logStream);

    E_Condition write(DcmItem &dataset,
                      const DcmTagKey &tagKey,
                      OFConsole *logStream) const;

    inline const OFString &getValue() const
    {
        return Value;
    }

    E_Condition setValue(const OFString &stringValue);


  protected:

    virtual OFBool checkValue(const OFString &stringValue) const;


  private:

    OFString Value;
};


#endif


/*
 *  CVS/RCS Log:
 *  $Log: dsrstrvl.h,v $
 *  Revision 1.2  2000-10-18 17:08:11  joergr
 *  Added read and write methods.
 *
 *  Revision 1.1  2000/10/13 07:49:32  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
