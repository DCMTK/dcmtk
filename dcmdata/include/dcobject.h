/*
 *
 *  Copyright (C) 1994-2001, OFFIS
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
 *  Module:  dcmdata
 *
 *  Author:  Gerd Ehlers
 *
 *  Purpose: 
 *  This file contains the interface to routines which provide
 *  DICOM object encoding/decoding, search and lookup facilities.
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2001-09-25 17:19:27 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcobject.h,v $
 *  CVS/RCS Revision: $Revision: 1.27 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCOBJECT_H
#define DCOBJECT_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "ofconsol.h"
#include "ofglobal.h"
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
extern OFGlobal<OFBool> dcmEnableAutomaticInputDataCorrection; /* default OFTrue */

/*
** The base dicom object class
*/

class DcmObject 
{
protected:
    DcmTag Tag;
    Uint32 Length;
    E_TransferState fTransferState;
    OFCondition errorFlag;  // defined after fTransferState to workaround 
                            // memory layout problem with Borland C++
    Uint32 fTransferredBytes;

    // The next two functions require that the memory for the info
    // field is minimum 4 chars longer than strlen(info)
    virtual void printInfoLine(ostream & out, const OFBool showFullData,
                               const int level, const char *info );
    virtual void printInfoLine(ostream & out, const OFBool showFullData,
                               const int level, DcmTag &tag,
                               const Uint32 length, const char *info );

    OFCondition writeTag(DcmStream & outStream, const DcmTag & tag,
                         const E_TransferSyntax oxfer); // in

    virtual OFCondition writeTagAndLength(DcmStream & outStream,  
                                          const E_TransferSyntax oxfer, // in
                                          Uint32 & writtenBytes ); // out

public:
    DcmObject(const DcmTag & tag, const Uint32 len = 0);
    DcmObject(const DcmObject & obj);

    virtual ~DcmObject();

    DcmObject &operator=(const DcmObject &obj);
    
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
    virtual void print(ostream & out, const OFBool showFullData = OFTrue,
                       const int level = 0, const char *pixelFileName = NULL,
                       size_t *pixelCounter = NULL) = 0;
    inline OFCondition error(void) const { return errorFlag; }

    inline E_TransferState transferState(void) const { return fTransferState; }
    virtual void transferInit(void);
    virtual void transferEnd(void);

    inline Uint16 getGTag() const { return Tag.getGTag(); }
    inline Uint16 getETag() const { return Tag.getETag(); }
    inline const DcmTag & getTag() const { return Tag; }
    inline void setGTag(Uint16 gtag) { Tag.setGroup(gtag); }

    virtual OFCondition setVR(DcmEVR /*vr*/) { return EC_IllegalCall; }
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

    virtual OFCondition read(DcmStream & inStream,
                             const E_TransferSyntax ixfer,
                             const E_GrpLenEncoding glenc = EGL_noChange,
                             const Uint32 maxReadLength = DCM_MaxReadLength) = 0;

    virtual OFCondition write(DcmStream & outStream,
                              const E_TransferSyntax oxfer,
                              const E_EncodingType enctype = EET_UndefinedLength) = 0;

    /** special write method for creation of digital signatures
     */
    virtual OFCondition writeSignatureFormat(DcmStream & outStream,
					 const E_TransferSyntax oxfer,
					 const E_EncodingType enctype 
					 = EET_UndefinedLength) = 0;

    virtual OFCondition clear() = 0;
    virtual OFCondition verify(const OFBool autocorrect = OFFalse) = 0;

    virtual OFCondition nextObject(DcmStack & stack, const OFBool intoSub);
        
    virtual OFCondition search(const DcmTagKey &xtag,          // in
                               DcmStack &resultStack,          // inout
                               E_SearchMode mode = ESM_fromHere,  // in
                               OFBool searchIntoSub = OFTrue );       // in

    virtual OFCondition searchErrors( DcmStack &resultStack );         // inout

    virtual OFCondition loadAllDataIntoMemory(void) = 0;

}; // class DcmObject

#endif // DCOBJECT_H

/*
 * CVS/RCS Log:
 * $Log: dcobject.h,v $
 * Revision 1.27  2001-09-25 17:19:27  meichel
 * Adapted dcmdata to class OFCondition
 *
 * Revision 1.26  2001/06/01 15:48:41  meichel
 * Updated copyright header
 *
 * Revision 1.25  2000/11/07 16:56:07  meichel
 * Initial release of dcmsign module for DICOM Digital Signatures
 *
 * Revision 1.24  2000/04/14 16:02:39  meichel
 * Global flag dcmEnableAutomaticInputDataCorrection now derived from OFGlobal
 *   and, thus, safe for use in multi-thread applications.
 *
 * Revision 1.23  2000/03/08 16:26:16  meichel
 * Updated copyright header.
 *
 * Revision 1.22  2000/03/03 14:05:24  meichel
 * Implemented library support for redirecting error messages into memory
 *   instead of printing them to stdout/stderr for GUI applications.
 *
 * Revision 1.21  2000/02/10 10:50:52  joergr
 * Added new feature to dcmdump (enhanced print method of dcmdata): write
 * pixel data/item value fields to raw files.
 *
 * Revision 1.20  2000/02/01 10:12:02  meichel
 * Avoiding to include <stdlib.h> as extern "C" on Borland C++ Builder 4,
 *   workaround for bug in compiler header files.
 *
 * Revision 1.19  1999/03/31 09:24:42  meichel
 * Updated copyright header in module dcmdata
 *
 *
 */
