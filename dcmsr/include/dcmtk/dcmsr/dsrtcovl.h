/*
 *
 *  Copyright (C) 2000-2015, OFFIS e.V.
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
 *    classes: DSRTemporalCoordinatesValue
 *
 */


#ifndef DSRTCOVL_H
#define DSRTCOVL_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrtypes.h"
#include "dcmtk/dcmsr/dsrtcodt.h"
#include "dcmtk/dcmsr/dsrtcosp.h"
#include "dcmtk/dcmsr/dsrtcoto.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class for temporal coordinates values
 */
class DCMTK_DCMSR_EXPORT DSRTemporalCoordinatesValue
{
    // allow access to getValuePtr()
    friend class DSRContentItem;

  public:

    /** default constructor
     */
    DSRTemporalCoordinatesValue();

    /** constructor
     ** @param  temporalRangeType  type of temporal extent of the region of interest
     */
    DSRTemporalCoordinatesValue(const DSRTypes::E_TemporalRangeType temporalRangeType);

    /** copy constructor
     ** @param  coordinatesValue  temporal coordinates value to be copied (not checked !)
     */
    DSRTemporalCoordinatesValue(const DSRTemporalCoordinatesValue &coordinatesValue);

    /** destructor
     */
    virtual ~DSRTemporalCoordinatesValue();

    /** assignment operator
     ** @param  coordinatesValue  Temporal coordinates value to be copied (not checked !)
     ** @return reference to this Temporal coordinates value after 'coordinatesValue' has
     *          been copied
     */
    DSRTemporalCoordinatesValue &operator=(const DSRTemporalCoordinatesValue &coordinatesValue);

    /** clear all internal variables.
     *  Temporal range type is set to TRT_invalid.  Since an empty list of graphic data is
     *  invalid the temporal coordinates value becomes invalid afterwards.
     */
    virtual void clear();

    /** check whether the current temporal coordinates value is valid.
     *  The value is valid if the temporal range type is not TRT_invalid and the other data
     *  is valid.  See checkData() method for details.
     ** @return OFTrue if reference value is valid, OFFalse otherwise
     */
    virtual OFBool isValid() const;

    /** check whether the content is short.
     *  This method is used to check whether the rendered output of this content item can be
     *  expanded inline or not (used for renderHTML()).
     ** @param  flags  flag used to customize the output (see DSRTypes::HF_xxx)
     ** @return OFTrue if the content is short, OFFalse otherwise
     */
    virtual OFBool isShort(const size_t flags) const;

    /** print temporal coordinates.
     *  The output of a typical temporal coordinates value looks like this (depending on the
     *  referenced data list): (SEGMENT,1,2,3) or (SEGMENT,1,2.5,3.1) or (POINT,20001010120000)
     ** @param  stream  output stream to which the temporal coordinates value should be printed
     *  @param  flags   flag used to customize the output (see DSRTypes::PF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition print(STD_NAMESPACE ostream &stream,
                              const size_t flags) const;

    /** read temporal coordinates value from XML document
     ** @param  doc     document containing the XML file content
     *  @param  cursor  cursor pointing to the starting node
     *  @param  flags   flag used to customize the reading process (see DSRTypes::XF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition readXML(const DSRXMLDocument &doc,
                                DSRXMLCursor cursor,
                                const size_t flags);

    /** write temporal coordinates value in XML format
     ** @param  stream  output stream to which the XML document is written
     *  @param  flags   flag used to customize the output (see DSRTypes::XF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition writeXML(STD_NAMESPACE ostream &stream,
                                 const size_t flags) const;

    /** render temporal coordinates value in HTML/XHTML format
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

    /** get reference to temporal coordinates value
     ** @return reference to temporal coordinates value
     */
    inline const DSRTemporalCoordinatesValue &getValue() const
    {
        return *this;
    }

