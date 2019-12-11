/*
 *
 *  Copyright (C) 2019, Open Connections GmbH
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
 *  Module:  dcmect
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Class representing an Enhanced CT object
 *
 */

#ifndef DCMECT_ENHANCED_CT_H
#define DCMECT_ENHANCED_CT_H

#include "dcmtk/config/osconfig.h" // include OS configuration first

#include "dcmtk/dcmect/def.h"
#include "dcmtk/dcmect/types.h" // for Enhanced CT data types
#include "dcmtk/dcmfg/fginterface.h"
#include "dcmtk/dcmiod/iodimage.h"
#include "dcmtk/dcmiod/modacquisitioncontext.h"
#include "dcmtk/dcmiod/modenhequipment.h"
#include "dcmtk/dcmiod/modequipment.h"
#include "dcmtk/dcmiod/modimagepixel.h"
#include "dcmtk/dcmiod/modmultiframedimension.h"
#include "dcmtk/dcmiod/modmultiframefg.h"
#include "dcmtk/dcmiod/modsynchronisation.h"

class IODGeneralEquipmentModule;
class FGBase;
class ConcatenationLoader;
class ConcatenationCreator;

template <typename>
class IODImagePixelModule;

/** Class representing an object of the "Enhanced CT SOP Class".
 *  Supported Modules:
 *    - Patient (via iodimage.h)
 *    - Patient Study (via iodimage.h)
 *    - General Study (via iodimage.h)
 *    - General Series (via iodimage.h)
 *    - Enhanced CT Series (via this class)
 *    - Frame of Reference (via iodimage.h)
 *    - Synchronization (via modsynchronisation.h)
 *    - General Equipment (via iodimage.h)
 *    - Enhanced General Equipment (via modenhequipment.h)
 *    - Image Pixel (via this class)
 *    - Multi-frame Functional Groups (via modmultiframefg.h)
 *    - Multi-frame Dimension (via modmultiframedimension.h)
 *    - Acquisition Context (via modacquisitioncontext.h)
 *    - Enhanced CT (via mod_enhanced_ct.h)
 *    - SOP Common (via iodimage.h)
 *    - Common Instance Reference (via iodimage.h)
 *
 * Modules that are not supported:
 *     - Enhanced Contrast/Bolus
 *     - Cardiac Synchronization
 *     - Respiratory Synchronization
 *     - Supplemental Palette Color Lookup Table
 *     - Device
 *     - Specimen
 *     - Enhanced Multi-energy CT Acquisition
 *     - ICC Profile
 *     - Frame Extraction
 */
class DCMTK_DCMECT_EXPORT EctEnhancedCT : public DcmIODImage<IODImagePixelModule<Uint16>, IODImagePixelModule<Sint16> >
{

public:
    /** Inner class that offers typed interface to bulk data.
     */
    template <typename PixelType>
    class DCMTK_DCMECT_EXPORT Frames
    {
    public:
        /// Binary pixel type
        typedef PixelType pixel_type;

        /** Method that adds a frame to the Enhanced CT using the underlying
         *  binary data type.
         *  @param  data The frame data
         *  @param  numPixels The number of pixels (rows * columns) of the frame
         *  @param  perFrameInformation The per-frame functional groups applying for
         *  @return EC_Normal if adding was successful, error otherwise
         */
        OFCondition addFrame(PixelType* data, const size_t numPixels, const OFVector<FGBase*>& perFrameInformation);

        /** Type-specific getFrame() method that returns Sint16 or Uint16 (whatever
         *  is used in this Enhanced CT)
         *  @param  frameNumber The frame number to get, starting from 0
         *  @return Pointer to typed pixel data
         */
        PixelType* getFrame(const size_t frameNumber);

    private:
        /// Make sure the Enhanced CT object (and no one else) can use the constructor below.
        friend class EctEnhancedCT;

        /** Create Frames object in EctEnhancedCT (see friend declaration above)
         *  @param  ct The CT object that will contain the frames
         */
        Frames(EctEnhancedCT& ct);

        /// Reference to Enhanced CT object in order to access its pixel data
        EctEnhancedCT& m_CT;
    };

