/*
 *
 *  Copyright (C) 2015-2022, Open Connections GmbH
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation are maintained by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module:  dcmseg
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Class representing a Segmentation object
 *
 */

#ifndef SEGDOC_H
#define SEGDOC_H

#include "dcmtk/config/osconfig.h" // include OS configuration first

#include "dcmtk/dcmdata/dcvrui.h"
#include "dcmtk/dcmfg/concatenationcreator.h" // for writing concatenations
#include "dcmtk/dcmfg/concatenationloader.h"  // for loading concatenations
#include "dcmtk/dcmfg/fgfracon.h"             // for frame content functional group macro
#include "dcmtk/dcmfg/fginterface.h"          // for multi-frame functional group interface
#include "dcmtk/dcmiod/iodimage.h"            // common image IOD attribute access
#include "dcmtk/dcmiod/iodmacro.h"
#include "dcmtk/dcmiod/modenhequipment.h" // for enhanced general equipment module
#include "dcmtk/dcmiod/modimagepixel.h"
#include "dcmtk/dcmiod/modmultiframedimension.h" // for multi-frame dimension module
#include "dcmtk/dcmiod/modmultiframefg.h"        // for multi-frame functional group module
#include "dcmtk/dcmiod/modsegmentationseries.h"  // for segmentation series module
#include "dcmtk/dcmseg/segdef.h"
#include "dcmtk/dcmseg/segtypes.h" // for segmentation data types
#include "dcmtk/ofstd/ofvector.h"  // for OFVector

// Forward declarations
class DcmSegment;
class FGSegmentation;
class FGDerivationImage;

/** Class representing an object of the "Segmentation SOP Class".
 */
class DCMTK_DCMSEG_EXPORT DcmSegmentation : public DcmIODImage<IODImagePixelModule<Uint8> >
{

public:
    // -------------------- destruction -------------------------------

    /** Destructor, frees memory
     */
    virtual ~DcmSegmentation();

    // -------------------- loading and saving ---------------------

    /** Static method to load a Segmentation object from a file.
     *  The memory of the resulting Segmentation object has to be freed by the
     *  caller.
     *  @param  filename The file to read from
     *  @param  segmentation  The resulting segmentation object. NULL if dataset
     *          could not be read successfully.
     *  @return EC_Normal if reading was successful, error otherwise
     */
    static OFCondition loadFile(const OFString& filename, DcmSegmentation*& segmentation);

    /** Static method to load a Segmentation object from a dataset object.
     *  The memory of the resulting Segmentation object has to be freed by the
     *  caller.
     *  @param  dataset The dataset to read from
     *  @param  segmentation  The resulting segmentation object. NULL if dataset
     *          could not be read successfully.
     *  @return EC_Normal if reading was successful, error otherwise
     */
    static OFCondition loadDataset(DcmDataset& dataset, DcmSegmentation*& segmentation);

    /** Static method to load a concatenation of a DICOM Segmentation instance
     *  into a DcmSegmentation object.
     *  @param  cl The ConcatenationLoader instance to be used. Must be configured
     *          so that load() can be called.
     *  @param  concatenationUID The Concatenation UID identifying the Concatenation
     *          within the Concatenations know to ConcatenationLoader cl.
     *  @param  segmentation The resulting Segmentation object, if loading was successful
     *  @return EC_Normal if loading was successful, error otherwise
     */
    static OFCondition
    loadConcatenation(ConcatenationLoader& cl, const OFString& concatenationUID, DcmSegmentation*& segmentation);

    /** Save current object to given filename
     *  @param  filename The file to write to
     *  @param  writeXfer The transfer syntax to be used
     *  @return EC_Normal if writing was successful, error otherwise.
     */
    OFCondition saveFile(const OFString& filename, const E_TransferSyntax writeXfer = EXS_LittleEndianExplicit);

    /** Write current object to given item
     *  @param  dataset The item to write to
     *  @return EC_Normal if writing was successful, error otherwise.
     */
    OFCondition writeDataset(DcmItem& dataset);

