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
 *  Update Date:      $Date: 2000-10-13 07:49:36 $
 *  CVS/RCS Revision: $Revision: 1.1 $
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
  
    DSRWaveformChannelItem(const Uint16 multiplexGroupNumber = 0,
                           const Uint16 channelNumber = 0)
      : MultiplexGroupNumber(multiplexGroupNumber),
        ChannelNumber(channelNumber)
    {
    }

    OFBool operator==(const DSRWaveformChannelItem &item) const
    {
        return (item.MultiplexGroupNumber == MultiplexGroupNumber) && (item.ChannelNumber == ChannelNumber);
    }

    /* copy constructor and assignment operator are defined implicitly */
     
    Uint16 MultiplexGroupNumber;
    Uint16 ChannelNumber;
};


/** Class for waveform channel lists
 */
class DSRWaveformChannelList
  : public DSRListOfItems<DSRWaveformChannelItem>
{

  public:

    DSRWaveformChannelList();

    DSRWaveformChannelList(const DSRWaveformChannelList &list);

    DSRWaveformChannelList &operator=(const DSRWaveformChannelList &list);

    virtual ~DSRWaveformChannelList();

    E_Condition print(ostream &stream,
                      const size_t flags = 0) const;

    E_Condition read(DcmItem &dataset,
                     OFConsole *logStream);

    E_Condition write(DcmItem &dataset,
                      OFConsole *logStream) const;
                      
    E_Condition getItem(const size_t idx,
                        Uint16 &multiplexGroupNumber,
                        Uint16 &channelNumber) const;

    void addItem(const Uint16 multiplexGroupNumber,
                 const Uint16 channelNumber);
};


#endif


/*
 *  CVS/RCS Log:
 *  $Log: dsrwavch.h,v $
 *  Revision 1.1  2000-10-13 07:49:36  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