    /** Frames can contain Uint16, Sint16 or nothing (i.e.\ error condition)
     *  at all (in that case OFCondition denotes a corresponding error)
     */
    typedef OFvariant<OFCondition, Frames<Uint16>, Frames<Sint16> > FramesType;

    // -------------------- destruction -------------------------------

    /** Destructor, frees memory
     */
    virtual ~EctEnhancedCT();

    // -------------------- loading and saving ---------------------

    /** Static method to load a Enhanced CT object from a file.
     *  The memory of the resulting Enhanced CT object has to be freed by the
     *  caller.
     *  @param  filename The file to read from
     *  @param  ct The resulting Enhanced CT object. NULL if dataset
     *          could not be read successfully.
     *  @return EC_Normal if reading was successful, error otherwise
     */
    static OFCondition loadFile(const OFString& filename, EctEnhancedCT*& ct);

    /** Static method to load a Enhanced CT object from a dataset object.
     *  The memory of the resulting Enhanced CT object has to be freed by the
     *  caller.
     *  @param  dataset The dataset to read from
     *  @param  ct The resulting Enhanced CT object. NULL if dataset
     *          could not be read successfully.
     *  @return EC_Normal if reading was successful, error otherwise
     */
    static OFCondition loadDataset(DcmDataset& dataset, EctEnhancedCT*& ct);

    /** Static method to load a concatenation of a DICOM Enhanced CT instance
     *  into a EctEnhancedCT object.
     *  @param  cl The ConcatenationLoader instance to be used. Must be configured
     *          so that load() can be called.
     *  @param  concatenationUID The Concatenation UID identifying the Concatenation
     *          within the Concatenations know to ConcatenationLoader cl.
     *  @param  ct The resulting Enhanced CT object, if loading was successful
     *  @return EC_Normal if loading was successful, error otherwise
     */
    static OFCondition loadConcatenation(ConcatenationLoader& cl, const OFString& concatenationUID, EctEnhancedCT*& ct);

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

    // -------------------- creation ---------------------

    /** Factory method to create an Enhanced CT object from the minimal
     *  set of information required. The actual frame data is
     *  added separately.
     *  The memory of the resulting Enhanced CT object has to be freed by the
     *  caller.
     *  @param  ct The resulting Enhanced CT object if provided data is
     *          valid. Otherwise NULL is returned.
     *  @param  rows Number of rows of Enhanced CT frame data
     *  @param  columns Number of rows of Enhanced CT frame data
     *  @param  signedPixelData Denotes if pixel data is signed (OFTrue, Sint16) or
     *          unsigned (OFFalse, Uint16)
     *  @param  imageType1 First value for tag Image Type (0008,0008)
     *  @param  imageType3 Third Value for tag Image Type (0008,0008). See
     *          EctTypes::DT_ImageType3_... for Defined Terms.
     *  @param  imageType4 Fourth Value for tag Image Type (0008,0008). See
     *          EctTypes::DT_ImageType4_... for Defined Terms.
     *  @param  instanceNumber The value for tag Instance Number (0020,0013).
     *  @param  contentQualification The value for tag Content Qualification
     *          (0018,9004).
     *  @param  pixelPresentation The value for tag Pixel Presentation (0008,9205).
     *  @param  volumetricProperties The value for tag Volumetric Properties (0008,9206).
     *  @param  volumeBasedCalculationTechnique The value for tag Volume Based Calculation
     *          Technique (0008,9207).
     *  @param  equipmentInfo Equipment that is responsible for creating the EctEnhancedCT
     *  @param  acquisitionDateTime The date/time when the acquisition of data started.
     *          Required if value 1 of Image Type is is ORIGINAL or MIXED; may be present
     *          otherwise. If empty, value will not be set (default)
     *  @param  acquisitionDuration The time in seconds needed to complete the acquisition of data.
     *          Required if value 1 of Image Type is is ORIGINAL or MIXED; may be present
     *          otherwise. If negative, value will not be set (default)
     *  @return EC_Normal if creation was successful, error otherwise
     */
    static OFCondition create(EctEnhancedCT*& ct,
                              const Uint16 rows,
                              const Uint16 columns,
                              const OFBool signedPixelData,
                              const EctTypes::E_ImageType1 imageType1,
                              const OFString& imageType3,
                              const OFString& imageType4,
                              const OFString& instanceNumber,
                              const EctTypes::E_ContentQualification contentQualification,
                              const EctTypes::E_PixelPresentation pixelPresentation,
                              const EctTypes::E_VolumetricProperties volumetricProperties,
                              const OFString& volumeBasedCalculationTechnique,
                              const IODEnhGeneralEquipmentModule::EquipmentInfo& equipmentInfo,
                              const OFString& acquisitionDateTime = "",
                              const Float64& acquisitionDuration  = -1.0);