    /** Write current object to a Concatenation.
     *  @param  cc The Concatenation Creator object to be used. Must be configured
     *          so that writeNextInstance() is ready to be called.
     *  @return EC_Normal if writing was successful, error otherwise.
     */
    OFCondition writeConcatenation(ConcatenationCreator& cc);

    /** If enabled, functional group structure is checked before actual writing
     *  is performed in the write() method. Checking might be time consuming
     *  on functional groups with many frames, though disabling might result in
     *  invalid functional group structures. Disabling should only be done if the
     *  user knows that the functional groups are valid, wants to to adapt the
     *  functional groups manually after calling write() or knows what he's doing
     *  otherwise.<br>
     *  Per default, checking is enabled.
     *  @param  doCheck If OFTrue, checking will be performed. If OFFalse,
     *          no checks are performed.
     */
    virtual void setCheckFGOnWrite(const OFBool doCheck);

    /** Returns whether functional group structure is checked before actual
     *  writing is performed in the write() method.
     *  @return OFTrue if checking is performed, OFFalse otherwise
     */
    virtual OFBool getCheckFGOnWrite();

    /** If enabled, dimensions are checked before actual writing.
     *  This can be very time-consuming if many frames are present.
     *  Disabling should only be done if the user knows that the functional groups
     *  are valid, wants to to adapt the functional groups manually after calling
     *   write() or knows what he's doing otherwise.<br>
     *  Per default, checking is enabled.
     *  @param  doCheck If OFTrue, checking will be performed. If OFFalse,
     *          no checks are performed.
     */
    virtual void setCheckDimensionsOnWrite(const OFBool doCheck);

    /** Returns whether dimensions are checked before actual
     *  writing is performed in the write() method.
     *  @return OFTrue if checking is performed, OFFalse otherwise
     */
    virtual OFBool getCheckDimensionsOnWrite();

    // -------------------- creation ---------------------

    /** Factory method to create a binary segmentation object from the minimal
     *  set of information required. The actual segments and the frame data is
     *  added separately.
     *  The memory of the resulting Segmentation object has to be freed by the
     *  caller.
     *  @param  segmentation The resulting segmentation object if provided data is
     *          valid. Otherwise NULL is returned.
     *  @param  rows Number of rows of segmentation frame data
     *  @param  columns Number of rows of segmentation frame data
     *  @param  equipmentInfo Equipment that is responsible for creating the
     *          segmentation. All attributes in equipmentInfo must have
     *          non-empty values.
     *  @param  contentIdentification Basic content identification information
     *  @return EC_Normal if creation was successful, error otherwise
     */
    static OFCondition createBinarySegmentation(DcmSegmentation*& segmentation,
                                                const Uint16 rows,
                                                const Uint16 columns,
                                                const IODGeneralEquipmentModule::EquipmentInfo& equipmentInfo,
                                                const ContentIdentificationMacro& contentIdentification);

    /** Factory method to create a fractional segmentation object from the minimal
     *  set of information required. The actual segments and the frame data is
     *  added separately.
     *  The memory of the resulting Segmentation object has to be freed by the
     *  caller.
     *  @param  segmentation The resulting segmentation object if provided data is
     *          valid. Otherwise NULL is returned.
     *  @param  rows Number of rows of segmentation frame data
     *  @param  columns Number of rows of segmentation frame data
     *  @param  fractType Either probability (SFT_PROBABILITY) or
     *          occupancy (SFT_OCCUPANCY)
     *  @param  maxFractionalValue The value inside the frame data of this
     *          segmentation that represents 100% occupancy/probability
     *  @param  equipmentInfo Equipment that is responsible for creating the
     *          segmentation. All attributes in equipmentInfo must have
     *          non-empty values.
     *  @param  contentIdentification Basic content identification information
     *  @return EC_Normal if creation was successful, error otherwise
     */
    static OFCondition createFractionalSegmentation(DcmSegmentation*& segmentation,
                                                    const Uint16 rows,
                                                    const Uint16 columns,
                                                    const DcmSegTypes::E_SegmentationFractionalType fractType,
                                                    const Uint16& maxFractionalValue,
                                                    const IODGeneralEquipmentModule::EquipmentInfo& equipmentInfo,
                                                    const ContentIdentificationMacro& contentIdentification);

