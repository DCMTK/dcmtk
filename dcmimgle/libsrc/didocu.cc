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
 *  Purpose: DicomDocument (Source)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1998-11-27 15:53:36 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/libsrc/didocu.cc,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"
#include "dctk.h"

#include "didocu.h"
#include "diutils.h"


/*----------------*
 *  constructors  *
 *----------------*/

DiDocument::DiDocument(const char *filename,
                       const unsigned long flags,
                       const unsigned long fstart,
                       const unsigned long fcount)
  : Object(NULL),
    Xfer(EXS_Unknown),
    FrameStart(fstart),
    FrameCount(fcount),
    Flags(flags),
    DeleteObject(1)
{
    DcmFileStream stream(filename, DCM_ReadMode);
    if (stream.Fail())
    { 
        if (DicomImageClass::DebugLevel >= DicomImageClass::DL_Errors)
            cerr << "ERROR: can't open file '" << filename << "' !" << endl;
    } else
        Init(stream);
}


DiDocument::DiDocument(DcmStream &stream,
                       const unsigned long flags,
                       const unsigned long fstart,
                       const unsigned long fcount)
  : Object(NULL),
    Xfer(EXS_Unknown),
    FrameStart(fstart),
    FrameCount(fcount),
    Flags(flags),
    DeleteObject(1)
{
    if (stream.Fail())
    {
        if (DicomImageClass::DebugLevel >= DicomImageClass::DL_Errors)
            cerr << "ERROR: invalid file stream !" << endl;
    } else
        Init(stream);
}


DiDocument::DiDocument(DcmObject *object,
                       const E_TransferSyntax xfer,
                       const unsigned long flags,
                       const unsigned long fstart,
                       const unsigned long fcount)
  : Object(object),
    Xfer(xfer),
    FrameStart(fstart),
    FrameCount(fcount),
    Flags(flags),
    DeleteObject(0)
{
}


void DiDocument::Init(DcmStream &stream)
{
    Object = new DcmFileFormat;
    if (Object != NULL)
    {
        Object->transferInit();
        if (((DcmFileFormat *)Object)->read(stream) != EC_Normal)
        {
            if (DicomImageClass::DebugLevel >= DicomImageClass::DL_Errors)
                cerr << "ERROR: can't read DICOM stream !" << endl;
            delete Object;
            Object = NULL;
        }
        else
        {
            Object->transferEnd();
            DcmDataset *dataset = ((DcmFileFormat *)Object)->getDataset();
            if (dataset != NULL)
                Xfer = dataset->getOriginalXfer();
        }
    }
}


/*--------------*
 *  destructor  *
 *--------------*/

DiDocument::~DiDocument()
{
    if (DeleteObject)
        delete Object;
}


/********************************************************************/

DcmObject *DiDocument::search(const DcmTagKey &tag, DcmObject *obj) const
{
    DcmStack stack;
    if (obj == NULL)
        obj = Object;
    if ((obj != NULL) && (obj->search(tag, stack) == EC_Normal) && (stack.top()->getLength(Xfer) > 0))
        return stack.top();
    return NULL;
}


/********************************************************************/

int DiDocument::search(const DcmTagKey &tag, DcmStack &pstack) const
{
    if (pstack.empty())
        pstack.push(Object);
    DcmObject * obj = pstack.top();
    if ((obj != NULL) && (obj->search(tag, pstack) == EC_Normal) && (pstack.top()->getLength(Xfer) > 0))
        return 1;
    return 0;
}


/********************************************************************/


unsigned long DiDocument::getVM(const DcmTagKey &tag) const 
{
    DcmObject *searchedObj = search(tag);
    if (searchedObj != NULL)
        return searchedObj->getVM();
    return 0;
}


unsigned long DiDocument::getValue(const DcmTagKey &tag, Uint16 &returnVal, const unsigned long pos, DcmObject *item) const 
{
    DcmObject *searchedObj = search(tag, item);
    if (searchedObj != NULL)
    {
        ((DcmElement *)searchedObj)->getUint16(returnVal, pos);
        return searchedObj->getVM();
    }
    return 0;
}