    // -------------------- access ---------------------

    /** Get number of frames, based on the number of items in the shared
     *  functional functional groups sequence (i.e.\ the attribute Number of
     *  Frames) is not trusted).
     *  @return The number of frames handled by this object
     */
    size_t getNumberOfFrames() const;

    /** Perform some basic checking. This method is also invoked when
     *  writing the object to a DICOM dataset or file.
     *  @param  checkFGStructure If OFTrue (default), structure of functional
     *          groups is checked, too.
     *  @return OFTrue, if no errors were found, OFFalse otherwise.
     */
    virtual OFBool check(const OFBool checkFGStructure = OFTrue);

    /** Get access to functional groups. This is meant for reading data from
     *  functional groups that are not actively managed, i.e.\ made accessible by
     *  EctEnhancedCT. In rare cases, however, it makes sense to access it
     *  for writing too, e.g.\ in order to add Stacks; use with care!
     *  @return Reference to the functional groups
     */
    virtual FGInterface& getFunctionalGroups();

    /** Get reference t Concatenation information
     *  @return Reference to ConcatenationInfo object
     */
    virtual IODMultiFrameFGModule::ConcatenationInfo& getConcatenationInfo();

    // -------------------- modification ---------------------

    /** Add a functional group for all frames
     *  @param  group The group to be added as shared functional group. The
     *  @return EC_Normal if adding was successful, error otherwise
     */
    virtual OFCondition addForAllFrames(const FGBase& group);

    /** Return reference to multi-fame dimension module
     *  @return Reference to multi-frame dimension module
     */
    virtual IODMultiframeDimensionModule& getDimensions();

    /** Return Frames
     *  @return All frames currently assigned to the object
     */
    virtual FramesType getFrames();

    /** Import Patient, Study, and Frame of Reference level information from the
     *  given item. The method does only import Frame of Reference if Frame of
     *  Reference (FoR) UID is found in the image (and no error is reported if not).
     *  The reason is that if the source images of the Enhanced CT do have an
     *  FoR, the Enhanced CT object must have the same one, so we must import it.
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
     *  The reason is that if the source images of the Enhanced CT do have an
     *  FoR, the Enhanced CT object must have the same one, so we must import it.
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

    // ---------------------- Get / Set modules ----------------------

    /** Get Patient Module
     *  @return a reference to the IOD Patient Module
     */
    virtual IODPatientModule& getIODPatientModule();

    /** Get General Study Module
     *  @return a reference to the IOD General Study Module
     */
    virtual IODGeneralStudyModule& getIODGeneralStudyModule();

    /** Get Patient Study Module
     *  @return a reference to the IOD Patient Study Module
     */
    virtual IODPatientStudyModule& getIODPatientStudyModule();

    /** Get General Series Module
     *  @return a reference to the IOD General Series Module
     */
    virtual IODGeneralSeriesModule& getIODGeneralSeriesModule();

    /** Get Frame of Reference Module
     *  @return a reference to the IOD Frame of Reference Module
     */
    virtual IODFoRModule& getIODFrameOfReferenceModule();

    /** Get Synchronization Module
     *  @return a reference to the IOD Frame of Reference Module
     */
    virtual IODSynchronizationModule& getIODSynchronizationModule();

    /** Set whether Synchronization Module will be enabled when writing
     *  the Enhanced CT object.
     *  @param  enabled If OFTrue, Synchronization Module will be written,
     *          otherwise not.
     */
    virtual void setIODSynchronisationModuleEnabled(const OFBool enabled);