    /** Helps to create a valid Derivation Image Functional Group Macro
     *  The memory of the resulting functional group object has to be freed by the
     *  caller.
     *  @param derivationImages to image SOP instances
     *  @param derivationDescription Free text describing how the derivation was
     *         achieved.
     *  @return The created functional group, or NULL if not successful
     */
    static FGDerivationImage* createDerivationImageFG(const OFVector<ImageSOPInstanceReferenceMacro>& derivationImages,
                                                      const OFString& derivationDescription);

    // -------------------- access ---------------------

    /** Get number of frames, based on the number of items in the shared
     *  functional functional groups sequence (i.e.\ the attribute Number of
     *  Frames) is not trusted). Note that this returns the numbers of frames
     *  present in memory. In practice (i.e. for writing later on), the number
     *  of frames must not exceed 2^31-1 (maximum value of Number of Frames
     *  attribute, enforced by writing routines).
     *  @return The number of frames handled by this object
     */
    size_t getNumberOfFrames();

    /** Get Segmentation Type
     *  @return Type of the segmentation, as defined by DcmSegTypes::E_SegmentationType
     */
    DcmSegTypes::E_SegmentationType getSegmentationType()
    {
        return this->m_SegmentationType;
    }

    /** Get Fractional Segmentation Type
     *  @return Type of the segmentation, as defined by
     *  DcmSegTypes::E_SegmentationFractionalTypes (returns SFT_UNKNOWN if not
     *  fractional at all)
     */
    DcmSegTypes::E_SegmentationFractionalType getSegmentationFractionalType()
    {
        return this->m_SegmentationFractionalType;
    }

    /** Get the Number of Segments. Note that this returns the numbers of Segments
     *  present in memory. In practice, the number of segments must not exceed
     *  2^16-1 (enforced by writing routines).
     *  @return The number of segments handled by this object
     */
    size_t getNumberOfSegments();

    /** Perform some basic checking. This method is also invoked when
     *  writing the object to a DICOM dataset or file.
     *  @param  checkFGStructure If OFTrue (default), structure of functional
     *          groups is checked, too.
     *  @return OFTrue, if no errors were found, OFFalse otherwise.
     */
    virtual OFBool check(const OFBool checkFGStructure = OFTrue);

    /** Get access to functional groups. This is meant for reading data from
     *  functional groups that are not actively managed, i.e.\ made accessible by
     *  DcmSegmentation. In rare cases, however, it makes sense to access it
     *  for writing too, e.g.\ in order to add Stacks; use with care!
     *  @return Reference to the functional groups
     */
    virtual FGInterface& getFunctionalGroups();

    /** Get reference t Concatenation information
     *  @return Reference to ConcatenationInfo object
     */
    virtual IODMultiFrameFGModule::ConcatenationInfo& getConcatenationInfo();

    /** Get General Equipment Module
     *  @return General Equipment Module
     */
    virtual IODGeneralEquipmentModule& getEquipment();

    /** Get Segmentation Series Module
     *  @return Segmentation Series Module
     */
    virtual IODSegmentationSeriesModule& getSegmentationSeriesModule();

    /** Get modality (overwrite from DcmIODCommon. We always return "SEG" here)
     *  @param  value  Reference to variable in which the value should be stored
     *  @param  pos    Index of the value to get. Not evaluated (here for
     *          consistency with other setter functions).
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getModality(OFString& value, const signed long pos = 0) const;

    /** Get segment by providing the logical segment number
     *  @param  segmentNumber The logical segment number
     *  @return The segment if segment number is valid, NULL otherwise
     */
    virtual DcmSegment* getSegment(const size_t segmentNumber);

    /** Get logical segment number by providing a pointer to a given segment
     *  @param  segment The segment to find the logical segment number for
     *  @param  segmentNumber The segment number. 0 if segment could not be found.
     *  @return OFTrue if segment could be found, OFFalse otherwise.
     */
    virtual OFBool getSegmentNumber(const DcmSegment* segment, size_t& segmentNumber);

    /** Reference to the Performed Procedure Step that led to the creation of this
     *  segmentation object. This is required if this object is created in an MPPS
     *  or GPPS workflow.
     *  @return Reference to the referenced PPS object
     */
    virtual SOPInstanceReferenceMacro& getReferencedPPS();

