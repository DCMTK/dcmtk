/********************************************************************
**
**  DicomDocument (Source)
**
**  author   : Joerg Riesmeier
**  created  : 10.12.96
**  modified : 18.04.97
**
*********************************************************************/


#include "osconfig.h"
#include "dctk.h"

#include "didocu.h"



/*----------------*
 *  constructors  *
 *----------------*/

DiDocument::DiDocument(const char *filename)
  : DeleteObject(1)
{
    DcmFileStream stream(filename, DCM_ReadMode);
    if (stream.Fail()) 
    {
        cerr << "ERROR: can't open file '" << filename << "' !" << endl;
		Object = NULL;
    } 
    else
		Init(stream);
}


DiDocument::DiDocument(DcmStream &stream)
  : DeleteObject(1)
{
    if (stream.Fail()) 
    {
        cerr << "ERROR: invalid file stream !" << endl;
		Object = NULL;
    } 
    else
		Init(stream);
}


DiDocument::DiDocument(DcmObject *object)
  : Object(object),
	DeleteObject(0)
{
}


void DiDocument::Init(DcmStream &stream)
{
	Object = new DcmFileFormat;
	if (Object != NULL)
	{
 		if (((DcmFileFormat *)Object)->read(stream) != EC_Normal)
		{
			cerr << "ERROR: can't read DICOM stream !" << endl;
			delete Object;
			Object = NULL;
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
    if ((obj != NULL) && (obj->search(tag, stack) == EC_Normal) && (stack.top()->getLength() > 0))
		return stack.top();
	return NULL;
}


/********************************************************************/


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
        if (searchedObj->ident() == EVR_OW)
			return searchedObj->getLength() / sizeof(Uint16);
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


