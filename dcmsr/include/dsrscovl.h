/*
 *
 *  Copyright (C) 2000, OFFIS
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
 *    classes: DSRSpatialCoordinatesValue
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2000-10-18 17:07:30 $
 *  CVS/RCS Revision: $Revision: 1.3 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DSRSCOVL_H
#define DSRSCOVL_H

#include "osconfig.h"   /* make sure OS specific configuration is included first */

#include "dsrtypes.h"
#include "dsrscogr.h"


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
     */
    virtual E_Condition print(ostream &stream,
                              const size_t flags) const;

    /** render spatial coordinates value in HTML format
     ** @param  docStream    output stream to which the main HTML document is written
     *  @param  annexStream  output stream to which the HTML document annex is written
     *  @param  annexNumber  reference to the variable where the current annex number is stored.
     *                       Value is increased automatically by 1 after a new entry has been added.
     *  @param  flags        flag used to customize the output (see DSRTypes::HF_xxx)
     *  @param  logStream    pointer to error/warning output stream (output disabled if NULL)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual E_Condition renderHTML(ostream &docStream,
                                   ostream &annexStream,
                                   size_t &annexNumber,
                                   const size_t flags,
                                   OFConsole *logStream) const;

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
    E_Condition setGraphicType(const DSRTypes::E_GraphicType graphicType);

    /** get copy of spatial coordinates value
     ** @param  coordinatesValue  reference to variable in which the value should be stored
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition getValue(DSRSpatialCoordinatesValue &coordinatesValue) const;

    /** set spatial coordinates value.
     *  Before setting the value the graphic type and data are checked (see checkData()).
     *  If the value is invalid the current value is not replaced and remains unchanged.
     ** @param  coordinatesValue  value to be set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition setValue(const DSRSpatialCoordinatesValue &coordinatesValue);

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

    /** read spatial coordinates value from dataset
     ** @param  dataset    DICOM dataset from which the value should be read
     *  @param  logStream  pointer to error/warning output stream (output disabled if NULL)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual E_Condition read(DcmItem &dataset,
                             OFConsole *logStream);

    /** write spatial coordinates reference value to dataset
     ** @param  dataset    DICOM dataset to which the value should be written
     *  @param  logStream  pointer to error/warning output stream (output disabled if NULL)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual E_Condition write(DcmItem &dataset,
                              OFConsole *logStream) const;
                              
    /** check the graphic type and data for validity.
     *  If 'graphicType' is valid the number of entries in the 'graphicDatalist' are checked.
     *  A POINT needs exactly 1 value pair (column,row), a MULTIPOINT at least 1?, a closed 
     *  POLYLINE at least 1? where the first and last pair are equal, a CIRCLE exactly 2 and an
     *  ELLIPSE exactly 4.
     ** @param  graphicType      graphic type to be checked
     *  @param  graphicDataList  list of graphic data to be checked
     *  @param  logStream        pointer to error/warning output stream (output disabled if NULL)
     ** @return OFTrue if graphic type and data are valid, OFFalse otherwise
     */
    OFBool checkData(const DSRTypes::E_GraphicType graphicType,
                     const DSRGraphicDataList &graphicDataList,
                     OFConsole *logStream = NULL) const;


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
 *  Revision 1.3  2000-10-18 17:07:30  joergr
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
