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
 *  Update Date:      $Date: 2000-10-18 17:00:29 $
 *  CVS/RCS Revision: $Revision: 1.2 $
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
    // allow access to getValuePtr()
    friend class DSRContentItem;

  public:

    /** default contructor
     */
    DSRCodedEntryValue();

    /** constructor.
     *  The code triple is only set if it passed the validity check (see setValue()).
     ** @param  codeValue               computer readable and computer searchable identifier
     *                                  of the code to be set. (VR=SH, mandatory)
     *  @param  codingSchemeDesignator  textual identifier of the table where the 'codeValue'
     *                                  is linked to its 'codeMeaning'. (VR=SH, mandatory)
     *  @param  codeMeaning             human-readable translation of the 'codeValue'.  Used
     *                                  for display when code dictionary is not available.
     *                                  (VR=LO, mandatory)
     */
    DSRCodedEntryValue(const OFString &codeValue,
                       const OFString &codingSchemeDesignator,
                       const OFString &codeMeaning);

    /** constructor.
     *  The code 4-tuple is only set if it passed the validity check (see setValue()).
     ** @param  codeValue               computer readable and computer searchable identifier
     *                                  of the code to be set. (VR=SH, mandatory)
     *  @param  codingSchemeDesignator  textual identifier of the table where the 'codeValue'
     *                                  is linked to its 'codeMeaning'. (VR=SH, mandatory)
     *  @param  codingSchemeVersion     version of the coding scheme.  Used when a coding
     *                                  scheme has multiple versions and the 'codingScheme
     *                                  Designator' does not explicitly (or adequately)
     *                                  specify the version number. (VR=SH, optional)
     *  @param  codeMeaning             human-readable translation of the 'codeValue'. Used
     *                                  for display when (appropriate) code dictionary is not
     *                                  available. (VR=LO, mandatory)
     */
    DSRCodedEntryValue(const OFString &codeValue,
                       const OFString &codingSchemeDesignator,
                       const OFString &codingSchemeVersion,
                       const OFString &codeMeaning);

    /** copy constructor.
     ** @param  codedEntryValue  code to be copied (not checked !)
     */
    DSRCodedEntryValue(const DSRCodedEntryValue &codedEntryValue);

    /** destructor
     */
    virtual ~DSRCodedEntryValue();

    /** assignment operator
     ** @param  codedEntryValue  code to be copied (not checked !)
     ** @return reference to this code after 'codedEntryValue' has been copied
     */
    DSRCodedEntryValue &operator=(const DSRCodedEntryValue &codedEntryValue);

    /** comparison operator.
     *  Two codes are equal if the code value, coding scheme designator and the (optional)
     *  coding scheme version are equal.  The code meaning is not important for this.
     ** @param  codedEntryValue  code which should be compared to the current one
     ** @return OFTrue if both codes are equal, OFFalse otherwise
     */
    OFBool operator==(const DSRCodedEntryValue &codedEntryValue) const;

    /** clear all internal variables.
     *  Since an empty code is invalid the code becomes invalid afterwards.
     */
    virtual void clear();

    /** check whether the current code is valid.
     *  See checkCode() for details.
     ** @return OFTrue if code is valid, OFFalse otherwise
     */
    virtual OFBool isValid() const;

    /** check whether the current code is empty.
     *  Checks whether all four components of the code are empty.
     ** @return OFTrue if code is empty, OFFalse otherwise
     */
    virtual OFBool isEmpty() const;

    /** check whether the current code belongs to the private "dcmtk" coding scheme.
     *  This check is performed by comparing the coding scheme designator and the private
     *  coding scheme creator UID - a coding scheme version is currently not used.
     ** @return OFTrue if code belongs to the private "dcmtk" coding scheme, OFFalse otherwise
     */
    OFBool isPrivateDcmtkCodingScheme() const;

    /** print code.
     *  The output of a typical code triple looks like this: (1234,99_OFFIS_DCMTK,"Code Meaning")
     ** @param  stream          output stream to which the code should be printed
     *  @param  printCodeValue  flag indicating whether the code value and coding scheme
     *                          designator should be printed (default) or not.  If OFFalse the
     *                          output looks like this: (,,"Code Meaning")
     *  @param  printInvalid    flag indicating whether the text "invalid code" should be
     *                          printed for invalid codes or not (default)
     */
    void print(ostream &stream,
               const OFBool printCodeValue = OFTrue,
               const OFBool printInvalid = OFFalse) const;

    /** read code sequence from dataset.
     *  The number of items within the code sequence is checked.  If error/warning output are
     *  enabled a warning message is printed if the sequence is empty or contains more than
     *  one item.
     ** @param  dataset    DICOM dataset from which the code sequence should be read
     *  @param  tagKey     DICOM tag specifying the attribute (= sequence) which should be read
     *  @param  type       value type of the sequence (valid value: "1", "2", something else).
     *                     This parameter is used for checking purpose, any difference is reported.
     *  @param  logStream  pointer to error/warning output stream (output disabled if NULL)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition readSequence(DcmItem &dataset,
                             const DcmTagKey &tagKey,
                             const OFString &type,
                             OFConsole *logStream);

    /** write code sequence to dataset
     ** @param  dataset    DICOM dataset to which the code sequence should be written
     *  @param  tagKey     DICOM tag specifying the attribute (= sequence) which should be written
     *  @param  logStream  pointer to error/warning output stream (output disabled if NULL)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition writeSequence(DcmItem &dataset,
                              const DcmTagKey &tagKey,
                              OFConsole *logStream) const;

    /** render code in HTML format
     ** @param  stream     output stream to which the HTML document is written
     *  @param  logStream  pointer to error/warning output stream (output disabled if NULL)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition renderHTML(ostream &stream,
                           OFConsole *logStream) const;

    /** get reference to code value
     ** @return reference to code value
     */
    inline const DSRCodedEntryValue &getValue() const
    {
        return *this;
    }

    /** get copy of code value
     ** @param  codedEntryValue  reference to variable in which the code should be stored
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition getValue(DSRCodedEntryValue &codedEntryValue) const;

    /** get code value.
     *  This is a computer readable and computer searchable identifier of the code.
     ** @return current code value (might be invalid or an empty string)
     */
    inline const OFString &getCodeValue() const
    {
        return CodeValue;
    }

    /** get coding scheme designator.
     *  This is a textual identifier of the table where the code value is linked to its
     *  code meaning.
     ** @return current coding scheme designator (might be invalid or an empty string)
     */
    inline const OFString &getCodingSchemeDesignator() const
    {
        return CodingSchemeDesignator;
    }

    /** get coding scheme version.
     *  Optional - Used when a coding scheme has multiple versions and the coding scheme
     *  designator does not explicitly (or adequately) specify the version number.
     ** @return current coding scheme version (might be invalid or an empty string)
     */
    inline const OFString &getCodingSchemeVersion() const
    {
        return CodingSchemeVersion;
    }

    /** get code meaning.
     *  Human-readable translation of the code value.  Used for display when code dictionary
     *  is not available.
     ** @return current code meaning (might be invalid or an empty string)
     */
    inline const OFString &getCodeMeaning() const
    {
        return CodeMeaning;
    }

    /** set code value.
     *  Before setting the code it is checked (see checkCode()).  If the code is invalid
     *  the current code is not replaced and remains unchanged.
     ** @param  codedEntryValue  code to be set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition setValue(const DSRCodedEntryValue &codedEntryValue);

    /** set code value.
     *  Before setting the code it is checked (see checkCode()).  If the code is invalid
     *  the current code is not replaced and remains unchanged.
     ** @param  codeValue               computer readable and computer searchable identifier
     *                                  of the code to be set. (VR=SH, mandatory)
     *  @param  codingSchemeDesignator  textual identifier of the table where the 'codeValue'
     *                                  is linked to its 'codeMeaning'. (VR=SH, mandatory)
     *  @param  codeMeaning             human-readable translation of the 'codeValue'.  Used
     *                                  for display when code dictionary is not available.
     *                                  (VR=LO, mandatory)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition setValue(const OFString &codeValue,
                         const OFString &codingSchemeDesignator,
                         const OFString &codeMeaning);

    /** set code value.
     *  Before setting the specified code it is checked (see checkCode()).  If the code is
     *  invalid the current code is not replaced and remains unchanged.
     ** @param  codeValue               computer readable and computer searchable identifier
     *                                  of the code to be set. (VR=SH, mandatory)
     *  @param  codingSchemeDesignator  textual identifier of the table where the 'codeValue'
     *                                  is linked to its 'codeMeaning'. (VR=SH, mandatory)
     *  @param  codingSchemeVersion     version of the coding scheme.  Used when a coding
     *                                  scheme has multiple versions and the 'codingScheme
     *                                  Designator' does not explicitly (or adequately)
     *                                  specify the version number. (VR=SH, optional)
     *  @param  codeMeaning             human-readable translation of the 'codeValue'.  Used
     *                                  for display when code dictionary is not available.
     *                                  (VR=LO, mandatory)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition setValue(const OFString &codeValue,
                         const OFString &codingSchemeDesignator,
                         const OFString &codingSchemeVersion,
                         const OFString &codeMeaning);


  protected:

    /** get pointer to code value
     ** @return pointer to code value (never NULL)
     */
    inline DSRCodedEntryValue *getValuePtr()
    {
        return this;
    }

    /** read code from dataset
     ** @param  dataset    DICOM dataset from which the code should be read
     *  @param  logStream  pointer to error/warning output stream (output disabled if NULL)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition readItem(DcmItem &dataset,
                         OFConsole *logStream);

    /** write code to dataset
     ** @param  dataset    DICOM dataset to which the code should be written
     *  @param  logStream  pointer to error/warning output stream (output disabled if NULL)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition writeItem(DcmItem &dataset,
                          OFConsole *logStream) const;

    /** check the specified code for validity.
     *  Currently the only check that is performed is that the three string values are not empty.
     *  Later on it might be checked whether the specified code really belongs to the specified
     *  coding scheme, etc.  This require the presence of the relevant code dictionaries.
     ** @param  codeValue               code value to be checked
     *  @param  codingSchemeDesignator  coding scheme designator to be checked
     *  @param  codeMeaning             code meaning tobe checked
     ** @return OFTrue if code is valid, OFFalse otherwise
     */
    OFBool checkCode(const OFString &codeValue,
                     const OFString &codingSchemeDesignator,
                     const OFString &codeMeaning) const;


  private:

    /// code value (VR=SH, mandatory)
    OFString CodeValue;
    /// coding scheme designator (VR=SH, mandatory)
    OFString CodingSchemeDesignator;
    /// coding scheme version (VR=SH, optional)
    OFString CodingSchemeVersion;
    /// code meaning (VR=LO, mandatory)
    OFString CodeMeaning;
    /// private coding scheme creator UID.  Used to identify private coding schemes.
    OFString PrivateCodingSchemeCreatorUID;
};


#endif


/*
 *  CVS/RCS Log:
 *  $Log: dsrcodvl.h,v $
 *  Revision 1.2  2000-10-18 17:00:29  joergr
 *  Added methods allowing direct access to certain content item values.
 *  Added doc++ comments.
 *
 *  Revision 1.1  2000/10/13 07:49:23  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
