/*
 *
 *  Copyright (C) 1994-2010, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module:  dcmdata
 *
 *  Author:  Gerd Ehlers
 *
 *  Purpose: Interface of class DcmOtherByteOtherWord
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:15:43 $
 *  CVS/RCS Revision: $Revision: 1.35 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DCVROBOW_H
#define DCVROBOW_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dcelem.h"


/** a class representing the DICOM value representations 'Other Byte String' (OB)
 *  and 'Other Word String' (OW)
 */
class DcmOtherByteOtherWord
  : public DcmElement
{

 public:

    /** constructor.
     *  Create new element from given tag and length.
     *  @param tag DICOM tag for the new element
     *  @param len value length for the new element
     */
    DcmOtherByteOtherWord(const DcmTag &tag,
                          const Uint32 len = 0);

    /** copy constructor
     *  @param old element to be copied
     */
    DcmOtherByteOtherWord(const DcmOtherByteOtherWord &old);

    /** destructor
     */
    virtual ~DcmOtherByteOtherWord();

    /** assignment operator
     *  @param obj element to be assigned/copied
     *  @return reference to this object
     */
    DcmOtherByteOtherWord &operator=(const DcmOtherByteOtherWord &obj);

    /** clone method
     *  @return deep copy of this object
     */
    virtual DcmObject *clone() const
    {
      return new DcmOtherByteOtherWord(*this);
    }

    /** Virtual object copying. This method can be used for DcmObject
     *  and derived classes to get a deep copy of an object. Internally
     *  the assignment operator is called if the given DcmObject parameter
     *  is of the same type as "this" object instance. If not, an error
     *  is returned. This function permits copying an object by value
     *  in a virtual way which therefore is different to just calling the
     *  assignment operator of DcmElement which could result in slicing
     *  the object.
     *  @param rhs - [in] The instance to copy from. Has to be of the same
     *                class type as "this" object
     *  @return EC_Normal if copying was successful, error otherwise
     */
    virtual OFCondition copyFrom(const DcmObject& rhs);

    /** get element type identifier
     *  @return type identifier of this class
     */
    virtual DcmEVR ident() const;

    /** check whether stored value conforms to the VR and to the specified VM
     *  @param vm parameter not used for this VR
     *  @param oldFormat parameter not used for this VR (only for DA, TM, PN)
     *  @return always returns EC_Normal, i.e. currently no checks are performed
     */
    virtual OFCondition checkValue(const OFString &vm = "",
                                   const OFBool oldFormat = OFFalse);

    /** get value multiplicity
     *  @return always returns 1 (according to the DICOM standard)
     */
    virtual unsigned long getVM();

    /** set/change the current value representation
     *  @param vr new value representation to be set.  All VRs except for OW (Other
     *    Word String) are treated as 8 bit data (OB).  This is particularily useful
     *    for unknown (UN) or unsupported VRs.
     *  @return status status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setVR(DcmEVR vr);

    /** print the current value to a stream.
     *  The output format of the binary value is a backslash separated sequence of
     *  2- or 4-digit hex numbers, e.g. "00\01\dd" or "0000\7777\aaaa\ffff".
     *  @param out output stream
     *  @param flags optional flag used to customize the output (see DCMTypes::PF_xxx)
     *  @param level current level of nested items. Used for indentation.
     *  @param pixelFileName not used
     *  @param pixelCounter not used
     */
    virtual void print(STD_NAMESPACE ostream&out,
                       const size_t flags = 0,
                       const int level = 0,
                       const char *pixelFileName = NULL,
                       size_t *pixelCounter = NULL);

    /** check whether the transfer syntax can be changed as specified
     *  @param newXfer transfer syntax to be checked
     *  @param oldXfer not used
     *  @return OFTrue if transfer syntax can be changed to the new one, OFFalse otherwise
     */
    virtual OFBool canWriteXfer(const E_TransferSyntax newXfer,
                                const E_TransferSyntax oldXfer);

    /** write object to a stream
     *  @param outStream DICOM output stream
     *  @param oxfer output transfer syntax
     *  @param enctype encoding types (undefined or explicit length)
     *  @param wcache pointer to write cache object, may be NULL
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition write(
      DcmOutputStream &outStream,
      const E_TransferSyntax oxfer,
      const E_EncodingType enctype,
      DcmWriteCache *wcache);

    /** write object in XML format to a stream
     *  @param out output stream to which the XML document is written
     *  @param flags optional flag used to customize the output (see DCMTypes::XF_xxx)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition writeXML(STD_NAMESPACE ostream&out,
                                 const size_t flags = 0);

    /** special write method for creation of digital signatures
     *  @param outStream DICOM output stream
     *  @param oxfer output transfer syntax
     *  @param enctype encoding types (undefined or explicit length)
     *  @param wcache pointer to write cache object, may be NULL
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition writeSignatureFormat(
      DcmOutputStream &outStream,
      const E_TransferSyntax oxfer,
      const E_EncodingType enctype,
      DcmWriteCache *wcache);

    /** get particular 8 bit value.
     *  This method is only applicable to non-OW data, e.g. OB.
     *  @param byteVal reference to result variable (cleared in case of error)
     *  @param pos index of the value to be retrieved (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getUint8(Uint8 &byteVal,
                                 const unsigned long pos = 0);

    /** get particular 16 bit value.
     *  This method is only applicable to OW data.
     *  @param wordVal reference to result variable (cleared in case of error)
     *  @param pos index of the value to be retrieved (0..vm-1)
     *  @return status status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getUint16(Uint16 &wordVal,
                                  const unsigned long pos = 0);

    /** get reference to stored 8 bit data.
     *  This method is only applicable to non-OW data, e.g. OB.
     *  @param byteVals reference to result variable
     *  @return status status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getUint8Array(Uint8 *&byteVals);

    /** get reference to stored 16 bit data.
     *  This method is only applicable to OW data.
     *  @param wordVals reference to result variable
     *  @return status status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getUint16Array(Uint16 *&wordVals);

    /** get a particular value as a character string.
     *  The numeric value is converted to hex mode, i.e. an 8 bit value is
     *  represented by 2 characters (00..ff) and a 16 bit value by 4 characters
     *  (0000..ffff).
     *  @param stringVal variable in which the result value is stored
     *  @param pos index of the value in case of multi-valued elements (0..vm-1)
     *  @param normalize not used
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getOFString(OFString &stringVal,
                                    const unsigned long pos,
                                    OFBool normalize = OFTrue);

    /** get element value as a character string.
     *  The numeric values are converted to hex mode, i.e. an 8 bit value is
     *  represented by 2 characters (00..ff) and a 16 bit value by 4 characters
     *  (0000..ffff).
     *  In case of VM > 1 the single values are separated by a backslash ('\').
     *  @param stringVal variable in which the result value is stored
     *  @param normalize not used
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getOFStringArray(OFString &stringVal,
                                         OFBool normalize = OFTrue);

    /** set element value to given 8 bit data.
     *  This method is only applicable to non-OW data, e.g. OB.
     *  @param byteValue 8 bit data to be set (copied)
     *  @param numBytes number of bytes (8 bit) to be set
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition putUint8Array(const Uint8 *byteValue,
                                      const unsigned long numBytes);

    /** set element value to given 16 bit data.
     *  This method is only applicable to OW data.
     *  @param wordValue 16 bit data to be set (copied)
     *  @param numWords number of words (16 bit) to be set. Local byte-ordering
     *    expected.
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition putUint16Array(const Uint16 *wordValue,
                                       const unsigned long numWords);

    /** create an empty Uint8 array of given number of bytes and set it.
     *  All array elements are initialized with a value of 0 (using 'memzero').
     *  This method is only applicable to non-OW data, e.g. OB.
     *  @param numBytes number of bytes (8 bit) to be created
     *  @param bytes stores the pointer to the resulting buffer
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition createUint8Array(const Uint32 numBytes,
                                         Uint8 *&bytes);

    /** create an empty Uint16 array of given number of words and set it.
     *  All array elements are initialized with a value of 0 (using 'memzero').
     *  This method is only applicable to OW data.
     *  @param numWords number of words (16 bit) to be created
     *  @param words stores the pointer to the resulting buffer
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition createUint16Array(const Uint32 numWords,
                                          Uint16 *&words);

    /** set element value from the given character string.
     *  The input string is expected to have the same format as described for
     *  'getOFStringArray()' above, i.e. a backslash separated sequence of
     *  hexa-decimal numbers.
     *  @param stringVal input character string
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition putString(const char *stringVal);

    /** check the currently stored element value
     *  @param autocorrect correct value padding (even length) if OFTrue
     *  @return status, EC_Normal if value length is correct, an error code otherwise
     */
    virtual OFCondition verify(const OFBool autocorrect = OFFalse);


 protected:

    /** method is called after the element value has been loaded.
     *  Can be used to correct the value before it is used for the first time.
     */
    virtual void postLoadValue();

    /** align the element value to an even length (padding)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition alignValue();

    /** print pixel data and optionally write it to a binary file.
     *  Optional pixel data file is always written in little endian byte-ordering.
     *  @param out output stream
     *  @param flags optional flag used to customize the output (see DCMTypes::PF_xxx)
     *  @param level current level of nested items. Used for indentation.
     *  @param pixelFileName optional filename used to write the raw pixel data file
     *  @param pixelCounter optional counter used for automatic pixel data filename creation
     */
    void printPixel(STD_NAMESPACE ostream&out,
                    const size_t flags,
                    const int level,
                    const char *pixelFileName,
                    size_t *pixelCounter);

private:

    /** this flag is used during write operations and indicates that compact() should be
     *  called once the write operation finishes.
     */
    OFBool compactAfterTransfer;

};


