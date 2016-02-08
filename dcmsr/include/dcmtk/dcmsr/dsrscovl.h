/*
 *
 *  Copyright (C) 2000-2016, OFFIS e.V.
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
 *    classes: DSRSpatialCoordinatesValue
 *
 */


#ifndef DSRSCOVL_H
#define DSRSCOVL_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrtypes.h"
#include "dcmtk/dcmsr/dsrscogr.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class for spatial coordinate values
 */
class DCMTK_DCMSR_EXPORT DSRSpatialCoordinatesValue
{
    // allow access to getValuePtr()
    friend class DSRContentItem;

  public:

    /** default constructor
     */
    DSRSpatialCoordinatesValue();

    /** constructor
     ** @param  graphicType  graphic type specifying the geometry of the coordinates
     */
    DSRSpatialCoordinatesValue(const DSRTypes::E_GraphicType graphicType);

    /** copy constructor
     ** @param  coordinatesValue  spatial coordinates value to be copied (not checked !)
     */
    DSRSpatialCoordinatesValue(const DSRSpatialCoordinatesValue &coordinatesValue);

    /** destructor
     */
    virtual ~DSRSpatialCoordinatesValue();

    /** assignment operator
     ** @param  coordinatesValue  spatial coordinates value to be copied (not checked !)
     ** @return reference to this spatial coordinates value after 'coordinatesValue' has
     *          been copied
     */
    DSRSpatialCoordinatesValue &operator=(const DSRSpatialCoordinatesValue &coordinatesValue);

    /** clear all internal variables.
     *  Graphic type is set to GT_invalid.  Since an empty list of graphic data is invalid
     *  the spatial coordinates value becomes invalid afterwards.
     */
    virtual void clear();

    /** check whether the current spatial coordinates value is valid.
     *  The value is valid if the graphic type is not GT_invalid and the graphic data is
     *  valid.  See checkGraphicData() method for details.
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

    /** print spatial coordinates.
     *  The output of a typical spatial coordinates value looks like this: (CIRCLE,0/0,255/255).
     ** @param  stream  output stream to which the spatial coordinates value should be printed
     *  @param  flags   flag used to customize the output (see DSRTypes::PF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition print(STD_NAMESPACE ostream &stream,
                              const size_t flags) const;

    /** read spatial coordinates value from dataset
     ** @param  dataset  DICOM dataset from which the value should be read
     *  @param  flags    flag used to customize the reading process (see DSRTypes::RF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition read(DcmItem &dataset,
                             const size_t flags);

    /** write spatial coordinates reference value to dataset
     ** @param  dataset  DICOM dataset to which the value should be written
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition write(DcmItem &dataset) const;

    /** read spatial coordinates value from XML document
     ** @param  doc     document containing the XML file content
     *  @param  cursor  cursor pointing to the starting node
     *  @param  flags   flag used to customize the reading process (see DSRTypes::XF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition readXML(const DSRXMLDocument &doc,
                                DSRXMLCursor cursor,
                                const size_t flags);

    /** write spatial coordinates value in XML format
     ** @param  stream  output stream to which the XML document is written
     *  @param  flags   flag used to customize the output (see DSRTypes::XF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition writeXML(STD_NAMESPACE ostream &stream,
                                 const size_t flags) const;

    /** render spatial coordinates value in HTML/XHTML format
     ** @param  docStream    output stream to which the main HTML/XHTML document is written
     *  @param  annexStream  output stream to which the HTML/XHTML document annex is written
     *  @param  annexNumber  reference to the variable where the current annex number is
     *                       stored.  Value is increased automatically by 1 after a new entry
     *                       has been added.
     *  @param  flags        flag used to customize the output (see DSRTypes::HF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition renderHTML(STD_NAMESPACE ostream &docStream,
                                   STD_NAMESPACE ostream &annexStream,
                                   size_t &annexNumber,
                                   const size_t flags) const;

    /** get copy of spatial coordinates value
     ** @param  coordinatesValue  reference to variable in which the value should be stored
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition getValue(DSRSpatialCoordinatesValue &coordinatesValue) const;

    /** get reference to spatial coordinates value
     ** @return reference to spatial coordinates value
     */
    inline const DSRSpatialCoordinatesValue &getValue() const
    {
        return *this;
    }

