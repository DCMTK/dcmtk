/*
 *
 *  Copyright (C) 1994-2002, OFFIS
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
 *  Purpose: Interface of class DcmElement
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2002-08-27 16:55:31 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcelem.h,v $
 *  CVS/RCS Revision: $Revision: 1.24 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCELEM_H
#define DCELEM_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcerror.h"
#include "dctypes.h"
#include "dcobject.h"


class OFString;
class DcmInputStreamFactory;

class DcmElement : public DcmObject 
{
private:
    DcmInputStreamFactory * fLoadValue; // Information to load Value later
    Uint8 * fValue;                     // Value of Element

protected:
    E_ByteOrder fByteOrder;

    void * getValue(const E_ByteOrder newByteOrder = gLocalByteOrder);


    OFCondition changeValue(const void * value,     // new Value
                            const Uint32 position,  // position in value array
                            const Uint32 num);      // number of new value bytes

    OFCondition putValue(const void * value,    // new value
                         const Uint32 length);  // number of new value bytes

    OFCondition createEmptyValue(const Uint32 length); // number of new value bytes

    OFCondition loadValue(DcmInputStream * inStream = NULL);
    virtual void postLoadValue(void);

    virtual Uint8 * newValueField(void);

    void swapValueField(size_t valueWidth);

    /** write element start tag in XML format
     *  @param out output stream to which the XML start tag is written
     *  @param flags flag used to customize the output (not yet used)
     *  @param attrText extra attribute text to be added to the element tag
     */
    virtual void writeXMLStartTag(ostream &out,
                                  const size_t flags,
                                  const char *attrText = NULL);

    /** write element end tag in XML format
     *  @param out output stream to which the XML end tag is written
     *  @param flags flag used to customize the output (not yet used)
     */
    virtual void writeXMLEndTag(ostream &out,
                                const size_t flags);