    /** Get (const) frame data of a specific frame
     *  @param  frameNo The number of the frame to get (starting with 0)
     *  @return The frame requested or NULL if not existing
     */
    virtual const DcmIODTypes::Frame* getFrame(const size_t& frameNo);

    /** Get the frame numbers that belong to a specific segment number
     *  @param  segmentNumber The segment to search frames for
     *  @param  frameNumbers  The frame numbers belonging to that segment
     */
    virtual void getFramesForSegment(const size_t& segmentNumber, OFVector<size_t>& frameNumbers);

    // -------------------- modification ---------------------

    /** Add segment to segmentation object
     *  @param  seg The segment that should be added
     *  @param  segmentNumber The logical segment number that was assigned for
     *          this segment. Contains 0 if adding failed.
     *  @return EC_Normal if adding was successful, error otherwise
     */
    virtual OFCondition addSegment(DcmSegment* seg, Uint16& segmentNumber);

    /** Add a functional group for all frames
     *  @param  group The group to be added as shared functional group. The
     *  @return EC_Normal if adding was successful, error otherwise
     */
    virtual OFCondition addForAllFrames(const FGBase& group);

    /** Add frame to segmentation object
     *  @param  pixData Pixel data to be added. Length must be rows*columns bytes.
     *          For binary segmentations (bit depth i.e.\ Bits
     *          Allocated/Stored=1), each byte equal to 0 will be interpreted as
     *          "not set", while every other value is interpreted as "set". For
     *          fractional segmentations the full byte is copied as is.
     *  @param  segmentNumber The logical segment number (>=1) this frame refers to.
     *          The segment identified by the segmentNumber must already exist.
     *  @param  perFrameInformation The functional groups that identify this frame (i.e.
     *          which are planned to be not common for all other frames). The
     *          functional groups are copied, so ownership of each group stays
     *          with the caller no matter what the method returns.
     *  @return EC_Normal if adding was successful, error otherwise
     */
    virtual OFCondition
    addFrame(Uint8* pixData, const Uint16 segmentNumber, const OFVector<FGBase*>& perFrameInformation);

    /** Return reference to content content identification of this segmentation object
     *  @return Reference to content identification data
     */
    virtual ContentIdentificationMacro& getContentIdentification();

    /** Return reference to multi-fame dimension module
     *  @return Reference to multi-frame dimension module
     */
    virtual IODMultiframeDimensionModule& getDimensions();

    /** Set lossy compression flag of the segmentation object to "01". If one of the
     *  source images of this segmentation has undergone lossy compression then
     *  this function should be called.
     *  @param  ratios Compression ratios (separated by backslash) of the applied
     *          lossy compression steps. Only one value (and no backslash) if only
     *          one step was performed.
     *  @param  methods Methods (separated by backslash) of the applied
     *          lossy compression steps. Only one value (and no backslash) if only
     *          one step was performed.
     *  @param  checkValues If OFTrue, the data provided is checked for validity
     *  @return EC_Normal if lossy compression info could be set, error code otherwise
     */
    virtual OFCondition
    setLossyImageCompressionFlag(const OFString& ratios, const OFString& methods, const OFBool checkValues = OFTrue);

    /** Set equipment info for this segmentation object
     *  @param  equipmentInfo  The description of the equipment used to create
     *          this segmentation
     *  @param  checkValue If OFTrue, the data provided is checked for validity
     *  @return EC_Normal if equipment information could be set successfully, error otherwise
     */
    virtual OFCondition setEquipmentInfo(const IODGeneralEquipmentModule::EquipmentInfo& equipmentInfo,
                                         const OFBool checkValue = OFTrue);

    /** Set content identification info for this segmentation object
     *  @param  contentIdentification  The content identification of this segmentation
     *  @param  checkValue If OFTrue, the data provided is checked for validity
     *          (as possible)
     *  @return EC_Normal if content identification could be set correctly, OFFalse otherwise
     */
    virtual OFCondition setContentIdentification(const ContentIdentificationMacro& contentIdentification,
                                                 const OFBool checkValue = OFTrue);

