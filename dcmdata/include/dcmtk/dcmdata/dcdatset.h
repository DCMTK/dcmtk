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
 *  Purpose: Interface of the class DcmDataset
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:15:40 $
 *  CVS/RCS Revision: $Revision: 1.36 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
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
class DcmDataset
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
     *  0x0005, 0x0007 and 0xFFFF
     */
    virtual void removeInvalidGroups();

    /** return the transfer syntax in which this dataset was originally read.
     *  @return transfer syntax in which this dataset was originally read, EXS_Unknown if the dataset was created in memory
     */
    E_TransferSyntax getOriginalXfer() const;

    /** print all elements of the dataset to a stream
     *  @param out output stream
     *  @param flags optional flag used to customize the output (see DCMTypes::PF_xxx)
     *  @param level current level of nested items. Used for indentation.
     *  @param pixelFileName optional filename used to write the raw pixel data file
     *  @param pixelCounter optional counter used for automatic pixel data filename creation
     */
    virtual void print(STD_NAMESPACE ostream&out,
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
     *  @param oldXfer transfer syntax in which the DICOM object was read or created.
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
     *  @param oxfer output transfer syntax
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
     *  @param oxfer          The transfer syntax which shall be used.
     *  @param enctype        Encoding type for sequences; specifies how sequences
     *                        will be handled.
     *  @param wcache pointer to write cache object, may be NULL
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

    /** write object in XML format.
     *  The XML declaration (e.g. <?xml version="1.0"?>) is not written by this function.
     *  @param out output stream to which the XML document is written
     *  @param flags optional flag used to customize the output (see DCMTypes::XF_xxx)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition writeXML(STD_NAMESPACE ostream&out,
                                 const size_t flags = 0);

    /** load object from a DICOM file.
     *  This method only supports DICOM objects stored as a dataset, i.e. without meta header.
     *  Use DcmFileFormat::loadFile() to load files with meta header.
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

    /** save object to a DICOM file.
     *  This method only supports DICOM objects stored as a dataset, i.e. without meta header.
     *  Use DcmFileFormat::saveFile() to save files with meta header.
     *  @param fileName name of the file to save
     *  @param writeXfer transfer syntax used to write the data (EXS_Unknown means use current)
     *  @param encodingType flag, specifying the encoding with undefined or explicit length
     *  @param groupLength flag, specifying how to handle the group length tags
     *  @param padEncoding flag, specifying how to handle the padding tags
     *  @param padLength number of bytes used for the dataset padding (has to be an even number)
     *  @param subPadLength number of bytes used for the item padding (has to be an even number)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition saveFile(const char *fileName,
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


  private:

    /// current transfer syntax of the dataset
    E_TransferSyntax Xfer;
};


#endif // DCDATSET_H


/*
** CVS/RCS Log:
** $Log: dcdatset.h,v $
** Revision 1.36  2010-10-14 13:15:40  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.35  2010-10-08 08:37:00  joergr
** Enhanced documentation on pixel-related parameters.
**
** Revision 1.34  2010-10-05 16:20:47  joergr
** Fixed various Doxygen API documentation issues.
**
** Revision 1.33  2010-03-01 09:08:44  uli
** Removed some unnecessary include directives in the headers.
**
** Revision 1.32  2010-02-22 11:39:53  uli
** Remove some unneeded includes.
**
** Revision 1.31  2009-08-25 13:00:52  joergr
** Added new methods which remove all data elements with an invalid group number
** from the meta information header, dataset and/or fileformat.
**
** Revision 1.30  2008-07-17 11:19:48  onken
** Updated copyFrom() documentation.
**
** Revision 1.29  2008-07-17 10:30:22  onken
** Implemented copyFrom() method for complete DcmObject class hierarchy, which
** permits setting an instance's value from an existing object. Implemented
** assignment operator where necessary.
**
** Revision 1.28  2008-06-23 12:09:13  joergr
** Fixed inconsistencies in Doxygen API documentation.
**
** Revision 1.27  2007/11/29 14:30:19  meichel
** Write methods now handle large raw data elements (such as pixel data)
**   without loading everything into memory. This allows very large images to
**   be sent over a network connection, or to be copied without ever being
**   fully in memory.
**
** Revision 1.26  2006/08/15 15:49:56  meichel
** Updated all code in module dcmdata to correctly compile when
**   all standard C++ classes remain in namespace std.
**
** Revision 1.25  2005/12/08 16:28:02  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.24  2005/11/11 10:31:57  meichel
** Added explicit DcmDataset::clear() implementation.
**
** Revision 1.23  2004/07/01 12:28:25  meichel
** Introduced virtual clone method for DcmObject and derived classes.
**
** Revision 1.22  2003/03/21 13:06:46  meichel
** Minor code purifications for warnings reported by MSVC in Level 4
**
** Revision 1.21  2002/12/09 09:31:13  wilkens
** Modified/Added doc++ documentation.
**
** Revision 1.20  2002/12/06 12:49:08  joergr
** Enhanced "print()" function by re-working the implementation and replacing
** the boolean "showFullData" parameter by a more general integer flag.
** Added doc++ documentation.
** Made source code formatting more consistent with other modules/files.
**
** Revision 1.19  2002/08/27 16:55:30  meichel
** Initial release of new DICOM I/O stream classes that add support for stream
**   compression (deflated little endian explicit VR transfer syntax)
**
** Revision 1.18  2002/04/25 09:40:13  joergr
** Added support for XML output of DICOM objects.
**
** Revision 1.17  2002/04/11 12:22:52  joergr
** Added new methods for loading and saving DICOM files.
**
** Revision 1.16  2001/09/25 17:19:24  meichel
** Adapted dcmdata to class OFCondition
**
** Revision 1.15  2001/06/01 15:48:34  meichel
** Updated copyright header
**
** Revision 1.14  2000/11/07 16:56:05  meichel
** Initial release of dcmsign module for DICOM Digital Signatures
**
** Revision 1.13  2000/04/14 15:31:31  meichel
** Removed default value from output stream passed to print() method.
**   Required for use in multi-thread environments.
**
** Revision 1.12  2000/03/08 16:26:12  meichel
** Updated copyright header.
**
** Revision 1.11  2000/03/03 14:05:22  meichel
** Implemented library support for redirecting error messages into memory
**   instead of printing them to stdout/stderr for GUI applications.
**
** Revision 1.10  2000/02/10 10:50:50  joergr
** Added new feature to dcmdump (enhanced print method of dcmdata): write
** pixel data/item value fields to raw files.
**
** Revision 1.9  1999/03/31 09:24:32  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.8  1997/07/21 08:14:38  andreas
** - New environment for encapsulated pixel representations. DcmPixelData
**   can contain different representations and uses codecs to convert
**   between them. Codecs are derived from the DcmCodec class. New error
**   codes are introduced for handling of representations. New internal
**   value representation (only for ident()) for PixelData
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.7  1997/05/27 13:48:26  andreas
** - Add method canWriteXfer to class DcmObject and all derived classes.
**   This method checks whether it is possible to convert the original
**   transfer syntax to an new transfer syntax. The check is used in the
**   dcmconv utility to prohibit the change of a compressed transfer
**   syntax to a uncompressed.
**
** Revision 1.6  1997/05/16 08:23:45  andreas
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
** Revision 1.5  1996/08/05 08:45:17  andreas
** new print routine with additional parameters:
**         - print into files
**         - fix output length for elements
** corrected error in search routine with parameter ESM_fromStackTop
**
** Revision 1.4  1996/01/09 11:06:14  andreas
** New Support for Visual C++
** Correct problems with inconsistent const declarations
**
** Revision 1.3  1996/01/05 13:22:52  andreas
** - changed to support new streaming facilities
** - more cleanups
** - merged read / write methods for block and file transfer
**
*/
