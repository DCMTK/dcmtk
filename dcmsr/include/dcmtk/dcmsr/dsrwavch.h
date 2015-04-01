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
 *    classes: DSRWaveformChannelItem, DSRWaveformChannelList
 *
 */


#ifndef DSRWAVCH_H
#define DSRWAVCH_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrtypes.h"
#include "dcmtk/dcmsr/dsrtlist.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class for waveform channel items
 */
class DCMTK_DCMSR_EXPORT DSRWaveformChannelItem
{
  public:

    /** (default) constructor
     ** @param  multiplexGroupNumber  optional multiplex group number
     *  @param  channelNumber         optional channel number
     */
    DSRWaveformChannelItem(const Uint16 multiplexGroupNumber = 0,
                           const Uint16 channelNumber = 0)
      : MultiplexGroupNumber(multiplexGroupNumber),
        ChannelNumber(channelNumber)
    {
    }

    /** comparison operator
     ** @param  item  item with which the (m,c) pair should be compared
     ** @return OFTrue if both pairs are equal, OFFalse otherwise
     */
    inline OFBool operator==(const DSRWaveformChannelItem &item) const
    {
        return (item.MultiplexGroupNumber == MultiplexGroupNumber) && (item.ChannelNumber == ChannelNumber);
    }

    /** comparison operator
     ** @param  item  item with which the (m,c) pair should be compared
     ** @return true if not equal, false if equal
     */
    inline OFBool operator!=(const DSRWaveformChannelItem &item) const
    {
        return (item.MultiplexGroupNumber != MultiplexGroupNumber) || (item.ChannelNumber != ChannelNumber);
    }

    /** comparison operator
     ** @param  item  item with which the (m,c) pair should be compared
     ** @return true if smaller. ChannelNumber takes precedence over MultiplexGroupNumber.
     */
    inline OFBool operator<(const DSRWaveformChannelItem &item) const
    {
        return (ChannelNumber < item.ChannelNumber) || ((ChannelNumber == item.ChannelNumber) && (MultiplexGroupNumber < item.MultiplexGroupNumber));
    }

    /** comparison operator
     ** @param  item  item with which the (m,c) pair should be compared
     ** @return true if greater. ChannelNumber takes precedence over MultiplexGroupNumber.
     */
    inline OFBool operator>(const DSRWaveformChannelItem &item) const
    {
        return (ChannelNumber > item.ChannelNumber) || ((ChannelNumber == item.ChannelNumber) && (MultiplexGroupNumber > item.MultiplexGroupNumber));
    }

    /* copy constructor and assignment operator are defined implicitly */

    /// multiplex group number value (VR=US)
    Uint16 MultiplexGroupNumber;
    /// channel number value (VR=US)
    Uint16 ChannelNumber;
};


/** Class for waveform channel lists
 */
class DCMTK_DCMSR_EXPORT DSRWaveformChannelList
  : public DSRListOfItems<DSRWaveformChannelItem>
{

  public:

    /** default constructor
     */
    DSRWaveformChannelList();

    /** copy constructor
     ** @param  lst  list to be copied
     */
    DSRWaveformChannelList(const DSRWaveformChannelList &lst);

    /** destructor
     */
    virtual ~DSRWaveformChannelList();

    /** assignment operator
     ** @param  lst  list to be copied
     ** @return reference to this list after 'lst' has been copied
     */
    DSRWaveformChannelList &operator=(const DSRWaveformChannelList &lst);

    /** print list of waveform channels.
     *  The output of a typical list looks like this: 1/2,3/4,5/6
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

    /** read list of waveform channels
     ** @param  dataset  DICOM dataset from which the list should be read
     *  @param  flags    flag used to customize the reading process (see DSRTypes::RF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition read(DcmItem &dataset,
                     const size_t flags);

    /** write list of waveform channels
     ** @param  dataset  DICOM dataset to which the list should be written
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition write(DcmItem &dataset) const;

    /** check whether specified value pair is contained in the list
     ** @param  multiplexGroupNumber  multiplex group number to be checked
     *  @param  channelNumber         channel number to be checked
     ** @return OFTrue if the value pair is in the list, OFFalse otherwise
     */
    OFBool isElement(const Uint16 multiplexGroupNumber,
                     const Uint16 channelNumber) const;

    /** get copy of the specified value pair
     ** @param  idx                   index of the value pair to be returned (starting from 1)
     *  @param  multiplexGroupNumber  multiplex group number of the specified index (set to 0)
     *  @param  channelNumber         channel number of the specified index (set to 0 first)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition getItem(const size_t idx,
                        Uint16 &multiplexGroupNumber,
                        Uint16 &channelNumber) const;

    /** add value pair to the list
     ** @param  multiplexGroupNumber  multiplex group number to be added
     *  @param  channelNumber         channel number to be added
     */
    void addItem(const Uint16 multiplexGroupNumber,
                 const Uint16 channelNumber);

    /** put list of waveform channels as a string.
     *  This function expects the same input format as created by print(), i.e. a comma
     *  separated list of numerical value pairs.
     ** @param  stringValue  string value to be set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition putString(const char *stringValue);
};


#endif