    /** Returns whether Synchronization Module is enabled for writing
     *  the Enhanced CT object.
     *  @return OFTrue, if Synchronization Module will be written,
     *          OFFalse otherwise.
     */
    virtual OFBool getIODSynchronisationModuleEnabled();

    /** Get General Equipment Module
     *  @return a reference to the General Equipment Module
     */
    virtual IODGeneralEquipmentModule& getIODGeneralEquipmentModule();

    /** Get Enhanced General Equipment Module
     *  @return a reference to the Enhanced General Equipment Module
     */
    virtual IODEnhGeneralEquipmentModule& getIODEnhGeneralEquipmentModule();

    /** Get Multi-frame Functional Groups Module
     *  @return a reference to the Multi-frame Functional Groups Module
     */
    virtual IODMultiFrameFGModule& getIODMultiFrameFGModule();

    /** Get Multi-frame Dimension Module
     *  @return a reference to the Multi-frame Dimension Module
     */
    virtual IODMultiframeDimensionModule& getIODMultiframeDimensionModule();

    /** Get Acquisition Context Module
     *  @return a reference to the Acquisition Context Module
     */
    virtual IODAcquisitionContextModule& getIODAcquisitionContextModule();

    /** Get Common Instance Reference Module
     *  @return a reference to the Common Instance Reference Module
     */
    virtual IODCommonInstanceReferenceModule& getIODCommonInstanceReferenceModule();

    /** Get SOP Common Module
     *  @return a reference to the SOP Common Module
     */
    virtual IODSOPCommonModule& getIODSOPCommonModule();

    // ---------------- CT Image level getter() --------------------

    /** Get Image Type
     *  @param  value Reference to variable that should hold the result
     *  @param  pos Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal, if value could be returned, error otherwise
     */
    virtual OFCondition getImageType(OFString& value, const long pos);

    /** Get Acquisition Number
     *  @param  value Reference to variable that should hold the result
     *  @param  pos Index of the value to get (0..vm)
     *  @return EC_Normal, if value could be returned, error otherwise
     */
    virtual OFCondition getAcquisitionNumber(Sint32& value, const unsigned long pos);

    /** Get Acquisition Number
     *  @param  value Reference to variable that should hold the result
     *  @param  pos Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal, if value could be returned, error otherwise
     */
    virtual OFCondition getAcquisitionNumber(OFString& value, const long pos);

    /** Get Acquisition DateTime
     *  @param  value Reference to variable that should hold the result
     *  @param  pos Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal, if value could be returned, error otherwise
     */
    virtual OFCondition getAcquisitionDateTime(OFString& value, const long pos);

    /** Get Acquisition Duration
     *  @param  value Reference to variable that should hold the result
     *  @param  pos Index of the value to get (0..vm)
     *  @return EC_Normal, if value could be returned, error otherwise
     */
    virtual OFCondition getAcquisitionDuration(Float64& value, const unsigned long pos);

    /** Get Acquisition Duration
     *  @param  value Reference to variable that should hold the result
     *  @param  pos Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal, if value could be returned, error otherwise
     */
    virtual OFCondition getAcquisitionDuration(OFString& value, const long pos);

    /** Get Content Qualification
     *  @param  value Reference to variable that should hold the result
     *  @param  pos Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal, if value could be returned, error otherwise
     */
    virtual OFCondition getContentQualification(OFString& value, const long pos);

    /** Get Image Comments
     *  @param  value Reference to variable that should hold the result
     *  @param  pos Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal, if value could be returned, error otherwise
     */
    virtual OFCondition getImageComments(OFString& value, const long pos);

    /** Get Burned In Annotation (if filled in correctly by creator, this
     *  should always be "NO").
     *  @param  value Reference to variable that should hold the result
     *  @param  pos Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal, if value could be returned, error otherwise
     */
    virtual OFCondition getBurnedInAnnotation(OFString& value, const long pos);

    /** Get Recognizable Visual Features
     *  @param  value Reference to variable that should hold the result
     *  @param  pos Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal, if value could be returned, error otherwise
     */
    virtual OFCondition getRecognizableVisualFeatures(OFString& value, const long pos);

