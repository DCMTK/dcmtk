/*
 *
 *  Copyright (C) 2000-2003, OFFIS
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
 *    classes: DSRTemporalCoordinatesValue
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2003-08-07 12:53:13 $
 *  CVS/RCS Revision: $Revision: 1.6 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DSRTCOVL_H
#define DSRTCOVL_H

#include "osconfig.h"   /* make sure OS specific configuration is included first */

#include "dsrtypes.h"
#include "dsrtcodt.h"
#include "dsrtcosp.h"
#include "dsrtcoto.h"
#include "dsrxmlc.h"


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
    virtual OFCondition print(ostream &stream,
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
     *  @param  logStream  pointer to error/warning output stream (output disabled if NULL)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition writeXML(ostream &stream,
                                 const size_t flags,
                                 OFConsole *logStream) const;

    /** render temporal coordinates value in HTML format
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
     *  @param  logStream  pointer to error/warning output stream (output disabled if NULL)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition read(DcmItem &dataset,
                             OFConsole *logStream);

    /** write temporal coordinates reference value to dataset.
     *  Please note that only one of the three lists is actually written to the dataset.
     ** @param  dataset    DICOM dataset to which the value should be written
     *  @param  logStream  pointer to error/warning output stream (output disabled if NULL)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition write(DcmItem &dataset,
                              OFConsole *logStream) const;

    /** check the temporal range type and other data for validity.
     *  The data is valid if the 'temporalRangeType' is valid and at least one of the three
     *  lists are non-empty.  If more the one list is non-empty a warning is reported since
     *  they are mutually exclusive (type 1C).
     ** @param  temporalRangeType   temporal range type to be checked
     *  @param  samplePositionList  list of referenced sample positions to be checked
     *  @param  timeOffsetList      list of referenced time offsets to be checked
     *  @param  datetimeList        list of referenced datetime to be checked
     *  @param  logStream           pointer to error/warning output stream (output disabled if NULL)
     ** @return OFTrue if data is valid, OFFalse otherwise
     */
    OFBool checkData(const DSRTypes::E_TemporalRangeType temporalRangeType,
                     const DSRReferencedSamplePositionList &samplePositionList,
                     const DSRReferencedTimeOffsetList &timeOffsetList,
                     const DSRReferencedDatetimeList &datetimeList,
                     OFConsole *logStream = NULL) const;

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
 *  Revision 1.6  2003-08-07 12:53:13  joergr
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
