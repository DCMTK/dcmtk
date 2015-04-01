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
 *    classes: DSRGraphicDataItem, DSRGraphicDataList
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
class DCMTK_DCMSR_EXPORT DSRGraphicDataItem
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
class DCMTK_DCMSR_EXPORT DSRGraphicDataList
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
     ** @param  dataset  DICOM dataset from which the list should be read
     *  @param  flags    flag used to customize the reading process (see DSRTypes::RF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition read(DcmItem &dataset,
                     const size_t flags);

    /** write list of graphic data
     ** @param  dataset  DICOM dataset to which the list should be written
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