    /** Import Patient, Study, and Frame of Reference level information from the
     *  given item. The method does only import Frame of Reference if Frame of
     *  Reference (FoR) UID is found in the image (and no error is reported if not).
     *  The reason is that if the source images of the segmentation do have an
     *  FoR, the segmentation object must have the same one, so we must import it.
     *  If the images do not have an FoR, we must not try at all importing it.
     *  If the log stream is set and valid the, the reason for any error might be
     *  obtained from the error/warning output.
     *  @param  dataset Reference to DICOM dataset from which the document
     *          should be read
     *  @param  takeOverCharset If OFTrue (default), Specific Character Set is
     *                          taken over from imported dataset. If it's not
     *                          present or empty (invalid), the attribute will
     *                          not be present in this class either.
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition importFromSourceImage(DcmItem& dataset, const OFBool takeOverCharset = OFTrue);

    /** Import Patient, Study, and Frame of Reference level information from the
     *  given item. The method does only import Frame of Reference if Frame of
     *  Reference UID is found in the image (and no error is reported if not).
     *  The reason is that if the source images of the segmentation do have an
     *  FoR, the segmentation object must have the same one, so we must import it.
     *  If the images do not have an FoR, we must not try at all importing it.
     *  The current content of this object is not deleted before reading.
     *  If the log stream is set and valid the, the reason for any error might be
     *  obtained from the error/warning output.
     *  @param  filename Reference to DICOM dataset from which the document
     *          should be read
     *  @param  takeOverCharset If OFTrue (default), Specific Character Set is
     *                          taken over from imported dataset. If it's not
     *                          present or empty (invalid), the attribute will
     *                          not be present in this class either.
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition importFromSourceImage(const OFString& filename, const OFBool takeOverCharset = OFTrue);

protected:
    /** Protected default constructor. Library users should the factory create..()
     *  method in order to create an object from scratch
     */
    DcmSegmentation();

    /** Overwrites DcmIODImage::read()
     *  @param  dataset The dataset to read from
     *  @return EC_Normal if reading succeeded, error otherwise
     */
    OFCondition read(DcmItem& dataset);

    /** Reads dataset without pixel data
     *  @param  dataset The dataset to read from
     *  @return EC_Normal if reading succeeded, error otherwise
     */
    OFCondition readWithoutPixelData(DcmItem& dataset);

    /** Writes the complete dataset without pixel data
     *  @param  dataset The dataset to write to
     *  @param  pixData Buffer for pixel data to write to
     *  @param  pixDataLength Length of pixData buffer
     *  @return EC_Normal if writing succeeded, error otherwise
     */
    OFCondition writeWithSeparatePixelData(DcmItem& dataset, Uint8*& pixData, size_t& pixDataLength);

    /** Create those data structures common for binary and fractional
     *  segmentations
     *  @param  segmentation The segmentation object created
     *  @param  rows The number of rows for the segmentation
     *  @param  columns The number of columns for the segmentation
     *  @param  equipmentInfo Equipment information
     *  @param  contentIdentification Content meta information
     *  @return EC_Normal if creation was successful, error otherwise
     */
    static OFCondition createCommon(DcmSegmentation*& segmentation,
                                    const Uint16 rows,
                                    const Uint16 columns,
                                    const IODGeneralEquipmentModule::EquipmentInfo& equipmentInfo,
                                    const ContentIdentificationMacro& contentIdentification);

    /** Hide same function from IODImage since do not want the user to access
     *  the image pixel module manually.
     *  @return The Image Pixel Module
     */
    virtual IODImagePixelModule<Uint8>& getImagePixel();

    /** Initialize IOD rules
     */
    virtual void initIODRules();

    /** Read segments from given item
     *  @param  item The item to read from
     *  @return EC_Normal if reading was successful, error otherwise
     */
    virtual OFCondition readSegments(DcmItem& item);

    /** Write fractional frames to given pixel data buffer
     *  @param  pixData The filled pixel data buffer returned by the method
     *  @return EC_Normal if writing was successful, error otherwise
     */
    OFCondition writeFractionalFrames(Uint8* pixData);

