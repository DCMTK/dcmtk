/*
 *
 *  Copyright (C) 1996-99, OFFIS
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
 *  Module:  dcmimgle
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: DicomDocument (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1998-12-14 17:16:32 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/include/Attic/didocu.h,v $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __DIDOCU_H
#define __DIDOCU_H

#include "osconfig.h"
#include "dctypes.h"
#include "dcxfer.h"

#include "diobjcou.h"

#include <stddef.h>


/*------------------------*
 *  forward declarations  *
 *------------------------*/

class DcmStream;
class DcmStack;
class DcmObject;
class DcmTagKey;
class DcmElement;
class DcmSequenceOfItems;


/*---------------------*
 *  class declaration  *
 *---------------------*/

class DiDocument
  : public DiObjectCounter
{

 public:

    DiDocument(const char *filename,
               const unsigned long flags = 0,
               const unsigned long fstart = 0,
               const unsigned long fcount = 0);

    DiDocument(DcmStream &stream,
               const unsigned long flags = 0,
               const unsigned long fstart = 0,
               const unsigned long fcount = 0);

    DiDocument(DcmObject *object,
               const E_TransferSyntax xfer,
               const unsigned long flags = 0,
               const unsigned long fstart = 0,
               const unsigned long fcount = 0);

    virtual ~DiDocument();
   
    inline int good() const
    {
        return Object != NULL;
    }

    inline unsigned long getFrameStart() const
    {
        return FrameStart;
    }
    
    inline unsigned long getFrameCount() const
    {
        return FrameCount;
    }

    inline unsigned long getFlags() const
    {
        return Flags;
    }
    
    DcmElement *search(const DcmTagKey &tag,
                       DcmObject *obj = NULL) const;

    int search(const DcmTagKey &tag,
               DcmStack &pstack) const;
    
    unsigned long getVM(const DcmTagKey &tag) const;
    
    unsigned long getValue(const DcmTagKey &tag,
                           Uint16 &returnVal,
                           const unsigned long pos = 0,
                           DcmObject *item = NULL) const;

    unsigned long getValue(const DcmTagKey &tag,
                           Sint16 &returnVal,
                           const unsigned long pos = 0) const;

    unsigned long getValue(const DcmTagKey &tag,
                           Uint32 &returnVal,
                           const unsigned long pos = 0) const;

    unsigned long getValue(const DcmTagKey &tag,
                           Sint32 &returnVal,
                           const unsigned long pos = 0) const;

    unsigned long getValue(const DcmTagKey &tag,
                           double &returnVal,
                           const unsigned long pos = 0) const;

    unsigned long getValue(const DcmTagKey &tag,
                           const Uint16 *&returnVal,
                           DcmObject *item = NULL) const;

    unsigned long getValue(const DcmTagKey &tag,
                           const char *&returnVal) const;

    unsigned long getSequence(const DcmTagKey &tag,
                              DcmSequenceOfItems *&seq) const;

    static unsigned long getElemValue(const DcmElement *elem,
                                      Uint16 &returnVal,
                                      const unsigned long pos);

    static unsigned long getElemValue(const DcmElement *elem,
                                      const Uint16 *&returnVal);

    static unsigned long getElemValue(const DcmElement *elem,
                                      const char *&returnVal);


 protected:

    void Init(DcmStream &);


 private:

    DcmObject *Object;
    E_TransferSyntax Xfer;

    unsigned long FrameStart;
    unsigned long FrameCount;

    unsigned long Flags;
    int DeleteObject;

 // --- declarations to avoid compiler warnings
 
    DiDocument(const DiDocument &);
    DiDocument &operator=(const DiDocument &);
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: didocu.h,v $
 * Revision 1.2  1998-12-14 17:16:32  joergr
 * Added (simplified) methods to return values of a given DcmElement object.
 *
 * Revision 1.1  1998/11/27 14:53:59  joergr
 * Added copyright message.
 * Added static methods to return the value of a given element.
 * Added support of frame start and count for future use.
 *
 * Revision 1.7  1998/07/01 08:39:20  joergr
 * Minor changes to avoid compiler warnings (gcc 2.8.1 with additional
 * options), e.g. add copy constructors.
 *
 * Revision 1.6  1998/06/25 08:50:09  joergr
 * Added compatibility mode to support ACR-NEMA images and wrong
 * palette attribute tags.
 *
 * Revision 1.5  1998/05/11 14:53:15  joergr
 * Added CVS/RCS header to each file.
 *
 *
 */
