/*
 *
 *  Copyright (C) 1994-2000, OFFIS
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
 *  Author:  Gerd Ehlers, Andreas Barth
 *
 *  Purpose: Interface of class DcmByteString
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2000-11-07 16:56:05 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcbytstr.h,v $
 *  CVS/RCS Revision: $Revision: 1.20 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCBYTSTR_H
#define DCBYTSTR_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "ofconsol.h"
#include "dcerror.h"
#include "dctypes.h"
#include "dcelem.h"

class OFString;

typedef enum
{
	DCM_MachineString,
	DCM_DicomString,
	DCM_UnknownString
} E_StringMode;

class DcmByteString : public DcmElement 
{
private:
    Uint32 realLength;
    E_StringMode fStringMode;

protected:
    char paddingChar;
    Uint32 maxLength;

    virtual Uint8 * newValueField(void);
    virtual void postLoadValue(void);

    E_Condition makeMachineByteString(void);
    E_Condition makeDicomByteString(void);


public:
    DcmByteString(const DcmTag &tag,
		  const Uint32 len = 0);
    DcmByteString(const DcmByteString& old);

    virtual ~DcmByteString();
    DcmByteString& operator=(const DcmByteString& obj);
    
    virtual DcmEVR ident(void) const { return EVR_UNKNOWN; } 
    virtual void print(ostream & out, const OFBool showFullData = OFTrue,
		       const int level = 0, const char *pixelFileName = NULL,
		       size_t *pixelCounter = NULL);
    virtual unsigned long getVM();

    Uint32 getRealLength(void);

    virtual Uint32 getLength(
	const E_TransferSyntax xfer = EXS_LittleEndianImplicit,
	const E_EncodingType enctype = EET_UndefinedLength);

    virtual E_Condition write(DcmStream & outStream,
			      const E_TransferSyntax oxfer,
			      const E_EncodingType enctype 
			      = EET_UndefinedLength);

    /** special write method for creation of digital signatures
     */
    virtual E_Condition writeSignatureFormat(DcmStream & outStream,
					 const E_TransferSyntax oxfer,
					 const E_EncodingType enctype 
					 = EET_UndefinedLength);

    virtual E_Condition putString(const char *byteStringValue);

    // Sets the value of a complete (possibly multi-valued) string attribute.
    virtual E_Condition putOFStringArray(const OFString& stringValue);

    // Gets a copy of one string value component.  For multi-valued 
    // string attributes (i.e those using \ separators), 
    // this method extracts the pos component (counting from zero base).
    // The pos parameter does not have a default value to make explicit
    // the selective retrieval action of this method.
    virtual E_Condition getOFString(OFString & str,
				    const unsigned long pos,
				    OFBool normalize = OFTrue);
 
    // Gets a copy of the complete string value (including 
    // all components and separators).
    virtual E_Condition getOFStringArray(OFString & str, OFBool normalize);

    // Gets a pointer to the current string value (including 
    // all components and separators). 
    virtual E_Condition getString(char * & byteStringValue);

    virtual E_Condition clear();
    virtual E_Condition verify(const OFBool autocorrect = OFFalse);
};

/* Function to get part out of a String for VM > 1 */

E_Condition getStringPart(
    OFString & result,
    char * orgStr,
    const unsigned long which);


/* Function for String Normalisation - delete leading and trailing Spaces */

const OFBool DELETE_TRAILING = OFTrue;
const OFBool DELETE_LEADING = OFTrue;
const OFBool MULTIPART = OFTrue;

void normalizeString(
    OFString & str,
    const OFBool multiPart,    
    const OFBool leading,
    const OFBool trailing);
 

#endif // DCBYTSTR_H

