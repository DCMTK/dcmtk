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
 *    classes: DSRImageReferenceValue
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2000-10-13 07:49:28 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DSRIMGVL_H
#define DSRIMGVL_H

#include "osconfig.h"   /* make sure OS specific configuration is included first */

#include "dsrtypes.h"
#include "dsrrefvl.h"
#include "dsrimgfr.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class for image reference values
 */
class DSRImageReferenceValue
  : public DSRReferenceValue
{

  public:

    DSRImageReferenceValue();

    DSRImageReferenceValue(const OFString &sopClassUID,
                           const OFString &sopInstanceUID);

    DSRImageReferenceValue(const OFString &imageSOPClassUID,
                           const OFString &imageSOPInstanceUID,
                           const OFString &pstateSOPClassUID,
                           const OFString &pstateSOPInstanceUID);

    DSRImageReferenceValue(const DSRImageReferenceValue &referenceValue);

    DSRImageReferenceValue(const DSRReferenceValue &imageReferenceValue,
                           const DSRReferenceValue &pstateReferenceValue);

    virtual ~DSRImageReferenceValue();

    DSRImageReferenceValue &operator=(const DSRImageReferenceValue &referenceValue);

    virtual void clear();

    virtual OFBool isShort(const size_t flags) const;

    virtual E_Condition print(ostream &stream,
                              const size_t flags) const;

    virtual E_Condition renderHTML(ostream &docStream,
                                   ostream &annexStream,
                                   size_t &annexNumber,
                                   const size_t flags,
                                   OFConsole *logStream) const;

    const DSRImageReferenceValue &getValue() const
    {
        return *this;
    }

    E_Condition getValue(DSRImageReferenceValue &referenceValue) const;

    E_Condition setValue(const DSRImageReferenceValue &referenceValue);

    const DSRReferenceValue &getPresentationState() const
    {
        return PresentationState;
    }

    E_Condition setPresentationState(const DSRReferenceValue &referenceValue);

    DSRImageFrameList &getFrameList()
    {
        return FrameList;
    }


  protected:

    virtual E_Condition readItem(DcmItem &dataset,
                                 OFConsole *logStream);

    virtual E_Condition writeItem(DcmItem &dataset,
                                  OFConsole *logStream) const;

    virtual OFBool checkSOPClassUID(const OFString &sopClassUID) const;


  private:

    DSRReferenceValue PresentationState;

    DSRImageFrameList FrameList;
};


#endif


/*
 *  CVS/RCS Log:
 *  $Log: dsrimgvl.h,v $
 *  Revision 1.1  2000-10-13 07:49:28  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
