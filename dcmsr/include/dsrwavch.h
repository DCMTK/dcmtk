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
 *    classes: DSRWaveformChannelList
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2000-11-01 16:22:57 $
 *  CVS/RCS Revision: $Revision: 1.4 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DSRWAVCH_H
#define DSRWAVCH_H

#include "osconfig.h"   /* make sure OS specific configuration is included first */

#include "dsrtypes.h"
#include "dsrtlist.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class for waveform channel items
 */
class DSRWaveformChannelItem
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
     ** @param  item  item which which the (m,c) pair should be compared
     ** @return OFTrue if both pairs are equal, OFFalse otherwise
     */
    inline OFBool operator==(const DSRWaveformChannelItem &item) const
    {
        return (item.MultiplexGroupNumber == MultiplexGroupNumber) && (item.ChannelNumber == ChannelNumber);
    }

    /* copy constructor and assignment operator are defined implicitly */

    /// multiplex group number value (VR=US)
    Uint16 MultiplexGroupNumber;
    /// channel number value (VR=US)
    Uint16 ChannelNumber;
};


/** Class for waveform channel lists
 */
class DSRWaveformChannelList
  : public DSRListOfItems<DSRWaveformChannelItem>
{

  public:

    /** default constructor
     */
    DSRWaveformChannelList();

    /** copy constructor
     ** @param  list  list to be copied
     */
    DSRWaveformChannelList(const DSRWaveformChannelList &list);

    /** destructor
     */
    virtual ~DSRWaveformChannelList();

    /** assignment operator
     ** @param  list  list to be copied
     ** @return reference to this list after 'list' has been copied
     */
    DSRWaveformChannelList &operator=(const DSRWaveformChannelList &list);

    /** print list of waveform channels.
     *  The output of a typical list looks like this: 1/2,3/4,5/6
     ** @param  stream  output stream to which the list should be printed
     *  @param  flags   flag used to customize the output (see DSRTypes::PF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition print(ostream &stream,
                      const size_t flags = 0) const;

    /** read list of waveform channels
     ** @param  dataset    DICOM dataset from which the list should be read
     *  @param  logStream  pointer to error/warning output stream (output disabled if NULL)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition read(DcmItem &dataset,
                     OFConsole *logStream);

    /** write list of waveform channels
     ** @param  dataset    DICOM dataset to which the list should be written
     *  @param  logStream  pointer to error/warning output stream (output disabled if NULL)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition write(DcmItem &dataset,
                      OFConsole *logStream) const;

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
    E_Condition getItem(const size_t idx,
                        Uint16 &multiplexGroupNumber,
                        Uint16 &channelNumber) const;

    /** add value pair to the list
     ** @param  multiplexGroupNumber  multiplex group number to be added
     *  @param  channelNumber         channel number to be added
     */
    void addItem(const Uint16 multiplexGroupNumber,
                 const Uint16 channelNumber);
};


#endif


/*
 *  CVS/RCS Log:
 *  $Log: dsrwavch.h,v $
 *  Revision 1.4  2000-11-01 16:22:57  joergr
 *  Updated comments/formatting.
 *
 *  Revision 1.3  2000/10/18 17:10:51  joergr
 *  Added doc++ comments.
 *
 *  Revision 1.2  2000/10/16 11:52:29  joergr
 *  Added new method checking whether a waveform content item applies to a
 *  certain channel.
 *
 *  Revision 1.1  2000/10/13 07:49:36  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
