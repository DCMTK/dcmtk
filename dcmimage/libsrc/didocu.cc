/*
**
** Author:  Joerg Riesmeier
** Created: 10.12.96
**
** Module:  didocu.cc
**
** Purpose: DicomDocument (Source)
**
** Last Update:      $Author: joergr $
** Update Date:      $Date: 1998-07-01 08:39:35 $
** Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/libsrc/Attic/didocu.cc,v $
** CVS/RCS Revision: $Revision: 1.9 $
** Status:           $State: Exp $
**
** CVS/RCS Log at end of file
**
*/


#include "osconfig.h"
#include "dctk.h"

#include "didocu.h"


/*----------------*
 *  constructors  *
 *----------------*/

DiDocument::DiDocument(const char *filename, const unsigned long flags)
  : Object(NULL),
    Xfer(EXS_Unknown),
    Flags(flags),
    DeleteObject(1)
{
    DcmFileStream stream(filename, DCM_ReadMode);
    if (stream.Fail()) 
        cerr << "ERROR: can't open file '" << filename << "' !" << endl;
    else
        Init(stream);
}


DiDocument::DiDocument(DcmStream &stream, const unsigned long flags)
  : Object(NULL),
    Xfer(EXS_Unknown),
    Flags(flags),
    DeleteObject(1)
{
    if (stream.Fail()) 
        cerr << "ERROR: invalid file stream !" << endl;
    else
        Init(stream);
}


DiDocument::DiDocument(DcmObject *object, E_TransferSyntax xfer, const unsigned long flags)
  : Object(object),
    Xfer(xfer),
    Flags(flags),
    DeleteObject(0)
{
}


void DiDocument::Init(DcmStream &stream)
{
    DcmFileFormat * dfile = new DcmFileFormat;
    Object = dfile;
    if (dfile != NULL)
    {
        dfile->transferInit();
        if (dfile->read(stream) != EC_Normal)
        {
            cerr << "ERROR: can't read DICOM stream !" << endl;
            delete dfile;
            Object = NULL;
        }
        else
        {
            dfile->transferEnd();
            Xfer =  dfile->getDataset()->getOriginalXfer();
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
    if ((obj != NULL) && (obj->search(tag, stack) == EC_Normal) && 
    (stack.top()->getLength(Xfer) > 0))
    return stack.top();
    return NULL;
}


/********************************************************************/

int DiDocument::search(const DcmTagKey &tag, DcmStack & pstack) const
{
    if (pstack.empty())
    pstack.push(Object);
    DcmObject * obj = pstack.top();
    if ((obj != NULL) && (obj->search(tag, pstack) == EC_Normal) && 
    (pstack.top()->getLength(Xfer) > 0))
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


unsigned long DiDocument::getValue(const DcmTagKey &tag, Uint16 &returnVal, const unsigned long pos, DcmObject *item)
    const 
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


/*
**
** CVS/RCS Log:
** $Log: didocu.cc,v $
** Revision 1.9  1998-07-01 08:39:35  joergr
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
