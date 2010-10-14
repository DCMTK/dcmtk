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
 *    classes: DSRTemporalCoordinatesValue
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:16:33 $
 *  CVS/RCS Revision: $Revision: 1.12 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
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

/** Class for spatial coordinate values
 */
class DSRTemporalCoordinatesValue
{
    // allow access to getValuePtr()
    friend class DSRContentItem;

  public:

    /** default contructor
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
     *  invalid the spatial coordinates value becomes invalid afterwards.
     */
    virtual void clear();

    /** check whether the current spatial coordinates value is valid.
     *  The value is valid if the temporal range type is not TRT_invalid and the other data
     *  is valid (see checkData() for details).
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
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition readXML(const DSRXMLDocument &doc,
                                DSRXMLCursor cursor);

    /** write temporal coordinates value in XML format
     ** @param  stream     output stream to which the XML document is written
     *  @param  flags      flag used to customize the output (see DSRTypes::XF_xxx)
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
     *  Before setting the value the temporal range type and other data are checked (see
     *  checkData()).  If the value is invalid the current value is not replaced and remains
     *  unchanged.
     ** @param  coordinatesValue  value to be set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setValue(const DSRTemporalCoordinatesValue &coordinatesValue);

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
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setTemporalRangeType(const DSRTypes::E_TemporalRangeType temporalRangeType);

    /** get reference to list of referenced datetime
     ** @return reference to list
     */
    inline DSRReferencedDatetimeList &getDatetimeList()
    {
        return DatetimeList;
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
     *  one list is present a warning messsage is reported.
     ** @param  dataset    DICOM dataset from which the value should be read
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition read(DcmItem &dataset);

    /** write temporal coordinates reference value to dataset.
     *  Please note that only one of the three lists is actually written to the dataset.
     ** @param  dataset    DICOM dataset to which the value should be written
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition write(DcmItem &dataset) const;

    /** check the temporal range type and other data for validity.
     *  The data is valid if the 'temporalRangeType' is valid and at least one of the three
     *  lists are non-empty.  If more the one list is non-empty a warning is reported since
     *  they are mutually exclusive (type 1C).
     ** @param  temporalRangeType   temporal range type to be checked
     *  @param  samplePositionList  list of referenced sample positions to be checked
     *  @param  timeOffsetList      list of referenced time offsets to be checked
     *  @param  datetimeList        list of referenced datetime to be checked
     ** @return OFTrue if data is valid, OFFalse otherwise
     */
    OFBool checkData(const DSRTypes::E_TemporalRangeType temporalRangeType,
                     const DSRReferencedSamplePositionList &samplePositionList,
                     const DSRReferencedTimeOffsetList &timeOffsetList,
                     const DSRReferencedDatetimeList &datetimeList) const;

  private:

    /// temporal range type (associated DICOM VR=CS, type 1)
    DSRTypes::E_TemporalRangeType   TemporalRangeType;

    /// list of referenced sample positions (associated DICOM VR=UL, VM=1-n, type 1C)
    DSRReferencedSamplePositionList SamplePositionList;
    /// list of referenced time offsets (associated DICOM VR=DS, VM=1-n, type 1C)
    DSRReferencedTimeOffsetList     TimeOffsetList;
    /// list of referenced datetime (associated DICOM VR=DT, VM=1-n, type 1C)
    DSRReferencedDatetimeList       DatetimeList;
};


#endif


/*
 *  CVS/RCS Log:
 *  $Log: dsrtcovl.h,v $
 *  Revision 1.12  2010-10-14 13:16:33  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.11  2009-10-13 14:57:50  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.10  2007-11-15 16:33:30  joergr
 *  Added support for output in XHTML 1.1 format.
 *
 *  Revision 1.9  2006/08/15 16:40:03  meichel
 *  Updated the code in module dcmsr to correctly compile when
 *    all standard C++ classes remain in namespace std.
 *
 *  Revision 1.8  2005/12/08 16:05:24  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.7  2003/08/07 18:01:42  joergr
 *  Removed libxml dependency from header files.
 *
 *  Revision 1.6  2003/08/07 12:53:13  joergr
 *  Added readXML functionality.
 *
 *  Revision 1.5  2001/09/26 13:04:12  meichel
 *  Adapted dcmsr to class OFCondition
 *
 *  Revision 1.4  2001/06/01 15:51:05  meichel
 *  Updated copyright header
 *
 *  Revision 1.3  2000/11/06 11:20:32  joergr
 *  Moved some protected methods to public part.
 *
 *  Revision 1.2  2000/11/01 16:23:26  joergr
 *  Added support for conversion to XML.
 *
 *  Revision 1.1  2000/10/26 14:23:26  joergr
 *  Added support for TCOORD content item.
 *
 *
 */
