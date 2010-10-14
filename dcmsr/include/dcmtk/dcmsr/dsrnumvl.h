/*
 *
 *  Copyright (C) 2000-2010, OFFIS e.V.
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
 *  Module: dcmsr
 *
 *  Author: Joerg Riesmeier
 *
 *  Purpose:
 *    classes: DSRNumericMeasurementValue
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:16:32 $
 *  CVS/RCS Revision: $Revision: 1.15 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DSRNUMVL_H
#define DSRNUMVL_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrcodvl.h"

#include "dcmtk/ofstd/ofstring.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class for numeric measurement values
 */
class DSRNumericMeasurementValue
{
    // allow access to getValuePtr()
    friend class DSRContentItem;

  public:

    /** default constructor
     */
    DSRNumericMeasurementValue();

    /** constructor.
     *  The code triple is only set if it passed the validity check (see setValue()).
     ** @param  numericValue     numeric measurement value. (VR=DS, type 1)
     *  @param  measurementUnit  code representing the measurement name (code meaning)
     *                           and unit (code value). (type 2)
     */
    DSRNumericMeasurementValue(const OFString &numericValue,
                               const DSRCodedEntryValue &measurementUnit);

    /** constructor.
     *  The two codes are only set if they passed the validity check (see setValue()).
     ** @param  numericValue     numeric measurement value. (VR=DS, type 1)
     *  @param  measurementUnit  code representing the measurement name (code meaning)
     *                           and unit (code value). (type 2)
     *  @param  valueQualifier   code representing the numeric value qualifier. (type 3)
     */
    DSRNumericMeasurementValue(const OFString &numericValue,
                               const DSRCodedEntryValue &measurementUnit,
                               const DSRCodedEntryValue &valueQualifier);

    /** copy constructor
     ** @param  numericMeasurement  numeric measurement value to be copied (not checked !)
     */
    DSRNumericMeasurementValue(const DSRNumericMeasurementValue &numericMeasurement);

    /** destructor
     */
    virtual ~DSRNumericMeasurementValue();

    /** assignment operator
     ** @param  numericMeasurement  numeric measurement value to be copied (not checked !)
     ** @return reference to this numeric value after 'numericMeasurement' has been copied
     */
    DSRNumericMeasurementValue &operator=(const DSRNumericMeasurementValue &numericMeasurement);

    /** clear all internal variables.
     *  Use this method to create an empty numeric measurement value.
     */
    virtual void clear();

    /** check whether the current numeric measurement value is valid.
     *  The value is valid if isEmpty() is true or all three values (numeric value, measurement
     *  unit and value qualifier) do contain valid values (see checkXXX() methods).
     ** @return OFTrue if value is valid, OFFalse otherwise
     */
    virtual OFBool isValid() const;

    /** check whether the current numeric measurement value is empty.
     *  Checks whether both the numeric value and the measurement unit are empty.
     ** @return OFTrue if value is empty, OFFalse otherwise
     */
    virtual OFBool isEmpty() const;

    /** print numeric measurement value.
     *  The output of a typical numeric measurement value looks like this:
     *  "3" (cm,99_OFFIS_DCMTK,"Length Unit").  If the value is empty the text "empty" is
     *  printed instead.  The numeric value qualifier is never printed.
     ** @param  stream  output stream to which the numeric measurement value should be printed
     *  @param  flags   flag used to customize the output (not used)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition print(STD_NAMESPACE ostream &stream,
                              const size_t flags) const;

    /** read numeric measurement value from XML document
     ** @param  doc     document containing the XML file content
     *  @param  cursor  cursor pointing to the starting node
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition readXML(const DSRXMLDocument &doc,
                                DSRXMLCursor cursor);

    /** write numeric measurement value in XML format
     ** @param  stream     output stream to which the XML document is written
     *  @param  flags      flag used to customize the output (see DSRTypes::XF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition writeXML(STD_NAMESPACE ostream &stream,
                                 const size_t flags) const;

    /** read measured value sequence and numeric value qualifier code sequence from dataset.
     *  The number of items within the sequences is checked.  If error/warning output are
     *  enabled a warning message is printed if a sequence is absent or contains more than
     *  one item.
     ** @param  dataset    DICOM dataset from which the sequences should be read
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition readSequence(DcmItem &dataset);

    /** write measured value sequence and numeric value qualifier code sequence to dataset.
     *  The measured value sequence is always written (might be empty, though).  The numeric
     *  value qualifier code sequence is optional and, therefore, only written if non-empty.
     ** @param  dataset    DICOM dataset to which the sequences should be written
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition writeSequence(DcmItem &dataset) const;

    /** render numeric measurement value in HTML/XHTML format
     ** @param  docStream    output stream to which the main HTML/XHTML document is written
     *  @param  annexStream  output stream to which the HTML/XHTML document annex is written
     *  @param  annexNumber  reference to the variable where the current annex number is stored.
     *                       Value is increased automatically by 1 after a new entry has been added.
     *  @param  flags        flag used to customize the output (see DSRTypes::HF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition renderHTML(STD_NAMESPACE ostream &docStream,
                                   STD_NAMESPACE ostream &annexStream,
                                   size_t &annexNumber,
                                   const size_t flags) const;

    /** get reference to numeric measurement value
     ** @return reference to numeric measurement value
     */
    inline const DSRNumericMeasurementValue &getValue() const
    {
        return *this;
    }

