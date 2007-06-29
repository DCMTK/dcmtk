/*
 *
 *  Copyright (C) 1994-2007, OFFIS
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
 *  Update Date:      $Date: 2007-06-29 14:17:49 $
 *  CVS/RCS Revision: $Revision: 1.33 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DCELEM_H
#define DCELEM_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dcerror.h"
#include "dcmtk/dcmdata/dctypes.h"
#include "dcmtk/dcmdata/dcobject.h"
#include "dcmtk/ofstd/ofstring.h"

// forward declarations
class DcmInputStreamFactory;


/** abstract base class for all DICOM elements
 */
class DcmElement
  : public DcmObject
{

  public:

    /** constructor.
     *  Create new element from given tag and length.
     *  @param tag DICOM tag for the new element
     *  @param len value length for the new element
     */
    DcmElement(const DcmTag &tag,
               const Uint32 len = 0);

    /** copy constructor
     *  @param old element to be copied
     */
    DcmElement(const DcmElement &old);

    /** destructor
     */
    virtual ~DcmElement();

    /** assignment operator
     *  @param obj element to be assigned/copied
     *  @return reference to this object
     */
    DcmElement &operator=(const DcmElement &obj);

    // returns length of element with tag, vr, ...
    virtual Uint32 calcElementLength(const E_TransferSyntax xfer,
                                     const E_EncodingType enctype);

    // returns length of value
    virtual Uint32 getLength(const E_TransferSyntax /*xfer*/ = EXS_LittleEndianImplicit,
                             const E_EncodingType /*enctype*/ = EET_UndefinedLength)
    {
        return getLengthField();
    }

    virtual OFBool isLeaf() const { return OFTrue; }
    inline OFBool valueLoaded() { return fValue != NULL || getLengthField() == 0; }

    virtual void transferInit();

    virtual OFBool canWriteXfer(const E_TransferSyntax newXfer,
                                const E_TransferSyntax oldXfer);

    /** This function reads the data value of an attribute which is
     *  captured in the input stream and captures this information
     *  in this. If not all information for an attribute could be
     *  read from the stream, the function returns EC_StreamNotifyClient.
     *  Note that if certain conditions are met, this function does
     *  not actually load the data value but creates and stores an object
     *  that enables us to load this information later.
     *  @param inStream      The stream which contains the information.
     *  @param ixfer         The transfer syntax which was used to encode
     *                       the information in inStream.
     *  @param glenc         Encoding type for group length; specifies what
     *                       will be done with group length tags.
     *  @param maxReadLength Maximum read length for reading an attribute value.
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition read(DcmInputStream &inStream,
                             const E_TransferSyntax ixfer,
                             const E_GrpLenEncoding glenc = EGL_noChange,
                             const Uint32 maxReadLength = DCM_MaxReadLength);

    /** This function writes this element's value to the outstream which was
     *  passed. When writing information, the byte ordering (little or big endian)
     *  of the transfer syntax which was passed will be accounted for. In case the
     *  outstream does not provide enough space for all bytes of the current
     *  element's value, only a certain part of the value will be written to the
     *  stream. This element's transfer state indicates if the all bytes of value
     *  have already been written to the stream (ERW_ready), if the writing is
     *  still in progress and more bytes need to be written to the stream
     *  (ERW_inWork) or if the writing of the bytes of this element's value has not
     *  even begun yet (ERW_init). The member variable fTransferredBytes indicates
     *  how many bytes (starting from byte 0) of this element's value have already
     *  been written to the stream. This function will return EC_Normal, if the
     *  entire value of this element has been written to the stream, it will return
     *  EC_StreamNotifyClient, if there is no more space in the buffer and _not_ all
     *  bytes of this element's value have been written, and it will return some
     *  other (error) value if there was an error.
     *  @param outStream The stream the information will be written to.
     *  @param oxfer The transfer syntax which shall be used.
     *  @param enctype encoding types (undefined or explicit length) (actually unused)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition write(DcmOutputStream &outStream,
                              const E_TransferSyntax oxfer,
                              const E_EncodingType enctype = EET_UndefinedLength);

    /** write object in XML format
     *  @param out output stream to which the XML document is written
     *  @param flags optional flag used to customize the output (see DCMTypes::XF_xxx)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition writeXML(STD_NAMESPACE ostream &out,
                                 const size_t flags = 0);

    /** special write method for creation of digital signatures
     *  @param outStream DICOM output stream
     *  @param oxfer output transfer syntax
     *  @param enctype encoding types (undefined or explicit length)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition writeSignatureFormat(DcmOutputStream &outStream,
                                             const E_TransferSyntax oxfer,
                                             const E_EncodingType enctype = EET_UndefinedLength);

    virtual OFCondition clear();

    virtual OFCondition loadAllDataIntoMemory();

    // GET-Operations

    // get copies of individual components
    virtual OFCondition getUint8(Uint8 &val, const unsigned long pos = 0);
    virtual OFCondition getSint16(Sint16 &val, const unsigned long pos = 0);
    virtual OFCondition getUint16(Uint16 &val, const unsigned long pos = 0);
    virtual OFCondition getSint32(Sint32 &val, const unsigned long pos = 0);
    virtual OFCondition getUint32(Uint32 &val, const unsigned long pos = 0);
    virtual OFCondition getFloat32(Float32 &val, const unsigned long pos = 0);
    virtual OFCondition getFloat64(Float64 &val, const unsigned long pos = 0);
    virtual OFCondition getTagVal(DcmTagKey &val, const unsigned long pos = 0);

    // Gets a copy of one string value component.  For multi-valued
    // string attributes (i.e those using \ separators),
    // this method extracts the pos component (counting from zero base).
    virtual OFCondition getOFString(OFString &str,
                                    const unsigned long pos,
                                    OFBool normalize = OFTrue);

    /** get entire element value as a character string.
     *  In case of VM > 1 the single values are separated by a backslash ('\').
     *  This method implements a general approach by concatenating the results of
     *  getOFString() for each value component. Derived class may implement more
     *  sophisticated methods.
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
    virtual OFCondition getString(char *&val);        // for strings
    virtual OFCondition getUint8Array(Uint8 *&val);   // for bytes
    virtual OFCondition getSint16Array(Sint16 *&val);
    virtual OFCondition getUint16Array(Uint16 *&val);
    virtual OFCondition getSint32Array(Sint32 *&val);
    virtual OFCondition getUint32Array(Uint32 *&val);
    virtual OFCondition getFloat32Array(Float32 *&val);
    virtual OFCondition getFloat64Array(Float64 *&val);

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
    virtual OFCondition putOFStringArray(const OFString &stringValue);

    // One Value
    virtual OFCondition putString(const char *val);

    // One Value at a position pos
    virtual OFCondition putSint16(const Sint16 val, const unsigned long pos = 0);
    virtual OFCondition putUint16(const Uint16 val, const unsigned long pos = 0);
    virtual OFCondition putSint32(const Sint32 val, const unsigned long pos = 0);
    virtual OFCondition putUint32(const Uint32 val, const unsigned long pos = 0);
    virtual OFCondition putFloat32(const Float32 val, const unsigned long pos = 0);
    virtual OFCondition putFloat64(const Float64 val, const unsigned long pos = 0);
    virtual OFCondition putTagVal(const DcmTagKey &attrTag, const unsigned long pos = 0);

    // num Values
    virtual OFCondition putUint8Array(const Uint8 *vals, const unsigned long num);
    virtual OFCondition putSint16Array(const Sint16 *vals, const unsigned long num);
    virtual OFCondition putUint16Array(const Uint16 *vals, const unsigned long num);
    virtual OFCondition putSint32Array(const Sint32 *vals, const unsigned long num);
    virtual OFCondition putUint32Array(const Uint32 *vals, const unsigned long num);
    virtual OFCondition putFloat32Array(const Float32 *vals, const unsigned long num);
    virtual OFCondition putFloat64Array(const Float64 *vals, const unsigned long num);

    /** create an empty Uint8 array of given number of bytes and set it.
     *  All array elements are initialized with a value of 0 (using 'memzero').
     *  This method is only applicable to certain VRs, e.g. OB.
     *  @param numBytes number of bytes (8 bit) to be created
     *  @param bytes stores the pointer to the resulting buffer
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition createUint8Array(const Uint32 numBytes, Uint8 *&bytes);

    /** create an empty Uint16 array of given number of words and set it.
     *  All array elements are initialized with a value of 0 (using 'memzero').
     *  This method is only applicable to OW data.
     *  @param numWords number of words (16 bit) to be created
     *  @param words stores the pointer to the resulting buffer
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition createUint16Array(const Uint32 numWords, Uint16 *&words);

  protected:

    /** This function returns this element's value. The returned value
     *  corresponds to the byte ordering (little or big endian) that
     *  was passed.
     *  @param newByteOrder The byte ordering that shall be accounted
     *                      for (little or big endian).
     */
    void *getValue(const E_ByteOrder newByteOrder = gLocalByteOrder);


    OFCondition changeValue(const void *value,      // new Value
                            const Uint32 position,  // position in value array
                            const Uint32 num);      // number of new value bytes

    OFCondition putValue(const void *value,     // new value
                         const Uint32 length);  // number of new value bytes

    OFCondition createEmptyValue(const Uint32 length); // number of new value bytes


    /** This function reads the data value of an attribute and stores the
     *  information which was read in this. The information is either read
     *  from the inStream or (if inStream is NULL) from a different stream
     *  which was created earlier and which is accessible through the fLoadValue
     *  member variable. Note that if not all information for an attribute
     *  could be read from the stream, the function returns EC_StreamNotifyClient.
     *  @param inStream The stream which contains the information.
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition loadValue(DcmInputStream *inStream = NULL);

    virtual void postLoadValue();

    /** This function creates a byte array of Length bytes and returns this
     *  array. In case Length is odd, an array of Length+1 bytes will be
     *  created and Length will be increased by 1.
     */
    virtual Uint8 *newValueField();

    void swapValueField(size_t valueWidth);

    /** write element start tag in XML format
     *  @param out output stream to which the XML start tag is written
     *  @param flags flag used to customize the output (not yet used)
     *  @param attrText extra attribute text to be added to the element tag
     */
    virtual void writeXMLStartTag(STD_NAMESPACE ostream &out,
                                  const size_t flags,
                                  const char *attrText = NULL);

    /** write element end tag in XML format
     *  @param out output stream to which the XML end tag is written
     *  @param flags flag used to customize the output (not yet used)
     */
    virtual void writeXMLEndTag(STD_NAMESPACE ostream &out,
                                const size_t flags);

    /** return the current byte order of the value field
     *  @return current byte order of the value field
     */
    E_ByteOrder getByteOrder() const { return fByteOrder; }

    /** set the current byte order of the value field
     *  @param val byte order of the value field
     */
    void setByteOrder(E_ByteOrder val) { fByteOrder = val; }

  private:

    /// current byte order of attribute value in memory
    E_ByteOrder fByteOrder;

    /// required information to load value later
    DcmInputStreamFactory *fLoadValue;

    /// value of the element
    Uint8 *fValue;
};