    /** Write binary frames to given given pixel data buffer
     *  @param  pixData The filled pixel data buffer returned by the method
     *  @param  rows Number of rows in frame
     *  @param  cols Number of columns in frame
     *  @param  pixDataLength The length of buffer in pixData (in bytes) returned by this method.
     *  @return EC_Normal if writing was successful, error otherwise
     */
    virtual OFCondition writeBinaryFrames(Uint8* pixData, Uint16 rows, Uint16 cols, const size_t pixDataLength);

    /** Write Segmentation Image Module
     *  @param dataset The item to write to, usually main dataset level
     *  @return EC_Normal if writing was successful, error otherwise
     */
    virtual OFCondition writeSegmentationImageModule(DcmItem& dataset);

    /** Write Segments
     *  @param item The item to write to
     *  @return EC_Normal if writing was successful, error otherwise
     */
    virtual OFCondition writeSegments(DcmItem& item);

    /** Write Multi-Frame Functional Groups
     *  @param dataset The item to write to, usually main dataset level
     *  @return EC_Normal if writing was successful, error otherwise
     */
    virtual OFCondition writeMultiFrameFunctionalGroupsModule(DcmItem& dataset);

    /** Write Multi-Frame Dimension Module
     *  @param dataset The item to write to, usually main dataset level
     *  @return EC_Normal if writing was successful, error otherwise
     */
    virtual OFCondition writeMultiFrameDimensionModule(DcmItem& dataset);

    /** Read frames
     *  @param  dataset Item to read from, usually main dataset level
     *  @return EC_Normal if reading was successful, error otherwise
     */
    virtual OFCondition readFrames(DcmItem& dataset);

    /** Get Image Pixel module attributes and perform some basic checking
     *  @param  dataset Item to read from, usually main dataset level
     *  @param  allocated Bits Allocated
     *  @param  stored Bits Stored
     *  @param  high High Bit
     *  @param  spp Samples Per Pixel
     *  @param  pixelRep Pixel Representation
     *  @param  rows Rows
     *  @param  cols Columns
     *  @param  numberOfFrames The number of frames (as set in the dataset)
     *  @param  colorModel The color model used
     *  @return EC_Normal if reading/checking was successful, error otherwise
     */
    OFCondition getAndCheckImagePixelAttributes(DcmItem& dataset,
                                                Uint16& allocated,
                                                Uint16& stored,
                                                Uint16& high,
                                                Uint16& spp,
                                                Uint16& pixelRep,
                                                Uint16& rows,
                                                Uint16& cols,
                                                Uint32& numberOfFrames,
                                                OFString& colorModel);

    /** This is the counterpart to the extractFrames() function. It takes a number
     *  of frames that are in binary segmentation format (i.e. "bit-packed") and
     *  concatenates them together so the resulting memory block fits the Pixel
     *  Data format for binary segmentations. Thus method ensure that frames
     *  are aligned bit for bit concatenated to each other with only (if
     *  applicable) having unused bits after the last frame.
     *  @param frames The source frames
     *  @param pixData The pixel data element data to be filled. Size must be
     *         at least bitsPerFrame * number of frames.
     *  @param bitsPerFrame Bits required per frame, usually rows * columns
     */
    virtual void concatFrames(OFVector<DcmIODTypes::Frame*> frames, Uint8* pixData, const size_t bitsPerFrame);

    /** Add frame to segmentation object.
     *  @param  pixData Pixel data to be added. Length must be rows*columns bytes.
     *          Pixel data is copied so it must be freed by the caller.
     *  @return EC_Normal if adding was successful, error otherwise
     */
    virtual OFCondition addFrame(Uint8* pixData);

private:
    // Modules supported:
    //
    // Patient Module (through DcmIODImage)
    // Patient Study Module (through DcmIODImage)
    // General Study Module (through DcmIODImage)
    // General Series Module (through DcmIODImage)
    // Segmentation Series Module
    // Frame of Reference Module (through DcmIODImage)
    // General Equipment Module (through DcmIODImage)
    // Enhanced General Equipment Module (through DcmIODImage)
    // General Image Module (through DcmIODImage)
    // Image Pixel Module (through DcmIODImage)
    // Segmentation Image Module (through this class)
    // Multi-frame Functional Group Module
    // Multi-Frame Dimension Module
    // SOP Common Module (through DcmIODImage)
    // Common Instance Reference Module (through DcmIODImage)