    /** get copy of numeric measurement value
     ** @param  numericMeasurement  reference to variable in which the value should be stored
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition getValue(DSRNumericMeasurementValue &numericMeasurement) const;

    /** get numeric value
     ** @return current numeric value (might be invalid or an empty string)
     */
    inline const OFString &getNumericValue() const
    {
        return NumericValue;
    }

    /** get measurement unit
     ** @return reference to current measurement unit code (might be invalid or empty)
     */
    inline const DSRCodedEntryValue &getMeasurementUnit() const
    {
        return MeasurementUnit;
    }

    /** get numeric value qualifier (optional)
     ** @return reference to current numeric value qualifier code (might be invalid or empty)
     */
    inline const DSRCodedEntryValue &getNumericValueQualifier() const
    {
        return ValueQualifier;
    }

    /** get copy of measurement unit
     ** @param  measurementUnit  reference to variable in which the code should be stored
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition getMeasurementUnit(DSRCodedEntryValue &measurementUnit) const;

    /** set numeric measurement value.
     *  Before setting the value it is checked (see checkXXX()).  If the value is invalid
     *  the current value is not replaced and remains unchanged.
     ** @param  numericMeasurement  value to be set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setValue(const DSRNumericMeasurementValue &numericMeasurement);

    /** set numeric value and measurement unit.
     *  Before setting the values they are checked (see checkXXX()).  If the value pair is
     *  invalid the current value pair is not replaced and remains unchanged.
     ** @param  numericValue     numeric value to be set
     *  @param  measurementUnit  measurement unit to be set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setValue(const OFString &numericValue,
                         const DSRCodedEntryValue &measurementUnit);

    /** set numeric value, measurement unit and numeric value qualifier.
     *  Before setting the values they are checked (see checkXXX()).  If one of the three
     *  values is invalid the current numeric measurement value is not replaced and remains
     *  unchanged.
     ** @param  numericValue     numeric value to be set
     *  @param  measurementUnit  measurement unit to be set
     *  @param  valueQualifier   numeric value qualifier to be set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setValue(const OFString &numericValue,
                         const DSRCodedEntryValue &measurementUnit,
                         const DSRCodedEntryValue &valueQualifier);

    /** set numeric value.
     *  Before setting the value it is checked (see checkNumericValue()).  If the value is
     *  invalid the current value is not replaced and remains unchanged.
     ** @param  numericValue     numeric value to be set (VR=DS)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setNumericValue(const OFString &numericValue);

    /** set measurement unit.
     *  Before setting the code it is checked (see checkMeasurementUnit()).  If the code is
     *  invalid the current code is not replaced and remains unchanged.
     ** @param  measurementUnit  measurement unit to be set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setMeasurementUnit(const DSRCodedEntryValue &measurementUnit);

    /** set numeric value qualifier.
     *  This optional code specifies the qualification of the Numeric Value in the Measured
     *  Value Sequence, or the reason for the absence of the Measured Value Sequence Item.
     *  Before setting the code it is checked (see checkNumericValueQualifier()).  If the
     *  code is invalid the current code is not replaced and remains unchanged.
     ** @param  valueQualifier  numeric value qualifier to be set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setNumericValueQualifier(const DSRCodedEntryValue &valueQualifier);


  protected:

    /** get pointer to numeric measurement value
     ** @return pointer to numeric measurement value (never NULL)
     */
    inline DSRNumericMeasurementValue *getValuePtr()
    {
        return this;
    }

