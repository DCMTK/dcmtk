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
 *    classes: DSRWaveformReferenceValue
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2000-10-23 15:13:42 $
 *  CVS/RCS Revision: $Revision: 1.5 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DSRWAVVL_H
#define DSRWAVVL_H

#include "osconfig.h"   /* make sure OS specific configuration is included first */

#include "dsrtypes.h"
#include "dsrcomvl.h"
#include "dsrwavch.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class for waveform reference values
 */
class DSRWaveformReferenceValue
  : public DSRCompositeReferenceValue
{
    // allow access to getValuePtr()
    friend class DSRContentItem;

  public:

    /** default contructor
     */
    DSRWaveformReferenceValue();

    /** constructor.
     *  The UID pair is only set if it passed the validity check (see setValue()).
     ** @param  sopClassUID     referenced SOP class UID of the waveform object.
     *                          (VR=UI, mandatory)
     *  @param  sopInstanceUID  referenced SOP instance UID of the waveform object.
     *                          (VR=UI, mandatory)
     */
    DSRWaveformReferenceValue(const OFString &sopClassUID,
                              const OFString &sopInstanceUID);

    /** copy constructor
     ** @param  referenceValue  waveform reference value to be copied (not checked !)
     */
    DSRWaveformReferenceValue(const DSRWaveformReferenceValue &referenceValue);

    /** destructor
     */
    virtual ~DSRWaveformReferenceValue();

    /** assignment operator
     ** @param  referenceValue  waveform reference value to be copied (not checked !)
     ** @return reference to this waveform reference value after 'referenceValue' has been copied
     */
    DSRWaveformReferenceValue &operator=(const DSRWaveformReferenceValue &referenceValue);

    /** clear all internal variables.
     *  Since an empty waveform reference is invalid the reference becomes invalid afterwards.
     */
    virtual void clear();

    /** check whether the content is short.
     *  This method is used to check whether the rendered output of this content item can be
     *  expanded inline or not (used for renderHTML()).
     ** @param  flags  flag used to customize the output (see DSRTypes::HF_xxx)
     ** @return OFTrue if the content is short, OFFalse otherwise
     */
    virtual OFBool isShort(const size_t flags) const;

    /** print image reference.
     *  The output of a typical waveform reference value looks like this: (HemodynamicWaveform
     *  Storage,"1.2.3")
     *  If the SOP class UID is unknown the UID is printed instead of the related name.
     ** @param  stream  output stream to which the image reference value should be printed
     *  @param  flags   flag used to customize the output (see DSRTypes::PF_xxx)
     */
    virtual E_Condition print(ostream &stream,
                              const size_t flags) const;

    /** render waveform reference value in HTML format
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

    /** get reference to waveform reference value
     ** @return reference to waveform reference value
     */
    inline const DSRWaveformReferenceValue &getValue() const
    {
        return *this;
    }

    /** get copy of waveform reference value
     ** @param  referenceValue  reference to variable in which the value should be stored
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition getValue(DSRWaveformReferenceValue &referenceValue) const;

    /** set waveform reference value.
     *  Before setting the reference it is checked (see check...()).  If the value is
     *  invalid the current value is not replaced and remains unchanged.
     ** @param  referenceValue  value to be set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition setValue(const DSRWaveformReferenceValue &referenceValue);

    /** get reference to list of referenced waveform channels
     ** @return reference to channel list
     */
    inline DSRWaveformChannelList &getChannelList()
    {
        return ChannelList;
    }
    
    /** check whether the waveform reference applies to a specific channel.
     *  The waveform reference applies to a channel if the list of referenced waveform
     *  channels is empty or the group/channel pair is part of the list.
     ** @param  multiplexGroupNumber  multiplex group number of the referenced channel
     *  @param  channelNumber         channel number of the referenced channel
     ** @return OFTrue if reference applies to the specified channel, OFFalse otherwise
     */
    OFBool appliesToChannel(const Uint16 multiplexGroupNumber,
                            const Uint16 channelNumber) const;


  protected:

    /** get pointer to waveform reference value
     ** @return pointer to waveform reference value (never NULL)
     */
    inline DSRWaveformReferenceValue *getValuePtr()
    {
        return this;
    }

    /** read waveform reference value from dataset
     ** @param  dataset    DICOM dataset from which the value should be read
     *  @param  logStream  pointer to error/warning output stream (output disabled if NULL)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual E_Condition readItem(DcmItem &dataset,
                                 OFConsole *logStream);

    /** write waveform reference value to dataset
     ** @param  dataset    DICOM dataset to which the value should be written
     *  @param  logStream  pointer to error/warning output stream (output disabled if NULL)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual E_Condition writeItem(DcmItem &dataset,
                                  OFConsole *logStream) const;

    /** check the specified SOP class UID for validity.
     *  Currently all waveform SOP classes that are defined in supplement 30 are allowed.
     ** @param  sopClassUID  SOP class UID to be checked
     ** @return OFTrue if SOP class UID is valid, OFFalse otherwise
     */
    virtual OFBool checkSOPClassUID(const OFString &sopClassUID) const;


  private:
  
    /// list of referenced waveform channels (conditional)
    DSRWaveformChannelList ChannelList;
};


#endif


/*
 *  CVS/RCS Log:
 *  $Log: dsrwavvl.h,v $
 *  Revision 1.5  2000-10-23 15:13:42  joergr
 *  Added/updated doc++ comments.
 *
 *  Revision 1.4  2000/10/20 10:15:43  joergr
 *  Renamed class DSRReferenceValue to DSRCompositeReferenceValue.
 *
 *  Revision 1.3  2000/10/18 17:11:33  joergr
 *  Added methods allowing direct access to certain content item values.
 *  Made some functions inline.
 *  Added doc++ comments.
 *
 *  Revision 1.2  2000/10/16 11:52:29  joergr
 *  Added new method checking whether a waveform content item applies to a
 *  certain channel.
 *
 *  Revision 1.1  2000/10/13 07:49:37  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
