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
 *  Update Date:      $Date: 2000-10-13 07:49:37 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DSRWAVVL_H
#define DSRWAVVL_H

#include "osconfig.h"   /* make sure OS specific configuration is included first */

#include "dsrtypes.h"
#include "dsrrefvl.h"
#include "dsrwavch.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class for waveform reference values
 */
class DSRWaveformReferenceValue
  : public DSRReferenceValue
{

  public:

    DSRWaveformReferenceValue();

    DSRWaveformReferenceValue(const OFString &sopClassUID,
                              const OFString &sopInstanceUID);

    DSRWaveformReferenceValue(const DSRWaveformReferenceValue &referenceValue);

    virtual ~DSRWaveformReferenceValue();

    DSRWaveformReferenceValue &operator=(const DSRWaveformReferenceValue &referenceValue);

    virtual void clear();

    virtual OFBool isShort(const size_t flags) const;

    virtual E_Condition print(ostream &stream,
                              const size_t flags) const;

    virtual E_Condition renderHTML(ostream &docStream,
                                   ostream &annexStream,
                                   size_t &annexNumber,
                                   const size_t flags,
                                   OFConsole *logStream) const;

    const DSRWaveformReferenceValue &getValue() const
    {
        return *this;
    }

    E_Condition getValue(DSRWaveformReferenceValue &referenceValue) const;


    E_Condition setValue(const DSRWaveformReferenceValue &referenceValue);

    DSRWaveformChannelList &getChannelList()
    {
        return ChannelList;
    }
    

  protected:

    virtual E_Condition readItem(DcmItem &dataset,
                                 OFConsole *logStream);

    virtual E_Condition writeItem(DcmItem &dataset,
                                  OFConsole *logStream) const;

    virtual OFBool checkSOPClassUID(const OFString &sopClassUID) const;


  private:
  
    DSRWaveformChannelList ChannelList;
};


#endif


/*
 *  CVS/RCS Log:
 *  $Log: dsrwavvl.h,v $
 *  Revision 1.1  2000-10-13 07:49:37  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
