/*
**
** Author: Gerd Ehlers      27.04.94 -- First Creation
**         Andreas Barth    14.12.95 -- Unique design of Value Field
** Kuratorium OFFIS e.V.
**
** Module: dcelem.h
**
** Purpose:
**      Interface of class DcmElement
**
**
** Last Update:         $Author: joergr $
** Update Date:         $Date: 1998-07-15 15:48:47 $
** Source File:         $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcelem.h,v $
** CVS/RCS Revision:    $Revision: 1.14 $
** Status:              $State: Exp $
**
** CVS/RCS Log at end of file
**
*/


#ifndef DCELEM_H
#define DCELEM_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcerror.h"
#include "dctypes.h"
#include "dcobject.h"


class OFString;
class DcmLoadValueType;

class DcmElement : public DcmObject 
{
private:
    DcmLoadValueType * fLoadValue;      // Information to load Value later
    Uint8 * fValue;                     // Value of Element

 // --- declarations to avoid compiler warnings
 
    DcmElement &operator=(const DcmElement &);

protected:
    E_ByteOrder fByteOrder;

    void * getValue(const E_ByteOrder newByteOrder = gLocalByteOrder);


    E_Condition changeValue(const void * value,     // new Value
                            const Uint32 position,  // position in value array
                            const Uint32 num);      // number of new value bytes

    E_Condition putValue(const void * value,    // new value
                         const Uint32 length);  // number of new value bytes


    E_Condition loadValue(DcmStream * inStream = NULL);
    virtual void postLoadValue(void);

    virtual Uint8 * newValueField(void);

    void swapValueField(size_t valueWidth);

public:
    DcmElement(const DcmTag & tag, const Uint32 len = 0);
    DcmElement(const DcmElement & elem);
    virtual ~DcmElement();

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

    virtual E_Condition read(DcmStream & inStream, 
                             const E_TransferSyntax ixfer,
                             const E_GrpLenEncoding glenc = EGL_noChange, 
                             const Uint32 maxReadLength = DCM_MaxReadLength);

    virtual E_Condition write(DcmStream & outStream,
                              const E_TransferSyntax oxfer,
                              const E_EncodingType enctype 
                              = EET_UndefinedLength);


    virtual E_Condition clear(void);

    virtual E_Condition loadAllDataIntoMemory(void);

        // GET-Operations
        
    // get copies of individual components
    virtual E_Condition getUint8(Uint8 & val, const unsigned long pos = 0);
    virtual E_Condition getSint16(Sint16 & val, const unsigned long pos = 0);
    virtual E_Condition getUint16(Uint16 & val, const unsigned long pos = 0);
    virtual E_Condition getSint32(Sint32 & val, const unsigned long pos = 0);
    virtual E_Condition getUint32(Uint32 & val, const unsigned long pos = 0);
    virtual E_Condition getFloat32(Float32 & val, const unsigned long pos = 0);
    virtual E_Condition getFloat64(Float64 & val, const unsigned long pos = 0);
    virtual E_Condition getTagVal(DcmTagKey & val, const unsigned long pos = 0);

    // Gets a copy of one string value component.  For multi-valued 
    // string attributes (i.e those using \ separators), 
    // this method extracts the pos component (counting from zero base).
    virtual E_Condition getOFString(OFString & str,
                                    const unsigned long pos,
                                    OFBool normalize = OFTrue);
    // returns a copy of one string value component (perhaps normalized)
    // for multi-valued string attributes (e.g. those using \ separators)
    // this method extracts the pos component (zero base).
    virtual E_Condition getOFStringArray(OFString & val, OFBool normalize = OFTrue);

    // The following get operations do not copy, 
    // they return a reference of the element value 
    // The element value remains under control of the element
    // and is only valid until the next put.., read, or write
    // operation.
    virtual E_Condition getString(char * & val);        // for strings
    virtual E_Condition getUint8Array(Uint8 * & val);   // for bytes
    virtual E_Condition getSint16Array(Sint16 * & val);
    virtual E_Condition getUint16Array(Uint16 * & val);
    virtual E_Condition getSint32Array(Sint32 * & val);
    virtual E_Condition getUint32Array(Uint32 * & val);
    virtual E_Condition getFloat32Array(Float32 * & val);
    virtual E_Condition getFloat64Array(Float64 * & val);

    // detachValueField detaches the value field from the
    // DICOM element. After detaching the calling part of the
    // application has total control over the element value, especially
    // the value must be deleted from the heap after use. 
    // The DICOM element remains a copy of the value is the copy 
    // parameter is OFTrue else the value is erased in the DICOM
    // element.
    E_Condition detachValueField(OFBool copy = OFFalse);


// PUT-Operations
// Put operations copy the value.

    // Sets the value of a complete (possibly multi-valued) string attribute.
    virtual E_Condition putOFStringArray(const OFString& stringValue);

    // One Value
    virtual E_Condition putString(const char * val);

    // One Value at a position pos
    virtual E_Condition putSint16(const Sint16 val, const unsigned long pos = 0);
    virtual E_Condition putUint16(const Uint16 val, const unsigned long pos = 0);
    virtual E_Condition putSint32(const Sint32 val, const unsigned long pos = 0);
    virtual E_Condition putUint32(const Uint32 val, const unsigned long pos = 0);
    virtual E_Condition putFloat32(const Float32 val, const unsigned long pos = 0);
    virtual E_Condition putFloat64(const Float64 val, const unsigned long pos = 0);
    virtual E_Condition putTagVal(const DcmTagKey & attrTag, const unsigned long pos = 0);

    // num Values
    virtual E_Condition putUint8Array(const Uint8 * vals, const unsigned long num);
    virtual E_Condition putSint16Array(const Sint16 * vals, const unsigned long num);
    virtual E_Condition putUint16Array(const Uint16 * vals, const unsigned long num);
    virtual E_Condition putSint32Array(const Sint32 * vals, const unsigned long num);
    virtual E_Condition putUint32Array(const Uint32 * vals, const unsigned long num);
    virtual E_Condition putFloat32Array(const Float32 * vals, const unsigned long num);
    virtual E_Condition putFloat64Array(const Float64 * vals, const unsigned long num);

};


#endif // DCELEM_H

/*
** CVS/RCS Log:
** $Log: dcelem.h,v $
** Revision 1.14  1998-07-15 15:48:47  joergr
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
**   E_Condition get(Uint16 & value, const unsigned long pos);
**   becomes
**   E_Condition getUint16(Uint16 & value, const unsigned long pos);
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
**
**
*/

