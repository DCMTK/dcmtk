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
 *  Author:  Gerd Ehlers, Andreas Barth
 *
 *  Purpose: Interface of class DcmMetaInfo
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:15:41 $
 *  CVS/RCS Revision: $Revision: 1.33 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DCMETINF_H
#define DCMETINF_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dcitem.h"


/// magic string identifying DICOM files
#define DCM_Magic                       "DICM"

/// length of magic string identifying DICOM files
#define DCM_MagicLen                    4

/// length of DICOM file preamble, in bytes
#define DCM_PreambleLen                 128

/// transfer syntax used for encoding DICOM meta-headers
#define META_HEADER_DEFAULT_TRANSFERSYNTAX EXS_LittleEndianExplicit


/** a class representing the DICOM file meta information header
 */
class DcmMetaInfo
  : public DcmItem
{

  public:

    /** default constructor
     */
    DcmMetaInfo();

    /** copy constructor
     *  @param old item to be copied
     */
    DcmMetaInfo(const DcmMetaInfo &old);

    /** assignment operator.
     *  @param the metainfo to be copied
     */
    DcmMetaInfo &operator=(const DcmMetaInfo &obj);

    /** destructor
     */
    virtual ~DcmMetaInfo();

    /** clone method
     *  @return deep copy of this object
     */
    virtual DcmObject *clone() const
    {
      return new DcmMetaInfo(*this);
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

    /** get type identifier
     *  @return type identifier of this class (EVR_item)
     */
    virtual DcmEVR ident() const;

    /** remove all elements with an invalid group number, i.e. everything but 0x0002
     */
    virtual void removeInvalidGroups();

    /** return the transfer syntax in which this dataset was originally read.
     *  @return transfer syntax in which this dataset was originally read, EXS_Unknown if the dataset was created in memory
     */
    E_TransferSyntax getOriginalXfer() const;

    /** print meta information header to a stream
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

    /** initialize the transfer state of this object. This method must be called
     *  before this object is written to a stream or read (parsed) from a stream.
     */
    virtual void transferInit();

    /** finalize the transfer state of this object. This method must be called
     *  when reading/writing this object from/to a stream has been completed.
     */
    virtual void transferEnd();

    /** calculate the length of this DICOM element when encoded with the
     *  given transfer syntax and the given encoding type for sequences.
     *  For elements, the length includes the length of the tag, length field,
     *  VR field and the value itself, for items and sequences it returns
     *  the length of the complete item or sequence including delimitation tags
     *  if applicable. Never returns undefined length.
     *  @param xfer transfer syntax for length calculation
     *  @param enctype sequence encoding type for length calculation
     *  @return length of DICOM element
     */
    virtual Uint32 calcElementLength(const E_TransferSyntax xfer,
                                     const E_EncodingType enctype);

    /** read object from a stream.
     *  @param inStream DICOM input stream
     *  @param xfer transfer syntax to use when parsing
     *  @param glenc handling of group length parameters
     *  @param maxReadLength attribute values larger than this value are skipped
     *    while parsing and read later upon first access if the stream type supports
     *    this.
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition read(DcmInputStream &inStream,
                             const E_TransferSyntax xfer = EXS_Unknown,
                             const E_GrpLenEncoding glenc = EGL_noChange,
                             const Uint32 maxReadLength = DCM_MaxReadLength);

    /** write meta information header to a stream
     *  @param outStream DICOM output stream
     *  @param oxfer output transfer syntax
     *  @param enctype encoding types (undefined or explicit length)
     *  @param wcache pointer to write cache object, may be NULL
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition write(DcmOutputStream &outStream,
                              const E_TransferSyntax oxfer,
                              const E_EncodingType enctype,
                              DcmWriteCache *wcache);

    /** write object in XML format
     *  @param out output stream to which the XML document is written
     *  @param flags optional flag used to customize the output (see DCMTypes::XF_xxx)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition writeXML(STD_NAMESPACE ostream&out,
                                 const size_t flags = 0);

    /** load object from a DICOM file
     *  @param fileName name of the file to load
     *  @param readXfer transfer syntax used to read the data (auto detection if EXS_Unknown)
     *  @param groupLength flag, specifying how to handle the group length tags
     *  @param maxReadLength maximum number of bytes to be read for an element value.
     *    Element values with a larger size are not loaded until their value is retrieved
     *    (with getXXX()) or loadAllDataElements() is called.
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition loadFile(const char *fileName,
                                 const E_TransferSyntax readXfer = EXS_Unknown,
                                 const E_GrpLenEncoding groupLength = EGL_noChange,
                                 const Uint32 maxReadLength = DCM_MaxReadLength);


  private:

    /// initialize the preamble buffer with 128 zero bytes followed by "DICM"
    void setPreamble();

    /** check if the next 132 bytes in the input stream are a preamble
     *  followed by the DICOM "magic word" DICM. If so, try to determine
     *  the transfer syntax of the meta-header and return in parameter
     *  newxfer. If not, reset stream to start position.
     *  @param inStream input stream, should be at start position
     *  @param newxfer as input parameter contains the expected transfer syntax,
     *    as output parameter returns the real transfer syntax of the
     *    meta-header as determined heuristically
     *  @return true if meta-header found and read, false otherwise
     */
    OFBool checkAndReadPreamble(DcmInputStream &inStream,
                                E_TransferSyntax &newxfer);  // out

    /** peeks into the input stream and checks whether the next element
     *  is group 0002, i.e. belongs to the meta-header
     *  @return true if next element is part of meta-header, false otherwise
     */
    OFBool nextTagIsMeta(DcmInputStream &inStream);

    /** read meta-header group length element which is important because
     *  it tells us where to switch from meta-header transfer syntax to
     *  dataset transfer syntax. Insert element into dataset
     *  @param inStream input stream
     *  @param xfer transfer syntax of meta-header
     *  @param xtag attribute tag for group length
     *  @param glenc handling of group length encoding element in dataset
     *  @param headerLen output parameter; length of meta-header as encoded in group length element
     *  @param bytesRead output parameter; number of bytes read when reading group length (for counting the remaining number of meta-header bytes)
     *  @param maxReadLength max read length for elements
     *  @return EC_Normal if successful, an error code otherwise
     */
    OFCondition readGroupLength(DcmInputStream &inStream,       // inout
                                const E_TransferSyntax xfer,    // in
                                const DcmTagKey &xtag,          // in
                                const E_GrpLenEncoding glenc,   // in
                                Uint32 &headerLen,              // out
                                Uint32 &bytesRead,              // out
                                const Uint32 maxReadLength = DCM_MaxReadLength);   // in

    /// buffer for 132 byte DICOM file preamble
    char filePreamble[DCM_PreambleLen + DCM_MagicLen];

    /// true if the preamble was read from stream
    OFBool preambleUsed;

    /// transfer state of the preamble
    E_TransferState fPreambleTransferState;

    /// transfer syntax in which the meta-header was read
    E_TransferSyntax Xfer;

};

#endif // DCMETINF_H

/*
** CVS/RCS Log:
** $Log: dcmetinf.h,v $
** Revision 1.33  2010-10-14 13:15:41  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.32  2010-06-07 13:54:13  joergr
** Added new method that allows for loading the meta-header only.
**
** Revision 1.31  2010-03-01 09:08:44  uli
** Removed some unnecessary include directives in the headers.
**
** Revision 1.30  2010-02-22 11:39:54  uli
** Remove some unneeded includes.
**
** Revision 1.29  2009-08-25 13:00:52  joergr
** Added new methods which remove all data elements with an invalid group number
** from the meta information header, dataset and/or fileformat.
**
** Revision 1.28  2008-07-17 11:19:48  onken
** Updated copyFrom() documentation.
**
** Revision 1.27  2008-07-17 10:30:23  onken
** Implemented copyFrom() method for complete DcmObject class hierarchy, which
** permits setting an instance's value from an existing object. Implemented
** assignment operator where necessary.
**
** Revision 1.26  2008-06-23 12:09:13  joergr
** Fixed inconsistencies in Doxygen API documentation.
**
** Revision 1.25  2007/11/29 14:30:19  meichel
** Write methods now handle large raw data elements (such as pixel data)
**   without loading everything into memory. This allows very large images to
**   be sent over a network connection, or to be copied without ever being
**   fully in memory.
**
** Revision 1.24  2007/02/19 14:57:22  meichel
** Declaration of copy assignment operator now private, as it should be
**
** Revision 1.23  2006/08/15 15:49:56  meichel
** Updated all code in module dcmdata to correctly compile when
**   all standard C++ classes remain in namespace std.
**
** Revision 1.22  2005/12/08 16:28:21  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.21  2005/11/07 16:59:24  meichel
** Cleaned up some copy constructors in the DcmObject hierarchy.
**
** Revision 1.20  2004/07/01 12:28:25  meichel
** Introduced virtual clone method for DcmObject and derived classes.
**
** Revision 1.19  2003/03/21 13:06:46  meichel
** Minor code purifications for warnings reported by MSVC in Level 4
**
** Revision 1.18  2002/12/06 12:49:11  joergr
** Enhanced "print()" function by re-working the implementation and replacing
** the boolean "showFullData" parameter by a more general integer flag.
** Added doc++ documentation.
** Made source code formatting more consistent with other modules/files.
**
** Revision 1.17  2002/08/27 16:55:35  meichel
** Initial release of new DICOM I/O stream classes that add support for stream
**   compression (deflated little endian explicit VR transfer syntax)
**
** Revision 1.16  2002/04/25 09:40:56  joergr
** Added support for XML output of DICOM objects.
**
** Revision 1.15  2001/09/25 17:19:27  meichel
** Adapted dcmdata to class OFCondition
**
** Revision 1.14  2001/06/01 15:48:41  meichel
** Updated copyright header
**
** Revision 1.13  2000/04/14 15:31:32  meichel
** Removed default value from output stream passed to print() method.
**   Required for use in multi-thread environments.
**
** Revision 1.12  2000/03/08 16:26:16  meichel
** Updated copyright header.
**
** Revision 1.11  2000/03/03 14:05:24  meichel
** Implemented library support for redirecting error messages into memory
**   instead of printing them to stdout/stderr for GUI applications.
**
** Revision 1.10  2000/02/10 10:50:51  joergr
** Added new feature to dcmdump (enhanced print method of dcmdata): write
** pixel data/item value fields to raw files.
**
** Revision 1.9  1999/03/31 09:24:41  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.8  1997/09/22 14:56:12  hewett
** Added a method to retreive the original transfer syntax of a read
** meta-header (getOriginalXfer).  This functionality is needed by
** the DCMCHECK package.
**
** Revision 1.7  1997/07/21 08:25:09  andreas
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.6  1997/05/16 08:23:47  andreas
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
** Revision 1.5  1996/08/05 08:45:24  andreas
** new print routine with additional parameters:
**         - print into files
**         - fix output length for elements
** corrected error in search routine with parameter ESM_fromStackTop
**
** Revision 1.4  1996/01/09 11:06:16  andreas
** New Support for Visual C++
** Correct problems with inconsistent const declarations
**
** Revision 1.3  1996/01/05 13:22:57  andreas
** - changed to support new streaming facilities
** - more cleanups
** - merged read / write methods for block and file transfer
**
*/
