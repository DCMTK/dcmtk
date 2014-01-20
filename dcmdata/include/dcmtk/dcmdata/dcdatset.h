/*
 *
 *  Copyright (C) 1994-2014, OFFIS e.V.
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
 *  Purpose: Interface of the class DcmDataset
 *
 */


#ifndef DCDATSET_H
#define DCDATSET_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dcitem.h"


// forward declarations
class DcmInputStream;
class DcmOutputStream;
class DcmRepresentationParameter;


/** a class handling the DICOM dataset format (files without meta header)
 */
class DCMTK_DCMDATA_EXPORT DcmDataset
  : public DcmItem
{

  public:

    /** default constructor
     */
    DcmDataset();

    /** copy constructor
     *  @param old dataset to be copied
     */
    DcmDataset(const DcmDataset &old);

    /** destructor
     */
    virtual ~DcmDataset();

    /** assignment operator
     *  @param obj the dataset to be copied
     */
    DcmDataset& operator=(const DcmDataset& obj);

    /** clone method
     *  @return deep copy of this object
     */
    virtual DcmObject *clone() const
    {
      return new DcmDataset(*this);
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
     *  @return type identifier of this class (EVR_dataset)
     */
    virtual DcmEVR ident() const;

    /** clear (remove) attribute value
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition clear();

    /** remove all elements with an invalid group number, i.e. 0x0000 to 0x0003,
     *  0x0005, 0x0007 and 0xFFFF in case of a data set.  For sequence items, also
     *  group 0x0006 is disallowed.  For command sets, only group 0x0000 is allowed,
     *  i.e. the elements from all other groups are removed.
     *  @param cmdSet specifies whether this object represents a command or data set
     */
    virtual void removeInvalidGroups(const OFBool cmdSet = OFFalse);

    /** return the transfer syntax in which this dataset was originally read or created.
     *  See updateOriginalXfer() on how to update this value when created in memory.
     *  @return transfer syntax in which this dataset was originally read. Might be
     *    EXS_Unknown if the dataset was created in memory.
     */
    E_TransferSyntax getOriginalXfer() const;

    /** return the current transfer syntax, i.e.\ the one that was last used with
     *  chooseRepresentation() in order to select a specific representation or with write()
     *  or writeSignatureFormat() in order to create a byte stream according to the DICOM
     *  encoding rules.  The default value is the transfer syntax in which this dataset was
     *  originally read (see getOriginalXfer()) or, if this dataset was created from memory,
     *  the explicit VR with local endianness.
     *  Please note that the current transfer syntax might also change after calling
     *  updateOriginalXfer().
     *  @return transfer syntax in which this dataset is currently stored (see above)
     */
    E_TransferSyntax getCurrentXfer() const;

    /** update the original transfer syntax, e.g.\ in case the dataset was created in memory
     *  and pixel data was added with a particular representation. Icon images and other
     *  nested pixel data elements are not checked. If previously unknown, the original
     *  transfer syntax is set to the default EXS_LittleEndianExplicit. Please note that the
     *  current transfer syntax might also be updated if its value was not in sync with the
     *  internal representation of the pixel data.
     */
    virtual void updateOriginalXfer();

    /** print all elements of the dataset to a stream
     *  @param out output stream
     *  @param flags optional flag used to customize the output (see DCMTypes::PF_xxx)
     *  @param level current level of nested items. Used for indentation.
     *  @param pixelFileName optional filename used to write the raw pixel data file
     *  @param pixelCounter optional counter used for automatic pixel data filename creation
     */
    virtual void print(STD_NAMESPACE ostream &out,
                       const size_t flags = 0,
                       const int level = 0,
                       const char *pixelFileName = NULL,
                       size_t *pixelCounter = NULL);

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
    Uint32 calcElementLength(const E_TransferSyntax xfer,
                             const E_EncodingType enctype);

    /** check if this DICOM object can be encoded in the given transfer syntax.
     *  @param newXfer transfer syntax in which the DICOM object is to be encoded
     *  @param oldXfer transfer syntax in which the DICOM object was read or created
     *    (optional). If EXS_Unknown, the return value of getOriginalXfer() is used.
     *  @return true if object can be encoded in desired transfer syntax, false otherwise.
     */
    virtual OFBool canWriteXfer(const E_TransferSyntax newXfer,
                                const E_TransferSyntax oldXfer = EXS_Unknown);

    /** This function reads the information of all attributes which
     *  are captured in the input stream and captures this information
     *  in this->elementList. Each attribute is represented as an
     *  element in this list. Having read all information for this
     *  particular data set or command, this function will also take
     *  care of group length (according to what is specified in glenc)
     *  and padding elements (don't change anything).
     *  @param inStream      The stream which contains the information.
     *  @param xfer          The transfer syntax which was used to encode
     *                       the information in inStream.
     *  @param glenc         Encoding type for group length; specifies what
     *                       will be done with group length tags.
     *  @param maxReadLength Maximum read length for reading an attribute value.
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition read(DcmInputStream &inStream,
                             const E_TransferSyntax xfer = EXS_Unknown,
                             const E_GrpLenEncoding glenc = EGL_noChange,
                             const Uint32 maxReadLength = DCM_MaxReadLength);

    /** write dataset to a stream
     *  @param outStream DICOM output stream
     *  @param oxfer output transfer syntax (EXS_Unknown means use original)
     *  @param enctype encoding types (undefined or explicit length)
     *  @param wcache pointer to write cache object, may be NULL
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition write(DcmOutputStream &outStream,
                              const E_TransferSyntax oxfer,
                              const E_EncodingType enctype,
                              DcmWriteCache *wcache);

    /** This function writes data values which are contained in this
     *  DcmDataset object to the stream which is passed as first argument.
     *  With regard to the writing of information, the other parameters
     *  which are passed are accounted for. The function will return
     *  EC_Normal, if the information from all elements of this data
     *  set has been written to the buffer, it will return EC_StreamNotifyClient,
     *  if there is no more space in the buffer and _not_ all elements
     *  have been written to it, and it will return some other (error)
     *  value if there was an error.
     *  @param outStream      The stream that the information will be written to.
     *  @param oxfer          The transfer syntax which shall be used (EXS_Unknown
     *                        means use original).
     *  @param enctype        Encoding type for sequences; specifies how sequences
     *                        will be handled.
     *  @param wcache         pointer to write cache object, may be NULL
     *  @param glenc          Encoding type for group length; specifies what will
     *                        be done with group length tags.
     *  @param padenc         Encoding type for padding. Specifies what will be done
     *                        with padding tags.
     *  @param padlen         The length up to which the dataset shall be padded, if
     *                        padding is desired.
     *  @param subPadlen      For sequences (ie sub elements), the length up to which
     *                        item shall be padded, if padding is desired.
     *  @param instanceLength Number of extra bytes added to the item/dataset length
     *                        used when computing the padding; this parameter is for
     *                        instance used to pass the length of the file meta header
     *                        from the DcmFileFormat to the DcmDataset object.
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition write(DcmOutputStream &outStream,
                              const E_TransferSyntax oxfer,
                              const E_EncodingType enctype,
                              DcmWriteCache *wcache,
                              const E_GrpLenEncoding glenc,
                              const E_PaddingEncoding padenc = EPD_noChange,
                              const Uint32 padlen = 0,
                              const Uint32 subPadlen = 0,
                              Uint32 instanceLength = 0);

    /** special write method for creation of digital signatures
     *  @param outStream DICOM output stream
     *  @param oxfer output transfer syntax (EXS_Unknown means use original)
     *  @param enctype encoding types (undefined or explicit length)
     *  @param wcache pointer to write cache object, may be NULL
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition writeSignatureFormat(DcmOutputStream &outStream,
                                             const E_TransferSyntax oxfer,
                                             const E_EncodingType enctype,
                                             DcmWriteCache *wcache);

    /** write object in XML format.
     *  The XML declaration (e.g. <?xml version="1.0"?>) is not written by this function.
     *  @param out output stream to which the XML document is written
     *  @param flags optional flag used to customize the output (see DCMTypes::XF_xxx)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition writeXML(STD_NAMESPACE ostream &out,
                                 const size_t flags = 0);

    /** load object from a DICOM file.
     *  This method only supports DICOM objects stored as a dataset, i.e. without meta header.
     *  Use DcmFileFormat::loadFile() to load files with meta header.
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

    /** save object to a DICOM file.
     *  This method only supports DICOM objects stored as a dataset, i.e. without meta header.
     *  Use DcmFileFormat::saveFile() to save files with meta header.
     *  @param fileName name of the file to save (may contain wide chars if support enabled).
     *    Since there are various constructors for the OFFilename class, a "char *", "OFString"
     *    or "wchar_t *" can also be passed directly to this parameter.
     *  @param writeXfer transfer syntax used to write the data (EXS_Unknown means use original)
     *  @param encodingType flag, specifying the encoding with undefined or explicit length
     *  @param groupLength flag, specifying how to handle the group length tags
     *  @param padEncoding flag, specifying how to handle the padding tags
     *  @param padLength number of bytes used for the dataset padding (has to be an even number)
     *  @param subPadLength number of bytes used for the item padding (has to be an even number)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition saveFile(const OFFilename &fileName,
                                 const E_TransferSyntax writeXfer = EXS_Unknown,
                                 const E_EncodingType encodingType = EET_UndefinedLength,
                                 const E_GrpLenEncoding groupLength = EGL_recalcGL,
                                 const E_PaddingEncoding padEncoding = EPD_noChange,
                                 const Uint32 padLength = 0,
                                 const Uint32 subPadLength = 0);

    // methods for different pixel representations

    /** select a specific representation (compressed or uncompressed) of the dataset
     *  and create the representation if needed. This may cause compression or decompression
     *  to be applied to the pixel data in the dataset.
     *  @param repType desired transfer syntax
     *  @param repParam desired representation parameter (e.g. quality factor for lossy compression)
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition chooseRepresentation(const E_TransferSyntax repType,
                                     const DcmRepresentationParameter *repParam);

    /** check if all PixelData elements in this dataset have a representation conforming
     *  to the given transfer syntax and representation parameters (see dcpixel.h for
     *  definition of "conforming").
     *  @param repType desired transfer syntax
     *  @param repParam desired representation parameter (e.g. quality factor for lossy compression)
     *  @return true if all pixel elements have the desired representation, false otherwise
     */
    OFBool hasRepresentation(const E_TransferSyntax repType,
                             const DcmRepresentationParameter *repParam);

    /** removes all but the original representation in all pixel data elements
     */
    void removeAllButOriginalRepresentations();

    /** removes all but the current representation and sets the original
     *  representation to current
     */
    void removeAllButCurrentRepresentations();

    /** mode specifying whether the SpecificCharacterSet (0008,0005) element should be
     *  checked by convertCharacterSet() or not, i.e.\ whether this element might be
     *  present on this dataset-level.
     *  @return always returns OFTrue, i.e.\ SpecificCharacterSet should be checked
     */
    virtual OFBool checkForSpecificCharacterSet() const { return OFTrue; }

  private:

    /// original transfer syntax of the dataset
    E_TransferSyntax OriginalXfer;
    /// current transfer syntax of the dataset
    E_TransferSyntax CurrentXfer;
};


#endif // DCDATSET_H
