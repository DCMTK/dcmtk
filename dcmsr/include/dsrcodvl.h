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
 *    classes: DSRCodedEntryValue
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2000-10-13 07:49:23 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DSRCODVL_H
#define DSRCODVL_H

#include "osconfig.h"   /* make sure OS specific configuration is included first */

#include "dsrtypes.h"

#include "ofstring.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class for coded entry values
 */
class DSRCodedEntryValue
{

  public:

    DSRCodedEntryValue();

    DSRCodedEntryValue(const OFString &codeValue,
                       const OFString &codingSchemeDesignator,
                       const OFString &codeMeaning);

    DSRCodedEntryValue(const OFString &codeValue,
                       const OFString &codingSchemeDesignator,
                       const OFString &codingSchemeVersion,
                       const OFString &codeMeaning);

    DSRCodedEntryValue(const DSRCodedEntryValue &codedEntryValue);

    virtual ~DSRCodedEntryValue();

    DSRCodedEntryValue &operator=(const DSRCodedEntryValue &codedEntryValue);

    OFBool operator==(const DSRCodedEntryValue &codedEntryValue) const;

    void clear();

    OFBool isValid() const;

    OFBool isPrivateDcmtkCodingScheme() const;

    void print(ostream &stream,
               const OFBool printCodeValue = OFTrue,
               const OFBool printInvalid = OFFalse) const;

    E_Condition readSequence(DcmItem &dataset,
                             const DcmTagKey &tagKey,
                             OFConsole *logStream);

    E_Condition writeSequence(DcmItem &dataset,
                              const DcmTagKey &tagKey,
                              OFConsole *logStream) const;

    E_Condition renderHTML(ostream &stream,
                           OFConsole *logStream) const;

    const DSRCodedEntryValue &getValue() const
    {
        return *this;
    }

    E_Condition getValue(DSRCodedEntryValue &codedEntryValue) const;

    E_Condition setValue(const DSRCodedEntryValue &codedEntryValue);

    const OFString &getCodeValue() const
    {
        return CodeValue;
    }

    const OFString &getCodingSchemeDesignator() const
    {
        return CodingSchemeDesignator;
    }

    const OFString &getCodingSchemeVersion() const
    {
        return CodingSchemeVersion;
    }

    const OFString &getCodeMeaning() const
    {
        return CodeMeaning;
    }

    E_Condition setValue(const OFString &codeValue,
                         const OFString &codingSchemeDesignator,
                         const OFString &codeMeaning);

    E_Condition setValue(const OFString &codeValue,
                         const OFString &codingSchemeDesignator,
                         const OFString &codingSchemeVersion,
                         const OFString &codeMeaning);


  protected:

    E_Condition readItem(DcmItem &dataset,
                         OFConsole *logStream);

    E_Condition writeItem(DcmItem &dataset,
                          OFConsole *logStream) const;

    OFBool checkCode(const OFString &codeValue,
                     const OFString &codingSchemeDesignator,
                     const OFString &codeMeaning) const;


  private:

    OFString CodeValue;
    OFString CodingSchemeDesignator;
    OFString CodingSchemeVersion;
    OFString CodeMeaning;
    OFString PrivateCodingSchemeCreatorUID;
};


#endif


/*
 *  CVS/RCS Log:
 *  $Log: dsrcodvl.h,v $
 *  Revision 1.1  2000-10-13 07:49:23  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