    /** Get Lossy Image Compression
     *  @param  value Reference to variable that should hold the result
     *  @param  pos Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal, if value could be returned, error otherwise
     */
    virtual OFCondition getLossyImageCompression(OFString& value, const long pos);

    /** Get Lossy Image Compression Ratio
     *  @param  value Reference to variable that should hold the result
     *  @param  pos Index of the value to get (0..vm)
     *  @return EC_Normal, if value could be returned, error otherwise
     */
    virtual OFCondition getLossyImageCompressionRatio(Float64& value, const unsigned long pos);

    /** Get Lossy Image Compression Ratio
     *  @param  value Reference to variable that should hold the result
     *  @param  pos Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal, if value could be returned, error otherwise
     */
    virtual OFCondition getLossyImageCompressionRatio(OFString& value, const long pos);

    /** Get Lossy Image Compression Method
     *  @param  value Reference to variable that should hold the result
     *  @param  pos Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal, if value could be returned, error otherwise
     */
    virtual OFCondition getLossyImageCompressionMethod(OFString& value, const long pos);

    /** Get Presentation LUT Shape
     *  @param  value Reference to variable that should hold the result. Should
     *          always return the value "IDENTITY" if creator filled in the value
     *          correctly.
     *  @param  pos Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal, if value could be returned, error otherwise
     */
    virtual OFCondition getPresentationLUTShape(OFString& value, const long pos);

    /** Get Multi-Energy CT Acquisition
     *  @param  value Reference to variable that should hold the result.
     *  @param  pos Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal, if value could be returned, error otherwise
     */
    virtual OFCondition getMultiEnergyCTAcquisition(OFString& value, const long pos);

    /** Get Pixel Presentation
     *  @param  value Reference to variable that should hold the result.
     *  @param  pos Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal, if value could be returned, error otherwise
     */
    virtual OFCondition getPixelPresentation(OFString& value, const long pos);

    /** Get Volumetric Properties
     *  @param  value Reference to variable that should hold the result.
     *  @param  pos Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal, if value could be returned, error otherwise
     */
    virtual OFCondition getVolumetricProperties(OFString& value, const long pos);

    /** Get Volume Based Calculation Technique
     *  @param  value Reference to variable that should hold the result.
     *  @param  pos Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal, if value could be returned, error otherwise
     */
    virtual OFCondition getVolumeBasedCalculationTechnique(OFString& value, const long pos);

    /** Get Isocenter Position
     *  @param  value Reference to variable that should hold the result.
     *  @param  pos Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal, if value could be returned, error otherwise
     */
    virtual OFCondition getIsocenterPosition(OFString& value, const long pos);

    /** Get Isocenter Position
     *  @param  values Reference to variable that should hold the result. Should
     *          hold 3 values if not empty and filled correctly by the creator of
     *          the Enhanced CT object.
     *  @return EC_Normal, if value could be returned, error otherwise
     */
    virtual OFCondition getIsocenterPosition(OFVector<Float64>& values);

    /** Get number of rows
     *  @param  rows The number of rows (output)
     *  @return EC_Normal if getting was successful, error otherwise
     */
    virtual OFCondition getRows(Uint16& rows);

    /** Get number of cols
     *  @param  cols The number of columns (output)
     *  @return EC_Normal if getting was successful, error otherwise
     */
    virtual OFCondition getColumns(Uint16& cols);

    // ---------------- Setters() --------------------

    virtual OFCondition setImageType(const OFString& value, const OFBool check = OFTrue);

    virtual OFCondition setAcquisitionNumber(const OFString& value, const OFBool check = OFTrue);

    virtual OFCondition setAcquisitionDateTime(const OFString& value, const OFBool check = OFTrue);

    virtual OFCondition setAcquisitionDuration(const Float64& value, const OFBool check = OFTrue);

    virtual OFCondition setContentQualification(const EctTypes::E_ContentQualification value,
                                                const OFBool check = OFTrue);

    virtual OFCondition setImageComments(const OFString& value, const OFBool check = OFTrue);

    virtual OFCondition setRecognizableVisualFeatures(const EctTypes::E_RecognizableVisualFeatures value,
                                                      const OFBool check = OFTrue);