public:
    DcmElement(const DcmTag & tag, const Uint32 len = 0);
    DcmElement(const DcmElement & elem);
    virtual ~DcmElement();

    DcmElement &operator=(const DcmElement &obj);

    // returns length of element with tag, vr, ...
    virtual Uint32 calcElementLength(const E_TransferSyntax xfer,
                                     const E_EncodingType enctype);

    // returns length of value
    virtual Uint32 getLength(const E_TransferSyntax /*xfer*/ 
                             = EXS_LittleEndianImplicit,
                             const E_EncodingType /*enctype*/ 
                             = EET_UndefinedLength)
    {
        return Length;
    }

    virtual OFBool isLeaf(void) const { return OFTrue; }
    inline OFBool valueLoaded(void) { return fValue != NULL || Length == 0; }

    virtual void transferInit(void);

    virtual OFBool canWriteXfer(const E_TransferSyntax newXfer,
                              const E_TransferSyntax oldXfer);

    virtual OFCondition read(DcmInputStream & inStream, 
                             const E_TransferSyntax ixfer,
                             const E_GrpLenEncoding glenc = EGL_noChange, 
                             const Uint32 maxReadLength = DCM_MaxReadLength);

    virtual OFCondition write(DcmOutputStream & outStream,
                              const E_TransferSyntax oxfer,
                              const E_EncodingType enctype 
                              = EET_UndefinedLength);

    /** write object in XML format
     *  @param out output stream to which the XML document is written
     *  @param flags optional flag used to customize the output (see DCMTypes::XF_xxx)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition writeXML(ostream &out,
                                 const size_t flags = 0);

    /** special write method for creation of digital signatures
     */
    virtual OFCondition writeSignatureFormat(DcmOutputStream & outStream,
					 const E_TransferSyntax oxfer,
					 const E_EncodingType enctype 
					 = EET_UndefinedLength);

    virtual OFCondition clear(void);

    virtual OFCondition loadAllDataIntoMemory(void);

        // GET-Operations
        
    // get copies of individual components
    virtual OFCondition getUint8(Uint8 & val, const unsigned long pos = 0);
    virtual OFCondition getSint16(Sint16 & val, const unsigned long pos = 0);
    virtual OFCondition getUint16(Uint16 & val, const unsigned long pos = 0);
    virtual OFCondition getSint32(Sint32 & val, const unsigned long pos = 0);
    virtual OFCondition getUint32(Uint32 & val, const unsigned long pos = 0);
    virtual OFCondition getFloat32(Float32 & val, const unsigned long pos = 0);
    virtual OFCondition getFloat64(Float64 & val, const unsigned long pos = 0);
    virtual OFCondition getTagVal(DcmTagKey & val, const unsigned long pos = 0);

    // Gets a copy of one string value component.  For multi-valued 
    // string attributes (i.e those using \ separators), 
    // this method extracts the pos component (counting from zero base).
    virtual OFCondition getOFString(OFString & str,
                                    const unsigned long pos,
                                    OFBool normalize = OFTrue);

    /** get entire element value as a character string.
     *  In case of VM > 1 the single values are separated by a backslash ('\').
     *  This method implements a general approach by concatenating the results of
     *  getOFString() for each value component. Derived class may implement more
     *  sophiticated methods.
     *  @param value variable in which the result value is stored
     *  @param normalize normalize each element value prior to concatenation
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getOFStringArray(OFString &value,
                                         OFBool normalize = OFTrue);

    // The following get operations do not copy, 
    // they return a reference of the element value 
    // The element value remains under control of the element
    // and is only valid until the next put.., read, or write
    // operation.
    virtual OFCondition getString(char * & val);        // for strings
    virtual OFCondition getUint8Array(Uint8 * & val);   // for bytes
    virtual OFCondition getSint16Array(Sint16 * & val);
    virtual OFCondition getUint16Array(Uint16 * & val);
    virtual OFCondition getSint32Array(Sint32 * & val);
    virtual OFCondition getUint32Array(Uint32 * & val);
    virtual OFCondition getFloat32Array(Float32 * & val);
    virtual OFCondition getFloat64Array(Float64 * & val);

    // detachValueField detaches the value field from the
    // DICOM element. After detaching the calling part of the
    // application has total control over the element value, especially
    // the value must be deleted from the heap after use. 
    // The DICOM element remains a copy of the value if the copy 
    // parameter is OFTrue else the value is erased in the DICOM
    // element.
    OFCondition detachValueField(OFBool copy = OFFalse);


// PUT-Operations
// Put operations copy the value.

    // Sets the value of a complete (possibly multi-valued) string attribute.
    virtual OFCondition putOFStringArray(const OFString& stringValue);

    // One Value
    virtual OFCondition putString(const char * val);

    // One Value at a position pos
    virtual OFCondition putSint16(const Sint16 val, const unsigned long pos = 0);
    virtual OFCondition putUint16(const Uint16 val, const unsigned long pos = 0);
    virtual OFCondition putSint32(const Sint32 val, const unsigned long pos = 0);
    virtual OFCondition putUint32(const Uint32 val, const unsigned long pos = 0);
    virtual OFCondition putFloat32(const Float32 val, const unsigned long pos = 0);
    virtual OFCondition putFloat64(const Float64 val, const unsigned long pos = 0);
    virtual OFCondition putTagVal(const DcmTagKey & attrTag, const unsigned long pos = 0);

    // num Values
    virtual OFCondition putUint8Array(const Uint8 * vals, const unsigned long num);
    virtual OFCondition putSint16Array(const Sint16 * vals, const unsigned long num);
    virtual OFCondition putUint16Array(const Uint16 * vals, const unsigned long num);
    virtual OFCondition putSint32Array(const Sint32 * vals, const unsigned long num);
    virtual OFCondition putUint32Array(const Uint32 * vals, const unsigned long num);
    virtual OFCondition putFloat32Array(const Float32 * vals, const unsigned long num);
    virtual OFCondition putFloat64Array(const Float64 * vals, const unsigned long num);

};


#endif // DCELEM_H

/*
** CVS/RCS Log:
** $Log: dcelem.h,v $
** Revision 1.24  2002-08-27 16:55:31  meichel
** Initial release of new DICOM I/O stream classes that add support for stream
**   compression (deflated little endian explicit VR transfer syntax)
**
** Revision 1.23  2002/04/25 10:06:09  joergr
** Added/modified getOFStringArray() implementation.
** Added support for XML output of DICOM objects.
**
** Revision 1.22  2001/09/25 17:19:25  meichel
** Adapted dcmdata to class OFCondition
**
** Revision 1.21  2001/06/01 15:48:39  meichel
** Updated copyright header
**
** Revision 1.20  2001/05/10 12:50:21  meichel
** Added protected createEmptyValue() method in class DcmElement.
**
** Revision 1.19  2000/11/07 16:56:06  meichel
** Initial release of dcmsign module for DICOM Digital Signatures
**
** Revision 1.18  2000/03/08 16:26:14  meichel
** Updated copyright header.
**
** Revision 1.17  2000/02/02 14:31:16  joergr
** Replaced 'delete' statements by 'delete[]' for objects created with 'new[]'.
**
** Revision 1.16  1999/03/31 09:24:37  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.15  1998/11/12 16:47:38  meichel
** Implemented operator= for all classes derived from DcmObject.
**
** Revision 1.14  1998/07/15 15:48:47  joergr
** Removed several compiler warnings reported by gcc 2.8.1 with
** additional options, e.g. missing copy constructors and assignment
** operators, initialization of member variables in the body of a
** constructor instead of the member initialization list, hiding of
** methods by use of identical names, uninitialized member variables,
** missing const declaration of char pointers. Replaced tabs by spaces.
**
** Revision 1.13  1997/09/11 15:13:10  hewett
** Modified getOFString method arguments by removing a default value
** for the pos argument.  By requiring the pos argument to be provided
** ensures that callers realise getOFString only gets one component of
** a multi-valued string.
**
** Revision 1.12  1997/08/29 08:32:38  andreas
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
** Revision 1.11  1997/07/31 06:57:59  andreas
** new protected method swapValueField for DcmElement
**
** Revision 1.10  1997/07/21 07:57:53  andreas
** - New method DcmElement::detachValueField to give control over the
**   value field to the calling part (see dcelem.h)
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.9  1997/05/27 13:48:26  andreas
** - Add method canWriteXfer to class DcmObject and all derived classes.
**   This method checks whether it is possible to convert the original
**   transfer syntax to an new transfer syntax. The check is used in the
**   dcmconv utility to prohibit the change of a compressed transfer
**   syntax to a uncompressed.
**
** Revision 1.8  1997/05/16 08:23:46  andreas
** - Revised handling of GroupLength elements and support of
**   DataSetTrailingPadding elements. The enumeratio E_GrpLenEncoding
**   got additional enumeration values (for a description see dctypes.h).
**   addGroupLength and removeGroupLength methods are replaced by
**   computeGroupLengthAndPadding. To support Padding, the parameters of
**   element and sequence write functions changed.
** - Added a new method calcElementLength to calculate the length of an
**   element, item or sequence. For elements it returns the length of
**   tag, length field, vr field, and value length, for item and
**   sequences it returns the length of the whole item. sequence including
**   the Delimitation tag (if appropriate).  It can never return
**   UndefinedLength.
**
** Revision 1.7  1997/04/18 08:13:28  andreas
** - The put/get-methods for all VRs did not conform to the C++-Standard
**   draft. Some Compilers (e.g. SUN-C++ Compiler, Metroworks
**   CodeWarrier, etc.) create many warnings concerning the hiding of
**   overloaded get methods in all derived classes of DcmElement.
**   So the interface of all value representation classes in the
**   library are changed rapidly, e.g.
**   OFCondition get(Uint16 & value, const unsigned long pos);
**   becomes
**   OFCondition getUint16(Uint16 & value, const unsigned long pos);
**   All (retired) "returntype get(...)" methods are deleted.
**   For more information see dcmdata/include/dcelem.h
**
** Revision 1.6  1996/07/17 12:38:58  andreas
** new nextObject to iterate a DicomDataset, DicomFileFormat, Item, ...
**
** Revision 1.5  1996/04/16 16:01:36  andreas
** - put methods for AttributeTag with DcmTagKey Parameter
** - better support for NULL values
**
** Revision 1.4  1996/03/12 15:31:56  hewett
** The base virtual get & put functions now support char*.
**
** Revision 1.3  1996/01/05 13:22:55  andreas
** - changed to support new streaming facilities
** - more cleanups
** - merged read / write methods for block and file transfer
**
*/