    /// Segmentation Series Module
    IODSegmentationSeriesModule m_SegmentationSeries;

    /// IODEnhancedEquipmentModule
    IODEnhGeneralEquipmentModule m_EnhancedGeneralEquipmentModule;

    /// Multi-frame Functional Groups module
    IODMultiFrameFGModule m_FG;

    /// Multi-frame Dimension Module
    IODMultiframeDimensionModule m_DimensionModule;

    /// Binary frame data
    OFVector<DcmIODTypes::Frame*> m_Frames;

    /* Image level information */

    /// Image Type: (CS, VM 2-n, Type 1), in Segmentations fixed to "DERIVED\PRIMARY"
    const OFString m_ImageType;

    // Instance Number: (IS, VM 1, Type 1)

    /// Content Identification Macro
    ContentIdentificationMacro m_ContentIdentificationMacro;

    /// Segmentation Type: (CS, 1, 1)
    DcmSegTypes::E_SegmentationType m_SegmentationType;

    /// Segmentation Fractional Type: (CS, 1, 1C) (required if FRACTIONAL)
    DcmSegTypes::E_SegmentationFractionalType m_SegmentationFractionalType;

    /// Maximum Fractional Value: (US, 1, 1C) (required if fractional type is FRACTIONAL)
    DcmUnsignedShort m_MaximumFractionalValue;

    /// Segment descriptions from Segment Sequence
    OFVector<DcmSegment*> m_Segments;

    /// Multi-frame Functional Groups high level interface
    FGInterface m_FGInterface;

    // --------------- private helper functions -------------------

    /** Clear old data
     */
    void clearData();

    /** Check the length of the pixel data
     *  @param  pixelData The Pixel Data element
     *  @param  rows Number of rows
     *  @param  cols Number of columns
     *  @param  numberOfFrames Number of frames
     *  @result OFTrue if length is valid, OFFalse otherwise
     */
    OFBool
    checkPixDataLength(DcmElement* pixelData, const Uint16 rows, const Uint16 cols, const Uint32& numberOfFrames);

    /** Loads file
     *  @param  dcmff The file format to load into
     *  @param  filename The filename of the file to load
     *  @param  dset Pointer to dataset after loading
     *  @return EC_Normal if loading was successful, error otherwise
     */
    static OFCondition loadFile(DcmFileFormat& dcmff, const OFString& filename, DcmDataset*& dset);

    /** Computes the number of total bytes required for the frame data of this
     *  segmentation object. Takes into account dimensions and number of frames,
     *  as well as the type of segmentation (member variables). May file if
     *  size_t type is not able to hold the result of intermediate computations.
     *  @param  rows Number of rows of a frame
     *  @param  cols Number of cols of a frame
     *  @param  numberOfFrames The number of frames of the object
     *  @param  bytesRequired Will hold the result of the computation,
     *          if successful. Does not any padding into account.
     *  @return EC_Normal if computation was possible, EC_TooManyBytesRequested
     *          otherwise.
     */
    OFCondition
    getTotalBytesRequired(const Uint16& rows, const Uint16& cols, const Uint32& numberOfFrames, size_t& bytesRequired);

    /** Read Fractional Type of segmentation.
     *  @param  item The item to read from
     *  @return EC_Normal if type could be read, EC_TagNotFound if tag is not present,
     *  error otherwise
     */
    OFCondition readSegmentationFractionalType(DcmItem& item);

    /** Read Segmentation Type of segmentation object
     *  @param  item The item to read from
     *  @return EC_Normal if type could be read, EC_TagNotFound if tag is not present,
     *  error otherwise
     */
    OFCondition readSegmentationType(DcmItem& item);

    /** Decompress the given dataset, if compressed in Deflated or RLE transfer syntax.
     *  @param  dset The dataset to be decompressed
     *  @return EC_Normal if decompression worked (or dataset has already been
     *  decompressed), IOD_EC_CannotDecompress otherwise
     */
    static OFCondition decompress(DcmDataset& dset);
};

#endif // SEGDOC_H
