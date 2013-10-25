/*
 *
 *  Copyright (C) 2000-2013, OFFIS e.V.
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
 *    classes: DSRStringValue
 *
 */


#ifndef DSRSTRVL_H
#define DSRSTRVL_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrtypes.h"

#include "dcmtk/ofstd/ofstring.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class for string values
 */
class DCMTK_DCMSR_EXPORT DSRStringValue
{

  public:

    /** default constructor
     */
    DSRStringValue();

    /** constructor
     ** @param  stringValue  initial value to be set
     *  @param  check        if enabled, check 'stringValue' for validity before setting it.
     *                       See checkValue() for details.  An empty value is never accepted.
     */
    DSRStringValue(const OFString &stringValue,
                   const OFBool check = OFTrue);

    /** copy constructor
     ** @param  stringValue  string value to be copied (not checked !)
     */
    DSRStringValue(const DSRStringValue &stringValue);

    /** destructor
     */
    virtual ~DSRStringValue();

    /** assignment operator
     ** @param  stringValue  string value to be copied (not checked !)
     ** @return reference to this string value after 'stringValue' has been copied
     */
    DSRStringValue &operator=(const DSRStringValue &stringValue);

    /** clear all internal variables.
     *  Please note that the string value might become invalid afterwards.
     */
    virtual void clear();

    /** check whether the current code is valid.
     *  See checkValue() for details.
     ** @return OFTrue if code is valid, OFFalse otherwise
     */
    virtual OFBool isValid() const;

    /** print string value.
     *  The output of a typical string value looks like this: "Short text" or "Very long t..."
     *  (incl. the quotation marks).
     ** @param  stream     output stream to which the string value should be printed
     *  @param  maxLength  maximum number of characters to be printed.  If the string value is
     *                     longer the output is shortened automatically and three dots "..." are
     *                     added.  The value of 'maxLength' includes these three trailing char's.
     *                     A value of 0 turns this mechanism off (default), i.e. the full string
     *                     value is printed.
     */
    void print(STD_NAMESPACE ostream &stream,
               const size_t maxLength = 0) const;

    /** read string value from dataset.
     *  If error/warning output is enabled, a warning message is printed if the string value
     *  does not conform with the type (1), value multiplicity (1) and/or value representation.
     ** @param  dataset  DICOM dataset from which the string value should be read
     *  @param  tagKey   DICOM tag specifying the attribute which should be read
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition read(DcmItem &dataset,
                     const DcmTagKey &tagKey);

    /** write string value to dataset
     ** @param  dataset  DICOM dataset to which the string value should be written
     *  @param  tagKey   DICOM tag specifying the attribute which should be written
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition write(DcmItem &dataset,
                      const DcmTagKey &tagKey) const;

    /** read string value from XML document
     ** @param  doc       document containing the XML file content
     *  @param  cursor    cursor pointing to the starting node
     *  @param  encoding  use encoding handler if OFTrue, ignore character set otherwise
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition readXML(const DSRXMLDocument &doc,
                        DSRXMLCursor cursor,
                        const OFBool encoding = OFFalse);

    /** render string value in HTML/XHTML format
     ** @param  docStream  output stream to which the main HTML/XHTML document is written
     *  @param  flags      flag used to customize the output (see DSRTypes::HF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition renderHTML(STD_NAMESPACE ostream &docStream,
                           const size_t flags) const;

    /** get string value
     ** @return reference to string value
     */
    inline const OFString &getValue() const
    {
        return Value;
    }

    /** set string value.
     *  Before setting the string value, it is usually checked.  If the value is invalid, the
     *  current value is not replaced and remains unchanged.  Use the clear() method to empty
     *  the string value (which becomes invalid afterwards).
     ** @param  stringValue  value to be set (various VRs, mandatory)
     *  @param  check        check 'stringValue' for conformance with VR and VM if enabled.
     *                       An empty value is never accepted.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setValue(const OFString &stringValue,
                         const OFBool check = OFTrue);


  protected:

    /** check the specified string value for validity.
     *  This base class just checks that the string value is not empty (since all corresponding
     *  DICOM attributes are type 1).  Derived classes should overwrite this method to perform
     *  more sophisticated tests.
     ** @param  stringValue  value to be checked
     ** @return status, EC_Normal if current value is valid, an error code otherwise
     */
    virtual OFCondition checkValue(const OFString &stringValue) const;

    /** check the currently stored string value for validity.
     *  See above checkValue() method for details.
     ** @return status, EC_Normal if value is valid, an error code otherwise
     */
    OFCondition checkCurrentValue() const;


  private:

    /// string value (various VRs, type 1)
    OFString Value;
};


#endif