    /** read numeric measurement value from dataset
     ** @param  dataset    DICOM dataset from which the value should be read
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition readItem(DcmItem &dataset);

    /** write numeric measurement value to dataset
     ** @param  dataset    DICOM dataset to which the value should be written
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition writeItem(DcmItem &dataset) const;

    /** check the specified numeric value for validity.
     *  Currently the only check that is performed is that the string is not empty.  Later
     *  on it might be checked whether the format conforms to the definition of DS.
     ** @param  numericValue  numeric value to be checked
     ** @return OFTrue if numeric value is valid, OFFalse otherwise
     */
    virtual OFBool checkNumericValue(const OFString &numericValue) const;

    /** check the specified measurement unit for validity.
     *  The isValid() method in class DSRCodedEntryValue is used for this purpose.
     ** @param  measurementUnit  measurement unit to be checked
     ** @return OFTrue if measurement unit is valid, OFFalse otherwise
     */
    virtual OFBool checkMeasurementUnit(const DSRCodedEntryValue &measurementUnit) const;

    /** check the specified numeric value qualifier for validity.
     *  The isEmpty() and isValid() methods in class DSRCodedEntryValue are used for this
     *  purpose.  The conformance with the Context Group 42 (as defined in the DICOM
     *  standard) is not yet checked.
     ** @param  valueQualifier  numeric value qualifier to be checked
     ** @return OFTrue if value qualifier is valid, OFFalse otherwise
     */
    virtual OFBool checkNumericValueQualifier(const DSRCodedEntryValue &valueQualifier) const;


  private:

    /// numeric value (VR=DS, type 1)
    OFString           NumericValue;
    /// measurement unit (type 2)
    DSRCodedEntryValue MeasurementUnit;
    /// numeric value qualifier (type 3)
    DSRCodedEntryValue ValueQualifier;
};


#endif


/*
 *  CVS/RCS Log:
 *  $Log: dsrnumvl.h,v $
 *  Revision 1.15  2010-10-14 13:16:32  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.14  2009-10-13 14:57:50  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.13  2007-11-15 16:33:30  joergr
 *  Added support for output in XHTML 1.1 format.
 *
 *  Revision 1.12  2006/08/15 16:40:03  meichel
 *  Updated the code in module dcmsr to correctly compile when
 *    all standard C++ classes remain in namespace std.
 *
 *  Revision 1.11  2006/05/11 09:18:21  joergr
 *  Moved containsExtendedCharacters() from dcmsr to dcmdata module.
 *
 *  Revision 1.10  2005/12/08 16:05:11  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.9  2004/11/22 16:39:09  meichel
 *  Added method that checks if the SR document contains non-ASCII characters
 *    in any of the strings affected by SpecificCharacterSet.
 *
 *  Revision 1.8  2003/08/07 12:43:44  joergr
 *  Added readXML functionality.
 *
 *  Revision 1.7  2002/12/10 13:20:07  joergr
 *  Added support for the Numeric Value Qualifier Code Sequence (introduced with
 *  CP 260).
 *
 *  Revision 1.6  2001/09/26 13:04:09  meichel
 *  Adapted dcmsr to class OFCondition
 *
 *  Revision 1.5  2001/06/01 15:51:02  meichel
 *  Updated copyright header
 *
 *  Revision 1.4  2000/11/06 11:17:40  joergr
 *  Moved some protected methods to public part.
 *
 *  Revision 1.3  2000/11/01 16:23:22  joergr
 *  Added support for conversion to XML.
 *
 *  Revision 1.2  2000/10/18 17:05:12  joergr
 *  Added methods allowing direct access to certain content item values.
 *  Added doc++ comments.
 *  Made some functions inline.
 *
 *  Revision 1.1  2000/10/13 07:49:29  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