#endif // DCVROBOW_H


/*
** CVS/RCS Log:
** $Log: dcvrobow.h,v $
** Revision 1.35  2010-10-14 13:15:43  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.34  2010-04-23 15:26:13  joergr
** Specify an appropriate default value for the "vm" parameter of checkValue().
**
** Revision 1.33  2010-04-23 14:25:27  joergr
** Added new method to all VR classes which checks whether the stored value
** conforms to the VR definition and to the specified VM.
**
** Revision 1.32  2008-08-15 09:26:31  meichel
** Under certain conditions (odd length compressed pixel data fragments)
**   class DcmOtherByteOtherWord needs to load the attribute value into main
**   memory during a write() operation, in order to add a pad byte. A new flag
**   compactAfterTransfer now makes sure that the memory is released once the
**   write operation has finished, so that only a single fragment at a time
**   needs to fully reside in memory.
**
** Revision 1.31  2008-07-17 11:19:49  onken
** Updated copyFrom() documentation.
**
** Revision 1.30  2008-07-17 10:30:23  onken
** Implemented copyFrom() method for complete DcmObject class hierarchy, which
** permits setting an instance's value from an existing object. Implemented
** assignment operator where necessary.
**
** Revision 1.29  2007-11-29 14:30:19  meichel
** Write methods now handle large raw data elements (such as pixel data)
**   without loading everything into memory. This allows very large images to
**   be sent over a network connection, or to be copied without ever being
**   fully in memory.
**
** Revision 1.28  2007/06/07 09:01:15  joergr
** Added createUint8Array() and createUint16Array() methods.
**
** Revision 1.27  2006/08/15 15:49:56  meichel
** Updated all code in module dcmdata to correctly compile when
**   all standard C++ classes remain in namespace std.
**
** Revision 1.26  2005/12/08 16:29:03  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.25  2004/07/01 12:28:25  meichel
** Introduced virtual clone method for DcmObject and derived classes.
**
** Revision 1.24  2003/07/09 12:13:13  meichel
** Included dcmodify in MSVC build system, updated headers
**
** Revision 1.23  2003/06/12 13:29:28  joergr
** Fixed inconsistent API documentation reported by Doxygen.
**
** Revision 1.22  2002/12/06 12:49:17  joergr
** Enhanced "print()" function by re-working the implementation and replacing
** the boolean "showFullData" parameter by a more general integer flag.
** Added doc++ documentation.
** Made source code formatting more consistent with other modules/files.
**
** Revision 1.21  2002/08/27 16:55:40  meichel
** Initial release of new DICOM I/O stream classes that add support for stream
**   compression (deflated little endian explicit VR transfer syntax)
**
** Revision 1.20  2002/04/25 10:03:45  joergr
** Added getOFString() implementation.
** Added/modified getOFStringArray() implementation.
** Added support for XML output of DICOM objects.
**
** Revision 1.19  2001/10/02 11:47:34  joergr
** Added getUint8/16 routines to class DcmOtherByteOtherWord.
**
** Revision 1.18  2001/09/25 17:19:32  meichel
** Adapted dcmdata to class OFCondition
**
** Revision 1.17  2001/06/01 15:48:51  meichel
** Updated copyright header
**
** Revision 1.16  2000/11/07 16:56:10  meichel
** Initial release of dcmsign module for DICOM Digital Signatures
**
** Revision 1.15  2000/04/14 15:31:34  meichel
** Removed default value from output stream passed to print() method.
**   Required for use in multi-thread environments.
**
** Revision 1.14  2000/03/08 16:26:24  meichel
** Updated copyright header.
**
** Revision 1.13  2000/03/03 14:05:27  meichel
** Implemented library support for redirecting error messages into memory
**   instead of printing them to stdout/stderr for GUI applications.
**
** Revision 1.12  2000/02/10 10:50:55  joergr
** Added new feature to dcmdump (enhanced print method of dcmdata): write
** pixel data/item value fields to raw files.
**
** Revision 1.11  1999/03/31 09:25:03  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.10  1998/11/12 16:47:51  meichel
** Implemented operator= for all classes derived from DcmObject.
**
** Revision 1.9  1997/07/21 08:25:15  andreas
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.8  1997/05/27 13:48:30  andreas
** - Add method canWriteXfer to class DcmObject and all derived classes.
**   This method checks whether it is possible to convert the original
**   transfer syntax to an new transfer syntax. The check is used in the
**   dcmconv utility to prohibit the change of a compressed transfer
**   syntax to a uncompressed.
**
** Revision 1.7  1997/05/16 08:31:20  andreas
** - Revised handling of GroupLength elements and support of
**   DataSetTrailingPadding elements. The enumeratio E_GrpLenEncoding
**   got additional enumeration values (for a description see dctypes.h).
**   addGroupLength and removeGroupLength methods are replaced by
**   computeGroupLengthAndPadding. To support Padding, the parameters of
**   element and sequence write functions changed.
**
** Revision 1.6  1997/04/18 08:13:31  andreas
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
** Revision 1.5  1996/08/05 08:45:33  andreas
** new print routine with additional parameters:
**         - print into files
**         - fix output length for elements
** corrected error in search routine with parameter ESM_fromStackTop
**
** Revision 1.4  1996/01/29 13:38:17  andreas
** - new put method for every VR to put value as a string
** - better and unique print methods
**
** Revision 1.3  1996/01/05 13:23:07  andreas
** - changed to support new streaming facilities
** - more cleanups
** - merged read / write methods for block and file transfer
**
*/
