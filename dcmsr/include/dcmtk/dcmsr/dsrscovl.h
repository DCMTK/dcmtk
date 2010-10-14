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
 *    classes: DSRSpatialCoordinatesValue
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:16:33 $
 *  CVS/RCS Revision: $Revision: 1.15 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
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
class DSRSpatialCoordinatesValue
{
    // allow access to getValuePtr()
    friend class DSRContentItem;

  public:

    /** default contructor
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
     *  The output of a typical spatial coordinates value looks like this: (CIRCLE,0/0,255/255).
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

    /** set current graphic type.
     *  The graphic type specifies the geometry of the coordinates stored in the graphic data
     *  list.
     ** @param  graphicType  graphic type to be set (GT_invalid is not allowed)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setGraphicType(const DSRTypes::E_GraphicType graphicType);

    /** get copy of spatial coordinates value
     ** @param  coordinatesValue  reference to variable in which the value should be stored
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition getValue(DSRSpatialCoordinatesValue &coordinatesValue) const;

    /** set spatial coordinates value.
     *  Before setting the value the graphic type and data are checked (see checkData()).
     *  If the value is invalid the current value is not replaced and remains unchanged.
     ** @param  coordinatesValue  value to be set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setValue(const DSRSpatialCoordinatesValue &coordinatesValue);

    /** get reference to graphic data list.
     *  This list contains an ordered set of (columns,rows) pairs that denote positions in
     *  an image.  The allowed number of pairs is depending on the graphic type.
     ** @return reference to graphic data list
     */
    inline DSRGraphicDataList &getGraphicDataList()
    {
        return GraphicDataList;
    }


  protected:

    /** get pointer to spatial coordinates value
     ** @return pointer to spatial coordinates value (never NULL)
     */
    inline DSRSpatialCoordinatesValue *getValuePtr()
    {
        return this;
    }

    /** check the graphic type and data for validity.
     *  If 'graphicType' is valid the number of entries in the 'graphicDatalist' are checked.
     *  A POINT needs exactly 1 value pair (column,row), a MULTIPOINT at least 1?, a POLYLINE
     *  at least 1?, a CIRCLE exactly 2 and an ELLIPSE exactly 4.
     ** @param  graphicType      graphic type to be checked
     *  @param  graphicDataList  list of graphic data to be checked
     ** @return OFTrue if graphic type and data are valid, OFFalse otherwise
     */
    OFBool checkData(const DSRTypes::E_GraphicType graphicType,
                     const DSRGraphicDataList &graphicDataList) const;


  private:

    /// graphic type (associated DICOM VR=CS, type 1)
    DSRTypes::E_GraphicType GraphicType;
    /// graphic data (associated DICOM VR=FL, VM=2-n, type 1)
    DSRGraphicDataList      GraphicDataList;
};


#endif


/*
 *  CVS/RCS Log:
 *  $Log: dsrscovl.h,v $
 *  Revision 1.15  2010-10-14 13:16:33  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.14  2010-09-28 14:03:37  joergr
 *  Updated comment on the requirements for POLYLINE according to CP-233.
 *
 *  Revision 1.13  2009-10-13 14:57:50  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.12  2007-11-15 16:33:30  joergr
 *  Added support for output in XHTML 1.1 format.
 *
 *  Revision 1.11  2006/08/15 16:40:03  meichel
 *  Updated the code in module dcmsr to correctly compile when
 *    all standard C++ classes remain in namespace std.
 *
 *  Revision 1.10  2005/12/08 16:05:17  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.9  2003/08/07 18:01:42  joergr
 *  Removed libxml dependency from header files.
 *
 *  Revision 1.8  2003/08/07 12:47:10  joergr
 *  Added readXML functionality.
 *
 *  Revision 1.7  2001/09/26 13:04:11  meichel
 *  Adapted dcmsr to class OFCondition
 *
 *  Revision 1.6  2001/06/01 15:51:03  meichel
 *  Updated copyright header
 *
 *  Revision 1.5  2000/11/06 11:18:09  joergr
 *  Moved some protected methods to public part.
 *
 *  Revision 1.4  2000/11/01 16:23:24  joergr
 *  Added support for conversion to XML.
 *
 *  Revision 1.3  2000/10/18 17:07:30  joergr
 *  Added methods allowing direct access to certain content item values.
 *  Added doc++ comments.
 *  Made some functions inline.
 *
 *  Revision 1.2  2000/10/16 11:57:23  joergr
 *  Added methods allowing direct access to certain content item values.
 *
 *  Revision 1.1  2000/10/13 07:49:32  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
