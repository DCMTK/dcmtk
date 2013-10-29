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
 *    classes: DSRCompositeReferenceValue
 *
 */


#ifndef DSRCOMVL_H
#define DSRCOMVL_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrtypes.h"

#include "dcmtk/ofstd/ofstring.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class for composite reference values
 */
class DCMTK_DCMSR_EXPORT DSRCompositeReferenceValue
{
    // allow access to getValuePtr()
    friend class DSRContentItem;

  public:

    /** default constructor
     */
    DSRCompositeReferenceValue();

    /** constructor
     ** @param  sopClassUID     referenced SOP class UID of the composite object.
     *                          (VR=UI, mandatory)
     *  @param  sopInstanceUID  referenced SOP instance UID of the composite object.
     *                          (VR=UI, mandatory)
     *  @param  check           if enabled, check 'sopClassUID' and 'sopInstanceUID' for
     *                          validity before setting them.  See checkXXX() for details.
     *                          Empty values are never accepted.
     */
    DSRCompositeReferenceValue(const OFString &sopClassUID,
                               const OFString &sopInstanceUID,
                               const OFBool check = OFTrue);

    /** copy constructor
     ** @param  referenceValue  reference value to be copied (not checked !)
     */
    DSRCompositeReferenceValue(const DSRCompositeReferenceValue &referenceValue);

    /** destructor
     */
    virtual ~DSRCompositeReferenceValue();

    /** assignment operator
     ** @param  referenceValue  reference value to be copied (not checked !)
     ** @return reference to this reference value after 'referenceValue' has been copied
     */
    DSRCompositeReferenceValue &operator=(const DSRCompositeReferenceValue &referenceValue);

    /** clear all internal variables.
     *  Since an empty reference value is invalid the reference becomes invalid afterwards.
     */
    virtual void clear();

    /** check whether the current reference value is valid.
     *  The reference value is valid if SOP class UID and SOP instance UID are valid.  See
     *  checkSOPClassUID() and checkSOPInstanceUID() methods for details.
     ** @return OFTrue if reference value is valid, OFFalse otherwise
     */
    virtual OFBool isValid() const;

    /** check whether the current reference value is empty.
     *  Checks whether both UIDs of the reference value are empty.
     ** @return OFTrue if value is empty, OFFalse otherwise
     */
    virtual OFBool isEmpty() const;

    /** print reference value.
     *  The output of a typical composite reference value looks like this: (BasicTextSR,"1.2.3").
     *  If the SOP class UID is unknown, the UID is printed instead of the related name.
     ** @param  stream  output stream to which the reference value should be printed
     *  @param  flags   flag used to customize the output (see DSRTypes::PF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition print(STD_NAMESPACE ostream &stream,
                              const size_t flags) const;

    /** read reference value from XML document
     ** @param  doc     document containing the XML file content
     *  @param  cursor  cursor pointing to the starting node
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition readXML(const DSRXMLDocument &doc,
                                DSRXMLCursor cursor);

    /** write reference value in XML format
     ** @param  stream  output stream to which the XML document is written
     *  @param  flags   flag used to customize the output (see DSRTypes::XF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition writeXML(STD_NAMESPACE ostream &stream,
                                 const size_t flags) const;

    /** read referenced SOP sequence from dataset.
     *  The number of items within the sequence is checked.  If error/warning output are
     *  enabled, a warning message is printed if the sequence is absent or contains more than
     *  one item.
     ** @param  dataset  DICOM dataset from which the sequence should be read
     *  @param  tagKey   DICOM tag specifying the attribute (= sequence) which should be read
     *  @param  type     value type of the sequence (valid value: "1", "2", something else)
     *                   This parameter is used for checking purpose, any difference is reported.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition readSequence(DcmItem &dataset,
                                     const DcmTagKey &tagKey,
                                     const OFString &type);

    /** write referenced SOP sequence to dataset.
     *  If the value is empty an empty sequence (without any items) is written.
     ** @param  dataset  DICOM dataset to which the sequence should be written
     *  @param  tagKey   DICOM tag specifying the attribute (= sequence) which should be written
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition writeSequence(DcmItem &dataset,
                                      const DcmTagKey &tagKey) const;

    /** render composite reference value in HTML/XHTML format
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

    /** get SOP class UID
     ** @return current SOP class UID (might be invalid or an empty string)
     */
    inline const OFString &getSOPClassUID() const
    {
        return SOPClassUID;
    }