    /** Set lossy compression flag of the image to "00" or "01" If set to "01",
     *  ratios and methods have to be provided, too.
     *  @param  isLossy If OFTrue, Lossy Image Compression is set to "01", otherwise
     *          to "00".
     *  @param  ratios Compression ratios (separated by backslash) of the applied
     *          lossy compression steps. Only one value (and no backslash) if only
     *          one step was performed. The parameter is ignored if isLossy is OFFalse.
     *  @param  methods Methods (separated by backslash) of the applied
     *          lossy compression steps. Only one value (and no backslash) if only
     *          one step was performed. The parameter is ignored if isLossy is OFFalse.
     *  @param  check If OFTrue, the data provided is checked for validity
     *  @return EC_Normal if lossy compression info could be set, error code otherwise
     */
    virtual OFCondition setLossyImageCompression(const OFBool isLossy,
                                                 const OFString& ratios  = "",
                                                 const OFString& methods = "",
                                                 const OFBool check      = OFTrue);

    /** Set Isocenter Position
     *  @param  values Value that should be set
     *  @param  check If OFTrue, basic checks are performed whether the values are
     *          valid for this attribute
     *  @return EC_Normal, if value was set, error otherwise
     */
    virtual OFCondition setISOCenterPosition(const OFVector<Float64>& values, const OFBool check = OFTrue);

    /** Set Isocenter Position
     *  @param  value Value that should be set
     *  @param  check If OFTrue, basic checks are performed whether the value is
     *          valid for this attribute
     *  @return EC_Normal, if value was set, error otherwise
     */
    virtual OFCondition setISOCenterPosition(const OFString& value, const OFBool check = OFTrue);

    /** Set Multi-Energy CT Acquisition
     *  @param  value Value that should be set
     *  @param  check If OFTrue, basic checks are performed whether the value is
     *          valid for this attribute
     *  @return EC_Normal, if value was set, error otherwise
     */
    virtual OFCondition setMultiEnergyCTAcquisition(const OFString& value, const OFBool check = OFTrue);

    /** Set Pixel Presentation
     *  @param  value Value that should be set
     *  @param  check Ignored in current implementation (enum value is always checked for validity).
     *  @return EC_Normal, if value was set, error otherwise
     */
    virtual OFCondition setPixelPresentation(const EctTypes::E_PixelPresentation value, const OFBool check = OFTrue);

    /** Set Volumetric Properties
     *  @param  value Value that should be set
     *  @param  check Ignored in current implementation (enum value is always checked for validity).
     *  @return EC_Normal, if value was set, error otherwise
     */
    virtual OFCondition setVolumetricProperties(const EctTypes::E_VolumetricProperties value,
                                                const OFBool check = OFTrue);

    /** Set Volume Based Calculation Technique
     *  @param  value Value that should be set
     *  @param  check If OFTrue, basic checks are performed whether the value is
     *          valid for this attribute
     *  @return EC_Normal, if value was set, error otherwise
     */
    virtual OFCondition setVolumeBasedCalculationTechnique(const OFString& value, const OFBool check = OFTrue);

protected:
    /** Protected default constructor. Library users should the factory create..()
     *  method in order to create an object from scratch
     */
    template <typename ImagePixel>
    EctEnhancedCT(OFin_place_type_t(ImagePixel));

    /** Overwrites and publicly hides DcmIODImage::read()
     *  @param  dataset The dataset to read from
     *  @return EC_Normal if reading succeeded, error otherwise
     */
    virtual OFCondition read(DcmItem& dataset);

    /** Overwrites and publicly hides DcmIODImage::write()
     *  @param  dataset The dataset to write to
     *  @return EC_Normal if writing succeeded, error otherwise
     */
    virtual OFCondition write(DcmItem& dataset);

    /** Writes pixel data to item
     *  @param  dataset The dataset to write to
     *  @return EC_Normal if writing succeeded, error otherwise
     */
    virtual OFCondition writeGeneric(DcmItem& dataset);

