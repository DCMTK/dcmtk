/*
**
** Author: Gerd Ehlers	    Created:  09.04.94
**                          Modified: 26.11.95
**
** Module: dcobject.h
**
** Purpose:
** This file contains the interface to routines which provide
** DICOM object encoding/decoding, search and lookup facilities.
**
** Last Update:   $Author: andreas $
** Revision:      $Revision: 1.11 $
** Status:	  $State: Exp $
**
*/

#ifndef DCOBJECT_H
#define DCOBJECT_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcerror.h"
#include "dctypes.h"
#include "dcxfer.h"
#include "dcstream.h"
#include "dctag.h"
#include "dclist.h"
#include "dcstack.h"


// Undefined Length Identifier now defined in dctypes.h

// Maxinum number of read bytes for a Value Element
const Uint32 DCM_MaxReadLength = 4096;	

// Maximun Length of Tag and Length in a DICOM element
const Uint32 DCM_TagInfoLength = 12;	

// Optimum Line Length if not all data printed
const Uint32 DCM_OptPrintLineLength = 70;

/*
** Should automatic correction be applied to input data (e.g. stripping
** of padding blanks, removal of blanks in UIDs, etc).
*/
extern BOOL dcmEnableAutomaticInputDataCorrection; /* default TRUE */

/*
** The base dicom object class
*/

class DcmObject 
{
protected:
    int testConstructDestruct;   // for debugging

    DcmTag *Tag;
    Uint32 Length;
    E_Condition errorFlag;
    E_TransferState fTransferState;
    Uint32 fTransferredBytes;

    // The next two functions require that the memory for the info
    // field is minimum 4 chars longer than strlen(info)
    virtual void printInfoLine(ostream & out, const BOOL showFullData,
			       const int level, char *info );
    virtual void printInfoLine(ostream & out, const BOOL showFullData,
			       const int level, const DcmTag &tag,
			       const Uint32 length, char *info );

    E_Condition writeTag(DcmStream & outStream,	const DcmTag & tag,
			 const E_TransferSyntax oxfer); // in

    virtual E_Condition writeTagAndLength(DcmStream & outStream,  
					  const E_TransferSyntax oxfer,	// in
					  Uint32 & writtenBytes ); // out

    void swapIfNecessary(const E_ByteOrder newByteOrder, 
			 const E_ByteOrder oldByteOrder,
			 void * value, const Uint32 byteLength,
			 const size_t valWidth);

public:
    DcmObject(const DcmTag & tag, const Uint32 len = 0);
    DcmObject(const DcmObject & obj);

    virtual ~DcmObject();

    virtual DcmEVR ident(void) const = 0;
    virtual BOOL isLeaf(void) const = 0;
    virtual DcmObject * nextInContainer(const DcmObject * obj);
    virtual void print(ostream & out = cout, const BOOL showFullData = TRUE,
		       const int level = 0) = 0;
    inline E_Condition error(void) const { return errorFlag; }

    inline E_TransferState transferState(void) const { return fTransferState; }
    virtual void transferInit(void);
    virtual void transferEnd(void);

    virtual Uint16 getGTag();
    virtual Uint16 getETag();
    const DcmTag & getTag(void) const;

    DcmEVR getVR(void);

    virtual E_Condition setVR(DcmEVR vr);
    virtual unsigned long getVM() = 0;

    // calculate length of Dicom element 
    virtual Uint32 calcElementLength(const E_TransferSyntax xfer,
				     const E_EncodingType enctype) = 0;

    // returns value length
    virtual Uint32 getLength(const E_TransferSyntax xfer 
			     = EXS_LittleEndianImplicit,
			     const E_EncodingType enctype 
			     = EET_UndefinedLength) = 0;

     virtual E_Condition read(DcmStream & inStream,
			     const E_TransferSyntax ixfer,
			     const E_GrpLenEncoding glenc = EGL_noChange,
			     const Uint32 maxReadLength = DCM_MaxReadLength) = 0;

    virtual E_Condition write(DcmStream & outStream,
			      const E_TransferSyntax oxfer,
			      const E_EncodingType enctype = EET_UndefinedLength) = 0;

    virtual E_Condition clear() = 0;
    virtual E_Condition verify(const BOOL autocorrect = FALSE) = 0;

    virtual E_Condition nextObject(DcmStack & stack, const BOOL intoSub);
    virtual E_Condition search(const DcmTag &tag,                 // in
			       DcmStack &resultStack,	       // inout
			       E_SearchMode mode = ESM_fromHere,  // in
			       BOOL searchIntoSub = TRUE );       // in
	
    virtual E_Condition search(const DcmTagKey &xtag,	       // in
			       DcmStack &resultStack,	       // inout
			       E_SearchMode mode = ESM_fromHere,  // in
			       BOOL searchIntoSub = TRUE );       // in

    virtual E_Condition searchErrors( DcmStack &resultStack );	       // inout

    virtual E_Condition loadAllDataIntoMemory(void) = 0;

}; // class DcmObject


#endif // DCOBJECT_H