/*
** CVS/RCS Log:
** $Log: dcbytstr.h,v $
** Revision 1.20  2000-11-07 16:56:05  meichel
** Initial release of dcmsign module for DICOM Digital Signatures
**
** Revision 1.19  2000/04/14 15:31:31  meichel
** Removed default value from output stream passed to print() method.
**   Required for use in multi-thread environments.
**
** Revision 1.18  2000/03/08 16:26:11  meichel
** Updated copyright header.
**
** Revision 1.17  2000/03/03 14:05:22  meichel
** Implemented library support for redirecting error messages into memory
**   instead of printing them to stdout/stderr for GUI applications.
**
** Revision 1.16  2000/02/10 10:50:49  joergr
** Added new feature to dcmdump (enhanced print method of dcmdata): write
** pixel data/item value fields to raw files.
**
** Revision 1.15  1999/03/31 09:24:30  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.14  1998/11/12 16:47:36  meichel
** Implemented operator= for all classes derived from DcmObject.
**
** Revision 1.13  1997/09/11 15:13:09  hewett
** Modified getOFString method arguments by removing a default value
** for the pos argument.  By requiring the pos argument to be provided
** ensures that callers realise getOFString only gets one component of
** a multi-valued string.
**
** Revision 1.12  1997/08/29 13:11:38  andreas
** Corrected Bug in getOFStringArray Implementation
**
** Revision 1.11  1997/08/29 08:32:37  andreas
** - Added methods getOFString and getOFStringArray for all
**   string VRs. These methods are able to normalise the value, i. e.
**   to remove leading and trailing spaces. This will be done only if
**   it is described in the standard that these spaces are not relevant.
**   These methods do not test the strings for conformance, this means
**   especially that they do not delete spaces where they are not allowed!
**   getOFStringArray returns the string with all its parts separated by \
**   and getOFString returns only one value of the string.
**   CAUTION: Currently getString returns a string with trailing
**   spaces removed (if dcmEnableAutomaticInputDataCorrection == OFTrue) and
**   truncates the original string (since it is not copied!). If you rely on this
**   behaviour please change your application now.
**   Future changes will ensure that getString returns the original
**   string from the DICOM object (NULL terminated) inclusive padding.
**   Currently, if you call getOF... before calling getString without
**   normalisation, you can get the original string read from the DICOM object.
**
** Revision 1.10  1997/07/21 08:25:05  andreas
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.9  1997/05/16 08:31:19  andreas
** - Revised handling of GroupLength elements and support of
**   DataSetTrailingPadding elements. The enumeratio E_GrpLenEncoding
**   got additional enumeration values (for a description see dctypes.h).
**   addGroupLength and removeGroupLength methods are replaced by
**   computeGroupLengthAndPadding. To support Padding, the parameters of
**   element and sequence write functions changed.
**
** Revision 1.8  1997/04/18 08:13:28  andreas
** - The put/get-methods for all VRs did not conform to the C++-Standard
**   draft. Some Compilers (e.g. SUN-C++ Compiler, Metroworks
**   CodeWarrier, etc.) create many warnings concerning the hiding of
**   overloaded get methods in all derived classes of DcmElement.
**   So the interface of all value representation classes in the
**   library are changed rapidly, e.g.
**   E_Condition get(Uint16 & value, const unsigned long pos);
**   becomes
**   E_Condition getUint16(Uint16 & value, const unsigned long pos);
**   All (retired) "returntype get(...)" methods are deleted.
**   For more information see dcmdata/include/dcelem.h
**
** Revision 1.7  1996/08/05 08:45:15  andreas
** new print routine with additional parameters:
**         - print into files
**         - fix output length for elements
** corrected error in search routine with parameter ESM_fromStackTop
**
** Revision 1.6  1996/03/12 15:26:52  hewett
** Removed get method for unsigned char*
**
** Revision 1.5  1996/01/29 13:38:11  andreas
** - new put method for every VR to put value as a string
** - better and unique print methods
**
** Revision 1.4  1996/01/09 11:06:13  andreas
** New Support for Visual C++
** Correct problems with inconsistent const declarations
**
** Revision 1.3  1996/01/05 13:22:52  andreas
** - changed to support new streaming facilities
** - more cleanups
** - merged read / write methods for block and file transfer
**
*/