    /** get SOP instance UID
     ** @return current SOP instance UID (might be invalid or an empty string)
     */
    inline const OFString &getSOPInstanceUID() const
    {
        return SOPInstanceUID;
    }

    /** get reference to composite reference value
     ** @return reference to composite reference value
     */
    inline const DSRCompositeReferenceValue &getValue() const
    {
        return *this;
    }

    /** get copy of composite reference value
     ** @param  referenceValue  reference to variable in which the value should be stored
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition getValue(DSRCompositeReferenceValue &referenceValue) const;

    /** set composite reference value.
     *  Before setting the reference, it is usually checked.  If the value is invalid, the
     *  current value is not replaced and remains unchanged.
     ** @param  referenceValue  value to be set
     *  @param  check           if enabled, check value for validity before setting it.
     *                          See checkXXX() for details.  Empty values are never accepted.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setValue(const DSRCompositeReferenceValue &referenceValue,
                         const OFBool check = OFTrue);

    /** set SOP class UID and SOP instance UID value.
     *  Before setting the values, they are usually checked.  If the value pair is invalid
     *  the current value pair is not replaced and remains unchanged.
     ** @param  sopClassUID     referenced SOP class UID to be set. (VR=UI, mandatory)
     *  @param  sopInstanceUID  referenced SOP instance UID to be set. (VR=UI, mandatory)
     *  @param  check           if enabled, check 'sopClassUID' and 'sopInstanceUID' for
     *                          validity before setting them.  See checkXXX() for details.
     *                          Empty values are never accepted.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setReference(const OFString &sopClassUID,
                             const OFString &sopInstanceUID,
                             const OFBool check = OFTrue);

    /** set SOP class UID value.
     *  Before setting the value, it is usually checked.  If the value is invalid, the current
     *  value is not replaced and remains unchanged.
     ** @param  sopClassUID  SOP class UID to be set
     *  @param  check        if enabled, check 'sopClassUID' for validity before setting it.
     *                       See checkSOPClassUID() for details.  Empty values are never
     *                       accepted.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setSOPClassUID(const OFString &sopClassUID,
                               const OFBool check = OFTrue);

    /** set SOP instance UID value.
     *  Before setting the value, it is usually checked.  If the value is invalid, the current
     *  value is not replaced and remains unchanged.
     ** @param  sopInstanceUID  SOP instance UID to be set
     *  @param  check           if enabled, check 'sopInstanceUID' for validity before setting
     *                          it.  See checkSOPInstanceUID() for details.  Empty values are
     *                          never accepted.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setSOPInstanceUID(const OFString &sopInstanceUID,
                                  const OFBool check = OFTrue);


  protected:

    /** get pointer to reference value
     ** @return pointer to reference value (never NULL)
     */
    inline DSRCompositeReferenceValue *getValuePtr()
    {
        return this;
    }

    /** read reference value from dataset
     ** @param  dataset    DICOM dataset from which the value should be read
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition readItem(DcmItem &dataset);

    /** write reference value to dataset
     ** @param  dataset    DICOM dataset to which the value should be written
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition writeItem(DcmItem &dataset) const;

    /** check the specified SOP class UID for validity.
     *  The only checks performed are that the UID is non-empty and that it conforms to the
     *  corresponding VR (UI) and VM (1).  Derived classes should overwrite this method for
     *  more specific tests (e.g. allowing only particular SOP classes).
     ** @param  sopClassUID   SOP class UID to be checked
     ** @return status, EC_Normal if value is valid, an error code otherwise
     */
    virtual OFCondition checkSOPClassUID(const OFString &sopClassUID) const;

    /** check the specified SOP instance UID for validity.
     *  The only checks performed are that the UID is non-empty and that it conforms to the
     *  corresponding VR (UI) and VM (1).  Derived classes should overwrite this method for
     *  more specific tests.
     *  @param  sopInstanceUID  SOP instance UID to be checked
     ** @return status, EC_Normal if value is valid, an error code otherwise
     */
    virtual OFCondition checkSOPInstanceUID(const OFString &sopInstanceUID) const;

    /** check the currently stored reference value for validity.
     *  See above checkXXX() methods for details.
     ** @return status, EC_Normal if current value is valid, an error code otherwise
     */
    OFCondition checkCurrentValue() const;

    /// reference SOP class UID (VR=UI, type 1)
    OFString SOPClassUID;
    /// reference SOP instance UID (VR=UI, type 1)
    OFString SOPInstanceUID;
};


#endif
