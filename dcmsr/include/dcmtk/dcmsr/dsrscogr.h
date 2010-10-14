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
 *    classes: DSRGraphicDataItem, DSRGraphicDataList
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:16:33 $
 *  CVS/RCS Revision: $Revision: 1.15 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DSRSCOGR_H
#define DSRSCOGR_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrtypes.h"
#include "dcmtk/dcmsr/dsrtlist.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class for graphic data items
 */
class DSRGraphicDataItem
{
  public:

    /** (default) constructor
     ** @param  column  optional column value
     *  @param  row     optional row value
     */
    DSRGraphicDataItem(const Float32 column = 0,
                       const Float32 row = 0)
      : Column(column),
        Row(row)
    {
    }

    /** comparison operator
     ** @param  item  item with which the (column,row) pair should be compared
     ** @return OFTrue if both pairs are equal, OFFalse otherwise
     */
    inline OFBool operator==(const DSRGraphicDataItem &item) const
    {
        return (item.Column == Column) && (item.Row == Row);
    }

    /** comparison operator
     ** @param  item  item with which the (column,row) pair should be compared
     ** @return true if not equal, false if equal
     */
    inline OFBool operator!=(const DSRGraphicDataItem &item) const
    {
        return (item.Column != Column) || (item.Row != Row);
    }

    /** comparison operator
     ** @param  item  item with which the (column,row) pair should be compared
     ** @return true if smaller. Row takes precedence over column.
     */
    inline OFBool operator<(const DSRGraphicDataItem &item) const
    {
        return (Row < item.Row) || ((Row == item.Row) && (Column < item.Column));
    }

    /** comparison operator
     ** @param  item  item with which the (column,row) pair should be compared
     ** @return true if greater. Row takes precedence over column.
     */
    inline OFBool operator>(const DSRGraphicDataItem &item) const
    {
        return (Row > item.Row) || ((Row == item.Row) && (Column > item.Column));
    }

    /* copy constructor and assignment operator are defined implicitly */

    /// column value (VR=FL)
    Float32 Column;
    /// row value (VR=FL)
    Float32 Row;
};


/** Class for graphic data lists
 */
class DSRGraphicDataList
  : public DSRListOfItems<DSRGraphicDataItem>
{

  public:

    /** default constructor
     */
    DSRGraphicDataList();

    /** copy constructor
     ** @param  lst  list to be copied
     */
    DSRGraphicDataList(const DSRGraphicDataList &lst);

    /** destructor
     */
    virtual ~DSRGraphicDataList();

    /** assignment operator
     ** @param  lst  list to be copied
     ** @return reference to this list after 'lst' has been copied
     */
    DSRGraphicDataList &operator=(const DSRGraphicDataList &lst);

    /** print list of graphic data.
     *  The output of a typical list looks like this: 0/0,127/127,255/255
     ** @param  stream         output stream to which the list should be printed
     *  @param  flags          flag used to customize the output (see DSRTypes::PF_xxx)
     *  @param  pairSeparator  character specifying the separator between the value pairs
     *  @param  itemSeparator  character specifying the separator between the list items
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition print(STD_NAMESPACE ostream &stream,
                      const size_t flags = 0,
                      const char pairSeparator = '/',
                      const char itemSeparator = ',') const;

    /** read list of graphic data
     ** @param  dataset    DICOM dataset from which the list should be read
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition read(DcmItem &dataset);

    /** write list of graphic data
     ** @param  dataset    DICOM dataset to which the list should be written
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition write(DcmItem &dataset) const;

    /** get reference to the specified item
     ** @param  idx  index of the item to be returned (starting from 1)
     ** @return reference to the specified item if successful, EmptyItem otherwise
     */
    const DSRGraphicDataItem &getItem(const size_t idx) const;

    /** get copy of the specified value pair
     ** @param  idx     index of the value pair to be returned (starting from 1)
     *  @param  column  column value of the specified index (set to 0 first)
     *  @param  row     row value of the specified index (set to 0 first)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition getItem(const size_t idx,
                        Float32 &column,
                        Float32 &row) const;

    /** add value pair to the list
     ** @param  column  column value to be added
     *  @param  row     row value to be added
     */
    void addItem(const Float32 column,
                 const Float32 row);

    /** put list of graphic data as a string.
     *  This function expects the same input format as created by print(), i.e. a comma
     *  separated list of numerical value pairs.
     ** @param  stringValue  string value to be set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition putString(const char *stringValue);
};


#endif


/*
 *  CVS/RCS Log:
 *  $Log: dsrscogr.h,v $
 *  Revision 1.15  2010-10-14 13:16:33  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.14  2009-10-13 14:57:50  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.13  2007-11-15 16:33:19  joergr
 *  Fixed coding style to be more consistent.
 *
 *  Revision 1.12  2006/08/15 16:40:03  meichel
 *  Updated the code in module dcmsr to correctly compile when
 *    all standard C++ classes remain in namespace std.
 *
 *  Revision 1.11  2005/12/08 16:05:15  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.10  2003/08/07 12:45:38  joergr
 *  Added new putString() method.
 *
 *  Revision 1.9  2003/06/04 12:33:13  meichel
 *  Added comparison operators, needed by MSVC5 with STL
 *
 *  Revision 1.8  2003/06/03 10:16:44  meichel
 *  Renamed local variables to avoid name clashes with STL
 *
 *  Revision 1.7  2001/09/26 13:04:10  meichel
 *  Adapted dcmsr to class OFCondition
 *
 *  Revision 1.6  2001/05/07 16:13:24  joergr
 *  Updated CVS header.
 *
 *  Revision 1.5  2001/01/25 11:48:44  joergr
 *  Corrected typos / enhanced comments.
 *
 *  Revision 1.4  2000/11/06 11:31:04  joergr
 *  Added parameter to print() method specifying the item separator character.
 *
 *  Revision 1.3  2000/11/01 16:21:16  joergr
 *  Updated comments/formatting.
 *
 *  Revision 1.2  2000/10/18 17:06:51  joergr
 *  Added doc++ comments.
 *
 *  Revision 1.1  2000/10/13 07:49:31  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
