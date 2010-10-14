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
 *    classes: DSRWaveformReferenceValue
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:16:33 $
 *  CVS/RCS Revision: $Revision: 1.17 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DSRWAVVL_H
#define DSRWAVVL_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrtypes.h"
#include "dcmtk/dcmsr/dsrcomvl.h"
#include "dcmtk/dcmsr/dsrwavch.h"


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

    /** print waveform reference.
     *  The output of a typical waveform reference value looks like this: (HemodynamicWaveform
     *  Storage,"1.2.3")
     *  If the SOP class UID is unknown the UID is printed instead of the related name.
     ** @param  stream  output stream to which the image reference value should be printed
     *  @param  flags   flag used to customize the output (see DSRTypes::PF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition print(STD_NAMESPACE ostream &stream,
                              const size_t flags) const;

    /** read waveform reference from XML document
     ** @param  doc     document containing the XML file content
     *  @param  cursor  cursor pointing to the starting node
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition readXML(const DSRXMLDocument &doc,
                                DSRXMLCursor cursor);

    /** write waveform reference in XML format
     ** @param  stream     output stream to which the XML document is written
     *  @param  flags      flag used to customize the output (see DSRTypes::XF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition writeXML(STD_NAMESPACE ostream &stream,
                                 const size_t flags) const;

    /** render waveform reference value in HTML/XHTML format
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
    OFCondition getValue(DSRWaveformReferenceValue &referenceValue) const;

    /** set waveform reference value.
     *  Before setting the reference it is checked (see checkXXX()).  If the value is
     *  invalid the current value is not replaced and remains unchanged.
     ** @param  referenceValue  value to be set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setValue(const DSRWaveformReferenceValue &referenceValue);

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
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition readItem(DcmItem &dataset);

    /** write waveform reference value to dataset
     ** @param  dataset    DICOM dataset to which the value should be written
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition writeItem(DcmItem &dataset) const;

    /** check the specified SOP class UID for validity.
     *  Currently all waveform SOP classes that are defined in DICOM PS 3.x 2003 are allowed.
     ** @param  sopClassUID  SOP class UID to be checked
     ** @return OFTrue if SOP class UID is valid, OFFalse otherwise
     */
    virtual OFBool checkSOPClassUID(const OFString &sopClassUID) const;


  private:

    /// list of referenced waveform channels (associated DICOM VR=US, VM=2-2n, type 1C)
    DSRWaveformChannelList ChannelList;
};


#endif


/*
 *  CVS/RCS Log:
 *  $Log: dsrwavvl.h,v $
 *  Revision 1.17  2010-10-14 13:16:33  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.16  2009-10-13 14:57:50  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.15  2007-11-15 16:33:30  joergr
 *  Added support for output in XHTML 1.1 format.
 *
 *  Revision 1.14  2006/08/15 16:40:03  meichel
 *  Updated the code in module dcmsr to correctly compile when
 *    all standard C++ classes remain in namespace std.
 *
 *  Revision 1.13  2005/12/08 16:05:34  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.12  2003/08/07 18:01:42  joergr
 *  Removed libxml dependency from header files.
 *
 *  Revision 1.11  2003/08/07 13:07:39  joergr
 *  Added readXML functionality.
 *
 *  Revision 1.10  2001/09/26 13:04:15  meichel
 *  Adapted dcmsr to class OFCondition
 *
 *  Revision 1.9  2001/06/01 15:51:06  meichel
 *  Updated copyright header
 *
 *  Revision 1.8  2000/11/06 11:21:53  joergr
 *  Moved some protected methods to public part.
 *
 *  Revision 1.7  2000/11/01 16:23:29  joergr
 *  Added support for conversion to XML.
 *
 *  Revision 1.6  2000/10/26 14:22:23  joergr
 *  Updated comments.
 *
 *  Revision 1.5  2000/10/23 15:13:42  joergr
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
