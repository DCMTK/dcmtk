/*
 *
 *  Copyright (C) 2000-2001, OFFIS
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
 *    classes: DSRNumericMeasurementValue
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2001-09-26 13:04:09 $
 *  CVS/RCS Revision: $Revision: 1.6 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DSRNUMVL_H
#define DSRNUMVL_H

#include "osconfig.h"   /* make sure OS specific configuration is included first */

#include "dsrcodvl.h"

#include "ofstring.h"


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

    /** default contructor
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

    /** copy constructor.
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
     *  The value is valid if both numeric value and measurement unit are empty or do contain
     *  itself valid values (see check...() methods).
     ** @return OFTrue if value is valid, OFFalse otherwise
     */
    virtual OFBool isValid() const;

    /** check whether the current numeric measurement value is empty.
     *  Checks whether both components of the numeric measurement value are empty.
     ** @return OFTrue if value is empty, OFFalse otherwise
     */
    virtual OFBool isEmpty() const;

    /** print numeric measurement value.
     *  The output of a typical numeric measurement value looks like this:
     *  "3" (cm,99_OFFIS_DCMTK,"Length Unit").  If the value is empty the text "empty" is
     *  printed instead.
     ** @param  stream  output stream to which the numeric measurement value should be printed
     *  @param  flags   flag used to customize the output (not used)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition print(ostream &stream,
                              const size_t flags) const;

    /** write numeric measurement value in XML format
     ** @param  stream     output stream to which the XML document is written
     *  @param  flags      flag used to customize the output (see DSRTypes::XF_xxx)
     *  @param  logStream  pointer to error/warning output stream (output disabled if NULL)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition writeXML(ostream &stream,
                                 const size_t flags,
                                 OFConsole *logStream) const;

    /** read measured value sequence from dataset.
     *  The number of items within the sequence is checked.  If error/warning output are
     *  enabled a warning message is printed if the sequence is absent or contains more than
     *  one item.
     ** @param  dataset    DICOM dataset from which the sequence should be read
     *  @param  logStream  pointer to error/warning output stream (output disabled if NULL)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition readSequence(DcmItem &dataset,
                                     OFConsole *logStream);

    /** write measured value sequence to dataset.
     *  If the value is empty an empty sequence (without any items) is written.
     ** @param  dataset    DICOM dataset to which the sequence should be written
     *  @param  logStream  pointer to error/warning output stream (output disabled if NULL)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition writeSequence(DcmItem &dataset,
                                      OFConsole *logStream) const;

    /** render numeric measurement value in HTML format
     ** @param  docStream    output stream to which the main HTML document is written
     *  @param  annexStream  output stream to which the HTML document annex is written
     *  @param  annexNumber  reference to the variable where the current annex number is stored.
     *                       Value is increased automatically by 1 after a new entry has been added.
     *  @param  flags        flag used to customize the output (see DSRTypes::HF_xxx)
     *  @param  logStream    pointer to error/warning output stream (output disabled if NULL)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition renderHTML(ostream &docStream,
                                   ostream &annexStream,
                                   size_t &annexNumber,
                                   const size_t flags,
                                   OFConsole *logStream) const;

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

    /** get copy of measurement unit
     ** @param  measurementUnit  reference to variable in which the code should be stored
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition getMeasurementUnit(DSRCodedEntryValue &measurementUnit) const;

    /** set numeric measurement value.
     *  Before setting the value it is checked (see check...()).  If the value is invalid
     *  the current value is not replaced and remains unchanged.
     ** @param  numericMeasurement  value to be set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setValue(const DSRNumericMeasurementValue &numericMeasurement);

    /** set numeric value and measurement unit.
     *  Before setting the values they are checked (see check...()).  If the value pair is
     *  invalid the current value pair is not replaced and remains unchanged.
     ** @param  numericValue     numeric value to be set
     *  @param  measurementUnit  measurement unit to be set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setValue(const OFString &numericValue,
                         const DSRCodedEntryValue &measurementUnit);

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
     *  @param  logStream  pointer to error/warning output stream (output disabled if NULL)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition readItem(DcmItem &dataset,
                                 OFConsole *logStream);

    /** write numeric measurement value to dataset
     ** @param  dataset    DICOM dataset to which the value should be written
     *  @param  logStream  pointer to error/warning output stream (output disabled if NULL)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition writeItem(DcmItem &dataset,
                                  OFConsole *logStream) const;

    /** check the specified numeric value for validity.
     *  Currently the only check that is performed is that the string is not empty.  Later
     *  on it might be checked whether the format conforms to the definition of DS.
     ** @param  numericValue  numeric value to be checked
     ** @return OFTrue if numeric value is valid, OFFalse otherwise
     */
    virtual OFBool checkNumericValue(const OFString &numericValue) const;

    /** check the specified measurement unit for validity.
     *  The checkCode() method in class DSRCodedEntryValue is used for this purpose.
     ** @param  measurementUnit  measurement unit to be checked
     ** @return OFTrue if measurement unit is valid, OFFalse otherwise
     */
    virtual OFBool checkMeasurementUnit(const DSRCodedEntryValue &measurementUnit) const;


  private:

    /// numeric value (VR=DS, type 1)
    OFString           NumericValue;
    /// measurement unit (type 2)
    DSRCodedEntryValue MeasurementUnit;
};


#endif


/*
 *  CVS/RCS Log:
 *  $Log: dsrnumvl.h,v $
 *  Revision 1.6  2001-09-26 13:04:09  meichel
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