unsigned long DiDocument::getValue(const DcmTagKey &tag, Sint16 &returnVal, const unsigned long pos) const
{
    DcmObject *searchedObj = search(tag);
    if (searchedObj != NULL)
    {
        ((DcmElement *)searchedObj)->getSint16(returnVal, pos);
        return searchedObj->getVM();
    }
    return 0;
}


unsigned long DiDocument::getValue(const DcmTagKey &tag, Uint32 &returnVal, const unsigned long pos) const
{
    DcmObject *searchedObj = search(tag);
    if (searchedObj != NULL)
    {
        ((DcmElement *)searchedObj)->getUint32(returnVal, pos);
        return searchedObj->getVM();
    }
    return 0;
}


unsigned long DiDocument::getValue(const DcmTagKey &tag, Sint32 &returnVal, const unsigned long pos) const
{
    DcmObject *searchedObj = search(tag);
    if (searchedObj != NULL)
    {
        ((DcmElement *)searchedObj)->getSint32(returnVal, pos);
        return searchedObj->getVM();
    }
    return 0;
}


unsigned long DiDocument::getValue(const DcmTagKey &tag, double &returnVal, const unsigned long pos) const
{
    DcmObject *searchedObj = search(tag);
    if (searchedObj != NULL)
    {
        ((DcmElement *)searchedObj)->getFloat64(returnVal, pos);
        return searchedObj->getVM();
    }
    return 0;
}


unsigned long DiDocument::getValue(const DcmTagKey &tag, const Uint16 *&returnVal, DcmObject *item) const
{
    DcmObject *searchedObj = search(tag, item);
    if (searchedObj != NULL)
    {
        Uint16 *val;    
        ((DcmElement *)searchedObj)->getUint16Array(val);
        returnVal = val;
        if (searchedObj->getVR() == EVR_OW)
            return searchedObj->getLength(Xfer) / sizeof(Uint16);
        return searchedObj->getVM();
    }
    return 0;
}


unsigned long DiDocument::getValue(const DcmTagKey &tag, const char *&returnVal) const
{
    DcmObject *searchedObj = search(tag);
    if (searchedObj != NULL)
    {
        char *val;
        ((DcmElement *)searchedObj)->getString(val);
        returnVal = val;
        return searchedObj->getVM();
    }
    return 0;
}


unsigned long DiDocument::getSequence(const DcmTagKey &tag, DcmSequenceOfItems *&seq) const
{
    DcmObject *obj = search(tag);
    if ((obj != NULL) && (obj->ident() == EVR_SQ))
        return (seq = (DcmSequenceOfItems *)obj)->card();
    return 0;
}


unsigned long DiDocument::getElemValue(const DcmUnsignedShort &elem, Uint16 &returnVal, const unsigned long pos)
{
    DcmElement *e = (DcmElement *)&elem;             // remove 'const' to use non-const methods
    e->getUint16(returnVal, pos);
    return e->getVM();
}


unsigned long DiDocument::getElemValue(const DcmUnsignedShort &elem, const Uint16 *&returnVal)
{
    DcmElement *e = (DcmElement *)&elem;             // remove 'const' to use non-const methods
    Uint16 *val;
    e->getUint16Array(val);
    returnVal = val;
    if (e->getVR() == EVR_OW)
        return e->getLength() / sizeof(Uint16);
    return e->getVM();
}


/*
**
** CVS/RCS Log:
** $Log: didocu.cc,v $
** Revision 1.1  1998-11-27 15:53:36  joergr
** Added copyright message.
** Added static methods to return the value of a given element.
** Added support of frame start and count for future use (will be explained
** later if it is fully implemented).
**
** Revision 1.9  1998/07/01 08:39:35  joergr
** Minor changes to avoid compiler warnings (gcc 2.8.1 with additional
** options), e.g. add copy constructors.
**
** Revision 1.8  1998/06/25 08:52:05  joergr
** Added compatibility mode to support ACR-NEMA images and wrong
** palette attribute tags.
**
** Revision 1.7  1998/05/11 14:52:28  joergr
** Added CVS/RCS header to each file.
**
**
*/
