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
 *    classes: DSRGraphicDataList
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2000-10-13 07:49:31 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DSRSCOGR_H
#define DSRSCOGR_H

#include "osconfig.h"   /* make sure OS specific configuration is included first */

#include "dsrtypes.h"
#include "dsrtlist.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class for graphic data items
 */
class DSRGraphicDataItem
{
  public:
  
    DSRGraphicDataItem(const Float32 column = 0,
                       const Float32 row = 0)
      : Column(column),
        Row(row)
    {
    }

    OFBool operator==(const DSRGraphicDataItem &item) const
    {
        return (item.Column == Column) && (item.Row == Row);
    }

    /* copy constructor and assignment operator are defined implicitly */
     
    Float32 Column;
    Float32 Row;
};


/** Class for graphic data lists
 */
class DSRGraphicDataList
  : public DSRListOfItems<DSRGraphicDataItem>
{

  public:

    DSRGraphicDataList();

    DSRGraphicDataList(const DSRGraphicDataList &list);

    DSRGraphicDataList &operator=(const DSRGraphicDataList &list);

    virtual ~DSRGraphicDataList();

    E_Condition print(ostream &stream,
                      const size_t flags = 0) const;

    E_Condition read(DcmItem &dataset,
                     OFConsole *logStream);

    E_Condition write(DcmItem &dataset,
                      OFConsole *logStream) const;
                      
    E_Condition getItem(const size_t idx,
                        Float32 &column,
                        Float32 &row) const;

    void addItem(const Float32 column,
                 const Float32 row);
};


#endif


/*
 *  CVS/RCS Log:
 *  $Log: dsrscogr.h,v $
 *  Revision 1.1  2000-10-13 07:49:31  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
