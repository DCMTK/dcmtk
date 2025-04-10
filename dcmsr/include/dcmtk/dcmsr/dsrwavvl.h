/*
 *
 *  Copyright (C) 2000-2025, OFFIS e.V.
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
 *    classes: DSRWaveformReferenceValue
 *
 */


#ifndef DSRWAVVL_H
#define DSRWAVVL_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrtypes.h"
#include "dcmtk/dcmsr/dsrcomvl.h"
#include "dcmtk/dcmsr/dsrwavch.h"


// include this file in doxygen documentation

/** @file dsrwavvl.h
 *  @brief definitions for class DSRWaveformReferenceValue
 */


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class for waveform reference values
 */
class DCMTK_DCMSR_EXPORT DSRWaveformReferenceValue
  : public DSRCompositeReferenceValue
{
    // allow access to getValuePtr()
    friend class DSRContentItem;

  public:

    /** default constructor
     */
    DSRWaveformReferenceValue();

    /** constructor
     ** @param  sopClassUID     referenced SOP class UID of the waveform object.
     *                          (VR=UI, mandatory)
     *  @param  sopInstanceUID  referenced SOP instance UID of the waveform object.
     *                          (VR=UI, mandatory)
     *  @param  check           if enabled, check 'sopClassUID' and 'sopInstanceUID' for
     *                          validity before setting them.  See checkXXX() for details.
     *                          Empty values are never accepted.
     */
    DSRWaveformReferenceValue(const OFString &sopClassUID,
                              const OFString &sopInstanceUID,
                              const OFBool check = OFTrue);

    /** copy constructor
     ** @param  referenceValue  waveform reference value to be copied (not checked !)
     */
    DSRWaveformReferenceValue(const DSRWaveformReferenceValue &referenceValue);

    /** destructor
     */
    virtual ~DSRWaveformReferenceValue();

    /** assignment operator
     ** @param  referenceValue  waveform reference value to be copied (not checked !)
     ** @return reference to this waveform reference value after 'referenceValue' has been copied
     */
    DSRWaveformReferenceValue &operator=(const DSRWaveformReferenceValue &referenceValue);

    /** clear all internal variables.
     *  Since an empty waveform reference is invalid the reference becomes invalid afterwards.
     */
    virtual void clear();

    /** check whether values are "equal"
     ** @param  referenceValue  waveform reference value that should be compared to the current one
     ** @return OFTrue if both waveform reference values are equal, OFFalse otherwise
     */
    OFBool isEqual(const DSRWaveformReferenceValue &referenceValue) const;

    /** check whether values are "not equal"
     ** @param  referenceValue  waveform reference value that should be compared to the current one
     ** @return OFTrue if both waveform reference values are not equal, OFFalse otherwise
     */
    OFBool isNotEqual(const DSRWaveformReferenceValue &referenceValue) const;

    /** check whether the content is short.
     *  This method is used to check whether the rendered output of this content item can be
     *  expanded inline or not (used for renderHTML()).
     ** @param  flags  flag used to customize the output (see DSRTypes::HF_xxx)
     ** @return OFTrue if the content is short, OFFalse otherwise
     */
    virtual OFBool isShort(const size_t flags) const;

    /** print waveform reference.
     *  The output of a typical waveform reference value looks like this:
     *  (HemodynamicWaveformStorage,"1.2.3")
     *  If the SOP class UID is unknown, the UID is printed instead of the related name.
     ** @param  stream  output stream to which the image reference value should be printed
     *  @param  flags   flag used to customize the output (see DSRTypes::PF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition print(STD_NAMESPACE ostream &stream,
                              const size_t flags) const;

    /** read waveform reference from XML document
     ** @param  doc     document containing the XML file content
     *  @param  cursor  cursor pointing to the starting node
     *  @param  flags   flag used to customize the reading process (see DSRTypes::XF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition readXML(const DSRXMLDocument &doc,
                                DSRXMLCursor cursor,
                                const size_t flags);

    /** write waveform reference in XML format
     ** @param  stream  output stream to which the XML document is written
     *  @param  flags   flag used to customize the output (see DSRTypes::XF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition writeXML(STD_NAMESPACE ostream &stream,
                                 const size_t flags) const;

    /** render waveform reference value in HTML/XHTML format
     ** @param  docStream    output stream to which the main HTML/XHTML document is written
     *  @param  annexStream  output stream to which the HTML/XHTML document annex is written
     *  @param  annexNumber  reference to the variable where the current annex number is stored.
     *                       Value is increased automatically by 1 after a new entry has been added.
     *  @param  flags        flag used to customize the output (see DSRTypes::HF_xxx)
     *  @param  urlPrefix    optional URL prefix used for hyperlink to referenced composite object
     ** @note Please note that using parameter 'urlPrefix' can lead to security issues, as an
     *        attacker could misuse it to potentially inject dangerous content into the HTML/XHTML
     *        output.  The value of this parameter is not checked.  This is also true for derived
     *        classes.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition renderHTML(STD_NAMESPACE ostream &docStream,
                                   STD_NAMESPACE ostream &annexStream,
                                   size_t &annexNumber,
                                   const size_t flags,
                                   const char *urlPrefix = NULL) const;

    /** get reference to waveform reference value
     ** @return reference to waveform reference value
     */
    inline const DSRWaveformReferenceValue &getValue() const
    {
        return *this;
    }

    /** get copy of waveform reference value
     ** @param  referenceValue  reference to variable in which the value should be stored
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition getValue(DSRWaveformReferenceValue &referenceValue) const;

    /** set waveform reference value.
     *  Before setting the reference, it is usually checked.  If the value is invalid, the
     *  current value is not replaced and remains unchanged.
     ** @param  referenceValue  value to be set
     *  @param  check           if enabled, check value for validity before setting it.
     *                          See checkXXX() for details.  Empty values are only accepted
     *                          for non-mandatory attributes.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setValue(const DSRWaveformReferenceValue &referenceValue,
                         const OFBool check = OFTrue);

    /** get reference to list of referenced waveform channels
     ** @return reference to channel list
     */
    inline DSRWaveformChannelList &getChannelList()
    {
        return ChannelList;
    }

    /** check whether the waveform reference applies to a specific channel.
     *  The waveform reference applies to a channel if the list of referenced waveform
     *  channels is empty or the group/channel pair is part of the list.
     ** @param  multiplexGroupNumber  multiplex group number of the referenced channel
     *  @param  channelNumber         channel number of the referenced channel
     ** @return OFTrue if reference applies to the specified channel, OFFalse otherwise
     */
    OFBool appliesToChannel(const Uint16 multiplexGroupNumber,
                            const Uint16 channelNumber) const;


  protected:

    /** get pointer to waveform reference value
     ** @return pointer to waveform reference value (never NULL)
     */
    inline DSRWaveformReferenceValue *getValuePtr()
    {
        return this;
    }

    /** read waveform reference value from dataset
     ** @param  dataset  DICOM dataset from which the value should be read
     *  @param  flags    flag used to customize the reading process (see DSRTypes::RF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition readItem(DcmItem &dataset,
                                 const size_t flags);

    /** write waveform reference value to dataset
     ** @param  dataset  DICOM dataset to which the value should be written
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition writeItem(DcmItem &dataset) const;

    /** check the specified SOP class UID for validity.
     *  This method further specializes the checks performed in the base class
     *  DSRCompositeReferenceValue.  All waveform SOP classes that are defined
     *  in DICOM PS 3.6-2023c are allowed.
     ** @param  sopClassUID     SOP class UID to be checked
     *  @param  reportWarnings  if enabled, report warning messages to the logger
     ** @return status, EC_Normal if value is valid, an error code otherwise
     */
    virtual OFCondition checkSOPClassUID(const OFString &sopClassUID,
                                         const OFBool reportWarnings = OFFalse) const;


  private:

    /// list of referenced waveform channels (associated DICOM VR=US, VM=2-2n, type 1C)
    DSRWaveformChannelList ChannelList;
};


/*------------------------*
 *  comparison operators  *
 *------------------------*/

/** equality operator.
 *  Internally, the DSRWaveformReferenceValue::isEqual() method is used.
 *  @param  lhs  left-hand side
 *  @param  rhs  right-hand side
 *  @return OFTrue if 'lhs' and 'rhs' are equal, OFFalse otherwise
 */
DCMTK_DCMSR_EXPORT OFBool operator==(const DSRWaveformReferenceValue &lhs,
                                     const DSRWaveformReferenceValue &rhs);

/** inequality operator.
 *  Internally, the DSRWaveformReferenceValue::isNotEqual() method is used.
 *  @param  lhs  left-hand side
 *  @param  rhs  right-hand side
 *  @return OFTrue if 'lhs' and 'rhs' are not equal, OFFalse otherwise
 */
DCMTK_DCMSR_EXPORT OFBool operator!=(const DSRWaveformReferenceValue &lhs,
                                     const DSRWaveformReferenceValue &rhs);


#endif