#endif // DCELEM_H


/*
** CVS/RCS Log:
** $Log: dcelem.h,v $
** Revision 1.33  2007-06-29 14:17:49  meichel
** Code clean-up: Most member variables in module dcmdata are now private,
**   not protected anymore.
**
** Revision 1.32  2007/06/07 09:01:15  joergr
** Added createUint8Array() and createUint16Array() methods.
**
** Revision 1.31  2006/10/13 10:12:02  joergr
** Fixed wrong formatting.
**
** Revision 1.30  2006/08/15 15:49:56  meichel
** Updated all code in module dcmdata to correctly compile when
**   all standard C++ classes remain in namespace std.
**
** Revision 1.29  2005/12/08 16:28:11  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.28  2004/07/01 12:28:25  meichel
** Introduced virtual clone method for DcmObject and derived classes.
**
** Revision 1.27  2003/07/04 13:25:35  meichel
** Replaced forward declarations for OFString with explicit includes,
**   needed when compiling with HAVE_STD_STRING
**
** Revision 1.26  2002/12/09 09:31:14  wilkens
** Modified/Added doc++ documentation.
**
** Revision 1.25  2002/12/06 12:49:09  joergr
** Enhanced "print()" function by re-working the implementation and replacing
** the boolean "showFullData" parameter by a more general integer flag.
** Added doc++ documentation.
** Made source code formatting more consistent with other modules/files.
**
** Revision 1.24  2002/08/27 16:55:31  meichel
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