    /** get copy of temporal coordinates value
     ** @param  coordinatesValue  reference to variable in which the value should be stored
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition getValue(DSRTemporalCoordinatesValue &coordinatesValue) const;

    /** set temporal coordinates value.
     *  Before setting the value, the temporal range type and other data are checked (see
     *  checkData()).  If the value is invalid, the current value is not replaced and remains
     *  unchanged.
     ** @param  coordinatesValue  value to be set
     *  @param  check             if enabled, check values for validity before setting them.
     *                            See checkData() method for details.  Empty values are only
     *                            accepted for non-mandatory attributes.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setValue(const DSRTemporalCoordinatesValue &coordinatesValue,
                         const OFBool check = OFTrue);

    /** get current temporal range type.
     *  This value represents the type of temporal extent of the region of interest.
     ** @return region type (might be TRT_invalid)
     */
    inline DSRTypes::E_TemporalRangeType getTemporalRangeType() const
    {
        return TemporalRangeType;
    }

    /** set current temporal range type.
     *  This value represents the type of temporal extent of the region of interest.
     ** @param  temporalRangeType  temporal range type to be set (TRT_invalid is not allowed)
     *  @param  check              dummy parameter (currently not used)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setTemporalRangeType(const DSRTypes::E_TemporalRangeType temporalRangeType,
                                     const OFBool check = OFTrue);

    /** get reference to list of referenced date/time
     ** @return reference to list
     */
    inline DSRReferencedDateTimeList &getDateTimeList()
    {
        return DateTimeList;
    }

    /** get reference to list of referenced sample positions
     ** @return reference to list
     */
    inline DSRReferencedSamplePositionList &getSamplePositionList()
    {
        return SamplePositionList;
    }

    /** get reference to list of referenced time offsets
     ** @return reference to list
     */
    inline DSRReferencedTimeOffsetList &getTimeOffsetList()
    {
        return TimeOffsetList;
    }


  protected:

    /** get pointer to temporal coordinates value
     ** @return pointer to temporal coordinates value (never NULL)
     */
    inline DSRTemporalCoordinatesValue *getValuePtr()
    {
        return this;
    }

    /** read temporal coordinates value from dataset.
     *  Please note that all three lists are (tried to) read from the dataset.  If more than
     *  one list is present a warning message is reported.
     ** @param  dataset  DICOM dataset from which the value should be read
     *  @param  flags    flag used to customize the reading process (see DSRTypes::RF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition read(DcmItem &dataset,
                             const size_t flags);

    /** write temporal coordinates reference value to dataset.
     *  Please note that only one of the three lists is actually written to the dataset.
     ** @param  dataset  DICOM dataset to which the value should be written
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition write(DcmItem &dataset) const;

    /** check the temporal range type and other data for validity.
     *  The data is valid if the 'temporalRangeType' is valid and at least one of the three
     *  lists are non-empty.  If more than one list is non-empty, a warning is reported (if
     *  enabled) since they are mutually exclusive (type 1C).
     ** @param  temporalRangeType   temporal range type to be checked
     *  @param  samplePositionList  list of referenced sample positions to be checked
     *  @param  timeOffsetList      list of referenced time offsets to be checked
     *  @param  dateTimeList        list of referenced date/time to be checked
     *  @param  reportWarnings      if enabled, report a warning message on each deviation
     *                              from an expected value to the logger
     ** @return status, EC_Normal if checked data is valid, an error code otherwise
     */
    OFCondition checkData(const DSRTypes::E_TemporalRangeType temporalRangeType,
                          const DSRReferencedSamplePositionList &samplePositionList,
                          const DSRReferencedTimeOffsetList &timeOffsetList,
                          const DSRReferencedDateTimeList &dateTimeList,
                          const OFBool reportWarnings = OFFalse) const;


  private:

    /// temporal range type (associated DICOM VR=CS, type 1)
    DSRTypes::E_TemporalRangeType   TemporalRangeType;

    /// list of referenced sample positions (associated DICOM VR=UL, VM=1-n, type 1C)
    DSRReferencedSamplePositionList SamplePositionList;
    /// list of referenced time offsets (associated DICOM VR=DS, VM=1-n, type 1C)
    DSRReferencedTimeOffsetList     TimeOffsetList;
    /// list of referenced date/time (associated DICOM VR=DT, VM=1-n, type 1C)
    DSRReferencedDateTimeList       DateTimeList;
};


#endif
