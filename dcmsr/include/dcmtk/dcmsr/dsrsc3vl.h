/*
 *
 *  Copyright (C) 2010, OFFIS e.V.
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
 *    classes: DSRSpatialCoordinates3DValue
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:16:32 $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DSRSC3VL_H
#define DSRSC3VL_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrtypes.h"
#include "dcmtk/dcmsr/dsrsc3gr.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class for spatial coordinate (3D) values
 */
class DSRSpatialCoordinates3DValue
{
    // allow access to getValuePtr()
    friend class DSRContentItem;

  public:

    /** default contructor
     */
    DSRSpatialCoordinates3DValue();

    /** constructor
     ** @param  graphicType  graphic type specifying the geometry of the coordinates
     */
    DSRSpatialCoordinates3DValue(const DSRTypes::E_GraphicType3D graphicType);

    /** copy constructor
     ** @param  coordinatesValue  spatial coordinates value to be copied (not checked !)
     */
    DSRSpatialCoordinates3DValue(const DSRSpatialCoordinates3DValue &coordinatesValue);

    /** destructor
     */
    virtual ~DSRSpatialCoordinates3DValue();

    /** assignment operator
     ** @param  coordinatesValue  spatial coordinates value to be copied (not checked !)
     ** @return reference to this spatial coordinates value after 'coordinatesValue' has
     *          been copied
     */
    DSRSpatialCoordinates3DValue &operator=(const DSRSpatialCoordinates3DValue &coordinatesValue);

    /** clear all internal variables.
     *  Graphic type is set to GT3_invalid.  Since an empty list of graphic data is invalid
     *  the spatial coordinates value becomes invalid afterwards.
     */
    virtual void clear();

    /** check whether the current spatial coordinates value is valid.
     *  The value is valid if the graphic type is not GT3_invalid and the graphic data is
     *  valid (see checkData() for details).
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
     *  The output of a typical spatial coordinates value looks like this: (POINT,,100/100/100).
     ** @param  stream  output stream to which the spatial coordinates value should be printed
     *  @param  flags   flag used to customize the output (see DSRTypes::PF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition print(STD_NAMESPACE ostream &stream,
                              const size_t flags) const;

    /** read spatial coordinates value from dataset
     ** @param  dataset  DICOM dataset from which the value should be read
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition read(DcmItem &dataset);

    /** write spatial coordinates reference value to dataset
     ** @param  dataset  DICOM dataset to which the value should be written
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition write(DcmItem &dataset) const;

    /** read spatial coordinates value from XML document
     ** @param  doc     document containing the XML file content
     *  @param  cursor  cursor pointing to the starting node
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition readXML(const DSRXMLDocument &doc,
                                DSRXMLCursor cursor);

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
     *  @param  annexNumber  reference to the variable where the current annex number is stored.
     *                       Value is increased automatically by 1 after a new entry has been added.
     *  @param  flags        flag used to customize the output (see DSRTypes::HF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition renderHTML(STD_NAMESPACE ostream &docStream,
                                   STD_NAMESPACE ostream &annexStream,
                                   size_t &annexNumber,
                                   const size_t flags) const;

    /** get reference to spatial coordinates value
     ** @return reference to spatial coordinates value
     */
    inline const DSRSpatialCoordinates3DValue &getValue() const
    {
        return *this;
    }

    /** get current graphic type.
     *  The graphic type specifies the geometry of the coordinates stored in the graphic data
     *  list.
     ** @return graphic type (might be GT3_invalid)
     */
    inline DSRTypes::E_GraphicType3D getGraphicType() const
    {
        return GraphicType;
    }

    /** set current graphic type.
     *  The graphic type specifies the geometry of the coordinates stored in the graphic data
     *  list.
     ** @param  graphicType  graphic type to be set (GT3_invalid is not allowed)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setGraphicType(const DSRTypes::E_GraphicType3D graphicType);

    /** get referenced frame of reference UID
     ** @return referenced frame of reference UID (might be empty)
     */
    inline const OFString &getFrameOfReferenceUID() const
    {
        return FrameOfReferenceUID;
    }

    /** set current referenced frame of reference UID
     ** @param  frameOfReferenceUID  referenced frame of reference UID to be set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setFrameOfReferenceUID(const OFString &frameOfReferenceUID);

    /** get copy of spatial coordinates value
     ** @param  coordinatesValue  reference to variable in which the value should be stored
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition getValue(DSRSpatialCoordinates3DValue &coordinatesValue) const;

    /** set spatial coordinates value.
     *  Before setting the value the graphic type, graphic data and frame of reference UID
     *  are checked (see checkData()).  If the value is invalid the current value is not
     *  replaced and remains unchanged.
     ** @param  coordinatesValue  value to be set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setValue(const DSRSpatialCoordinates3DValue &coordinatesValue);

    /** get reference to graphic data list.
     *  This list contains an ordered set of (x,y,z) triplets that denote positions in
     *  an image.  The allowed number of triplets is depending on the graphic type.
     ** @return reference to graphic data list
     */
    inline DSRGraphicData3DList &getGraphicDataList()
    {
        return GraphicDataList;
    }


  protected:

    /** get pointer to spatial coordinates value
     ** @return pointer to spatial coordinates value (never NULL)
     */
    inline DSRSpatialCoordinates3DValue *getValuePtr()
    {
        return this;
    }

    /** check the graphic type, graphic data and frame of reference UID for validity.
     *  If 'graphicType' is valid the number of entries in the 'graphicDatalist' are checked.
     *  A POINT needs exactly 1 value triplets (x,y,z), a MULTIPOINT at least 1?, a POLYLINE
     *  at least 1?, a POLYGON at least 1? where the first and last triplet are equal, an
     *  ELLIPSE exactly 4 and an ELLIPSOID exactly 6.
     ** @param  graphicType          graphic type to be checked
     *  @param  graphicDataList      list of graphic data to be checked
     *  @param  frameOfReferenceUID  referenced frame of reference UID to be checked
     ** @return OFTrue if graphic type and data are valid, OFFalse otherwise
     */
    OFBool checkData(const DSRTypes::E_GraphicType3D graphicType,
                     const DSRGraphicData3DList &graphicDataList,
                     const OFString &frameOfReferenceUID) const;


  private:

    /// graphic type (associated DICOM VR=CS, type 1)
    DSRTypes::E_GraphicType3D GraphicType;
    /// graphic data (associated DICOM VR=FL, VM=2-n, type 1)
    DSRGraphicData3DList      GraphicDataList;
    /// referenced frame of reference UID (associated DICOM VR=UI, VM=1, type 1)
    OFString                  FrameOfReferenceUID;
};


#endif


/*
 *  CVS/RCS Log:
 *  $Log: dsrsc3vl.h,v $
 *  Revision 1.2  2010-10-14 13:16:32  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.1  2010-09-28 14:08:14  joergr
 *  Added support for Colon CAD SR which requires a new value type (SCOORD3D).
 *
 *
 */
