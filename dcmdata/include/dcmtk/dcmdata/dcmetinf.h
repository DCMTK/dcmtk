/*
 *
 *  Copyright (C) 1994-2016, OFFIS e.V.
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
 */


#ifndef DCMETINF_H
#define DCMETINF_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dcitem.h"


/// magic string identifying DICOM files
#define DCM_Magic "DICM"

/// length of magic string identifying DICOM files
#define DCM_MagicLen 4

/// length of DICOM file preamble, in bytes
#define DCM_PreambleLen 128

/// transfer syntax used for encoding DICOM meta-headers
#define META_HEADER_DEFAULT_TRANSFERSYNTAX EXS_LittleEndianExplicit


/** a class representing the DICOM file meta information header
 */
class DCMTK_DCMDATA_EXPORT DcmMetaInfo
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
     *  @param obj the metainfo to be copied
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
     *  @param xfer transfer syntax to use when parsing. EXS_Unknown will
     *              lead to reading the preamble (if any) and parsing
     *              metaheader using detected transfer syntax, usually
     *              Explicit VR Little Endian.
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
    virtual OFCondition writeXML(STD_NAMESPACE ostream &out,
                                 const size_t flags = 0);

    /** load object from a DICOM file.  If the file preamble is missing, an error is returned.
     *  @param fileName name of the file to load (may contain wide chars if support enabled).
     *    Since there are various constructors for the OFFilename class, a "char *", "OFString"
     *    or "wchar_t *" can also be passed directly to this parameter.
     *  @param readXfer transfer syntax used to read the data (auto detection if EXS_Unknown)
     *  @param groupLength flag, specifying how to handle the group length tags
     *  @param maxReadLength maximum number of bytes to be read for an element value.
     *    Element values with a larger size are not loaded until their value is retrieved
     *    (with getXXX()) or loadAllDataIntoMemory() is called.
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition loadFile(const OFFilename &fileName,
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
