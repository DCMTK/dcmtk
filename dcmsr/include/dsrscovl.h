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
 *  Update Date:      $Date: 2000-10-16 11:57:23 $
 *  CVS/RCS Revision: $Revision: 1.2 $
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

  public:

    DSRSpatialCoordinatesValue();

    DSRSpatialCoordinatesValue(const DSRTypes::E_GraphicType graphicType);

    DSRSpatialCoordinatesValue(const DSRSpatialCoordinatesValue &coordinatesValue);

    virtual ~DSRSpatialCoordinatesValue();

    DSRSpatialCoordinatesValue &operator=(const DSRSpatialCoordinatesValue &coordinatesValue);

    virtual void clear();

    virtual OFBool isValid() const;

    virtual OFBool isShort(const size_t flags) const;

    virtual E_Condition print(ostream &stream,
                              const size_t flags) const;

    virtual E_Condition renderHTML(ostream &docStream,
                                   ostream &annexStream,
                                   size_t &annexNumber,
                                   const size_t flags,
                                   OFConsole *logStream) const;

    const DSRSpatialCoordinatesValue &getValue() const
    {
        return *this;
    }

    DSRSpatialCoordinatesValue *getValuePtr()
    {
        return this;
    }

    DSRTypes::E_GraphicType getGraphicType() const
    {
        return GraphicType;
    }

    E_Condition setGraphicType(const DSRTypes::E_GraphicType graphicType);

    E_Condition getValue(DSRSpatialCoordinatesValue &coordinatesValue) const;

    E_Condition setValue(const DSRSpatialCoordinatesValue &coordinatesValue);

    DSRGraphicDataList &getGraphicDataList()
    {
        return GraphicDataList;
    }
    

  protected:

    virtual E_Condition read(DcmItem &dataset,
                             OFConsole *logStream);

    virtual E_Condition write(DcmItem &dataset,
                              OFConsole *logStream) const;
                              
    OFBool checkData(OFConsole *logStream = NULL) const;


  private:
  
    DSRTypes::E_GraphicType GraphicType;
    DSRGraphicDataList      GraphicDataList;
};


#endif


/*
 *  CVS/RCS Log:
 *  $Log: dsrscovl.h,v $
 *  Revision 1.2  2000-10-16 11:57:23  joergr
 *  Added methods allowing direct access to certain content item values.
 *
 *  Revision 1.1  2000/10/13 07:49:32  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