    /** get current graphic type.
     *  The graphic type specifies the geometry of the coordinates stored in the graphic data
     *  list.
     ** @return graphic type (might be GT_invalid)
     */
    inline DSRTypes::E_GraphicType getGraphicType() const
    {
        return GraphicType;
    }

    /** get reference to graphic data list.
     *  This list contains an ordered set of (columns,rows) pairs that denote positions in an
     *  image.  The allowed number of pairs is depending on the graphic type.
     ** @return reference to graphic data list
     */
    inline DSRGraphicDataList &getGraphicDataList()
    {
        return GraphicDataList;
    }

    /** get fiducial UID.
     *  Optional - This is the globally unique identifier for this fiducial item.  It can be
     *  used to associate these spatial coordinates with other content items.
     ** @return fiducial UID (might be invalid or an empty string)
     */
    inline const OFString &getFiducialUID() const
    {
        return FiducialUID;
    }

    /** set spatial coordinates value.
     *  Before setting the value, the graphic type and data are usually checked.  If the value
     *  is invalid, the current value is not replaced and remains unchanged.
     ** @param  coordinatesValue  value to be set
     *  @param  check             if enabled, check values for validity before setting them.
     *                            See checkXXX() methods for details.  Empty values are only
     *                            accepted for non-mandatory attributes.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setValue(const DSRSpatialCoordinatesValue &coordinatesValue,
                         const OFBool check = OFTrue);

    /** set current graphic type.
     *  The graphic type specifies the geometry of the coordinates stored in the graphic data
     *  list.
     ** @param  graphicType  graphic type to be set (GT_invalid is not allowed)
     *  @param  check        dummy parameter (currently not used)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setGraphicType(const DSRTypes::E_GraphicType graphicType,
                               const OFBool check = OFTrue);

    /** set current fiducial UID.
     *  Globally unique identifier that can be used to associate these spatial coordinates
     *  with other content items.
     ** @param  fiducialUID  value to be set (VR=UI, optional)
     *  @param  check        if enabled, check value for validity before setting it.  See
     *                       checkFiducialUID() method for details.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setFiducialUID(const OFString &fiducialUID,
                               const OFBool check = OFTrue);


  protected:

    /** get pointer to spatial coordinates value
     ** @return pointer to spatial coordinates value (never NULL)
     */
    inline DSRSpatialCoordinatesValue *getValuePtr()
    {
        return this;
    }

    /** check the specified graphic type and data for validity.
     *  If 'graphicType' is valid, the number of entries in the 'graphicDatalist' are checked.
     *  A POINT needs exactly 1 value pair (column,row), a MULTIPOINT at least 1?, a POLYLINE
     *  at least 1?, a CIRCLE exactly 2 and an ELLIPSE exactly 4.
     ** @param  graphicType      graphic type to be checked
     *  @param  graphicDataList  list of graphic data to be checked
     *  @param  reportWarnings   if enabled, report a warning message on each deviation from
     *                           an expected value to the logger
     ** @return status, EC_Normal if graphic type and data are valid, an error code otherwise
     */
    OFCondition checkGraphicData(const DSRTypes::E_GraphicType graphicType,
                                 const DSRGraphicDataList &graphicDataList,
                                 const OFBool reportWarnings = OFFalse) const;

    /** check the specified fiducial UID for validity
     ** @param  fiducialUID  fiducial UID to be checked
     ** @return status, EC_Normal if fiducial UID is valid, an error code otherwise
     */
    OFCondition checkFiducialUID(const OFString &fiducialUID) const;


  private:

    /// Graphic Type (associated DICOM VR=CS, type 1)
    DSRTypes::E_GraphicType GraphicType;
    /// Graphic Data (associated DICOM VR=FL, VM=2-n, type 1)
    DSRGraphicDataList      GraphicDataList;
    /// Fiducial UID (VR=UI, VM=1, type 3)
    OFString                FiducialUID;
    /// Pixel Origin Interpretation (VR=CS, VM=1, type 1C)
     // - tbd: conditional attribute not yet supported
};


#endif
