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
** Last Update:   $Author: hewett $
** Revision:      $Revision: 1.15 $
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
extern OFBool dcmEnableAutomaticInputDataCorrection; /* default OFTrue */

/*
** The base dicom object class
*/

class DcmObject 
{
protected:
    int testConstructDestruct;   // for debugging

    DcmTag Tag;
    Uint32 Length;
    E_Condition errorFlag;
    E_TransferState fTransferState;
    Uint32 fTransferredBytes;

    // The next two functions require that the memory for the info
    // field is minimum 4 chars longer than strlen(info)
    virtual void printInfoLine(ostream & out, const OFBool showFullData,
			       const int level, char *info );
    virtual void printInfoLine(ostream & out, const OFBool showFullData,
			       const int level, const DcmTag &tag,
			       const Uint32 length, char *info );

    E_Condition writeTag(DcmStream & outStream,	const DcmTag & tag,
			 const E_TransferSyntax oxfer); // in

    virtual E_Condition writeTagAndLength(DcmStream & outStream,  
					  const E_TransferSyntax oxfer,	// in
					  Uint32 & writtenBytes ); // out

public:
    DcmObject(const DcmTag & tag, const Uint32 len = 0);
    DcmObject(const DcmObject & obj);

    virtual ~DcmObject();

    // class identification
    virtual DcmEVR ident(void) const = 0;

    // current value representation. If object was read from a stream
    // getVR returns the read value representation. It is possible that 
    // this vr is not the same as mentioned in the data dictionary
    // (e.g. private tags, encapsulated data ...)
    inline DcmEVR getVR(void) const { return Tag.getEVR(); }

    inline OFBool isaString(void) const { return Tag.getVR().isaString(); }

    virtual OFBool isLeaf(void) const = 0;
    virtual DcmObject * nextInContainer(const DcmObject * obj);
    virtual void print(ostream & out = cout, const OFBool showFullData = OFTrue,
		       const int level = 0) = 0;
    inline E_Condition error(void) const { return errorFlag; }

    inline E_TransferState transferState(void) const { return fTransferState; }
    virtual void transferInit(void);
    virtual void transferEnd(void);

    inline Uint16 getGTag() const { return Tag.getGTag(); }
    inline Uint16 getETag() const { return Tag.getETag(); }
    inline const DcmTag & getTag(void) const { return Tag; }


    virtual E_Condition setVR(DcmEVR /*vr*/) { return EC_IllegalCall; }
    virtual unsigned long getVM() = 0;

    // calculate length of Dicom element 
    virtual Uint32 calcElementLength(const E_TransferSyntax xfer,
				     const E_EncodingType enctype) = 0;

    // returns value length
    virtual Uint32 getLength(const E_TransferSyntax xfer 
			     = EXS_LittleEndianImplicit,
			     const E_EncodingType enctype 
			     = EET_UndefinedLength) = 0;

    virtual OFBool canWriteXfer(const E_TransferSyntax newXfer,
				 const E_TransferSyntax oldXfer) = 0;

    virtual E_Condition read(DcmStream & inStream,
			     const E_TransferSyntax ixfer,
			     const E_GrpLenEncoding glenc = EGL_noChange,
			     const Uint32 maxReadLength = DCM_MaxReadLength) = 0;

    virtual E_Condition write(DcmStream & outStream,
			      const E_TransferSyntax oxfer,
			      const E_EncodingType enctype = EET_UndefinedLength) = 0;

    virtual E_Condition clear() = 0;
    virtual E_Condition verify(const OFBool autocorrect = OFFalse) = 0;

    virtual E_Condition nextObject(DcmStack & stack, const OFBool intoSub);
	
    virtual E_Condition search(const DcmTagKey &xtag,	       // in
			       DcmStack &resultStack,	       // inout
			       E_SearchMode mode = ESM_fromHere,  // in
			       OFBool searchIntoSub = OFTrue );       // in

    virtual E_Condition searchErrors( DcmStack &resultStack );	       // inout

    virtual E_Condition loadAllDataIntoMemory(void) = 0;

}; // class DcmObject


#endif // DCOBJECT_H