    /** Read Enhanced CT's generic (not directly pixel-data related) attributes from
     *  DICOM dataset.
     *  @param  dataset Reference to DICOM dataset to read from.
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition readGeneric(DcmItem& dataset);

    /** General Image Module is not allowed in Enhanced CT, remove from public interface.
     *  @return Reference to General Image Module
     */
    virtual IODGeneralImageModule& getGeneralImage()
    {
        return IODImage::getGeneralImage();
    }

private:
    // Forward declarations (pixel type related Visitors)
    struct SetImagePixelModuleVisitor;
    struct ReadVisitor;
    struct WriteVisitor;
    struct WriteVisitorConcatenation;
    struct GetFramesVisitor;

    /// Synchronization Module
    IODSynchronizationModule m_SynchronisationModule;

    /// Denote whether Synchronization Module is enabled (OFTrue)
    /// or ignored (OFFalse) on writing and checking.
    OFBool m_SynchronisationModuleEnabled;

    /// IODEnhancedEquipmentModule
    IODEnhGeneralEquipmentModule m_EnhancedGeneralEquipmentModule;

    /// Multi-frame Functional Groups module
    IODMultiFrameFGModule m_FG;

    /// Multi-frame Dimension Module
    IODMultiframeDimensionModule m_DimensionModule;

    /// Acquisition Context Module
    IODAcquisitionContextModule m_AcquisitionContextModule;

    /// Common Instance Reference Module
    IODCommonInstanceReferenceModule m_CommonInstanceReferenceModule;

    /// Binary frame data
    OFVector<DcmIODTypes::Frame*> m_Frames;

    /// Multi-frame Functional Groups high level interface
    FGInterface m_FGInterface;

    /* Image level information */

    /// Image Type: (CS, VM 4, Type 1)
    DcmCodeString m_ImageType;

    /// Instance Number: (IS, VM 1, Type 1)
    DcmIntegerString m_InstanceNumber;

    /// Multi-Energy CT Acquisition: (CS VM 1, Type 3)
    DcmCodeString m_MultiEnergyCTAcquisition;

    /// Pixel Presentation: (CS, VM 1, Type 1)
    DcmCodeString m_PixelPresentation;

    /// Volumetric Properties: (CS, VM 1, Type 1)
    DcmCodeString m_VolumetricProperties;

    /// Volume Based Calculation Technique: (CS, VM 1, Type 1)
    DcmCodeString m_VolumeBasedCalculationTechnique;

    /// Acquisition Number: (IS, VM 1, Type 3)
    DcmIntegerString m_AcquisitionNumber;

    /// Acquisition DateTime: (DT, VM 1, Type 1C)
    DcmDateTime m_AcquisitionDateTime;

    /// Acquisition Duration: (FD, VM 1, Type 1C)
    DcmFloatingPointDouble m_AcquisitionDuration;

    /// Content Qualification: (CS, VM 1, Type 1C)
    DcmCodeString m_ContentQualification;

    /// Image Comments: (LT, VM 1, Type 3)
    DcmLongText m_ImageComments;

    /// Burned In Annotation: (CS, VM 1, Type 1C)
    DcmCodeString m_BurnedInAnnotation;

    /// Recognizable Visual Features: (CS, VM 1, Type 3)
    DcmCodeString m_RecognizableVisualFeatures;

    /// Lossy Image Compression: (CS, VM 1, Type 1C)
    DcmCodeString m_LossyImageCompression;

    /// Lossy Image Compression Ratio: (DS, VM 1, Type 1C)
    DcmDecimalString m_LossyImageCompressionRatio;

    /// Lossy Image Compression Method: (CS, VM 1, Type 1C)
    DcmCodeString m_LossyImageCompressionMethod;

    /// Presentation LUT Shape: (CS, VM 1, Type 1)
    DcmCodeString m_PresentationLUTShape;

    /// Isocenter Position: (DS, VM 3, Type 3)
    DcmDecimalString m_IsoCenterPosition;

    // --------------- private helper functions -------------------

    /** Decompress the given dataset
     *  @param  dset The dataset to be decompressed
     *  @return EC_Normal if decompression worked (or dataset has already been
     *  decompressed), IOD_EC_CannotDecompress otherwise
     */
    static OFCondition decompress(DcmDataset& dset);
};

#endif // DCMECT_ENHANCED_CT_H
